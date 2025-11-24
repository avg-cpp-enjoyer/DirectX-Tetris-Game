#pragma once

#include <d2d1_1.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "Dcomp.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Windowscodecs.lib")

class GraphicsDevice {
public:
	GraphicsDevice(const GraphicsDevice&) = delete;
	GraphicsDevice& operator=(const GraphicsDevice&) = delete;

	static void Initialize();
	static void Reinit();
	static IDCompositionDevice* DCompDevice();
	static IDXGIFactory2* DXGIFactory();
	static IWICImagingFactory* WICFactory();
	static ID3D11Device* D3DDevice();
	static ID2D1Device* D2DDevice();
	static IDWriteTextFormat* TextFormat();
	static ID2D1Factory1* Factory();
private:
	GraphicsDevice() = default;
	~GraphicsDevice() = default;

	static GraphicsDevice& GetInstance();
	void InitD3D();
	void InitD2D();
	void InitText();
	void InitDComp();
private:
	Microsoft::WRL::ComPtr<ID3D11Device>          m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>   m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGIDevice>           m_dxgiDevice;
	Microsoft::WRL::ComPtr<IDXGIFactory2>         m_dxgiFactory2;
	Microsoft::WRL::ComPtr<ID2D1Factory1>         m_d2dFactory;
	Microsoft::WRL::ComPtr<ID2D1Device>           m_d2dDevice;
	Microsoft::WRL::ComPtr<IDCompositionDevice>   m_dcompDevice;
	Microsoft::WRL::ComPtr<IWICImagingFactory>    m_wicFactory;
	Microsoft::WRL::ComPtr<IDWriteFactory>        m_writeFactory;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>     m_textFormat;
};