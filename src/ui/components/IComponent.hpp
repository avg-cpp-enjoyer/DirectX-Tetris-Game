#pragma once

#include <d2d1_2.h>

interface __declspec(novtable) IComponent {
	virtual ~IComponent() = default;
	virtual void Draw() const = 0;
};