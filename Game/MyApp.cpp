/*  =======================================================
	Direct3D 12 Project-Specific Application Implementation
	=======================================================  */

#include "MyApp.h"

MyApp::MyApp(HINSTANCE Instance) : D3DApp(Instance)
{
}

MyApp::~MyApp()
{
}

bool MyApp::Init()
{
	if (D3DApp::Init() == false) 
	{ 
		return false; 
	}

	return true;
}

void MyApp::ResizeWindow(UINT width, UINT height)
{
	windowWidth = width;
	windowHeight = height;
}

void MyApp::Update(const GameTimer& gt) 
{
}

void MyApp::Draw(const GameTimer& gt) 
{
}

void MyApp::OnMouseDown(WPARAM btnState, int x, int y)
{
}

void MyApp::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void MyApp::OnMouseMove(WPARAM btnState, int x, int y)
{
}

void MyApp::OnKeyPress(WPARAM key)
{
}
