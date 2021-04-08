#pragma once

#include "renoir-window/Exports.h"
#include <stdbool.h>

typedef enum RENOIR_KEY {
	RENOIR_KEY_BACKSPACE,
	RENOIR_KEY_TAB,
	RENOIR_KEY_ENTER,
	RENOIR_KEY_ESC,
	RENOIR_KEY_SPACE,
	RENOIR_KEY_MINUS,
	RENOIR_KEY_PLUS,
	RENOIR_KEY_PERIOD,
	RENOIR_KEY_COMMA,
	RENOIR_KEY_NUM_0,
	RENOIR_KEY_NUM_1,
	RENOIR_KEY_NUM_2,
	RENOIR_KEY_NUM_3,
	RENOIR_KEY_NUM_4,
	RENOIR_KEY_NUM_5,
	RENOIR_KEY_NUM_6,
	RENOIR_KEY_NUM_7,
	RENOIR_KEY_NUM_8,
	RENOIR_KEY_NUM_9,
	RENOIR_KEY_COLON,
	RENOIR_KEY_SEMICOLON,
	RENOIR_KEY_EQUAL,
	RENOIR_KEY_FORWARD_SLASH,
	RENOIR_KEY_LEFT_BRACKET,
	RENOIR_KEY_RIGHT_BRACKET,
	RENOIR_KEY_BACKSLASH,
	RENOIR_KEY_BACKQUOTE,
	RENOIR_KEY_A,
	RENOIR_KEY_B,
	RENOIR_KEY_C,
	RENOIR_KEY_D,
	RENOIR_KEY_E,
	RENOIR_KEY_F,
	RENOIR_KEY_G,
	RENOIR_KEY_H,
	RENOIR_KEY_I,
	RENOIR_KEY_J,
	RENOIR_KEY_K,
	RENOIR_KEY_L,
	RENOIR_KEY_M,
	RENOIR_KEY_N,
	RENOIR_KEY_O,
	RENOIR_KEY_P,
	RENOIR_KEY_Q,
	RENOIR_KEY_R,
	RENOIR_KEY_S,
	RENOIR_KEY_T,
	RENOIR_KEY_U,
	RENOIR_KEY_V,
	RENOIR_KEY_W,
	RENOIR_KEY_X,
	RENOIR_KEY_Y,
	RENOIR_KEY_Z,
	RENOIR_KEY_DELETE,
	RENOIR_KEY_NUMPAD_0,
	RENOIR_KEY_NUMPAD_1,
	RENOIR_KEY_NUMPAD_2,
	RENOIR_KEY_NUMPAD_3,
	RENOIR_KEY_NUMPAD_4,
	RENOIR_KEY_NUMPAD_5,
	RENOIR_KEY_NUMPAD_6,
	RENOIR_KEY_NUMPAD_7,
	RENOIR_KEY_NUMPAD_8,
	RENOIR_KEY_NUMPAD_9,
	RENOIR_KEY_NUMPAD_PERIOD,
	RENOIR_KEY_NUMPAD_DIVIDE,
	RENOIR_KEY_NUMPAD_PLUS,
	RENOIR_KEY_NUMPAD_MINUS,
	RENOIR_KEY_NUMPAD_ENTER,
	RENOIR_KEY_NUMPAD_EQUALS,
	RENOIR_KEY_NUMPAD_MULTIPLY,
	RENOIR_KEY_UP,
	RENOIR_KEY_DOWN,
	RENOIR_KEY_RIGHT,
	RENOIR_KEY_LEFT,
	RENOIR_KEY_INSERT,
	RENOIR_KEY_HOME,
	RENOIR_KEY_END,
	RENOIR_KEY_PAGEUP,
	RENOIR_KEY_PAGEDOWN,
	RENOIR_KEY_F1,
	RENOIR_KEY_F2,
	RENOIR_KEY_F3,
	RENOIR_KEY_F4,
	RENOIR_KEY_F5,
	RENOIR_KEY_F6,
	RENOIR_KEY_F7,
	RENOIR_KEY_F8,
	RENOIR_KEY_F9,
	RENOIR_KEY_F10,
	RENOIR_KEY_F11,
	RENOIR_KEY_F12,
	RENOIR_KEY_NUM_LOCK,
	RENOIR_KEY_CAPS_LOCK,
	RENOIR_KEY_SCROLL_LOCK,
	RENOIR_KEY_RIGHT_SHIFT,
	RENOIR_KEY_LEFT_SHIFT,
	RENOIR_KEY_RIGHT_CTRL,
	RENOIR_KEY_LEFT_CTRL,
	RENOIR_KEY_LEFT_ALT,
	RENOIR_KEY_RIGHT_ALT,
	RENOIR_KEY_LEFT_META,
	RENOIR_KEY_RIGHT_META,
	RENOIR_KEY_COUNT
} RENOIR_KEY;

