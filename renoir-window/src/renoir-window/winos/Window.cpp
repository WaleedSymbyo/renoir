#include "renoir-window/Window.h"

#include <mn/Memory.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>
#undef DELETE

#include <assert.h>

typedef struct Renoir_Window_WinOS {
	Renoir_Window window;
	HWND handle;
	HDC hdc;
	Renoir_Event event;
	bool running;
	DWORD style;
	DWORD ex_style;
} Renoir_Window_WinOS;

inline static RENOIR_KEY
_renoir_map_keyboard_key(WPARAM k)
{
	switch (k)
	{
		// letters
	case 'A':
		return RENOIR_KEY_A;
	case 'B':
		return RENOIR_KEY_B;
	case 'C':
		return RENOIR_KEY_C;
	case 'D':
		return RENOIR_KEY_D;
	case 'E':
		return RENOIR_KEY_E;
	case 'F':
		return RENOIR_KEY_F;
	case 'G':
		return RENOIR_KEY_G;
	case 'H':
		return RENOIR_KEY_H;
	case 'I':
		return RENOIR_KEY_I;
	case 'J':
		return RENOIR_KEY_J;
	case 'K':
		return RENOIR_KEY_K;
	case 'L':
		return RENOIR_KEY_L;
	case 'M':
		return RENOIR_KEY_M;
	case 'N':
		return RENOIR_KEY_N;
	case 'O':
		return RENOIR_KEY_O;
	case 'P':
		return RENOIR_KEY_P;
	case 'Q':
		return RENOIR_KEY_Q;
	case 'R':
		return RENOIR_KEY_R;
	case 'S':
		return RENOIR_KEY_S;
	case 'T':
		return RENOIR_KEY_T;
	case 'U':
		return RENOIR_KEY_U;
	case 'V':
		return RENOIR_KEY_V;
	case 'W':
		return RENOIR_KEY_W;
	case 'X':
		return RENOIR_KEY_X;
	case 'Y':
		return RENOIR_KEY_Y;
	case 'Z':
		return RENOIR_KEY_Z;

		// numbers
	case '0':
		return RENOIR_KEY_NUM_0;
	case '1':
		return RENOIR_KEY_NUM_1;
	case '2':
		return RENOIR_KEY_NUM_2;
	case '3':
		return RENOIR_KEY_NUM_3;
	case '4':
		return RENOIR_KEY_NUM_4;
	case '5':
		return RENOIR_KEY_NUM_5;
	case '6':
		return RENOIR_KEY_NUM_6;
	case '7':
		return RENOIR_KEY_NUM_7;
	case '8':
		return RENOIR_KEY_NUM_8;
	case '9':
		return RENOIR_KEY_NUM_9;

		// symbols
	case VK_OEM_2:
		return RENOIR_KEY_FORWARD_SLASH;
	case VK_OEM_5:
		return RENOIR_KEY_BACKSLASH;
	case VK_OEM_4:
		return RENOIR_KEY_LEFT_BRACKET;
	case VK_OEM_6:
		return RENOIR_KEY_RIGHT_BRACKET;
	case VK_OEM_3:
		return RENOIR_KEY_BACKQUOTE;
	case VK_OEM_PERIOD:
		return RENOIR_KEY_PERIOD;
	case VK_OEM_MINUS:
		return RENOIR_KEY_MINUS;
	case VK_OEM_PLUS:
		return RENOIR_KEY_PLUS; // no native key for equal?
	case VK_OEM_COMMA:
		return RENOIR_KEY_COMMA;
	case VK_OEM_1:
		return RENOIR_KEY_SEMICOLON; // no native key for colon? maybe it's the same as ;
	case VK_SPACE:
		return RENOIR_KEY_SPACE;

		// special keys
	case VK_BACK:
		return RENOIR_KEY_BACKSPACE;
	case VK_TAB:
		return RENOIR_KEY_TAB;
	case VK_RETURN:
		return RENOIR_KEY_ENTER;
	case VK_ESCAPE:
		return RENOIR_KEY_ESC;
	case VK_DELETE:
		return RENOIR_KEY_DELETE;
	case VK_UP:
		return RENOIR_KEY_UP;
	case VK_DOWN:
		return RENOIR_KEY_DOWN;
	case VK_RIGHT:
		return RENOIR_KEY_RIGHT;
	case VK_LEFT:
		return RENOIR_KEY_LEFT;
	case VK_INSERT:
		return RENOIR_KEY_INSERT;
	case VK_HOME:
		return RENOIR_KEY_HOME;
	case VK_END:
		return RENOIR_KEY_END;
	case VK_PRIOR:
		return RENOIR_KEY_PAGEUP;
	case VK_NEXT:
		return RENOIR_KEY_PAGEDOWN;

		// modifiers
	case VK_SHIFT:
		return RENOIR_KEY_LEFT_SHIFT;
	case VK_RSHIFT:
		return RENOIR_KEY_RIGHT_SHIFT;
	case VK_LSHIFT:
		return RENOIR_KEY_LEFT_SHIFT;
	case VK_CONTROL:
		return RENOIR_KEY_LEFT_CTRL;
	case VK_RCONTROL:
		return RENOIR_KEY_RIGHT_CTRL;
	case VK_LCONTROL:
		return RENOIR_KEY_LEFT_CTRL;
	case VK_MENU:
		return RENOIR_KEY_LEFT_ALT;

		// function keys
	case VK_F1:
		return RENOIR_KEY_F1;
	case VK_F2:
		return RENOIR_KEY_F2;
	case VK_F3:
		return RENOIR_KEY_F3;
	case VK_F4:
		return RENOIR_KEY_F4;
	case VK_F5:
		return RENOIR_KEY_F5;
	case VK_F6:
		return RENOIR_KEY_F6;
	case VK_F7:
		return RENOIR_KEY_F7;
	case VK_F8:
		return RENOIR_KEY_F8;
	case VK_F9:
		return RENOIR_KEY_F9;
	case VK_F10:
		return RENOIR_KEY_F10;
	case VK_F11:
		return RENOIR_KEY_F11;
	case VK_F12:
		return RENOIR_KEY_F12;
	default:
		return RENOIR_KEY_COUNT;
	}
}

