/*  ==============================================
	Direct3D 12 Generic Application Implementation
	==============================================  */

#include "D3DApp.h"

D3DApp* D3DApp::app = nullptr;

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return D3DApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			isPaused = true;
			gameTimer.Stop();
		}
		else
		{
			isPaused = false;
			gameTimer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		if (wParam == SIZE_MINIMIZED)
		{
			isPaused = true;
			isMinimized = true;
			isMaximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			isPaused = false;
			isMinimized = false;
			isMaximized = true;
			ResizeWindow(width, height);
		}
		else if (wParam == SIZE_RESTORED)
		{

			// Restoring from minimized state?
			if (isMinimized)
			{
				isPaused = false;
				isMinimized = false;
				ResizeWindow(width, height);
			}

			// Restoring from maximized state?
			else if (isMaximized)
			{
				isPaused = false;
				isMaximized = false;
				ResizeWindow(width, height);
			}
			else if (isResizing)
			{
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			}
			else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			{
				ResizeWindow(width, height);
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		isPaused = true;
		isResizing = true;
		gameTimer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		isPaused = false;
		isResizing = false;
		gameTimer.Start();
		ResizeWindow(width, height);
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		else
		{
			OnKeyPress(wParam);
		}
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE Instance) : appInstance(Instance)
{
	// Assert that only one instance of D3DApp has been created
	assert(app == nullptr);
	app = this;
	mainWindow = nullptr;
	// TODO: Does everything get initialize that is supposed to?
}

D3DApp::~D3DApp()
{
	// TODO: Does anything in this class need to be explicitly destroyed?
}

bool D3DApp::Init()
{
	// Initialize the Main Window
	return InitMainWindow();
}

bool D3DApp::Run()
{
	MSG msg = { 0 };

	gameTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			gameTimer.Tick();

			if (!isPaused)
			{
				Update(gameTimer);
				Draw(gameTimer);
			}
			else
			{
				// TODO: Is Sleep(100) the best thing to do while the app is paused?
				Sleep(100);
			}
		}
	}

	return true;
}

bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = appInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, 800, 600 };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	LPCTSTR mainWindowCaption = L"RhymeEngine - Default Caption";
	mainWindow = CreateWindow(L"MainWnd", mainWindowCaption, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, appInstance, 0);
	if (!mainWindow)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mainWindow, SW_SHOW);
	UpdateWindow(mainWindow);

	return true;
}

D3DApp* D3DApp::GetApp()
{
	return app;
}
