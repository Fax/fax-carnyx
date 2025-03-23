#pragma once

#include "Entities.h"
#include<vector>
#include <random>

struct GameOptions {
	int w = 800;
	int h = 600;



	int rows = 5;
	int cols = 10;
	Vector2 margins = Vector2(10, 10);
	Vector2 brickSize = Vector2(20, 10);
	float speedScale = 200;

	float scaleX = w / (brickSize.x * cols + margins.x * (cols + 2));
	float scaleY = (h / 2) / (brickSize.y * rows + margins.y * (rows + 2));
	Vector2 ballLaunchOrigin = { (float)w / 2,(float)h / 4 * 3 };


};

struct GameLoop {
public:
	GameOptions options;
	GameScore score;
	// random engine and distributions
	std::mt19937 gen;
	std::uniform_real_distribution<> dis;
	std::uniform_real_distribution<float> angleDist;
	// game objects
	std::vector<Brick> bricks;
	Vector2 scaledBrick;
	std::vector<Ball> balls;
	Player player;
	Sprites sprites;

	// and methods
	void init(GameOptions options);

	void tearDown();

	void initBricks();
	void initBalls();
	void initPlayer();
	void initSprites();
	void update(float dt);
	void draw(float dt);
	void input(float dt);
};