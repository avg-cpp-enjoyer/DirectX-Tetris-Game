#pragma once

#include "core/Utils.hpp"
#include "GraphicsDevice.hpp"

#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_2.h>
#include <dcomp.h>
#include <wrl/client.h>
#include <unordered_map>

class RenderTarget {
public:
	enum class BrushType { Background, UI, Border, Text, Grid };
	RenderTarget() = default;
	~RenderTarget();

	void Initialize(
		HWND window, IDXGIFactory2* factory, ID3D11Device* d3dDevice, 
		ID2D1Device* d2dDevice, IDCompositionDevice* dcompDevice
	);
	void CreateTarget();
	void Reset();
	void AddBrush(const D2D1_COLOR_F& color, BrushType type);
	ID2D1SolidColorBrush* GetBrush(BrushType type) const;
	void BeginRender();
	void EndRender();
	void SetTarget();
	ID2D1DeviceContext1* Context() const;
	IDXGISwapChain1* SwapChain() const;
private:
	Microsoft::WRL::ComPtr<IDXGISwapChain1>      m_swapChain;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext1>  m_context;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>         m_primaryBitmap;
	Microsoft::WRL::ComPtr<ID2D1Image>           m_targetBitmap;
	Microsoft::WRL::ComPtr<IDCompositionTarget>  m_dcompTarget;
	Microsoft::WRL::ComPtr<IDCompositionVisual>  m_root;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_fallbackBrush;

	HWND m_window = nullptr;
	float m_dpiX = 0.0f;
	float m_dpiY = 0.0f;
	bool m_initialized = false;

	std::unordered_map<BrushType, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>> m_brushes;
};