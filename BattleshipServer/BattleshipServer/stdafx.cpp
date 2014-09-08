// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// TangoMike.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"

// TODO: �ʿ��� �߰� �����
// �� ������ �ƴ� STDAFX.H���� �����մϴ�.



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
