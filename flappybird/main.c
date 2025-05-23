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
typedef struct {
	int x;
	int y;
	int width;
	int height;
}tubo;

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
// funcion para un numero random del 0 al 6, por si acaso
/*int numeroRandom() {
	int cent = 0;
	_asm {
		rdtsc
		mov edx,0
		mov ecx, 7
		div ecx;
		mov eax, edx
		mov cent,eax;
	}
	return cent;
}*/
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
tubo crearTubo(tubo arr[],int pos,int x,int y,int width, int height) {
	tubo k;
	_asm {
		LEA eax, k.x
		mov edx,x
		mov [eax],edx
		Lea eax, k.y
		mov edx, y
		mov [eax],edx
		Lea eax, k.width
		mov edx, width
		mov [eax], edx
		Lea eax, k.height
		mov edx, height
		mov[eax], edx
	}
	arr[pos] = k;
}
void updateTubo(tubo arr[], int monitorWidth) {
	int size = sizeof(tubo);
	int aux = 0;
	int monitor = monitorWidth;
	_asm {
		mov ecx,8
		mov aux,0
		ciclo:
			mov eax,arr
			mov edx, aux
			imul edx,16
			add eax, edx
			mov esi,-120
			cmp [eax], esi
			je continuar
			mov edx,4
			sub[eax],edx
			jmp fin
				continuar:
				mov ebx, 2800
				mov [eax], ebx
				nop
			fin:
				nop
			inc aux
		loop ciclo
	}
}
// ver si el pajaro choco con un tubo pero no esta terminado
int verificarChoqueTubo(tubo arr[], Bird bird) {
	int bandera = 0;
	tubo alto;
	alto = arr[obtenerTuboAlto(arr, bird)];
	tubo bajo;
	bajo = arr[obtenerTuboBajo(arr, bird)];

	int flag1 = 0;
	int topColision = 0;
	int bottomColision = 0;
	__asm {
		MOV eax, bird.posX
		MOV edx, 0
		MOV ebx, 2
		DIV ebx
		MOV ebx, bird.width
		ADD eax, ebx

		CMP eax, alto.x
		JGE set_flag
		JMP fin_first_condition
		set_flag: 
			MOV flag1, 1

		fin_first_condition:
		MOV eax, bird.posX
		MOV ebx, alto.x
		ADD ebx, alto.width
		
		CMP eax, ebx
		JG unset_flag
		JMP check_top_colision
		unset_flag:
			MOV flag1, 0

		check_top_colision:
		MOV eax, bird.height
		MOV ebx, 2
		DIV ebx
		ADD eax, bird.posY

		CMP  alto.height, eax
		JG activate_top_colision
		MOV topColision, 0
		JMP check_bottom_colision
		activate_top_colision:
			MOV topColision, 1

		check_bottom_colision:
		MOV eax, bird.posY
		ADD eax, bird.height
		SUB eax, 20

		CMP eax, bajo.y
		JG activate_bottom_colision
		MOV bottomColision, 0
		JMP end_checks
		activate_bottom_colision:
			MOV bottomColision, 1

		end_checks:
		
		MOV eax, topColision
		OR eax, bottomColision
		AND eax, flag1
		
		CMP eax, 1
		JE activate_colision
		MOV bandera, 0
		JMP fin_de_condiciones
		activate_colision:
			MOV bandera, 1
		fin_de_condiciones:
			nop
	}

	// checar colision vertical
	//if (flag1 &&
	//	(topColision || bottomColision) 
	//	) {
	//	bandera = 1; 
	//}

	return bandera;
}

void scoreToString(int score, char scoreString[]) {
	__asm {
		MOV eax, score
		MOV ebx, 10
		LEA esi, scoreString
		convertir_char:
			MOV edx, 0
			DIV ebx
			MOV ecx, edx
			ADD ecx, '0'
			MOV [ esi ], cl

			ADD esi, 4
			CMP eax, 0 
		JNE convertir_char
		ADD [ esi ], 0
	}
	return;
}

int obtenerTuboAlto(tubo tubos[], Bird bird) {
	int closest = 0;
	for (int i = 0; i < 8; i++) {
		if (tubos[i].x <= tubos[closest].x && tubos[i].y == 0) {
			closest = i;
		}
	}
	return closest;
}

int obtenerTuboBajo(tubo tubos[], Bird bird) {
	int closest = 0;
	for (int i = 0; i < 8; i++) {
		if (tubos[i].x <= tubos[closest].x && tubos[i].y != 0) {
			closest = i;
		}
	}
	return closest;
}

int main() {
	int contador = 0;
	tubo arreglo[8] = {0};
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
	int score = 0;
	char scoreString[30] = { 0 };
	int floorYPosition = 0;

	int monitorHeight = GetMonitorHeight(monitor);
	__asm {
		LEA eax, floorYPosition
		MOV edx, monitorHeight
		MOV [ eax ], edx
		MOV edx, floor.height
		SUB [ eax ], edx
	}

	crearTubo(arreglo, 0, 700, 0, 200, 400);
	crearTubo(arreglo, 1, 700, GetMonitorHeight(monitor) - 400, 200, 400);
	crearTubo(arreglo, 4, 1400, 0, 200, 300);
	crearTubo(arreglo, 5, 1400, GetMonitorHeight(monitor) - 500, 200, 800);
	crearTubo(arreglo, 2, 2100, 0, 200, 600);
	crearTubo(arreglo, 3, 2100, GetMonitorHeight(monitor) - 200, 200, 400);
	crearTubo(arreglo, 6, 2800, 0, 200, 400);
	crearTubo(arreglo, 7, 2800, GetMonitorHeight(monitor) - 400, 200, 400);

	
	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(SKYBLUE);
		scoreToString(score, scoreString);
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
		DrawRectangle(arreglo[0].x, arreglo[0].y, arreglo[0].width, arreglo[0].height, DARKGREEN);
		DrawRectangle(arreglo[1].x, arreglo[1].y, arreglo[1].width, arreglo[1].height, DARKGREEN);
		DrawRectangle(arreglo[2].x, arreglo[2].y, arreglo[2].width, arreglo[2].height, DARKGREEN);
		DrawRectangle(arreglo[3].x, arreglo[3].y, arreglo[3].width, arreglo[3].height, DARKGREEN);
		DrawRectangle(arreglo[4].x, arreglo[4].y, arreglo[4].width, arreglo[4].height, DARKGREEN);
		DrawRectangle(arreglo[5].x, arreglo[5].y, arreglo[5].width, arreglo[5].height, DARKGREEN);
		DrawRectangle(arreglo[6].x, arreglo[6].y, arreglo[6].width, arreglo[6].height, DARKGREEN);
		DrawRectangle(arreglo[7].x, arreglo[7].y, arreglo[7].width, arreglo[7].height, DARKGREEN);
		DrawText(scoreString, GetMonitorWidth(monitor) / 2, GetMonitorHeight(monitor) / 2, FONT_DEFAULT, WHITE);

		DrawTexture(sprite, bird.posX, bird.posY, RAYWHITE);
		DrawTexture(floor, 0, floorYPosition, RAYWHITE);

		DrawRectangleLines(bird.posX, bird.posY, bird.width, bird.height, GREEN);
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
		updateTubo(arreglo, GetMonitorWidth(monitor));
		//solo para ver si jala la funcion (no jalo)
		int terminar = verificarChoqueTubo(arreglo,bird);
		if (terminar == 1) {
			break;
		}
		EndDrawing();
	}

	CloseWindow();
	
	return 0;
}