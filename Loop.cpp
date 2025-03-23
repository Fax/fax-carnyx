#include "Loop.h"
#include <random>
#include <raymath.h>

#include "Utils.h"

void GameLoop::init(GameOptions opt) {
	options = opt;
	std::random_device rd;
	gen = std::mt19937(rd());
	dis = std::uniform_real_distribution<>(0.0, 1.0);
	angleDist = std::uniform_real_distribution<float>(-90 - 45, -90 + 45);



	initSprites();
	initBricks();
	initBalls();
	initPlayer();
}

void GameLoop::tearDown() {
	UnloadTexture(sprites.brick);
	UnloadTexture(sprites.player);
	UnloadTexture(sprites.ball);
}

void GameLoop::initBricks() {
	bricks.clear();
	for (int row = 0; row < options.rows; ++row) {
		for (int col = 0; col < options.cols; ++col) {
			Brick brick;
			brick.alive = true; // just to be sure
			brick.position.x = (options.margins.x + col * (options.brickSize.x + 10)) * options.scaleX;
			brick.position.y = (options.margins.y + row * (options.brickSize.y + 10)) * options.scaleY; // spacing 10px, height 10px
			if (dis(gen) < 0.2)
			{
				brick.color = BLUE;
				brick.life = 2;
			}
			bricks.push_back(brick);
		}
	}

	scaledBrick = Vector2Multiply(options.brickSize, Vector2(options.scaleX, options.scaleY));
}
void GameLoop::initBalls() {
	score.currentAngle = angleDist(gen);
	Vector2 ballLaunchDirection = Vector2Scale(angleToVector(score.currentAngle), options.speedScale);
	Ball b;
	b.position = options.ballLaunchOrigin;
	b.velocity = ballLaunchDirection;
	balls.push_back(b);
}


void GameLoop::initPlayer() {
	player.position = { (options.w / 2) - player.size.x / 2, (options.h / 8 * 7) - player.size.y / 2 };

}
void GameLoop::initSprites() {
	sprites.init();
}

void GameLoop::input(float dt) {
	if (IsKeyDown(KEY_RIGHT)) {
		player.position.x += dt * player.speed;
	}
	if (IsKeyDown(KEY_LEFT)) {
		player.position.x -= dt * player.speed;
	}
}

void addScore(GameScore& score) {
	score.score++;
}
void addBounce(GameScore& score) {
	score.bounces++;
}


void GameLoop::update(float dt) {
	for (Ball& ball : balls) {

		if ((ball.position.x < 5 && ball.velocity.x < 0) || ball.position.x > options.w - 5) {
			ball.velocity.x *= -1;
			addBounce(score);
		}
		if ((ball.position.y < 5 && ball.velocity.y < 0) || ball.position.y > options.h - 5) {
			ball.velocity.y *= -1;
			addBounce(score);
		}

		if (ball.position.y > options.h - 20) {

			ball.position = options.ballLaunchOrigin;
			score.currentAngle = angleDist(gen);
			ball.velocity = Vector2Scale(angleToVector(score.currentAngle), options.speedScale);
			score.losses++;
		}

		if (ball.position.x<0 || ball.position.x>options.w || ball.position.y > options.h || ball.position.y < 0) {
			ball.position = options.ballLaunchOrigin;
			score.currentAngle = angleDist(gen);
			ball.velocity = Vector2Scale(angleToVector(score.currentAngle), options.speedScale);
			score.losses++;

		}
		else {


			// now: Collisions?
			// get player bounding box
			// Rectangle r = { player.position.x,player.position.y,player.size.x,player.size.y };
			if (ball.position.y + 5 >= player.position.y
				&& ball.position.x - 5 >= player.position.x && ball.position.x + 5 <= player.position.x + player.size.x) {

				// bounce
				ball.velocity.y *= -1;
				addBounce(score);

			}

			// first of all, we can hit the boxes if we are in the top area of hte screen
			if (ball.position.y <= (float)options.h / 2) {
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
						addScore(score);

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
	}
}
void GameLoop::draw(float dt) {

	// draw bricks
	for (const Brick& brick : bricks) {
		if (brick.alive)
		{
			DrawTexturePro(sprites.brick, sprites.brickRect, { brick.position.x,brick.position.y,scaledBrick.x,scaledBrick.y }, Vector2Zero(), 0, brick.color);
			//DrawRectangleV(brick.position, scaledBrick, brick.color);
		}
	}

	// ball

	for (const Ball& ball : balls) {
		DrawCircleV(ball.position, 10, GREEN);
		DrawTexturePro(sprites.ball, sprites.ballRect, { ball.position.x - 5,ball.position.y - 5,10,10 }, Vector2Zero(), 0, ball.color);
	}


	// draw player
	DrawTexturePro(sprites.player, sprites.playerRect, { player.position.x,player.position.y ,player.size.x,player.size.y }, Vector2Zero(), 0, player.color);
	// draw hud
	DrawText(TextFormat("B: %i L: %i S: %i A:%f", score.bounces, score.losses, score.score, score.currentAngle), 200, 10, 20, BLACK);

}