inline static int
_renoir_convert_rune(WPARAM wparam)
{
	int size_needed = WideCharToMultiByte(
		CP_UTF8,
		NULL,
		(LPWSTR)&wparam,
		int(sizeof(wparam) / sizeof(WCHAR)),
		NULL,
		0,
		NULL,
		NULL);
	if (size_needed == 0)
		return 0;

	int r = 0;
	assert(size_needed <= sizeof(r));

	size_needed = WideCharToMultiByte(
		CP_UTF8,
		NULL,
		(LPWSTR)&wparam,
		int(sizeof(wparam) / sizeof(WCHAR)),
		(LPSTR)&r,
		int(sizeof(r)),
		NULL,
		NULL);
	return r;
}

LRESULT CALLBACK
_renoir_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch(msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		if (self)
		{
			self->running = false;
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_WINDOW_CLOSE;
		}
		return 0;
	case WM_KEYDOWN:
		if (self)
		{
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_KEYBOARD_KEY;
			self->event.keyboard.key = _renoir_map_keyboard_key(wparam);
			self->event.keyboard.state = RENOIR_KEY_STATE_DOWN;
		}
		break;
	case WM_KEYUP:
		if (self)
		{
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_KEYBOARD_KEY;
			self->event.keyboard.key = _renoir_map_keyboard_key(wparam);
			self->event.keyboard.state = RENOIR_KEY_STATE_UP;
		}
		break;
	case WM_CHAR:
		if (self)
		{
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_RUNE;
			self->event.rune = _renoir_convert_rune(wparam);
		}
		break;
	case WM_LBUTTONDOWN:
		if (self)
		{
			SetCapture(self->handle);
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_MOUSE_BUTTON;
			self->event.mouse.button = RENOIR_MOUSE_LEFT;
			self->event.mouse.state = RENOIR_KEY_STATE_DOWN;
		}
		break;
	case WM_LBUTTONUP:
		if (self)
		{
			ReleaseCapture();
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_MOUSE_BUTTON;
			self->event.mouse.button = RENOIR_MOUSE_LEFT;
			self->event.mouse.state = RENOIR_KEY_STATE_UP;
		}
		break;
	case WM_MBUTTONDOWN:
		if (self)
		{
			SetCapture(self->handle);
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_MOUSE_BUTTON;
			self->event.mouse.button = RENOIR_MOUSE_MIDDLE;
			self->event.mouse.state = RENOIR_KEY_STATE_DOWN;
		}
		break;
	case WM_MBUTTONUP:
		if (self)
		{
			ReleaseCapture();
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_MOUSE_BUTTON;
			self->event.mouse.button = RENOIR_MOUSE_MIDDLE;
			self->event.mouse.state = RENOIR_KEY_STATE_UP;
		}
		break;
	case WM_RBUTTONDOWN:
		if (self)
		{
			SetCapture(self->handle);
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_MOUSE_BUTTON;
			self->event.mouse.button = RENOIR_MOUSE_RIGHT;
			self->event.mouse.state = RENOIR_KEY_STATE_DOWN;
		}
		break;
	case WM_RBUTTONUP:
		if (self)
		{
			ReleaseCapture();
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_MOUSE_BUTTON;
			self->event.mouse.button = RENOIR_MOUSE_RIGHT;
			self->event.mouse.state = RENOIR_KEY_STATE_UP;
		}
		break;
	case WM_MOUSEMOVE:
		if (self)
		{
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_MOUSE_MOVE;
			self->event.mouse_move.x = GET_X_LPARAM(lparam);
			self->event.mouse_move.y = GET_Y_LPARAM(lparam);
		}
		break;
	case WM_MOUSEWHEEL:
		if (self)
		{
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_MOUSE_WHEEL;
			self->event.wheel = (signed short)HIWORD(wparam);
		}
		break;
	case WM_SIZE:
		if (self)
		{
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_WINDOW_RESIZE;
			self->event.resize.width = LOWORD(lparam);
			self->event.resize.height = HIWORD(lparam);
			self->window.width = LOWORD(lparam);
			self->window.height = HIWORD(lparam);
		}
		break;
	case WM_MOVE:
		if (self)
		{
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_WINDOW_MOVE;
			self->event.move.x = LOWORD(lparam);
			self->event.move.y = HIWORD(lparam);
			self->window.x = LOWORD(lparam);
			self->window.y = HIWORD(lparam);
		}
		break;
	case WM_NCHITTEST:
		if (self)
		{
			memset(&self->event, 0, sizeof(self->event));
			if (self->window.pass_input_to_underlying_window)
				return HTTRANSPARENT;
		}
	break;
	case WM_DISPLAYCHANGE:
		if (self)
		{
			memset(&self->event, 0, sizeof(self->event));
			self->event.kind = RENOIR_EVENT_KIND_DISPLAY_CHANGE;
		}
	default:
		break;
	}

	return DefWindowProcA(hwnd, msg, wparam, lparam);
}

