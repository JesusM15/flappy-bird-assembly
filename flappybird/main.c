#include <stdio.h>
#include "raylib.h"
#include <direct.h>           // _getcwd

void updateClouds(int cloudsX[], int monitorWidth) {
	// aqui habia problemad de overflow
	__asm {
		MOV edi, cloudsX
		mov ecx, 4

		ciclo:
			MOV ebx, monitorWidth
			MOV eax, [ edi ]
			ADD eax , 4

			CMP eax, ebx
			JAE reset
			JMP siguiente

			reset:
				MOV eax, 0
			siguiente:
			MOV [ edi ], eax
			ADD edi, 4
		loop ciclo
	}
}

int main() {
	int monitor = GetCurrentMonitor();
	int monitorWidth = GetMonitorWidth(monitor);
	InitWindow(GetMonitorWidth(monitor), GetMonitorHeight(monitor), "Flappy Bird");

	// texturas
	Texture2D bird = LoadTexture("debug/bird.png");
	Texture2D cloud1 = LoadTexture("debug/cloud.png");
	Texture2D cloud2 = LoadTexture("debug/cloud.png");
	Texture2D cloud3 = LoadTexture("debug/cloud.png");
	Texture2D cloud4 = LoadTexture("debug/cloud.png");
	Texture2D cloud5 = LoadTexture("debug/cloud.png");
	
	int birdPosY = 200;
	int birdJump = 40;

	int speedY = 0;
	float angleDeg = 0;


	SetTargetFPS(40);

	int cloudX[] = { 100, 600, 1000, 1400 };

	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(SKYBLUE);

		DrawTexture(cloud1, cloudX[0], 90, RAYWHITE);
		DrawTexture(cloud2, cloudX[1], 70, RAYWHITE);
		DrawTexture(cloud3, cloudX[2], 120, RAYWHITE);
		DrawTexture(cloud4, cloudX[3], 100, RAYWHITE);
		updateClouds(cloudX, GetMonitorWidth(monitor));

		DrawTexture(bird, 100, birdPosY, RAYWHITE);

		__asm {
			MOV eax, birdPosY
			INC eax; gravedad

			MOV birdPosY, eax
		}

		if (IsKeyPressed(KEY_SPACE)) {
			__asm {
				MOV eax, birdPosY
				SUB eax, birdJump
				MOV birdPosY, eax
			}
		}

		EndDrawing();
	}

	CloseWindow();
	
	return 0;
}