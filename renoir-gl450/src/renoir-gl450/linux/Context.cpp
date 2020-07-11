#include "renoir-gl450/Context.h"
#include "renoir-gl450/Handle.h"
#include "renoir/Renoir.h"

#include <mn/Memory.h>
#include <mn/Log.h>
#include <mn/Defer.h>

#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/glew.h>

#include <assert.h>

struct Renoir_GL450_Context
{
	::Display* display;
	bool owns_display;
	::GLXContext context;
	::Window dummy_window;
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

using glXCreateContextAttribsARBProc = GLXContext (*)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
using glXSwapIntervalEXTProc = void (*)(Display* display, GLXDrawable drawable, int interval);

// API
Renoir_GL450_Context*
renoir_gl450_context_new(Renoir_Settings* settings, void* given_display)
{
	if (settings->external_context) return nullptr;

	const int visual_attribs[] = {
		GLX_X_RENDERABLE        , True,
		GLX_DRAWABLE_TYPE       , GLX_WINDOW_BIT,
		GLX_DOUBLEBUFFER        , True,
		GLX_RENDER_TYPE         , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE       , GLX_TRUE_COLOR,
		GLX_RED_SIZE            , 8,
		GLX_GREEN_SIZE          , 8,
		GLX_BLUE_SIZE           , 8,
		GLX_ALPHA_SIZE          , 8,
		GLX_DEPTH_SIZE          , 24,
		GLX_STENCIL_SIZE        , 8,
		GLX_SAMPLE_BUFFERS_ARB  , (settings->msaa != RENOIR_MSAA_MODE_NONE) ? True : False,
		GLX_SAMPLES_ARB         , _renoir_gl450_msaa_to_int(settings->msaa),
		None
	};

	const int major = 4, minor = 5;
	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, major,
		GLX_CONTEXT_MINOR_VERSION_ARB, minor,
		GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	auto display = (Display*)given_display;
	if (display == nullptr)
	{
		display = XOpenDisplay(nullptr);
		if (display == nullptr)
			return nullptr;
	}
	mn_defer(if (display && given_display == nullptr) XCloseDisplay(display));

	int fbcount = 0;
	auto fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
	if (fbc == nullptr)
		return nullptr;
	mn_defer(XFree(fbc));

	int best_fbc = -1;
	int worst_fbc = -1;
	int best_num_samp = -1;
	int worst_num_samp = 999;
	for(int i = 0; i < fbcount; ++i)
	{
		XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc[i]);
		mn_defer(XFree(vi));
		if(vi)
		{
			int samp_buf, samples;
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

			if(best_fbc < 0 || (samp_buf && samples > best_num_samp))
				best_fbc = i, best_num_samp = samples;
			if(worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
				worst_fbc = i, worst_num_samp = samples;
		}
	}
	auto bestFbc = fbc[best_fbc];

	auto vi = glXGetVisualFromFBConfig(display, bestFbc);
	if (vi == nullptr)
		return nullptr;
	mn_defer(XFree(vi));

	auto color_map = XCreateColormap(display, RootWindow(display, vi->screen), vi->visual, AllocNone);

	XSetWindowAttributes swa{};
	swa.colormap = color_map;
	swa.background_pixmap = None;
	swa.border_pixel = BlackPixel(display, vi->screen);
	swa.background_pixel = WhitePixel(display, vi->screen);
	swa.event_mask = StructureNotifyMask | ExposureMask;
	auto dummy_window = XCreateWindow(
		display,
		RootWindow(display, vi->screen),
		0,
		0,
		100,
		100,
		0,
		vi->depth,
		InputOutput,
		vi->visual,
		CWBorderPixel | CWColormap | CWEventMask,
		&swa
	);
	if (dummy_window == None)
		return nullptr;
	mn_defer(if(dummy_window) XDestroyWindow(display, dummy_window));

	auto glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
	if (glXCreateContextAttribsARB == nullptr)
		return nullptr;

	auto context = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);
	if (context == nullptr)
		return nullptr;
	mn_defer(if (context) glXDestroyContext(display, context));

	glXMakeCurrent(display, dummy_window, context);

	glewExperimental = true;
	auto glew_result = glewInit();
	assert(glew_result == GLEW_OK);
	(void) glew_result;

	mn::log_info("OpenGL Renderer: {}", glGetString(GL_RENDERER));
	mn::log_info("OpenGL Version: {}", glGetString(GL_VERSION));
	mn::log_info("GLSL Version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEPTH_TEST);
	glDepthRange(0.0, 1.0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

	XSync(display, False);

	auto self = mn::alloc_zerod<Renoir_GL450_Context>();
	self->context = context;
	self->display = display;
	self->owns_display = given_display == nullptr;
	self->dummy_window = dummy_window;

	context = nullptr;
	display = nullptr;
	dummy_window = None;
	return self;
}

void
renoir_gl450_context_free(Renoir_GL450_Context* self)
{
	if (self == nullptr) return;

	if(self->dummy_window)
		XDestroyWindow(self->display, self->dummy_window);
	if (self->context)
		glXDestroyContext(self->display, self->context);
	if (self->display && self->owns_display)
		XCloseDisplay(self->display);
	mn::free(self);
}

void
renoir_gl450_context_window_init(Renoir_GL450_Context* self, Renoir_Handle* h, Renoir_Settings* settings)
{
	if (self == nullptr) return;

	bool result = glXMakeCurrent(self->display, (GLXDrawable)h->swapchain.handle, self->context);
	assert(result && "glXMakeCurrent");
	(void) result;

	glXSwapIntervalEXTProc swap_interval = nullptr;
	swap_interval = (glXSwapIntervalEXTProc)glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalEXT");
	assert(swap_interval != NULL && "glXGetProcAddressARB failed");

	switch (settings->vsync)
	{
	case RENOIR_VSYNC_MODE_ON:
		swap_interval(self->display, (GLXDrawable)h->swapchain.handle, 1);
		break;
	case RENOIR_VSYNC_MODE_OFF:
		swap_interval(self->display, (GLXDrawable)h->swapchain.handle, 0);
		break;
	default:
		assert(false && "unreachable");
		break;
	}
}

void
renoir_gl450_context_window_free(Renoir_GL450_Context* self, Renoir_Handle* h)
{
	// do nothing
}

void
renoir_gl450_context_window_bind(Renoir_GL450_Context* self, Renoir_Handle* h)
{
	if (self == nullptr) return;

	bool result = glXMakeCurrent(self->display, (GLXDrawable)h->swapchain.handle, self->context);
	assert(result && "glXMakeCurrent");
}

void
renoir_gl450_context_bind(Renoir_GL450_Context* self)
{
	if (self == nullptr) return;

	bool result = glXMakeCurrent(self->display, self->dummy_window, self->context);
	assert(result && "glXMakeCurrent failed");
}

void
renoir_gl450_context_unbind(Renoir_GL450_Context* self)
{
	if (self == nullptr) return;

	bool result = glXMakeCurrent(self->display, None, NULL);
	assert(result && "glXMakeCurrent failed");
}

void
renoir_gl450_context_window_present(Renoir_GL450_Context* self, Renoir_Handle* h)
{
	if (self == nullptr) return;

	glXSwapBuffers(self->display, (Window)h->swapchain.handle);
}

void
renoir_gl450_context_reload(Renoir_GL450_Context* self)
{
	if (self == nullptr) return;

	bool result = glXMakeCurrent(self->display, None, self->context);
	assert(result && "glXMakeCurrent failed");
	GLenum glew_result = glewInit();
	assert(glew_result == GLEW_OK && "glewInit failed");
	(void)glew_result;
}
