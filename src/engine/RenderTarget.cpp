#include "RenderTarget.hpp"

RenderTarget::RenderTarget(HWND window) : m_window(window), m_frameLatencyWaitable(nullptr) {}

RenderTarget::~RenderTarget() {
	if (m_frameLatencyWaitable) { 
		CloseHandle(m_frameLatencyWaitable); 
		m_frameLatencyWaitable = nullptr; 
	}
}

void RenderTarget::Initialize(
	IDXGIFactory2* factory, ID3D11Device* d3dDevice, ID2D1Device* d2dDevice,
	IDCompositionDevice* dcompDevice) {
	RECT rc{};
	GetClientRect(m_window, &rc);

	DXGI_SWAP_CHAIN_DESC1 desc{};
	desc.Width        = rc.right - rc.left;
	desc.Height       = rc.bottom - rc.top;
	desc.Format       = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc   = { 1, 0 };
	desc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount  = 2;
	desc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc.Scaling      = DXGI_SCALING_STRETCH;
	desc.AlphaMode    = DXGI_ALPHA_MODE_PREMULTIPLIED;
	desc.Flags        = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

	HR_LOG(factory->CreateSwapChainForComposition(d3dDevice, &desc, nullptr, &m_swapChain));
	HR_LOG(dcompDevice->CreateTargetForHwnd(m_window, true, &m_dcompTarget));

	Microsoft::WRL::ComPtr<IDXGISwapChain2> swapChain2;
	if (SUCCEEDED(m_swapChain.As(&swapChain2))) {
		swapChain2->SetMaximumFrameLatency(1);
		m_frameLatencyWaitable = swapChain2->GetFrameLatencyWaitableObject();
	}

	Microsoft::WRL::ComPtr<IDCompositionVisual> visual;
	HR_LOG(dcompDevice->CreateVisual(&visual));
	HR_LOG(visual->SetContent(m_swapChain.Get()));
	HR_LOG(m_dcompTarget->SetRoot(visual.Get()));
	m_root = visual;
	HR_LOG(dcompDevice->Commit());

	Microsoft::WRL::ComPtr<ID2D1DeviceContext> baseContext;
	HR_LOG(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &baseContext));
	HR_LOG(baseContext.As(&m_context));

	const float dpi = static_cast<float>(GetDpiForWindow(m_window));
	m_dpiX = m_dpiY = dpi;

	CreateTarget();
}

void RenderTarget::CreateTarget() {
	Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
	HR_LOG(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)));

	D2D1_BITMAP_PROPERTIES1 bitmapProps = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	HR_LOG(m_context->CreateBitmapFromDxgiSurface(dxgiBackBuffer.Get(), &bitmapProps, &m_primaryBitmap));

	m_targetBitmap = m_primaryBitmap.Get();
	m_context->SetTarget(m_targetBitmap.Get());
	m_context->SetDpi(m_dpiX, m_dpiY);
}

void RenderTarget::Reset() {
	m_context->SetTarget(nullptr);
	m_primaryBitmap.Reset();
	m_targetBitmap.Reset();
	m_root.Reset();
	m_dcompTarget.Reset();
	m_swapChain.Reset();
	m_brushes.clear();
	m_fallbackBrush.Reset();

	Initialize(GraphicsDevice::DXGIFactory(), GraphicsDevice::D3DDevice(), 
		GraphicsDevice::D2DDevice(), GraphicsDevice::DCompDevice());
}

void RenderTarget::AddBrush(const D2D1_COLOR_F& color, BrushType type) {
	if (auto it = m_brushes.find(type); it != m_brushes.end() && it->second) {
		it->second->SetColor(color);
	} else {
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
		HR_LOG(m_context->CreateSolidColorBrush(color, &brush));
		m_brushes.emplace(type, std::move(brush));
	}
}

ID2D1SolidColorBrush* RenderTarget::GetBrush(BrushType type) const {
	if (auto it = m_brushes.find(type); it != m_brushes.end() && it->second) {
		return it->second.Get();
	}

	if (!m_fallbackBrush) {
		auto self = const_cast<RenderTarget*>(this);
		HR_LOG(self->m_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Magenta, 1.0f), &self->m_fallbackBrush));
	}

	return m_fallbackBrush.Get();
}

void RenderTarget::Present() {
	long hrEnd = m_context->EndDraw();
	HR_LOG(hrEnd);
	if (hrEnd == D2DERR_RECREATE_TARGET) {
		m_context->SetTarget(nullptr);
		m_primaryBitmap.Reset();
		m_targetBitmap.Reset();
		CreateTarget();
		return;
	}

	long hrPresent = m_swapChain->Present(1, 0);
	HR_LOG(hrPresent);
	if (hrPresent == DXGI_ERROR_DEVICE_REMOVED || hrPresent == DXGI_ERROR_DEVICE_RESET) {
		GraphicsDevice::Reinit();
		Reset();
		return;
	}
}

void RenderTarget::SetTarget() {
	m_context->SetTarget(m_targetBitmap.Get());
}

ID2D1DeviceContext1* RenderTarget::Context() const {
	return m_context.Get();
}

IDXGISwapChain1* RenderTarget::SwapChain() const {
	return m_swapChain.Get();
}

void* RenderTarget::FrameWaitHandle() const {
	return m_frameLatencyWaitable;
}