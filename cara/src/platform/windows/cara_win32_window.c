#include <windows.h>

#include "cara/cara_config.h"
#include "cara_win32_internal.h"
#include "cara_win32_window.h"
#include "cara_event/cara_event_manager.h"
#include "cara_internal/cara_type.h"
#include "cara_input/cara_input.h"

#define CARA_WINDOW_CLASS_NAME	"CaraWindowClass"

static LRESULT CALLBACK cara_window_proc(
	HWND hwnd,
	UINT message,
	WPARAM wparam,
	LPARAM lparam
);

static cara_status_t cara_window_register_class(void);

static void cara_win32_present(
	cara_platform_t *platform,
	HDC dc
);

static cara_key_t cara_win32_translate_key(WPARAM key);


cara_status_t cara_window_create(
	const cara_window_config_t *window_config, 
	cara_platform_t *platform,
	cara_t *cara
)
{
	cara_status_t status = cara_window_register_class();
	if (status != CARA_STATUS_OK) {
		return status;
	}

	RECT rect = {
		0, 0,
		(LONG) window_config->width, (LONG) window_config->height
	};

	DWORD style = WS_OVERLAPPEDWINDOW;

	if (!AdjustWindowRectEx(&rect, style, FALSE, 0)) {
		return CARA_STATUS_PLATFORM_ERROR;
	}

	HWND window = CreateWindowExA(
		0,
		CARA_WINDOW_CLASS_NAME,
		window_config->title,
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		GetModuleHandleA(NULL),
		cara
	);

	if (!window) {
		return CARA_STATUS_PLATFORM_ERROR;
	}

	platform->window = window;

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	return CARA_STATUS_OK;
}


cara_status_t cara_platform_present(
	cara_platform_t *platform,
	const cara_pixbuf_t *buffer
)
{
	if (buffer) {
		if (buffer->width == 0 ||
			buffer->height == 0) {
			platform->pixbuf = NULL;
			return CARA_STATUS_INVALID_ARGUMENT;
		}
	}
	platform->pixbuf = buffer;


	if (!InvalidateRect(platform->window, NULL, FALSE)) {
		return CARA_STATUS_PLATFORM_ERROR;
	}

	return CARA_STATUS_OK;
}


static LRESULT CALLBACK cara_window_proc(
	HWND hwnd,
	UINT message,
	WPARAM wparam,
	LPARAM lparam
) {
	cara_t *cara = (cara_t *)GetWindowLongPtrA(hwnd, GWLP_USERDATA);

	switch (message) {
		case WM_NCCREATE: {
			CREATESTRUCTA *create_info = (CREATESTRUCTA *) lparam;
			cara = (cara_t *)create_info->lpCreateParams;

			SetWindowLongPtrA(
				hwnd,
				GWLP_USERDATA,
				(LONG_PTR) cara
			);

			return DefWindowProcA(hwnd, message, wparam, lparam);
		}
		case WM_CLOSE:
			cara_event_insert(cara->event_manager, (cara_event_t) { .type = CARA_CLOSE_REQUEST });
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_SIZE:
			cara_event_t event = {
				.type = CARA_RESIZED,
				.resize = {
					.new_width = LOWORD(lparam),
					.new_height = HIWORD(lparam)
				}
			};

			cara_event_insert(cara->event_manager, event);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		case WM_PAINT:
			PAINTSTRUCT paint;
			HDC dc = BeginPaint(hwnd, &paint);

			if (cara && cara->platform) {
				cara_win32_present(cara->platform, dc);
			}

			EndPaint(hwnd, &paint);
			return 0;
		case WM_ERASEBKGND:
			return 1;
		case WM_KEYDOWN:
		{
			cara_key_t key = cara_win32_translate_key(wparam);
			cara_input_key_down_event(cara->input, key);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		}
		case WM_KEYUP:
		{
			cara_key_t key = cara_win32_translate_key(wparam);
			cara_input_key_up_event(cara->input, key);
			return DefWindowProcA(hwnd, message, wparam, lparam);
		}
		default:
			return DefWindowProcA(hwnd, message, wparam, lparam);
	}
}

