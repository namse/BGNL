// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// TangoMike.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.



//Device-Dependent Resources
ID3D10Device *m_pDevice = NULL;
IDXGISwapChain *m_pSwapChain = NULL;
ID3D10RasterizerState *m_pState = NULL;
ID3D10RenderTargetView *m_pRenderTargetView = NULL;
ID3D10Texture2D *m_pLoupeTexture = NULL;
ID2D1Bitmap *m_pLoupeBitmap = NULL;

ID2D1RenderTarget *m_pBackBufferRT = NULL;

ID2D1SolidColorBrush *m_pTextBrush = NULL;
ID2D1SolidColorBrush *m_pLoupeBrush = NULL;
IDWriteTextLayout* m_pTextLayout = NULL;


ID2D1PathGeometry *m_pPathGeometry = NULL;
ID2D1GeometrySink *m_pGeometrySink = NULL;
ID2D1GradientStopCollection *m_pGradientStops = NULL;
ID2D1LinearGradientBrush *m_pLGBrush = NULL;

PendingAcceptList pendingAcceptList;

IWICImagingFactory* g_pWICFactory = nullptr;

float distance(D2D1_POINT_2F* a, D2D1_POINT_2F* b)
{
	return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}
