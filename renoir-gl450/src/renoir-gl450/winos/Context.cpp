#include "renoir-gl450/Context.h"
#include "renoir-gl450/Handle.h"
#include "renoir/Renoir.h"

#include <mn/Memory.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#undef DELETE

#include <assert.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/wglew.h>

LRESULT CALLBACK
_fake_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		break;
	}

	return DefWindowProcA(hwnd, msg, wparam, lparam);
}

struct Renoir_GL450_Context
{
	HGLRC context;
	HWND dummy_window;
	HDC dummy_dc;
};

inline static int
_renoir_gl450_msaa_to_int(RENOIR_MSAA_MODE mode)
{
	int res = 0;
	switch(mode)
	{
	case RENOIR_MSAA_MODE_NONE:
		res = 0;
		break;
	case RENOIR_MSAA_MODE_2:
		res = 2;
		break;
	case RENOIR_MSAA_MODE_4:
		res = 4;
		break;
	case RENOIR_MSAA_MODE_8:
		res = 8;
		break;
	default:
		assert(false && "unreachable");
		break;
	}
	return res;
}

// API
Renoir_GL450_Context*
renoir_gl450_context_new(Renoir_Settings* settings, void*)
{
	HGLRC fake_ctx = NULL;
	HDC fake_dc = NULL;
	HWND fake_wnd = NULL;
	HGLRC ctx = NULL;
	HDC dummy_dc = NULL;
	HWND dummy_window = NULL;

	// Setup Window Class that we'll use down there
	WNDCLASSEXA wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEXA));
	wc.cbSize		 = sizeof(WNDCLASSEXA);
	wc.style		 = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc	 = _fake_window_proc;
	wc.hInstance	 = NULL;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "renoirHiddenWindowClass";
	RegisterClassExA(&wc);

	// 1 pixel window dimension since all the windows we'll be doing down there are hidden
	RECT wr = {0, 0, LONG(1), LONG(1)};
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	// The first step in creating Modern GL Context is to create Legacy GL Context
	// so we setup fake window and dc to create the legacy context so that we could
	// initialize GLEW which will use wglGetProcAddress to load Modern OpenGL implmentation
	// off the GPU driver
	fake_wnd = CreateWindowExA(
		NULL,
		"renoirHiddenWindowClass",
		"Fake Window",
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		NULL,
		NULL
	);

	fake_dc = GetDC(fake_wnd);

	PIXELFORMATDESCRIPTOR fake_pfd;
	ZeroMemory(&fake_pfd, sizeof(fake_pfd));
	fake_pfd.nSize		= sizeof(fake_pfd);
	fake_pfd.nVersion	= 1;
	fake_pfd.dwFlags	= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE;
	fake_pfd.iPixelType = PFD_TYPE_RGBA;
	fake_pfd.cColorBits = 32;
	fake_pfd.cAlphaBits = 8;
	fake_pfd.cDepthBits = 24;
	fake_pfd.iLayerType = PFD_MAIN_PLANE;
	int fake_pfdid = ChoosePixelFormat(fake_dc, &fake_pfd);
	assert(fake_pfdid && "ChoosePixelFormat failed");
	if (fake_pfdid == 0)
		goto err;

	bool result = SetPixelFormat(fake_dc, fake_pfdid, &fake_pfd);
	assert(result && "SetPixelFormat failed");
	if (result == false)
		goto err;

	fake_ctx = wglCreateContext(fake_dc);
	assert(fake_ctx && "wglCreateContext failed");
	if (fake_ctx == NULL)
		goto err;

	result = wglMakeCurrent(fake_dc, fake_ctx);
	assert(result && "wglMakeCurrent failed");
	if (result == false)
		goto err;

	// At last GLEW initialized
	GLenum glew_result = glewInit();
	assert(glew_result == GLEW_OK && "glewInit failed");
	if (glew_result != GLEW_OK)
		goto err;

	// now create the hidden window and dc which will be attached to opengl context
	dummy_window = CreateWindowExA(
		NULL,
		"renoirHiddenWindowClass",
		"GL Context Window",
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		NULL,
		NULL);

	assert(dummy_window != NULL && "CreateWindowEx failed");

	dummy_dc = GetDC(dummy_window);

	// setup the modern pixel format in order to create the modern GL Context
	const int pixel_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, (settings->msaa != RENOIR_MSAA_MODE_NONE) ? GL_TRUE : GL_FALSE,
		WGL_SAMPLES_ARB, _renoir_gl450_msaa_to_int(settings->msaa),
		0, 0
	};

	int pixel_format_id;
	UINT num_formats;
	bool status = wglChoosePixelFormatARB(dummy_dc, pixel_attribs, NULL, 1, &pixel_format_id, &num_formats);
	assert(status && num_formats > 0 && "wglChoosePixelFormatARB failed");
	if (status == false || num_formats == 0)
		goto err;

	PIXELFORMATDESCRIPTOR pixel_format{};
	DescribePixelFormat(dummy_dc, pixel_format_id, sizeof(pixel_format), &pixel_format);
	SetPixelFormat(dummy_dc, pixel_format_id, &pixel_format);

	// now we are in a position to create the modern opengl context
	const int major_min = 4, minor_min = 5;
	int context_attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
		WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	ctx = wglCreateContextAttribsARB(dummy_dc, 0, context_attribs);
	assert(ctx && "wglCreateContextAttribsARB failed");
	if (ctx == NULL)
		goto err;

	result = wglMakeCurrent(dummy_dc, ctx);
	assert(result && "wglMakeCurrent failed");
	if (result == false)
		goto err;

	glew_result = glewInit();
	assert(glew_result == GLEW_OK && "glewInit failed");
	if (glew_result != GLEW_OK)
		goto err;

	glEnable(GL_DEPTH_TEST);
	glDepthRange(0.0, 1.0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

	wglDeleteContext(fake_ctx);
	ReleaseDC(fake_wnd, fake_dc);
	DestroyWindow(fake_wnd);

	auto self = mn::alloc<Renoir_GL450_Context>();
	self->context = ctx;
	self->dummy_dc = dummy_dc;
	self->dummy_window = dummy_window;
	return self;
err:
	if (fake_ctx) wglDeleteContext(fake_ctx);
	if (fake_dc) ReleaseDC(fake_wnd, fake_dc);
	if (fake_wnd) DestroyWindow(fake_wnd);
	if (ctx) wglDeleteContext(ctx);
	if (dummy_dc) ReleaseDC(dummy_window, dummy_dc);
	if (dummy_window) DestroyWindow(dummy_window);
	return NULL;
}

