#pragma once

#include <d2d1_1.h>
#include <d2d1_2.h>
#include <wrl/client.h>

class __declspec(novtable) I2DGraphicsComponent {
public:
	explicit I2DGraphicsComponent(ID2D1DeviceContext1* context);
	virtual ~I2DGraphicsComponent() = default;
	virtual void Draw() const = 0;
	virtual void Update(float) {}
	ID2D1CommandList* GetCommandList() const;
	void ResetCommandList();
	void SetRedraw(bool status);
	bool NeedsRedraw() const;
protected:
	Microsoft::WRL::ComPtr<ID2D1CommandList> m_commandList;
	ID2D1DeviceContext1* m_context;
	mutable bool m_needsRedraw = true;
};