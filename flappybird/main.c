#include <stdio.h>
#include "raylib.h"

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
	int a = 5, b = 4;
	int resultado = sumar(a, b);
	printf("(%d + %d), %d",a,b,resultado);

	InitWindow(900, 600, "Flappy Bird");

	SetTargetFPS(144);

	while (!WindowShouldClose()) {
		BeginDrawing();
		
			ClearBackground(RAYWHITE);
			DrawText("HELLO WORLD!", 320, 200, 20, DARKGRAY);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}