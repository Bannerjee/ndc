#include <ndc/allocator.h>
#include <ndc/window.h>
#include <ndc/error.h>
#include <ndc/gl.h>

#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>


typedef BOOL(WINAPI *PFNWGLDELETECONTEXTPROC)(HGLRC);
typedef PROC(WINAPI *PFNWGLGETPROCADDRESSPROC)(LPCSTR);

typedef BOOL(WINAPI *PFNWGLMAKECURRENTPROC)(HDC, HGLRC);
typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext,const int *attribList);
typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

typedef BOOL(WINAPI* PFNSWAPBUFFERSPROC)(HDC);

wglChoosePixelFormatARB_type* wglChoosePixelFormatARB;
wglCreateContextAttribsARB_type* wglCreateContextAttribsARB;

PFNSWAPBUFFERSPROC wgl_swap_buffers;
PFNWGLMAKECURRENTPROC wgl_make_current;
PFNWGLDELETECONTEXTPROC wgl_delete_context;
PFNWGLGETPROCADDRESSPROC wgl_get_proc_address;


typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext,const int *attribList);
static LRESULT CALLBACK window_callback(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;

    switch (msg) 
    {
        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
        	ndc_window* win = (ndc_window*)GetWindowLongPtr(window, GWLP_USERDATA);
        	if(win->key_callback)
        	{
                int scancode = (lparam >> 16) & 0xFF;
                if(scancode > NDC_KEY_UNKNOWN && scancode < NDC_KEY_MAX)
                {
                    win->key_callback(win,win->key_codes[scancode],lparam & 0xFFFF,scancode,msg == WM_KEYUP ? NDC_RELEASE : NDC_PRESS);
                }
        		
        	}
        	break;
        }
        case WM_MOUSEMOVE:
        { 
            ndc_window* win = (ndc_window*)GetWindowLongPtr(window, GWLP_USERDATA);
            if(win->mouse_move_callback)
            {
                win->mouse_move_callback(win,LOWORD(lparam),HIWORD(lparam));
            }
            break;
        }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            ndc_window* win = (ndc_window*)GetWindowLongPtr(window, GWLP_USERDATA);
            if(!win->mouse_click_callback) break;

            ndc_input_key_t key = 0;
            ndc_input_action_t action = 0;
            if(msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) key = NDC_KEY_LMB;
            if(msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP) key = NDC_KEY_RMB;
            if(msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP) key = NDC_KEY_MMB;

            if(msg == WM_MBUTTONUP || msg == WM_RBUTTONUP || msg == WM_LBUTTONUP) 
            {
                action = NDC_RELEASE;
            }
            else
            {
                action = NDC_PRESS;
            }
            
            win->mouse_click_callback(win,LOWORD(lparam),HIWORD(lparam),key,action);
            break;
        }
        /*case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
            ndc_key_
            if(msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
            break;
        }*/
       
        /*
            if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
                button = GLFW_MOUSE_BUTTON_LEFT;
            else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
                button = GLFW_MOUSE_BUTTON_RIGHT;
            else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
                button = GLFW_MOUSE_BUTTON_MIDDLE;
            else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
                button = GLFW_MOUSE_BUTTON_4;
            else
                button = GLFW_MOUSE_BUTTON_5;*/
        case WM_SIZE:
        {
            ndc_window* win = (ndc_window*)GetWindowLongPtr(window, GWLP_USERDATA);
            if(win->size_callback)
            {
                win->size_callback(win,lparam & 0xffff,(lparam >> 16) & 0xffff);
            }
            break;
        }
        default:
            result = DefWindowProcA(window, msg, wparam, lparam);
            break;
    }

    return result;
}

void ndc_show_window(ndc_window* win)
{
	if(!win) return;
	ShowWindow(win->hwnd, SW_NORMAL);
}
void ndc_hide_window(ndc_window* win)
{
	if(!win) return;
	ShowWindow(win->hwnd, SW_HIDE);
}
void ndc_swap_buffers(ndc_window* win)
{
	if(!win) return;
	wgl_swap_buffers(win->hdc);
}
void ndc_destroy_window(ndc_window* win)
{
	if(!win) return;

	if(win->hdc)
	{
		wgl_make_current(win->hdc, 0);
    	wgl_delete_context(win->hglrc);
    	ReleaseDC(win->hwnd, win->hdc);	
	}
	if(!DestroyWindow(win->hwnd))
	{
		NDC_CRITICAL("failed to destroy window");
	}
	if(!UnregisterClassA(win->class_name,win->instance))
	{
		NDC_CRITICAL("failed to unregister window class");
	}
    ndc_free(win);
    win = NULL;
}

