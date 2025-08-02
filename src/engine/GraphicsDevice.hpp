#pragma once

#include "core/Log.hpp"
#include "ui/Constants.hpp"

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_2.h>
#include <dcomp.h>
#include <dwrite.h>
#include <wincodec.h>
#include <vector>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dcomp.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Windowscodecs.lib")

class GraphicsDevice {
public:
	enum class BrushType { Background, UI, Border, Text, Grid, Tetramino };

	GraphicsDevice() = default;
	~GraphicsDevice() = default;

	void Initialize(HWND hwnd);
	ID2D1DeviceContext1* Context() const;
	IDXGISwapChain1* SwapChain() const;
	IDCompositionDevice* DCompDevice() const;
	ID2D1Factory1* Factory() const;
	IWICImagingFactory* WICFactory() const;
	IDWriteTextFormat* TextFormat() const;
	ID2D1SolidColorBrush* GetBrush(BrushType type) const;
private:
	void InitD3D(HWND window);
	void InitD2D();
	void InitDComp(HWND window);
	void InitText();
	void InitBrushes();
private:
	Microsoft::WRL::ComPtr<ID3D11Device>          m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>   m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain1>       m_swapChain;
	Microsoft::WRL::ComPtr<IDXGIDevice>           m_dxgiDevice;

	Microsoft::WRL::ComPtr<ID2D1Factory1>         m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device>           m_d2dDevice;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext1>   m_d2dContext;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1>          m_d2dTarget;

	Microsoft::WRL::ComPtr<IDCompositionDevice>   m_dcompDevice;
	Microsoft::WRL::ComPtr<IDCompositionTarget>   m_dcompTarget;
	Microsoft::WRL::ComPtr<IDCompositionVisual>   m_dcompVisual;

	Microsoft::WRL::ComPtr<IWICImagingFactory>    m_wicFactory;
	Microsoft::WRL::ComPtr<IDWriteFactory>        m_writeFactory;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>     m_textFormat;

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>  m_bgBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>  m_uiBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>  m_borderBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>  m_textBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>  m_gridBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>  m_tetraminoBrush;
};