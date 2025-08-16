#pragma once

#include <d2d1.h>

interface __declspec(novtable) IBuilder {
	virtual ~IBuilder() = default;
	virtual void BuildScene() = 0;
};