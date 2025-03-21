// fax-carnyx-cli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include <random>

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
Vector2 AngleToVector(float angleDegrees) {

	float angleRadians = DEG2RAD * angleDegrees;
	return Vector2{ cosf(angleRadians), sinf(angleRadians) };
}
int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	std::uniform_real_distribution<float> angleDist(-90 - 45, -90 + 45);


	float speedScale = 200;
	int w = 800;
	int h = 600;
	InitWindow(w, h, "Carnyx");

	SetTargetFPS(60);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	GameScore score;

	int cols = 10;
	int rows = 5;
	Vector2 margins = Vector2(10, 10);

	Vector2 brickSize = Vector2(20, 10);
	float scaleX = w / (brickSize.x * cols + margins.x * (cols + 2));
	float scaleY = (h / 2) / (brickSize.y * rows + margins.y * (rows + 2));



	std::vector<Brick> bricks;

	bricks.clear();
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			Brick brick;
			brick.alive = true; // just to be sure
			brick.position.x = (margins.x + col * (brickSize.x + 10)) * scaleX;
			brick.position.y = (margins.y + row * (brickSize.y + 10)) * scaleY; // spacing 10px, height 10px
			if (dis(gen) < 0.2)
			{
				brick.color = BLUE;
				brick.life = 2;
			}
			bricks.push_back(brick);
		}
	}
	Vector2 ballLaunchOrigin = { (float)w / 2,(float)h / 4 * 3 };
	score.currentAngle = angleDist(gen);
	Vector2 ballLaunchDirection = Vector2Scale(AngleToVector(score.currentAngle), speedScale);
	std::vector<Ball> balls;
	{
		Ball b;
		b.position = ballLaunchOrigin;
		b.velocity = ballLaunchDirection;
		balls.push_back(b);
	}

	Player player;
	player.position = { (w / 2) - player.size.x / 2, (h / 8 * 7) - player.size.y / 2 };




	while (!WindowShouldClose()) {
		float dt = GetFrameTime();
		Vector2 scaledBrick = Vector2Multiply(brickSize, Vector2(scaleX, scaleY));
		BeginDrawing();
		ClearBackground(LIGHTGRAY);

		for (const Brick& brick : bricks) {
			if (brick.alive)
			{
				DrawRectangleV(brick.position, scaledBrick, brick.color);
			}
		}
		for (Ball& ball : balls) {

			if ((ball.position.x < 5 && ball.velocity.x < 0) || ball.position.x > w - 5) {
				ball.velocity.x *= -1;
				score.bounces++;
			}
			if ((ball.position.y < 5 && ball.velocity.y < 0) || ball.position.y > h - 5) {
				ball.velocity.y *= -1;
				score.bounces++;
			}

			if (ball.position.y > h - 20) {

				ball.position = ballLaunchOrigin;
				score.currentAngle = angleDist(gen);
				ball.velocity = Vector2Scale(AngleToVector(score.currentAngle), speedScale);
				score.losses++;
			}

			if (ball.position.x<0 || ball.position.x>w || ball.position.y > h || ball.position.y < 0) {
				ball.position = ballLaunchOrigin;
				score.currentAngle = angleDist(gen);
				ball.velocity = Vector2Scale(AngleToVector(score.currentAngle), speedScale);
				score.losses++;

			}
			else {


				// now: Collisions?
				// get player bounding box
				// Rectangle r = { player.position.x,player.position.y,player.size.x,player.size.y };
				if (ball.position.y + 5 >= player.position.y && ball.position.x - 5 >= player.position.x && ball.position.x + 5 <= player.position.x + player.size.x) {

					// bounce
					ball.velocity.y *= -1;
					score.bounces++;
				}

				// first of all, we can hit the boxes if we are in the top area of hte screen
				if (ball.position.y <= (float)h / 2) {
					for (Brick& brick : bricks) {
						if (!brick.alive)continue;
						// for each frame I have to check all of them!
						// we can only hit 1 of them at the time, so if we find it, we get out
						// this means that if nothing happens, this loop is more expensive :|
						Rectangle r = {
							brick.position.x ,
							brick.position.y ,
							scaledBrick.x ,
							scaledBrick.y };
						// if the center is in this rectangle + radius, we have a collision
						if (CheckCollisionCircleRec(ball.position, 5.0, r)) {
							brick.life--;

							if (CheckCollisionCircleLine(ball.position, 5.0, brick.position, { brick.position.x, brick.position.y + scaledBrick.y })) {
								ball.velocity.x *= -1;
							}
							else if (CheckCollisionCircleLine(ball.position, 5.0, brick.position, { brick.position.x + scaledBrick.x,brick.position.y })) {
								ball.velocity.y *= -1;
							}
							else if (CheckCollisionCircleLine(ball.position, 5.0, { brick.position.x,brick.position.y + scaledBrick.y }, { brick.position.x + scaledBrick.x,brick.position.y + scaledBrick.y })) {
								ball.velocity.y *= -1;
							}
							else {
								ball.velocity.x *= -1;
							}


						}


						if (brick.life == 0) {
							brick.alive = false;
						}

					}
				}
				ball.position.x += dt * ball.velocity.x;
				ball.position.y += dt * ball.velocity.y;
			}


			DrawCircleV(ball.position, 10, ball.color);
		}

		// check user position

		if (IsKeyDown(KEY_RIGHT)) {
			player.position.x += dt * player.speed;
		}
		if (IsKeyDown(KEY_LEFT)) {
			player.position.x -= dt * player.speed;
		}


		DrawRectangleV(player.position, player.size, player.color);
		DrawText(TextFormat("B: %i L: %i S: %i A:%f", score.bounces, score.losses, score.score, score.currentAngle), 300, 10, 35, BLACK);

		DrawFPS(10, 10);
		EndDrawing();
	}

	CloseWindow();
}


