#pragma once

#include <raylib.h>

struct Brick {
public:
	Color color = RED;
	Vector2 position;
	int life = 1;
	bool alive = true;
};

struct Ball {
public:
	Color color = WHITE;
	Vector2 position = { 0,0 };
	Vector2 velocity = { 150,-150 };

};

struct Player {
public:
	Color color = ORANGE;
	Vector2 size = { 100,50 };
	Vector2 position = { 0,0 };
	float speed = 300;
};

struct GameScore {
public:
	int score = 0;
	int losses = 0;
	int bounces = 0;
	float currentAngle = 0.0;
};


struct Sprites {
	Texture2D brick;
	Rectangle brickRect;
	Texture2D player;
	Rectangle playerRect;
	Texture2D ball;
	Rectangle ballRect;

	void init() {
		brick = LoadTexture("resources/brick.png");
		brickRect = { 0.0f, 0.0f, (float)brick.width, (float)brick.height };
		player = LoadTexture("resources/bat.png");
		playerRect = { 0.0f, 0.0f, (float)player.width, (float)player.height };
		ball = LoadTexture("resources/ball.png");
		ballRect = { 0.0f, 0.0f, (float)ball.width, (float)ball.height };
	}
};