// API
Renoir_Window*
renoir_window_new(int width, int height, const char* title, RENOIR_WINDOW_MSAA_MODE)
{
	assert(width > 0 && height > 0);

	auto self = mn::alloc_zerod<Renoir_Window_WinOS>();

	self->window.x = 100;
	self->window.y = 100;
	self->window.width = width;
	self->window.height = height;
	self->window.title = title;
	self->running = true;

	WNDCLASSEXA wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = _renoir_window_proc;
	wc.hInstance = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "RenoirWindowClass";
	RegisterClassExA(&wc);

	RECT wr = {100, 100, LONG(100 + self->window.width), LONG(100 + self->window.height)};
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	self->handle = CreateWindowExA(
		NULL,
		"RenoirWindowClass",
		self->window.title,
		WS_OVERLAPPEDWINDOW,
		wr.left,
		wr.top,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		NULL,
		NULL);
	if (self->handle == INVALID_HANDLE_VALUE)
	{
		free(self);
		return nullptr;
	}

	self->hdc = GetDC(self->handle);
	ShowWindow(self->handle, SW_SHOW);
	SetForegroundWindow(self->handle);
	SetFocus(self->handle);
	SetWindowLongPtrA(self->handle, GWLP_USERDATA, (LONG_PTR)self);

	return &self->window;
}

Renoir_Window*
renoir_window_child_new(Renoir_Window* parent_window, int x, int y, int width, int height)
{
	assert(width > 0 && height > 0);

	auto self = mn::alloc_zerod<Renoir_Window_WinOS>();

	self->window.x = x;
	self->window.y = y;
	self->window.width = width;
	self->window.height = height;
	self->running = true;
	self->style = WS_POPUP;
	self->ex_style = WS_EX_APPWINDOW;

	RECT wr = {x, y, LONG(x + self->window.width), LONG(y + self->window.height)};
	AdjustWindowRectEx(&wr, self->style, FALSE, self->ex_style);

	void* parent_handle = nullptr;
	renoir_window_native_handles(parent_window, &parent_handle, nullptr);

	self->handle = CreateWindowExA(
		self->ex_style,
		"RenoirWindowClass",
		"Untitled",
		self->style,
		wr.left,
		wr.top,
		wr.right - wr.left,
		wr.bottom - wr.top,
		(HWND)parent_handle,
		NULL,
		NULL,
		NULL);
	if (self->handle == INVALID_HANDLE_VALUE)
	{
		free(self);
		return nullptr;
	}

	self->hdc = GetDC(self->handle);
	ShowWindow(self->handle, SW_SHOW);
	SetForegroundWindow(self->handle);
	SetFocus(self->handle);
	SetWindowLongPtrA(self->handle, GWLP_USERDATA, (LONG_PTR)self);

	return &self->window;
}

