#include "GraphicsDevice.hpp"

#include <dcomp.h>
#include <dxgi1_2.h>
#include <wincodec.h>
#include <d3d11.h>
#include <d3d11_4.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <cstdint>
#include <d3dcommon.h>
#include <dxgi.h>
#include <Windows.h>
#include <d2d1.h>

#include <ui/Constants.hpp>
#include <core/Utils.hpp>
#include <wrl/client.h>

void GraphicsDevice::Initialize() {
	GetInstance().InitD3D();
	GetInstance().InitD2D();
	GetInstance().InitDComp();
	GetInstance().InitText();
}

void GraphicsDevice::Reinit() {
	GetInstance().m_d2dDevice.Reset();
	GetInstance().m_d2dFactory.Reset();
	GetInstance().m_d3dContext.Reset();
	GetInstance().m_d3dDevice.Reset();
	GetInstance().m_dcompDevice.Reset();
	GetInstance().m_dxgiDevice.Reset();
	GetInstance().m_dxgiFactory2.Reset();
	GetInstance().m_textFormat.Reset();
	GetInstance().m_wicFactory.Reset();
	GetInstance().m_writeFactory.Reset();
	Initialize();
}

IDCompositionDevice* GraphicsDevice::DCompDevice() {
	return GetInstance().m_dcompDevice.Get();
}

IDXGIFactory2* GraphicsDevice::DXGIFactory() {
	return GetInstance().m_dxgiFactory2.Get();
}

IWICImagingFactory* GraphicsDevice::WICFactory() {
	return GetInstance().m_wicFactory.Get();
}

ID3D11Device* GraphicsDevice::D3DDevice() {
	return GetInstance().m_d3dDevice.Get();
}

ID2D1Device* GraphicsDevice::D2DDevice() {
	return GetInstance().m_d2dDevice.Get();
}

IDWriteTextFormat* GraphicsDevice::TextFormat() {
	return GetInstance().m_textFormat.Get();
}

ID2D1Factory1* GraphicsDevice::Factory() {
	return GetInstance().m_d2dFactory.Get();
}

GraphicsDevice& GraphicsDevice::GetInstance() {
	static GraphicsDevice instance;
	return instance;
}

void GraphicsDevice::InitD3D() {
	uint32_t flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HR_LOG(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
		nullptr, 0, D3D11_SDK_VERSION, &m_d3dDevice, nullptr, &m_d3dContext));

	HR_LOG(m_d3dDevice.As(&m_dxgiDevice));

	Microsoft::WRL::ComPtr<ID3D11Multithread> multithread;
	if (SUCCEEDED(m_d3dDevice.As(&multithread))) {
		multithread->SetMultithreadProtected(true);
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;
	HR_LOG(m_dxgiDevice->GetAdapter(&adapter));
	HR_LOG(adapter->GetParent(IID_PPV_ARGS(&factory)));
	HR_LOG(factory.As(&m_dxgiFactory2));

	Microsoft::WRL::ComPtr<IDXGIDevice1> dxgiDevice1;
	if (SUCCEEDED(m_dxgiDevice.As(&dxgiDevice1))) {
		HR_LOG(dxgiDevice1->SetMaximumFrameLatency(1));
	}
}

void GraphicsDevice::InitD2D() {
	ASSERT_TRUE(m_dxgiDevice, "InitD3D() must be called before InitD2D()");
	D2D1_FACTORY_OPTIONS opts = {};
#ifdef _DEBUG
	opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	HR_LOG(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), &opts,
		reinterpret_cast<void**>(m_d2dFactory.GetAddressOf())));

	HR_LOG(m_d2dFactory->CreateDevice(m_dxgiDevice.Get(), &m_d2dDevice));
}

void GraphicsDevice::InitText() {
	HR_LOG(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_wicFactory)));

	HR_LOG(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(m_writeFactory.GetAddressOf())));

	HR_LOG(m_writeFactory->CreateTextFormat(L"Bahnschrift", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL, UI::General::fontSize, L"", &m_textFormat));

	m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void GraphicsDevice::InitDComp() {
	HR_LOG(DCompositionCreateDevice(m_dxgiDevice.Get(), IID_PPV_ARGS(&m_dcompDevice)));
}