static cara_status_t cara_window_register_class(void)
{
	WNDCLASSEXA wc = {0};

	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = cara_window_proc;
	wc.hInstance = GetModuleHandleA(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = CARA_WINDOW_CLASS_NAME;

	if (!RegisterClassExA(&wc)) {
		if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
			return CARA_STATUS_PLATFORM_ERROR;
		}
	}

	return CARA_STATUS_OK;
}


static void cara_win32_present(
	cara_platform_t *platform,
	HDC dc
) {
	const cara_pixbuf_t *buffer = platform->pixbuf;

	RECT rect;
	if (!GetClientRect(platform->window, &rect)) {
		return;
	}

	int32_t width = rect.right - rect.left;
	int32_t height = rect.bottom - rect.top;

	HDC memory_dc = CreateCompatibleDC(dc);
	if (!memory_dc) {
		return;
	}

	HBITMAP bitmap = CreateCompatibleBitmap(dc, width, height);
	if (!bitmap) {
		DeleteDC(memory_dc);
		return;
	}

	HGDIOBJ old_bitmap = SelectObject(memory_dc, bitmap);

	FillRect(
		memory_dc,
		&rect,
		(HBRUSH) GetStockObject(BLACK_BRUSH)
	);

	if (buffer && buffer->pixels &&
		buffer->width > 0 && buffer->height > 0) {

		double scale_x = (double) width / buffer->width;
		double scale_y = (double) height / buffer->height;
		double scale = scale_x < scale_y ? scale_x : scale_y;

		int32_t scaled_width = (int32_t) (buffer->width * scale);
		int32_t scaled_height = (int32_t) (buffer->height * scale);

		int32_t offset_x = (width - scaled_width) / 2;
		int32_t offset_y = (height - scaled_height) / 2;

		BITMAPINFO bitmap_info = {0};
		bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmap_info.bmiHeader.biWidth = buffer->width;
		bitmap_info.bmiHeader.biHeight = -(LONG) buffer->height;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biCompression = BI_RGB;

		StretchDIBits(
			memory_dc,
			offset_x, offset_y,
			scaled_width, scaled_height,
			0, 0,
			buffer->width, buffer->height,
			buffer->pixels,
			&bitmap_info,
			DIB_RGB_COLORS,
			SRCCOPY
		);
	}

	BitBlt(
		dc,
		0, 0,
		width, height,
		memory_dc,
		0, 0,
		SRCCOPY
	);

	SelectObject(memory_dc, old_bitmap);
	DeleteObject(bitmap);
	DeleteDC(memory_dc);
}

static cara_key_t cara_win32_translate_key(WPARAM key)
{
	// Letter keys
	if (key >= 'A' && key <= 'Z') {
		return KEY_A + (key - 'A');
	}

	// Number keys
	if (key >= '0' && key <= '9') {
		return KEY_0 + (key - '0');
	}

	switch (key) {
		case VK_LEFT:  
			return KEY_LEFT;
		case VK_RIGHT: 
			return KEY_RIGHT;
		case VK_UP:    
			return KEY_UP;
		case VK_DOWN:  
			return KEY_DOWN;

		case VK_OEM_PLUS:
			return KEY_PLUS;
		case VK_OEM_MINUS:
			return KEY_MINUS;
		case VK_SPACE:  
			return KEY_SPACE;
		case VK_RETURN: 
			return KEY_ENTER;
		case VK_ESCAPE: 
			return KEY_ESCAPE;

		case VK_CONTROL: 
			return KEY_CTRL;
		case VK_MENU:    
			return KEY_ALT;
		case VK_SHIFT:   
			return KEY_SHIFT;

		case VK_TAB:    
			return KEY_TAB;
		case VK_BACK:   
			return KEY_BACK;

		default:
			return KEY_UNKNOWN;
	}
}