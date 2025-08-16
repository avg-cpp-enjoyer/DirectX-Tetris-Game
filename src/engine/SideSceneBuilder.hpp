#pragma once

#include "IBuilder.hpp"
#include "engine/RenderTarget.hpp"
#include "engine/Scene.hpp"
#include "ui/Constants.hpp"
#include "ui/components/UIComponents.hpp"

class SideSceneBuilder : public IBuilder {
public:
	SideSceneBuilder(int score, const RenderTarget& target, Scene& scene, HWND window);
	void BuildScene() override;
private:
	int m_score;
	const RenderTarget& m_renderTarget;
	Scene& m_scene;
	HWND m_window;
};