void
renoir_window_free(Renoir_Window* window)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;
	BOOL result = ReleaseDC(self->handle, self->hdc);
	assert(result && "ReleaseDC Failed");
	result = DestroyWindow(self->handle);
	assert(result && "DestroyWindow Failed");
	mn::free(self);
}

Renoir_Event
renoir_window_poll(Renoir_Window* window)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;
	memset(&self->event, 0, sizeof(self->event));
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	if (PeekMessageA(&msg, self->handle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	return self->event;
}

void
renoir_window_native_handles(Renoir_Window* window, void** handle, void** display)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;
	if (handle) *handle = self->handle;
	if (display) *display = nullptr;
}

void
renoir_window_pos_set(Renoir_Window* window, int x, int y)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;

	RECT rect = { x, y, x, y };
	::AdjustWindowRectEx(&rect, self->style, FALSE, self->ex_style); // Client to Screen
	::SetWindowPos(
		self->handle,
		nullptr,
		rect.left,
		rect.top,
		0,
		0,
		SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void
renoir_window_pos_get(Renoir_Window* window, int *out_x, int *out_y)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;

	POINT pos = {};
	::ClientToScreen(self->handle, &pos);
	*out_x = pos.x;
	*out_y = pos.y;
}

void
renoir_window_size_set(Renoir_Window* window, int width, int height)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;

	RECT rect = { 0, 0, width, height };
	::AdjustWindowRectEx(&rect, self->style, FALSE, self->ex_style); // Client to Screen
	::SetWindowPos(
		self->handle,
		nullptr,
		0,
		0,
		rect.right - rect.left,
		rect.bottom - rect.top,
		SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void
renoir_window_size_get(Renoir_Window* window, int *out_width, int *out_height)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;

	RECT rect;
	::GetClientRect(self->handle, &rect);
	*out_width = rect.right - rect.left;
	*out_height = rect.bottom - rect.top;
}

void
renoir_window_focus_set(Renoir_Window* window)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;
	::BringWindowToTop(self->handle);
	::SetForegroundWindow(self->handle);
	::SetFocus(self->handle);
}

bool
renoir_window_focus_get(Renoir_Window* window)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;
	return ::GetForegroundWindow() == self->handle;
}

bool
renoir_window_minimized_get(Renoir_Window* window)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;
	return ::IsIconic(self->handle);
}

void
renoir_window_title_set(Renoir_Window* window, const char* title)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;
	::SetWindowTextA(self->handle, title);
}

void *
renoir_window_handle_from_point(int x, int y)
{
	return ::WindowFromPoint(POINT{x, y});
}

bool
renoir_window_is_child(Renoir_Window* window)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;
	auto parent = ::GetParent(self->handle);
	return (parent && parent != self->handle);
}

bool
renoir_window_has_capture(Renoir_Window* window)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;
	return ::GetCapture() == self->handle;
}

void
renoir_window_set_capture(Renoir_Window* window)
{
	Renoir_Window_WinOS* self = (Renoir_Window_WinOS*)window;
	::ReleaseCapture();
	::SetCapture(self->handle);
}

static BOOL CALLBACK
_renoir_monitor_enum_proc(HMONITOR monitor, HDC, LPRECT, LPARAM data)
{
	MONITORINFO info = {};
	info.cbSize = sizeof(MONITORINFO);
	if (::GetMonitorInfo(monitor, &info) == false)
		return true;

	Renoir_Monitor_Info monitor_info = {};
	monitor_info.primary = info.dwFlags & MONITORINFOF_PRIMARY;
	monitor_info.main_pos_x = info.rcMonitor.left;
	monitor_info.main_pos_y = info.rcMonitor.top;
	monitor_info.main_width = info.rcMonitor.right - info.rcMonitor.left;
	monitor_info.main_height = info.rcMonitor.bottom - info.rcMonitor.top;
	monitor_info.work_pos_x = info.rcWork.left;
	monitor_info.work_pos_y = info.rcWork.top;
	monitor_info.work_width = info.rcWork.right - info.rcWork.left;
	monitor_info.work_height = info.rcWork.bottom - info.rcWork.top;

	// we support querying 4 monitors only
	Renoir_Monitor *result = (Renoir_Monitor*)data;
	result->monitors[result->monitor_count++] = monitor_info;
	if (result->monitor_count == 4)
		return false;
	return true;
}

Renoir_Monitor
renoir_monitor_query()
{
	Renoir_Monitor res = {};
	::EnumDisplayMonitors(NULL, NULL, _renoir_monitor_enum_proc, (LPARAM)&res);
	return res;
}