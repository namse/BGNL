#pragma once
#include "stdafx.h"
#include "Component.h"
#include "AutoMatcher.h"
#include "Screen.h"
/******************************************************************
*                                                                 *
*  Application                                                  *
*                                                                 *
******************************************************************/

class Application
	: public Component
{
public:
	Application();
	~Application();

	HRESULT Initialize();

	void RunMessageLoop();

private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();

	HRESULT OnRender();

	void OnMouseMove(
		LPARAM lParam
		);

	void OnKeyDown(
		SHORT vkey
		);

	void OnWheel(
		WPARAM w
		);

	float GetZoom()
	{
		return powf(2.0f, m_logZoom / WHEEL_DELTA);
	}

	PCWSTR GetAntialiasModeString();

	HRESULT RenderD2DContentIntoSurface(
		float time
		);

	HRESULT RenderLoupe();

	HRESULT RenderTextInfo();

	HRESULT CreateD3DDevice(
		IDXGIAdapter *pAdapter,
		D3D10_DRIVER_TYPE driverType,
		UINT flags,
		ID3D10Device1 **ppDevice
		);

	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);

	void virtual Render();
	void virtual Update(float dTime);
private:
	HWND m_hwnd;

	MyAntialiasMode::Enum m_antialiasMode;
	SampleType::Enum m_sampleType;
	bool m_paused;
	bool m_drawLoupe;
	UINT m_numSquares;
	float m_logZoom;
	D2D1_POINT_2F m_mousePos;

	
	DWORD prevTime = NULL;


	bool ShowText;


	Screen screen_;
};

