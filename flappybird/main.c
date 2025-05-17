#include <stdio.h>
#include "raylib.h"
#include <direct.h>           // _getcwd

typedef struct {
	int posX;
	int posY;
	int angle;
	int width;
	int height;
} Bird;

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

int checkColision(Bird bird, int floor) {
	int flag = 0;
	__asm {
		LEA eax, bird.posY
		MOV esi, [ eax ]
		ADD esi, bird.height


		CMP esi, floor
		JL fin
		MOV flag, 1

		fin: 
			NOP
	}
	return flag;
}

int main() {
	int monitor = GetCurrentMonitor();
	InitWindow(GetMonitorWidth(monitor), GetMonitorHeight(monitor), "Flappy Bird");

	// texturas
	Texture2D sprite = LoadTexture("debug/bird.png");
	Texture2D cloud1 = LoadTexture("debug/cloud.png");
	Texture2D cloud2 = LoadTexture("debug/cloud.png");
	Texture2D cloud3 = LoadTexture("debug/cloud.png");
	Texture2D cloud4 = LoadTexture("debug/cloud.png");
	Texture2D cloud5 = LoadTexture("debug/cloud.png");
	Texture2D floor = LoadTexture("debug/floor.png");
	floor.height = 100;
	floor.width = GetMonitorWidth(monitor);

	Bird bird;
	bird.posY = GetMonitorHeight(monitor) / 2;
	bird.posX = 100;
	bird.angle = 90;
	bird.height = sprite.height;
	bird.width = sprite.width;
	
	int jumpForce = 60;

	SetTargetFPS(40);

	int cloudX[] = { 100, 600, 1000, 1400 };
	int gameover = 0; 
	int points = 0;
	int floorYPosition = 0;

	int monitorHeight = GetMonitorHeight(monitor);
	__asm {
		LEA eax, floorYPosition
		MOV edx, monitorHeight
		MOV [ eax ], edx
		MOV edx, floor.height
		SUB [ eax ], edx

	}

	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(SKYBLUE);

		gameover = checkColision(bird, floorYPosition);
		__asm {
			MOV eax, gameover
			CMP eax, 0
			JE while_continue
		}
			break;
		__asm{
			while_continue:
				NOP
		}

		DrawTexture(cloud1, cloudX[0], 90, RAYWHITE);
		DrawTexture(cloud2, cloudX[1], 70, RAYWHITE);
		DrawTexture(cloud3, cloudX[2], 120, RAYWHITE);
		DrawTexture(cloud4, cloudX[3], 100, RAYWHITE);
		updateClouds(cloudX, GetMonitorWidth(monitor));
		//DrawTextureRe(sprite, source, (Vector2) { bird.posX,  bird.posY }, RAYWHITE);
		DrawTexture(sprite, bird.posX, bird.posY, RAYWHITE);
		DrawTexture(floor, 0, floorYPosition, RAYWHITE);

		__asm {
			LEA eax, bird.posY
			MOV ebx, [ eax ]


			ADD ebx, 3; gravedad
			MOV [ eax ], ebx
		}

		if (IsKeyPressed(KEY_SPACE)) {
			__asm {
				LEA eax, bird.posY
				MOV ebx, [ eax ]
				SUB ebx, jumpForce
				MOV [ eax ], ebx
			}
		}

		EndDrawing();
	}

	CloseWindow();
	
	return 0;
}