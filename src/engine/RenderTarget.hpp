#pragma once

#include "core/Log.hpp"
#include "GraphicsDevice.hpp"

#include <d3d12.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_2.h>
#include <dcomp.h>
#include <wrl/client.h>
#include <unordered_map>
#include <mutex>

class RenderTarget {
public:
	enum class BrushType { Background, UI, Border, Text, Grid };
	explicit RenderTarget(HWND window);
	~RenderTarget();

	void Initialize(
		IDXGIFactory2* factory, ID3D11Device* d3dDevice, ID2D1Device* d2dDevice,
		IDCompositionDevice* dcompDevice
	);
	void CreateTarget();
	void Reset();
	void AddBrush(const D2D1_COLOR_F& color, BrushType type);
	ID2D1SolidColorBrush* GetBrush(BrushType type) const;
	void Present();
	void SetTarget();
	ID2D1DeviceContext1* Context() const;
	IDXGISwapChain1* SwapChain() const;
	void* FrameWaitHandle() const;
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain1>      m_swapChain;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext1>  m_context;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>         m_primaryBitmap;
	Microsoft::WRL::ComPtr<ID2D1Image>           m_targetBitmap;
	Microsoft::WRL::ComPtr<IDCompositionTarget>  m_dcompTarget;
	Microsoft::WRL::ComPtr<IDCompositionVisual>  m_root;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_fallbackBrush;

	HWND m_window;
	void* m_frameLatencyWaitable;
	float m_dpiX = 0.0f;
	float m_dpiY = 0.0f;

	std::unordered_map<BrushType, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>> m_brushes;
};