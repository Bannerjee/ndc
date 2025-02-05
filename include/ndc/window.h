#pragma once

#include <stdint.h>
typedef enum
{
	NDC_PRESS,
	NDC_RELEASE,
} ndc_input_action_t;

typedef enum {

	NDC_KEY_UNKNOWN = -1,

    // mouse
    NDC_KEY_LMB,
    NDC_KEY_RMB,
    NDC_KEY_MMB,

    // keyboard
    NDC_KEY_ESCAPE,
    NDC_KEY_1,
    NDC_KEY_2,
    NDC_KEY_3,
    NDC_KEY_4,
    NDC_KEY_5,
    NDC_KEY_6,
    NDC_KEY_7,
    NDC_KEY_8,
    NDC_KEY_9,
    NDC_KEY_0,
    NDC_KEY_MINUS,
    NDC_KEY_EQUALS,
    NDC_KEY_BACKSPACE,
    NDC_KEY_TAB,
    NDC_KEY_Q,
    NDC_KEY_W,
    NDC_KEY_E,
    NDC_KEY_R,
    NDC_KEY_T,
    NDC_KEY_Y,
    NDC_KEY_U,
    NDC_KEY_I,
    NDC_KEY_O,
    NDC_KEY_P,
    NDC_KEY_L_BRACKET,
    NDC_KEY_R_BRACKET,
    NDC_KEY_ENTER,
    NDC_KEY_L_CONTROL,
    NDC_KEY_A,
    NDC_KEY_S,
    NDC_KEY_D,
    NDC_KEY_F,
    NDC_KEY_G,
    NDC_KEY_H,
    NDC_KEY_J,
    NDC_KEY_K,
    NDC_KEY_L,
    NDC_KEY_SEMICOLON,
    NDC_KEY_APOSTROPHE,
    NDC_KEY_GRAVE,
    NDC_KEY_L_SHIFT,
    NDC_KEY_BACKSLASH,
    NDC_KEY_Z,
    NDC_KEY_X,
    NDC_KEY_C,
    NDC_KEY_V,
    NDC_KEY_B,
    NDC_KEY_N,
    NDC_KEY_M,
    NDC_KEY_COMMA,
    NDC_KEY_PERIOD,
    NDC_KEY_SLASH,
    NDC_KEY_R_SHIFT,
    NDC_KEY_NUMPAD_MULTIPLY,
    NDC_KEY_L_ALT,
    NDC_KEY_SPACE,
    NDC_KEY_CAPSLOCK,
    NDC_KEY_F1,
    NDC_KEY_F2,
    NDC_KEY_F3,
    NDC_KEY_F4,
    NDC_KEY_F5,
    NDC_KEY_F6,
    NDC_KEY_F7,
    NDC_KEY_F8,
    NDC_KEY_F9,
    NDC_KEY_F10,
    NDC_KEY_NUMLOCK,
    NDC_KEY_SCROLLLOCK,
    NDC_KEY_NUMPAD_7,
    NDC_KEY_NUMPAD_8,
    NDC_KEY_NUMPAD_9,
    NDC_KEY_NUMPAD_SUBTRACT,
    NDC_KEY_NUMPAD_4,
    NDC_KEY_NUMPAD_5,
    NDC_KEY_NUMPAD_6,
    NDC_KEY_NUMPAD_ADD,
    NDC_KEY_NUMPAD_1,
    NDC_KEY_NUMPAD_2,
    NDC_KEY_NUMPAD_3,
    NDC_KEY_NUMPAD_0,
    NDC_KEY_NUMPAD_PERIOD,
    NDC_KEY_ALT_PRINTSCREEN, 
    NDC_KEY_BRACKETANGLE, 
    NDC_KEY_F11,
    NDC_KEY_F12,

    NDC_KEY_DELETE,

    NDC_KEY_NUMPAD_DECIMAL,
    NDC_KEY_NUMPAD_EQUAL,
    NDC_KEY_NUMPAD_ENTER,
    NDC_KEY_NUMPAD_DIVIDE,
    NDC_KEY_PRINTSCREEN,
    NDC_KEY_ALTRIGHT,
    NDC_KEY_CANCEL,
    NDC_KEY_UP,
    NDC_KEY_PAGEUP,
    NDC_KEY_LEFT,
    NDC_KEY_RIGHT,
    NDC_KEY_DOWN,
    NDC_KEY_PAGEDOWN,
    NDC_KEY_R_ALT,
    NDC_KEY_R_CONTROL,
    NDC_KEY_INSERT,


    NDC_KEY_MAX

} ndc_input_key_t;

typedef struct ndc_window ndc_window;
typedef void(*NDC_KEY_CALLBACK)(ndc_window* win,ndc_input_key_t key,uint32_t count,uint32_t scancode,ndc_input_action_t action);
typedef void(*NDC_SIZE_CALLBACK)(ndc_window* win,uint32_t new_size,uint32_t new_height);
typedef void(*NDC_MOUSE_MOVE_CALLBACK)(ndc_window* win,uint32_t x,uint32_t y);
typedef void(*NDC_MOUSE_CLICK_CALLBACK)(ndc_window* win,uint32_t x,uint32_t y,ndc_input_key_t key,ndc_input_action_t act);

struct ndc_window
{
	char alive;

	short int key_codes[512];

	NDC_KEY_CALLBACK key_callback;
	NDC_SIZE_CALLBACK size_callback;
    NDC_MOUSE_MOVE_CALLBACK mouse_move_callback;
    NDC_MOUSE_CLICK_CALLBACK mouse_click_callback;
#ifdef _WIN32

	void* hdc;
	void* hwnd;
	void* hglrc;
	void* instance;

	const char* class_name;

#endif

};

ndc_window* ndc_create_window(const char* name,int width,int height);
void ndc_destroy_window(ndc_window* win);

void ndc_show_window(ndc_window* win);
void ndc_hide_window(ndc_window* win);

//void ndc_create_context(ndc_window* win);
void ndc_swap_buffers(ndc_window* win);
void ndc_poll_events(ndc_window* win);

void ndc_set_key_callback(ndc_window* win,NDC_KEY_CALLBACK callback);
void ndc_set_size_callback(ndc_window* win,NDC_SIZE_CALLBACK callback);
void ndc_set_mouse_move_callback(ndc_window* win,NDC_MOUSE_MOVE_CALLBACK callback);
void ndc_set_mouse_click_callback(ndc_window* win,NDC_MOUSE_CLICK_CALLBACK callback);