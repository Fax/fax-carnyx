#pragma once
#include <raylib.h>
#include <raymath.h>


Vector2 angleToVector(float angleDegrees) {

	float angleRadians = DEG2RAD * angleDegrees;
	return Vector2{ cosf(angleRadians), sinf(angleRadians) };
}