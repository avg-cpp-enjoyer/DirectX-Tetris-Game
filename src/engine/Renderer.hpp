#pragma once

#include "ui/Constants.hpp"
#include "model/GameField.hpp"
#include "ResourceManager.hpp"
#include "Scene.hpp"
#include "ui/components/UIComponents.hpp"
#include "ui/components/GameComponents.hpp"
#include "core/Log.hpp"
#include "RenderTarget.hpp"
#include "IBuilder.hpp"

#include <d3d12.h>
#include <d2d1.h>
#include <d2d1_2.h>
#include <wrl/client.h>
#include <dcomp.h>
#include <mutex>
#include <functional>

class __declspec(novtable) Renderer {
public:
	explicit Renderer(HWND window);
	virtual ~Renderer() = default;

	Renderer(const Renderer&) = delete; 
	Renderer(Renderer&&) = delete;

	Renderer& operator=(const Renderer&) = delete; 
	Renderer& operator=(Renderer&&) = delete;

	void Start(int threadPriority, uintptr_t affinityMask);
	void Shutdown();
	RenderTarget& GetTarget();
protected:
	template <typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<IBuilder, T>>>
	void InitBuilder(Args&&... args);
	virtual void RenderLoop() = 0;
	virtual void RenderFrame() = 0;
protected:
	std::thread               m_renderThread;
	std::atomic<bool>         m_running{ false };
	RenderTarget              m_renderTarget;
	std::unique_ptr<IBuilder> m_builder = nullptr;
};

template<typename T, typename... Args, typename>
inline void Renderer::InitBuilder(Args&&... args) {
	m_builder = std::make_unique<T>(std::forward<Args>(args)...);
}