static void* get_gl_proc_addr(const char *name)
{
	// modern gl functions
	void *p = (void *)wgl_get_proc_address(name);
	if(p == 0 ||
	(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
	(p == (void*)-1) )
	{
		// 1.1 funcs
		HMODULE module = LoadLibraryA("opengl32.dll"); 
		p = (void *)GetProcAddress(module, name);
		FreeLibrary(module);
	}
	if(!p) 
	{
		char buffer[100] = "failed to get function address ";
		strcat(buffer,name);
        NDC_WARNING(buffer);
	}

	return p;
}
static ndc_error_t ndc_create_context(ndc_window* win)
{
	HMODULE opengl32 = LoadLibrary("opengl32.dll");
    HMODULE gdi32 = LoadLibrary("GDI32.dll");
    if (opengl32 == NULL || gdi32 == NULL) 
    {
    	NDC_CRITICAL("failed to load opengl32.dll or gdi32.dll\n");
    	return NDC_CONTEXT_CREATION_FAILURE;
    }

    // need to be global
    wgl_make_current = (PFNWGLMAKECURRENTPROC)GetProcAddress(opengl32, "wglMakeCurrent");
    wgl_delete_context = (PFNWGLDELETECONTEXTPROC)GetProcAddress(opengl32, "wglDeleteContext");
	wgl_get_proc_address = (PFNWGLGETPROCADDRESSPROC)GetProcAddress(opengl32, "wglGetProcAddress");
    wgl_swap_buffers = (PFNSWAPBUFFERSPROC)GetProcAddress(gdi32, "SwapBuffers");

    typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTPROC)(HDC);
    typedef int(WINAPI* PFNCHOOSEPIXELFORMATPROC)(HDC,const PIXELFORMATDESCRIPTOR *);
    typedef int(WINAPI* PFNDESCRIBEPIXELFORMATPROC)(HDC,int,UINT,LPPIXELFORMATDESCRIPTOR);
    typedef BOOL(WINAPI* PFNSETPIXELFORMATPROC)(HDC,int,const PIXELFORMATDESCRIPTOR *);

    PFNWGLCREATECONTEXTPROC wgl_create_context = (PFNWGLCREATECONTEXTPROC)GetProcAddress(opengl32, "wglCreateContext");
    PFNCHOOSEPIXELFORMATPROC choose_pixel_format = (PFNCHOOSEPIXELFORMATPROC)GetProcAddress(gdi32, "ChoosePixelFormat");
    PFNDESCRIBEPIXELFORMATPROC describe_pixel_format = (PFNDESCRIBEPIXELFORMATPROC)GetProcAddress(gdi32, "DescribePixelFormat");
    PFNSETPIXELFORMATPROC set_pixel_format =  (PFNSETPIXELFORMATPROC)GetProcAddress(gdi32, "SetPixelFormat");
    

    if( !wgl_make_current ||
        !wgl_delete_context || 
        !wgl_get_proc_address ||
        !wgl_swap_buffers ||
        !wgl_create_context || 
        !choose_pixel_format ||
        !describe_pixel_format ||
        !set_pixel_format)
    {
        NDC_CRITICAL("failed to retrieve functions required to setup gl context");
        return NDC_CONTEXT_CREATION_FAILURE;
    }
    FreeLibrary(opengl32);
    FreeLibrary(gdi32);
	win->hdc = GetDC(win->hwnd);
	{
    WNDCLASSA window_class = 
    {
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = DefWindowProcA,
        .hInstance = GetModuleHandle(0),
        .lpszClassName = "DUMMY_WGL_WINDOW",
    };

    if (!RegisterClassA(&window_class)) 
    {
    	NDC_CRITICAL("failed to register dummy gl class");
    	return NDC_CONTEXT_CREATION_FAILURE;
    }

    HWND dummy_window = CreateWindowExA(
        0,
        window_class.lpszClassName,
        "Dummy OpenGL Window",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        window_class.hInstance,
        0);

    if (!dummy_window) 
    {
    	NDC_CRITICAL("failed to register dummy gl window");
    	return NDC_CONTEXT_CREATION_FAILURE;
    }

    HDC dummy_dc = GetDC(dummy_window);

    PIXELFORMATDESCRIPTOR pfd = 
    {
        .nSize = sizeof(pfd),
        .nVersion = 1,
        .iPixelType = PFD_TYPE_RGBA,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .cColorBits = 32,
        .cAlphaBits = 8,
        .iLayerType = PFD_MAIN_PLANE,
        .cDepthBits = 24,
        .cStencilBits = 8,
    };

    int pixel_format = choose_pixel_format(dummy_dc, &pfd);
    if (!pixel_format) 
    {
    	NDC_CRITICAL("failed to find suitable pixel format");
        return NDC_CONTEXT_CREATION_FAILURE;
    }
    if (!set_pixel_format(dummy_dc, pixel_format, &pfd)) 
    {
    	NDC_CRITICAL("failed to set pixel format");
        return NDC_CONTEXT_CREATION_FAILURE;
    }

    HGLRC dummy_context = wgl_create_context(dummy_dc);
    if (!dummy_context) 
    {
    	NDC_CRITICAL("failed to create dummy context");
        return NDC_CONTEXT_CREATION_FAILURE;
    }
 
    if (!wgl_make_current(dummy_dc, dummy_context)) 
    {
    	NDC_CRITICAL("failed to set dummy gl context as current");
        return NDC_CONTEXT_CREATION_FAILURE;
    }
    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wgl_get_proc_address(
        "wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wgl_get_proc_address(
        "wglChoosePixelFormatARB");
    if(!wglChoosePixelFormatARB || !wglChoosePixelFormatARB)
    {
        NDC_CRITICAL("failed to retrieve functions required to setup gl context");
        return NDC_CONTEXT_CREATION_FAILURE;
    }
    wgl_make_current(dummy_dc, 0);
    wgl_delete_context(dummy_context);
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);
	}

    int pixel_format_attribs[] = 
    {
        0x2001, 1,		// WGL_DRAW_TO_WINDOW_ARB
        0x2010, 1,		//WGL_SUPPORT_OPENGL_ARB
        0x2011, 1,		// WGL_DOUBLE_BUFFER_ARB
        //0x20A9, 1,      //WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB doesn't work(?)
        0x2003, 0x2027,	//WGL_ACCELERATION_ARB | WGL_FULL_ACCELERATION_ARB 
        0x2013, 0x202B,	//WGL_PIXEL_TYPE_ARB | WGL_TYPE_RGBA_ARB                         
        0x2014, 32, 	// WGL_COLOR_BITS_ARB
        0x2022, 24,		// WGL_DEPTH_BITS_ARB
        0x2023, 8,		// WGL_STENCIL_BITS_ARB
        0
    };

    int pixel_format;
    UINT num_formats;
    wglChoosePixelFormatARB(win->hdc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats) 
    {
    	NDC_CRITICAL("failed to set gl pixel format");
        return NDC_CONTEXT_CREATION_FAILURE;
    }

    PIXELFORMATDESCRIPTOR pfd;
    describe_pixel_format(win->hdc, pixel_format, sizeof(pfd), &pfd);
    if (!set_pixel_format(win->hdc, pixel_format, &pfd)) 
    {
    	NDC_CRITICAL("failed to set gl pixel format");
        return NDC_CONTEXT_CREATION_FAILURE;
    }

    int attribs[] = {
        0x2091, 4,
        0x2092, 6,
        0x9126, 0x00000001,
        0,
    };

    win->hglrc = wglCreateContextAttribsARB(win->hdc, 0, attribs);
    if (!win->hglrc) 
    {
        NDC_CRITICAL("failed to create gl context");
        return NDC_CONTEXT_CREATION_FAILURE;
    }

    if (!wgl_make_current(win->hdc, win->hglrc)) 
    {
        NDC_CRITICAL("failed to activate gl context");
        return NDC_CONTEXT_CREATION_FAILURE;
    }
    ndc_load_gl_functions(get_gl_proc_addr);
    return NDC_SUCCESS;
}
ndc_window* ndc_create_window(const char* name,int width,int height)
{
    ndc_window* win = ndc_malloc(sizeof(ndc_window));
    win->hdc = NULL;
    win->hglrc = NULL;
    win->key_callback = NULL;
    win->size_callback = NULL;
    win->mouse_move_callback = NULL;

    memset(win->key_codes,NDC_KEY_UNKNOWN,sizeof(win->key_codes));
    win->key_codes[0x00B] = NDC_KEY_0;
    win->key_codes[0x002] = NDC_KEY_1;
    win->key_codes[0x003] = NDC_KEY_2;
    win->key_codes[0x004] = NDC_KEY_3;
    win->key_codes[0x005] = NDC_KEY_4;
    win->key_codes[0x006] = NDC_KEY_5;
    win->key_codes[0x007] = NDC_KEY_6;
    win->key_codes[0x008] = NDC_KEY_7;
    win->key_codes[0x009] = NDC_KEY_8;
    win->key_codes[0x00A] = NDC_KEY_9;
    win->key_codes[0x01E] = NDC_KEY_A;
    win->key_codes[0x030] = NDC_KEY_B;
    win->key_codes[0x02E] = NDC_KEY_C;
    win->key_codes[0x020] = NDC_KEY_D;
    win->key_codes[0x012] = NDC_KEY_E;
    win->key_codes[0x021] = NDC_KEY_F;
    win->key_codes[0x022] = NDC_KEY_G;
    win->key_codes[0x023] = NDC_KEY_H;
    win->key_codes[0x017] = NDC_KEY_I;
    win->key_codes[0x024] = NDC_KEY_J;
    win->key_codes[0x025] = NDC_KEY_K;
    win->key_codes[0x026] = NDC_KEY_L;
    win->key_codes[0x032] = NDC_KEY_M;
    win->key_codes[0x031] = NDC_KEY_N;
    win->key_codes[0x018] = NDC_KEY_O;
    win->key_codes[0x019] = NDC_KEY_P;
    win->key_codes[0x010] = NDC_KEY_Q;
    win->key_codes[0x013] = NDC_KEY_R;
    win->key_codes[0x01F] = NDC_KEY_S;
    win->key_codes[0x014] = NDC_KEY_T;
    win->key_codes[0x016] = NDC_KEY_U;
    win->key_codes[0x02F] = NDC_KEY_V;
    win->key_codes[0x011] = NDC_KEY_W;
    win->key_codes[0x02D] = NDC_KEY_X;
    win->key_codes[0x015] = NDC_KEY_Y;
    win->key_codes[0x02C] = NDC_KEY_Z;

    win->key_codes[0x028] = NDC_KEY_APOSTROPHE;
    win->key_codes[0x02B] = NDC_KEY_BACKSLASH;
    win->key_codes[0x033] = NDC_KEY_COMMA;
    win->key_codes[0x00D] = NDC_KEY_EQUALS;
    win->key_codes[0x029] = NDC_KEY_GRAVE;
    win->key_codes[0x01A] = NDC_KEY_L_BRACKET;
    win->key_codes[0x00C] = NDC_KEY_MINUS;
    win->key_codes[0x034] = NDC_KEY_PERIOD;
    win->key_codes[0x01B] = NDC_KEY_R_BRACKET;
    win->key_codes[0x027] = NDC_KEY_SEMICOLON;
    win->key_codes[0x035] = NDC_KEY_SLASH;

    win->key_codes[0x00E] = NDC_KEY_BACKSPACE;
    win->key_codes[0x153] = NDC_KEY_DELETE;
    win->key_codes[0x01C] = NDC_KEY_ENTER;
    win->key_codes[0x001] = NDC_KEY_ESCAPE;
    win->key_codes[0x152] = NDC_KEY_INSERT;
    
    win->key_codes[0x151] = NDC_KEY_PAGEDOWN;
    win->key_codes[0x149] = NDC_KEY_PAGEUP;
    win->key_codes[0x039] = NDC_KEY_SPACE;
    win->key_codes[0x00F] = NDC_KEY_TAB;
    win->key_codes[0x03A] = NDC_KEY_CAPSLOCK;
    win->key_codes[0x145] = NDC_KEY_NUMLOCK;
    win->key_codes[0x046] = NDC_KEY_SCROLLLOCK;
    win->key_codes[0x03B] = NDC_KEY_F1;
    win->key_codes[0x03C] = NDC_KEY_F2;
    win->key_codes[0x03D] = NDC_KEY_F3;
    win->key_codes[0x03E] = NDC_KEY_F4;
    win->key_codes[0x03F] = NDC_KEY_F5;
    win->key_codes[0x040] = NDC_KEY_F6;
    win->key_codes[0x041] = NDC_KEY_F7;
    win->key_codes[0x042] = NDC_KEY_F8;
    win->key_codes[0x043] = NDC_KEY_F9;
    win->key_codes[0x044] = NDC_KEY_F10;
    win->key_codes[0x057] = NDC_KEY_F11;
    win->key_codes[0x058] = NDC_KEY_F12;

    win->key_codes[0x038] = NDC_KEY_L_ALT;
    win->key_codes[0x01D] = NDC_KEY_L_CONTROL;
    win->key_codes[0x02A] = NDC_KEY_L_SHIFT;
    win->key_codes[0x137] = NDC_KEY_PRINTSCREEN;
    win->key_codes[0x138] = NDC_KEY_R_ALT;
    win->key_codes[0x11D] = NDC_KEY_R_CONTROL;
    win->key_codes[0x036] = NDC_KEY_R_SHIFT;
    win->key_codes[0x150] = NDC_KEY_DOWN;
    win->key_codes[0x14B] = NDC_KEY_LEFT;
    win->key_codes[0x14D] = NDC_KEY_RIGHT;
    win->key_codes[0x148] = NDC_KEY_UP;

    win->key_codes[0x052] = NDC_KEY_NUMPAD_0;
    win->key_codes[0x04F] = NDC_KEY_NUMPAD_1;
    win->key_codes[0x050] = NDC_KEY_NUMPAD_2;
    win->key_codes[0x051] = NDC_KEY_NUMPAD_3;
    win->key_codes[0x04B] = NDC_KEY_NUMPAD_4;
    win->key_codes[0x04C] = NDC_KEY_NUMPAD_5;
    win->key_codes[0x04D] = NDC_KEY_NUMPAD_6;
    win->key_codes[0x047] = NDC_KEY_NUMPAD_7;
    win->key_codes[0x048] = NDC_KEY_NUMPAD_8;
    win->key_codes[0x049] = NDC_KEY_NUMPAD_9;
    win->key_codes[0x04E] = NDC_KEY_NUMPAD_ADD;
    win->key_codes[0x053] = NDC_KEY_NUMPAD_DECIMAL;
    win->key_codes[0x135] = NDC_KEY_NUMPAD_DIVIDE;
    win->key_codes[0x11C] = NDC_KEY_NUMPAD_ENTER;
    win->key_codes[0x059] = NDC_KEY_NUMPAD_EQUAL;
    win->key_codes[0x037] = NDC_KEY_NUMPAD_MULTIPLY;
    win->key_codes[0x04A] = NDC_KEY_NUMPAD_SUBTRACT;


    win->instance = GetModuleHandle(0);
    win->class_name = name;

    WNDCLASSEXA cls = {0};
    cls.cbSize = sizeof(WNDCLASSEX);
    cls.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    cls.lpfnWndProc = window_callback;
    
    cls.hInstance = win->instance;
    cls.hCursor = LoadCursor(0,IDC_ARROW);

    cls.lpszClassName = name;
    //cls.cbClsExtra = 0;
    //cls.cbWndExtra = 0;
    //cls.hIcon = 0;
    //cls.hbrBackground = 0;
    //cls.lpszMenuName = 0;
    //cls.hIconSm = 0;

    if(!RegisterClassExA(&cls))
    {
        NDC_CRITICAL("failed to register class");
        ndc_free(win);
        return NULL;
    }
    win->hwnd = CreateWindowExA(
        0,
        name,
        name,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        0,
        0,
        win->instance,
        0);

    if (!win->hwnd)
    {
        ndc_free(win);
        NDC_CRITICAL("failed to create window");
        return NULL;
    }
    win->alive = 1;
    SetWindowLongPtrA(win->hwnd,GWLP_USERDATA,(LONG_PTR)win);
    if(ndc_create_context(win) != NDC_SUCCESS)
    {
        NDC_CRITICAL("gl context setup failure");
        ndc_free(win);
        return NULL;
    }
    return win;
}
void ndc_poll_events(ndc_window* win)
{
	MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
    {
    	if(msg.message == WM_QUIT)
    	{
    		win->alive = 0;
    	}
    	else
    	{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
    	}

    }
}
void ndc_set_key_callback(ndc_window* win,NDC_KEY_CALLBACK callback)
{
	if(callback)
	{
		win->key_callback = callback;
	}
	else
	{
        NDC_INFO("ndc_set_key_callback couldn't set callback as provided function is NULL");
	}
}
void ndc_set_size_callback(ndc_window* win,NDC_SIZE_CALLBACK callback)
{
    if(callback)
    {
        win->size_callback = callback;
    }
    else
    {
        NDC_INFO("ndc_set_size_callback couldn't set callback as provided function is NULL");
    }
}
void ndc_set_mouse_move_callback(ndc_window* win,NDC_MOUSE_MOVE_CALLBACK callback)
{
    if(callback)
    {
        win->mouse_move_callback = callback;
    }
    else
    {
        NDC_INFO("ndc_set_mouse_move_callback couldn't set callback as provided function is NULL");
    } 
}
void ndc_set_mouse_click_callback(ndc_window* win,NDC_MOUSE_CLICK_CALLBACK callback)
{
    if(callback)
    {
        win->mouse_click_callback = callback;
    }
    else
    {
        NDC_INFO("ndc_set_mouse_click_callback couldn't set callback as provided function is NULL");
    } 
}
#endif