/*  ===================================================
	Direct3D 12 Project-Specific Application Definition
	===================================================  */

#ifndef MYAPP_H
#define MYAPP_H

#include "D3DApp.h"

class MyApp : public D3DApp
{
public:
	MyApp(HINSTANCE Instance);
	~MyApp();

	bool Init() override;

	void Update(const GameTimer& gt) override;
	void Draw(const GameTimer& gt) override;
	void ResizeWindow(UINT width, UINT height) override;

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;
	void OnKeyPress(WPARAM key) override;

private:
	UINT windowWidth;
	UINT windowHeight;
};

#endif // MYAPP_H