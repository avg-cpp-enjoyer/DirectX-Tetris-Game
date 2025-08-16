#pragma once

#include "engine/GraphicsDevice.hpp"

#include <d2d1_1.h>
#include <wrl/client.h>

class __declspec(novtable) Component {
public:
	explicit Component(ID2D1DeviceContext1* context);
	virtual ~Component() = default;
	virtual void Draw() const = 0;
	ID2D1CommandList* GetCommandList() const;
	void ResetCommandList();
	void SetRedraw(bool status);
	bool NeedsRedraw() const;
protected:
	Microsoft::WRL::ComPtr<ID2D1CommandList> m_commandList;
	ID2D1DeviceContext1* m_context;
	mutable bool m_needsRedraw = true;
};