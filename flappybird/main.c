#include <stdio.h>
#include "raylib.h"
#include <direct.h>           // _getcwd

int sumar(int a, int b) {
	int result;
	__asm {
		MOV eax, a
		ADD eax, b
		MOV result, eax
	};
	return result;
}

int main() {
	InitWindow(900, 600, "Flappy Bird");
	Texture2D bird = LoadTexture("debug/bird.png");
	Texture2D cloud1 = LoadTexture("debug/cloud.png");
	Texture2D cloud2 = LoadTexture("debug/cloud.png");
	Texture2D cloud3 = LoadTexture("debug/cloud.png");
	Texture2D cloud4 = LoadTexture("debug/cloud.png");
	
	int birdPosY = 200;

	SetTargetFPS(40);

	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(SKYBLUE);

		DrawTexture(cloud1, 40, 30, RAYWHITE);
		DrawTexture(cloud2, 300, 40, RAYWHITE);
		DrawTexture(cloud3, 560, 30, RAYWHITE);
		DrawTexture(cloud4, 800, 10, RAYWHITE);

		DrawTexture(bird, 100, birdPosY, RAYWHITE);

		__asm {
			MOV eax, birdPosY
			INC eax
			MOV birdPosY, eax
		}

		if (IsKeyPressed(KEY_SPACE)) {
			__asm {
				MOV eax, birdPosY
				SUB eax, 40
				MOV birdPosY, eax
			}
		}
		EndDrawing();
	}

	CloseWindow();
	
	return 0;
}