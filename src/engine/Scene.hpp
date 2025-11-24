#pragma once

#include <core/PolymorphicBuffer.hpp>
#include <ui/components/UIComponents.hpp>
#include <ui/components/GameComponents.hpp>
#include <ui/components/Button.hpp>
#include <ui/components/TitleButton.hpp>
#include <ui/components/Component.hpp>
#include <unordered_map>
#include <type_traits>
#include <d2d1_2.h>

class Scene {
public:
	static constexpr size_t s_maxSize = MaxSizeOf<BgComponent, GridComponent, GameOverBgComponent, 
		TitleBarComponent, PreviewComponent, ScoreComponent, HighScoreComponent, GhostTetraminoView, 
		GameFieldView, ButtonComponent, TitleButtonComponent>::value;

	static constexpr size_t s_maxAlign = MaxAlignOf<BgComponent, GridComponent, GameOverBgComponent,
		TitleBarComponent, PreviewComponent, ScoreComponent, HighScoreComponent, GhostTetraminoView,
		GameFieldView, ButtonComponent, TitleButtonComponent>::value;

	explicit Scene(ID2D1DeviceContext1* context);
	template <typename U, typename... Args> requires std::is_base_of_v<I2DGraphicsComponent, U> && 
	(sizeof(U) <= Scene::s_maxSize && alignof(U) <= Scene::s_maxAlign)
	void Add(int id, Args&&... args);
	void UpdateCommandLists();
	template <typename T> requires std::is_base_of_v<I2DGraphicsComponent, T>
	T* GetById(int id);
	void DrawAll() const;
	void UpdateAll(float dt);
	void Clear();
private:
	ID2D1DeviceContext1* m_context;
	PolymorphicBuffer<I2DGraphicsComponent, s_maxSize, s_maxAlign> m_buffer{1024};
	std::unordered_map<size_t, int> m_lookupTable;
};

template <typename U, typename... Args> requires std::is_base_of_v<I2DGraphicsComponent, U> &&
(sizeof(U) <= Scene::s_maxSize && alignof(U) <= Scene::s_maxAlign)
inline void Scene::Add(int id, Args&&... args) {
	auto [index, ptr] = m_buffer.Allocate<U>(std::forward<Args>(args)...);
	m_lookupTable.emplace(id, index);
}

template <typename T> requires std::is_base_of_v<I2DGraphicsComponent, T>
inline T* Scene::GetById(int id) {
	auto it = m_lookupTable.find(id);
	if (it != m_lookupTable.end()) {
		return dynamic_cast<T*>(m_buffer.Get(it->second));
	}
	return nullptr;
}
