#include "GraphicsDevice.hpp"

void GraphicsDevice::Initialize(HWND window) {
	InitD3D(window);
	InitD2D();
	InitDComp(window);
	InitText();
	InitBrushes();
}

ID2D1DeviceContext1* GraphicsDevice::Context() const {
	return m_d2dContext.Get();
}

IDXGISwapChain1* GraphicsDevice::SwapChain() const {
	return m_swapChain.Get();
}

IDCompositionDevice* GraphicsDevice::DCompDevice() const {
	return m_dcompDevice.Get();
}

ID2D1Factory1* GraphicsDevice::Factory() const {
	return m_d2dFactory.Get();
}

IWICImagingFactory* GraphicsDevice::WICFactory() const {
	return m_wicFactory.Get();
}

IDWriteTextFormat* GraphicsDevice::TextFormat() const {
	return m_textFormat.Get();
}

ID2D1SolidColorBrush* GraphicsDevice::GetBrush(BrushType type) const {
	switch (type) {
	case BrushType::Background: return m_bgBrush.Get();
	case BrushType::UI:         return m_uiBrush.Get();
	case BrushType::Border:     return m_borderBrush.Get();
	case BrushType::Text:       return m_textBrush.Get();
	case BrushType::Grid:       return m_gridBrush.Get();
	}
	return m_bgBrush.Get();
}

void GraphicsDevice::InitD3D(HWND window) {
	uint32_t flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	HR_LOG(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
		nullptr, 0, D3D11_SDK_VERSION, &m_d3dDevice, nullptr, &m_d3dContext));

	HR_LOG(m_d3dDevice.As(&m_dxgiDevice));

	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
	HR_LOG(m_dxgiDevice->GetAdapter(&adapter));
	Microsoft::WRL::ComPtr<IDXGIFactory2> factory2;
	HR_LOG(adapter->GetParent(IID_PPV_ARGS(&factory2)));

	RECT rc; 
	GetClientRect(window, &rc);

	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width        = rc.right;
	scDesc.Height       = rc.bottom;
	scDesc.Format       = DXGI_FORMAT_B8G8R8A8_UNORM;
	scDesc.SampleDesc   = { 1, 0 };
	scDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount  = 3;
	scDesc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	scDesc.AlphaMode    = DXGI_ALPHA_MODE_PREMULTIPLIED;
	scDesc.Scaling      = DXGI_SCALING_STRETCH;

	HR_LOG(factory2->CreateSwapChainForComposition(m_d3dDevice.Get(), &scDesc, nullptr, &m_swapChain));
}

void GraphicsDevice::InitD2D() {
	D2D1_FACTORY_OPTIONS opts = {};
#ifdef _DEBUG
	opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	HR_LOG(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory1), &opts,
		reinterpret_cast<void**>(m_d2dFactory.GetAddressOf())));

	HR_LOG(m_d2dFactory->CreateDevice(m_dxgiDevice.Get(), &m_d2dDevice));

	Microsoft::WRL::ComPtr<ID2D1DeviceContext> baseContext;
	HR_LOG(m_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &baseContext));
	HR_LOG(baseContext.As(&m_d2dContext));

	Microsoft::WRL::ComPtr<ID3D11Texture2D> back;
	HR_LOG(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&back)));
	Microsoft::WRL::ComPtr<IDXGISurface> surface;
	HR_LOG(back.As(&surface));

	D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

	HR_LOG(m_d2dContext->CreateBitmapFromDxgiSurface(surface.Get(), bitmapProps, &m_d2dTarget));
	m_d2dContext->SetTarget(m_d2dTarget.Get());
}

void GraphicsDevice::InitDComp(HWND window) {
	HR_LOG(DCompositionCreateDevice(m_dxgiDevice.Get(), IID_PPV_ARGS(&m_dcompDevice)));
	HR_LOG(m_dcompDevice->CreateTargetForHwnd(window, true, &m_dcompTarget));
	HR_LOG(m_dcompDevice->CreateVisual(&m_dcompVisual));
	HR_LOG(m_dcompVisual->SetContent(m_swapChain.Get()));
	HR_LOG(m_dcompTarget->SetRoot(m_dcompVisual.Get()));

	Microsoft::WRL::ComPtr<IDCompositionVisual2> visual2;
	if (SUCCEEDED(m_dcompVisual.As(&visual2))) {
		visual2->SetBitmapInterpolationMode(DCOMPOSITION_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
	}

	HR_LOG(m_dcompDevice->Commit());
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

void GraphicsDevice::InitBrushes() {
	using namespace UI::General;

	HR_LOG(m_d2dContext->CreateSolidColorBrush(bgColor, &m_bgBrush));
	HR_LOG(m_d2dContext->CreateSolidColorBrush(uiColor, &m_uiBrush));
	HR_LOG(m_d2dContext->CreateSolidColorBrush(borderColor, &m_borderBrush));
	HR_LOG(m_d2dContext->CreateSolidColorBrush(textColor, &m_textBrush));
	HR_LOG(m_d2dContext->CreateSolidColorBrush(borderColor, &m_gridBrush));
	HR_LOG(m_d2dContext->CreateSolidColorBrush(tetraminoColor, &m_tetraminoBrush));
}