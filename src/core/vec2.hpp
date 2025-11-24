#pragma once

#include <cmath>

class float2 {
public:
	float x = 0;
	float y = 0;

	float2() = default;
	float2(float x, float y) : x(x), y(y) {}
	float2(const float2& other) : x(other.x), y(other.y) {}

	float2 operator+(const float2& other) const {
		return { x + other.x, y + other.y };
	}

	float2 operator-(const float2& other) const {
		return { x - other.x, y - other.y };
	}

	bool operator==(const float2& other) const {
		return x == other.x && y == other.y;
	}

	bool operator!=(const float2& other) const {
		return !(*this == other);
	}

	float2 operator*(float scalar) const {
		return { x * scalar, y * scalar };
	}

	float Length() const {
		return std::sqrt(x * x + y * y);
	}
};