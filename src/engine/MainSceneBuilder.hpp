#pragma once

#include "IBuilder.hpp"
#include "controller/GameController.hpp"
#include "ui/components/TitleButton.hpp"


class MainSceneBuilder : public IBuilder {
public:
	MainSceneBuilder(
		const RenderTarget& target, ActiveScene& activeScene, Scene& gameScene, 
		Scene& uiScene, const GameField& field, HWND window
	);
	void BuildScene() override;
private:
	Scene&              m_gameScene;
	Scene&              m_uiScene;
	ActiveScene&        m_activeScene;
	const RenderTarget& m_renderTarget;
	const GameField&    m_gameField;
	HWND                m_window;
};