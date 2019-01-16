/*  ==========================================
	Direct3D 12 Generic Application Definition
	==========================================  */

#ifndef D3DAPP_H
#define D3DAPP_H

#include <Windows.h>
#include <WindowsX.h>

#include <assert.h>

#include "GameTimer.h"

class D3DApp
{
public:
	D3DApp(HINSTANCE Instance);
	~D3DApp();

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static D3DApp* GetApp();

	virtual bool Init();
	bool Run();
	
	virtual void Update(const GameTimer& gt) = 0;
	virtual void Draw(const GameTimer& gt) = 0;
	virtual void ResizeWindow(UINT width, UINT height) = 0;
	
	virtual void OnMouseDown(WPARAM btnState, int x, int y) = 0;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) = 0;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) = 0;
	virtual void OnKeyPress(WPARAM key) = 0;

protected:
	// TODO: Can all of this be made private and not protected?
	// D3DApp is a singleton, only one instance is allowed
	static D3DApp* app;
	HINSTANCE appInstance;
	HWND mainWindow;

	GameTimer gameTimer;

	bool isPaused = false;
	bool isMinimized = false;
	bool isMaximized = false;
	bool isResizing = false;
	bool isFullscreen = false;

private:
	bool InitMainWindow();

	UINT width;
	UINT height;
};

#endif // D3DAPP_H