void
renoir_gl450_context_free(Renoir_GL450_Context* self)
{
	wglDeleteContext(self->context);
	ReleaseDC(self->dummy_window, self->dummy_dc);
	DestroyWindow(self->dummy_window);
	mn::free(self);
}

void
renoir_gl450_context_window_init(Renoir_GL450_Context* self, Renoir_Handle* h, Renoir_Settings* settings)
{
	auto hdc = GetDC((HWND)h->view_window.handle);
	h->view_window.hdc = hdc;

	// setup the modern pixel format in order to create the modern GL Context
	const int pixel_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, (settings->msaa != RENOIR_MSAA_MODE_NONE) ? GL_TRUE : GL_FALSE,
		WGL_SAMPLES_ARB, _renoir_gl450_msaa_to_int(settings->msaa),
		0, 0
	};

	int pixel_format_id;
	UINT num_formats;
	bool status = wglChoosePixelFormatARB(hdc, pixel_attribs, NULL, 1, &pixel_format_id, &num_formats);

	assert(status && num_formats > 0 && "wglChoosePixelFormatARB failed");

	PIXELFORMATDESCRIPTOR pixel_format{};
	DescribePixelFormat(hdc, pixel_format_id, sizeof(pixel_format), &pixel_format);
	bool result = SetPixelFormat(hdc, pixel_format_id, &pixel_format);
	assert(result && "SetPixelFormat failed");

	result = wglMakeCurrent(hdc, self->context);
	assert(result && "wglMakeCurrent failed");

	switch (settings->vsync)
	{
	case RENOIR_VSYNC_MODE_ON:
		wglSwapIntervalEXT(1);
		break;
	case RENOIR_VSYNC_MODE_OFF:
		wglSwapIntervalEXT(0);
		break;
	default:
		assert(false && "unreachable");
		break;
	}
}

void
renoir_gl450_context_window_free(Renoir_GL450_Context* self, Renoir_Handle* h)
{
	ReleaseDC((HWND)h->view_window.handle, (HDC)h->view_window.hdc);
}

void
renoir_gl450_context_window_bind(Renoir_GL450_Context* self, Renoir_Handle* h)
{
	wglMakeCurrent((HDC)h->view_window.hdc, (HGLRC)self->context);
}

void
renoir_gl450_context_bind(Renoir_GL450_Context* self)
{
	wglMakeCurrent((HDC)self->dummy_dc, (HGLRC)self->context);
}

void
renoir_gl450_context_unbind(Renoir_GL450_Context* self)
{
	wglMakeCurrent(NULL, NULL);
}

void
renoir_gl450_context_window_present(Renoir_GL450_Context* self, Renoir_Handle* h)
{
	SwapBuffers((HDC)h->view_window.hdc);
}

void
renoir_gl450_context_reload(Renoir_GL450_Context* self)
{
	wglMakeCurrent((HDC)self->dummy_dc, (HGLRC)self->context);
	GLenum glew_result = glewInit();
	assert(glew_result == GLEW_OK && "glewInit failed");
	(void)glew_result;
}