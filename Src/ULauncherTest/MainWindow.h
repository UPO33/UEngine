#include "Base.h"


LRESULT CALLBACK UWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


//////////////////////////////////////////////////////////////////////////
class WindowWin32
{
public:
	HWND mWndHandle = nullptr;
	unsigned mWidth = 800;
	unsigned mHeight = 600;
	IDXGISwapChain3*		mSwapChain = nullptr;
	ID3D12Resource*			mRTTextures[FRAME_COUNT] = {};
	DescHandleRTV			mRTVHead;

	WindowWin32(unsigned w, unsigned h, const wchar_t* wndClass);

	void CreateSwapChain();


};