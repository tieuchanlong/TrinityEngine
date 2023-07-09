#include <DirectX11BasePch.h>
#include "Application.h"
#include "Keyboard.h"
#include "Mouse.h"

using namespace DirectX;

// ------------------------------ Application Callback Handle ----------------------- //
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct; // Contains info about the application, and used to paint the client area of a window
	HDC hDC; // Device scene handle
	char msg[32];

	switch (message)
	{
	case WM_SIZE:
		// Resize the application
	{
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);
	}
	break;

	case WM_DISPLAYCHANGE:
		// Change display
		Application::GetInstance()->ReRender();
		break;

	case WM_PAINT:
		Application::GetInstance()->OnRender(hwnd);
		break;

	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			// This is where you'd implement the classic ALT+ENTER hotkey for fullscreen toggle
		}
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;
	case WM_MOUSEACTIVATE:
		// When you click to activate the window, we want Mouse to ignore that event.
		return MA_ACTIVATEANDEAT;

	case WM_DESTROY:
		PostQuitMessage(0); // Destroy the window
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

// ----------------------------- Main Application Running ----------------------- //

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	// Do some application initialization for application, Direct3D
	if (Application::GetInstance()->Initialize(hInstance, WndProc, cmdShow))
	{
		MessageBox(nullptr, TEXT("Failed to create applicaiton window."), TEXT("Error"), MB_OK);
		return -1;
	}

	// Do some application run down here
	int iRet = Application::GetInstance()->Run();

	Application::GetInstance()->Destroy();

	return iRet;
}