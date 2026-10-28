#include <windows.h>

#include "cara_win32_window.h"

#define CARA_WINDOW_CLASS_NAME	"CaraWindowClass"

static LRESULT CALLBACK cara_window_proc(
	HWND hwnd,
	UINT message,
	WPARAM wparam,
	LPARAM lparam
);
static cara_status_t cara_window_register_class(void);


cara_status_t cara_window_create(
	const cara_window_config_t *window_config, 
	cara_platform_t *platform
)
{
	if (!window_config || !platform) {
		return CARA_STATUS_INVALID_ARGUMENT;
	}

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
		platform
	);

	if (!window) {
		return CARA_STATUS_PLATFORM_ERROR;
	}

	platform->window = window;

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	return CARA_STATUS_OK;
}


static LRESULT CALLBACK cara_window_proc(
	HWND hwnd,
	UINT message,
	WPARAM wparam,
	LPARAM lparam
) {
	cara_platform_t *platform = (cara_platform_t *)
		GetWindowLongPtrA(hwnd, GWLP_USERDATA);

	switch (message) {
		case WM_NCCREATE: {
			CREATESTRUCTA *create_info = (CREATESTRUCTA *) lparam;
			platform = (cara_platform_t *)create_info->lpCreateParams;

			SetWindowLongPtrA(
				hwnd,
				GWLP_USERDATA,
				(LONG_PTR) platform
			);

			return TRUE;
		}
		case WM_CLOSE:
			DestroyWindow(hwnd);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
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