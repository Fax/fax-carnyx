// fax-carnyx-cli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#if defined(Emscripten)
#include <emscripten.h>
#endif
#include <iostream>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include <random>

#include "Entities.h"
#include "Loop.h"


Vector2 AngleToVector(float angleDegrees) {

	float angleRadians = DEG2RAD * angleDegrees;
	return Vector2{ cosf(angleRadians), sinf(angleRadians) };
}

void AddScore(GameScore& score) {
	score.score++;
}
void AddBounce(GameScore& score) {
	score.bounces++;
}
int main() {
	GameOptions options;
	GameLoop loop;

	InitWindow(options.w, options.h, "Carnyx");

	SetTargetFPS(60);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);


	loop.init(options);
	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		loop.input(dt);
		loop.update(dt);
		BeginDrawing();
		ClearBackground(LIGHTGRAY);
		loop.draw(dt);
		DrawFPS(10, 10);
		EndDrawing();
	}
	loop.tearDown();
	CloseWindow();

}