typedef enum RENOIR_MOUSE {
	RENOIR_MOUSE_LEFT,
	RENOIR_MOUSE_MIDDLE,
	RENOIR_MOUSE_RIGHT,
	RENOIR_MOUSE_COUNT
} RENOIR_MOUSE;

typedef enum RENOIR_KEY_STATE {
	RENOIR_KEY_STATE_NONE,
	RENOIR_KEY_STATE_UP,
	RENOIR_KEY_STATE_DOWN,
	RENOIR_KEY_STATE_COUNT
} RENOIR_KEY_STATE;

typedef enum RENOIR_EVENT_KIND {
	RENOIR_EVENT_KIND_NONE,
	RENOIR_EVENT_KIND_KEYBOARD_KEY,
	RENOIR_EVENT_KIND_RUNE,
	RENOIR_EVENT_KIND_MOUSE_BUTTON,
	RENOIR_EVENT_KIND_MOUSE_MOVE,
	RENOIR_EVENT_KIND_MOUSE_WHEEL,
	RENOIR_EVENT_KIND_WINDOW_RESIZE,
	RENOIR_EVENT_KIND_WINDOW_MOVE,
	RENOIR_EVENT_KIND_WINDOW_CLOSE,
	RENOIR_EVENT_KIND_DISPLAY_CHANGE
} RENOIR_EVENT_KIND;

typedef struct Renoir_Event {
	RENOIR_EVENT_KIND kind;
	union {
		struct {
			RENOIR_KEY key;
			RENOIR_KEY_STATE state;
		} keyboard;

		int rune;

		struct {
			RENOIR_MOUSE button;
			RENOIR_KEY_STATE state;
		} mouse;

		struct {
			int x, y;
		} mouse_move;

		float wheel;

		struct {
			int width, height;
		} resize;

		struct {
			int x, y;
		} move;
	};
} Renoir_Event;

typedef enum RENOIR_WINDOW_MSAA_MODE {
	RENOIR_WINDOW_MSAA_MODE_NONE = 0,
	RENOIR_WINDOW_MSAA_MODE_2 = 2,
	RENOIR_WINDOW_MSAA_MODE_4 = 4,
	RENOIR_WINDOW_MSAA_MODE_8 = 8
} RENOIR_WINDOW_MSAA_MODE;

typedef struct Renoir_Window {
	int x, y;
	int width, height;
	const char* title;
	void* userdata;
	bool pass_input_to_underlying_window;
} Renoir_Window;

RENOIR_WINDOW_EXPORT Renoir_Window*
renoir_window_new(int width, int height, const char* title, RENOIR_WINDOW_MSAA_MODE msaa);

RENOIR_WINDOW_EXPORT Renoir_Window*
renoir_window_child_new(Renoir_Window *parent_window, int x, int y, int width, int height);

RENOIR_WINDOW_EXPORT void
renoir_window_free(Renoir_Window* self);

RENOIR_WINDOW_EXPORT Renoir_Event
renoir_window_poll(Renoir_Window* self);

RENOIR_WINDOW_EXPORT void
renoir_window_native_handles(Renoir_Window* self, void** handle, void** display);

RENOIR_WINDOW_EXPORT void
renoir_window_pos_set(Renoir_Window* self, int x, int y);

RENOIR_WINDOW_EXPORT void
renoir_window_pos_get(Renoir_Window* self, int *out_x, int *out_y);

RENOIR_WINDOW_EXPORT void
renoir_window_size_set(Renoir_Window* window, int width, int height);

RENOIR_WINDOW_EXPORT void
renoir_window_size_get(Renoir_Window* window, int *out_width, int *out_height);

RENOIR_WINDOW_EXPORT void
renoir_window_focus_set(Renoir_Window* window);

RENOIR_WINDOW_EXPORT bool
renoir_window_focus_get(Renoir_Window* window);

RENOIR_WINDOW_EXPORT bool
renoir_window_minimized_get(Renoir_Window* window);

RENOIR_WINDOW_EXPORT void
renoir_window_title_set(Renoir_Window* window, const char* title);

RENOIR_WINDOW_EXPORT void *
renoir_window_handle_from_point(int x, int y);

RENOIR_WINDOW_EXPORT bool
renoir_window_is_child(Renoir_Window* window);

RENOIR_WINDOW_EXPORT bool
renoir_window_has_capture(Renoir_Window* window);

RENOIR_WINDOW_EXPORT void
renoir_window_set_capture(Renoir_Window* window);

typedef struct Renoir_Monitor_Info {
	bool primary;
	int main_pos_x, main_pos_y;
	int main_width, main_height;
	int work_pos_x, work_pos_y;
	int work_width, work_height;
} Renoir_Monitor_Info;

typedef struct Renoir_Monitor {
	Renoir_Monitor_Info monitors[4];
	int monitor_count;
} Renoir_Monitor;

RENOIR_WINDOW_EXPORT Renoir_Monitor
renoir_monitor_query();