#include <stdio.h>
#include "raylib.h"
#include <direct.h>           // _getcwd
#define MAX_INPUT_CHARS 64
#define MAX_PLAYERS 8
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
typedef struct {
	char name[MAX_INPUT_CHARS];
	int score;
}Player;   
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
int updateTubo(tubo arr[], int monitorWidth, int restado) {
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
			mov esi,-110
			cmp [eax], esi
			jle continuar
			mov edx, restado
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
	return restado;
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
		ADD eax, 20
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

	return bandera;
}

//if (currentIndex != prevTubeIndex) {
//	tubo tuboAlto = arreglo[currentIndex];

//	if (bird.posX > (tuboAlto.x + tuboAlto.width - 10)) {
//		prevTubeIndex = currentIndex;
//		score = score + 1;
//	}
//}

char* scoreToString(int score) {
	static char scoreString[50] = { 0 };
	int contador = 0;
	__asm {
		LEA ebx, score
		MOV eax, [ ebx ]
		MOV ebx, 10
		LEA esi, scoreString
		convertir_char:
			MOV edx, 0
			DIV ebx
			MOV ecx, edx
			ADD ecx, '0'
			PUSH ecx
			//MOV [ esi ], cl

			INC contador
			//INC esi
			CMP eax, 0 
		JNE convertir_char

		MOV ecx, contador

		agregar_char:
			MOV edx, 0
			POP edx
			MOV [ esi ], dl
			INC esi
		LOOP agregar_char

		MOV [ esi ], 0
	}
	return scoreString;
}

int obtenerTuboAlto(tubo tubos[], Bird bird) {
	int closest = 0;
	int aux = 0;
	_asm {
		mov ecx, 8
		mov aux, 0
		ciclo:
		mov eax, tubos
			mov edx, aux
			imul edx, 16
			add eax, edx
			mov ebx, tubos
			mov edx, closest
			imul edx, 16
			add ebx, edx
			mov edx, [eax]
			cmp edx, [ebx]
				jle continuar6
					jmp continuarCiclo
					continuar6 :
				nop
					mov eax, tubos
					mov edx, aux
					imul edx, 16
					add eax, edx
					mov ebx, 4
					add eax, ebx
					mov edx, [eax]
					cmp edx, 0
						je cambiarClosest
						jmp continuarCiclo
						cambiarClosest :
					mov ebx, aux
						mov closest, ebx
						continuarCiclo :
					nop
						inc aux
						loop ciclo
	}
	/*for (int i = 0; i < 8; i++) {
		if (tubos[i].x <= tubos[closest].x && tubos[i].y == 0) {
			closest = i;
		}
	}*/
	return closest;
}

int obtenerTuboBajo(tubo tubos[], Bird bird) {
	int closest = 0;
	int aux = 0;
	_asm {
		mov ecx,8
		mov aux,0
		ciclo:
		mov eax, tubos
		mov edx, aux
		imul edx,16
		add eax,edx
		mov ebx, tubos
		mov edx, closest
		imul edx,16
		add ebx, edx
		mov edx, [eax]
		cmp edx, [ebx]
		jle continuar6
		jmp continuarCiclo
		continuar6:
			nop
		mov eax, tubos
		mov edx, aux
		imul edx,16
		add eax, edx
		mov ebx, 4
		add eax, ebx
		mov edx, [eax]
		cmp edx, 0
		jne cambiarClosest
		jmp continuarCiclo
		cambiarClosest:
			mov ebx, aux
			mov closest, ebx
		continuarCiclo:
			nop
		inc aux
		loop ciclo
	}
	/*for (int i = 0; i < 8; i++) {
		if (tubos[i].x <= tubos[closest].x && tubos[i].y != 0) {
			closest = i;
		}
	}*/
	return closest;
}
void guardarJugador(Player j[], int puntuacion,int pos,char nombre[]) {
	int tamano = sizeof(Player);

	__asm {
		mov eax, j
		mov ebx, tamano
		imul ebx,pos
		add eax, ebx
		mov edx, nombre
		recorrerString:
			cmp [edx],0
			je salirAhora
			mov cl,[edx]
			mov bl, cl
			mov [eax], bl
			add edx,1
			add eax,1
			jmp recorrerString
		salirAhora:
			nop
		add eax,1
		mov [eax],'\0'
		mov eax, j
		mov ebx, tamano
		imul ebx, pos
		add eax, ebx
		add eax, 64
		mov ecx, puntuacion
		mov [eax], ecx
	}
}
int main() {
	int mostrarScore = 0;
	int nombreIntroducido = 0;
	int juegoIniciado=0;
	int factorResta = 4;
	int numJugador = 0;
	int maxPlayersReached = 0;

	tubo arreglo[8] = {0};
	Player players[MAX_PLAYERS] = {0};
	int monitor = GetCurrentMonitor();
	InitWindow(GetMonitorWidth(monitor), GetMonitorHeight(monitor), "Flappy Bird");
	// audio
	InitAudioDevice();

	Sound jumpSound = LoadSound("debug/jump_effect.mp3");
	Sound pointSound = LoadSound("debug/point_effect.mp3");
	Sound hitSound = LoadSound("debug/hit_effect.mp3");
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
	int tmpScore = 0;
	int floorYPosition = 0;
	int colisionWithTube = 0;
	int prevTubeIndex = -1;
	int currentIndex = 0;
	crearTubo(arreglo, 0, 700, 0, 200, 400);
	crearTubo(arreglo, 1, 700, GetMonitorHeight(monitor) - 400, 200, 400);
	crearTubo(arreglo, 4, 1400, 0, 200, 300);
	crearTubo(arreglo, 5, 1400, GetMonitorHeight(monitor) - 500, 200, 800);
	crearTubo(arreglo, 2, 2100, 0, 200, 600);
	crearTubo(arreglo, 3, 2100, GetMonitorHeight(monitor) - 200, 200, 400);
	crearTubo(arreglo, 6, 2800, 0, 200, 400);
	crearTubo(arreglo, 7, 2800, GetMonitorHeight(monitor) - 400, 200, 400);

	int monitorHeight = GetMonitorHeight(monitor);
	__asm {
		LEA eax, floorYPosition
		MOV edx, monitorHeight
		MOV [ eax ], edx
		MOV edx, floor.height
		SUB [ eax ], edx
	}

	//configuracion para leadboard
	int leadboardWidth = 600;
	int leadboardHeight = 600;
	int leadboardX = GetMonitorWidth(monitor) / 2 - leadboardWidth / 2;
	int leadboardY = GetMonitorHeight(monitor) / 2 - leadboardHeight / 2 - 100;

	int inputX = GetMonitorWidth(monitor) / 2 - 40;
	int inputY = GetMonitorHeight(monitor) / 2 - 40;

	char name[MAX_INPUT_CHARS + 1] = "\0";  
	int letterCount = 0;
	Player ranking[3] = { 0 };
	while (!WindowShouldClose()) {

		__asm {
			JMP start_draw
			reset_game:
				MOV juegoIniciado, 0
				MOV gameover, 0
				MOV score, 0
				MOV colisionWithTube, 0
				MOV prevTubeIndex, -1
				MOV currentIndex, 0
				MOV edx, 0
				MOV eax, monitorHeight
				MOV ebx, 2
				DIV ebx
				MOV bird.posY, eax
		}
		crearTubo(arreglo, 0, 700, 0, 200, 400);
		crearTubo(arreglo, 1, 700, GetMonitorHeight(monitor) - 400, 200, 400);
		crearTubo(arreglo, 4, 1400, 0, 200, 300);
		crearTubo(arreglo, 5, 1400, GetMonitorHeight(monitor) - 500, 200, 800);
		crearTubo(arreglo, 2, 2100, 0, 200, 600);
		crearTubo(arreglo, 3, 2100, GetMonitorHeight(monitor) - 200, 200, 400);
		crearTubo(arreglo, 6, 2800, 0, 200, 400);
		crearTubo(arreglo, 7, 2800, GetMonitorHeight(monitor) - 400, 200, 400);
		__asm{
			start_draw:
				nop
		}
		int key = GetCharPressed();
		
		while (key > 0||(sizeof(name)<0))
		{
			if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
			{
				if (key != KEY_SPACE && key != KEY_ENTER) {
					name[letterCount] = (char)key;
					name[letterCount + 1] = '\0';
					letterCount++;
				}
				else {
					__asm {
						mov mostrarScore, 0
					}
				}
			}

			key = GetCharPressed();
		}

		if (IsKeyPressed(KEY_BACKSPACE))
		{
			letterCount--;
			if (letterCount < 0) letterCount = 0;
			name[letterCount] = '\0';
		}

		BeginDrawing();
		ClearBackground(SKYBLUE);

		gameover = checkColision(bird, floorYPosition);
		colisionWithTube = verificarChoqueTubo(arreglo, bird);
		__asm {
			MOV eax, gameover
			OR eax, colisionWithTube
			CMP eax, 0
			JE while_continue
			mov nombreIntroducido,0
			mov mostrarScore,1
		}
		tmpScore = score;
		__asm {
			MOV eax, numJugador
			CMP eax, MAX_PLAYERS
			JE activate_max_players_reached
			add_new_player:
				nop
		}
		guardarJugador(players, score, numJugador, name);
		numJugador++;
		__asm {
			JMP less_than_the_last
			activate_max_players_reached:

				LEA esi, players
				
				MOV ecx, MAX_PLAYERS
				DEC ecx

				reach_final_player:
					ADD esi, 68
				LOOP reach_final_player
				ADD esi, 64
				MOV eax, score
				MOV ebx, [ esi ]

				CMP eax, ebx
				JG change_for_last
				JMP less_than_the_last
				change_for_last:
					DEC numJugador
					JMP add_new_player
				less_than_the_last:
					nop	
		}

		Player auxPlayer;
		auxPlayer.score = 0;
		for (int i = 0; i < MAX_INPUT_CHARS; i++) {
			auxPlayer.name[i] = '\0';
		}


		int scoreAAuxiliar = 0, scoreBAuxiliar = 0;
		int loopExterior = MAX_PLAYERS;
		int loopInterior = MAX_PLAYERS - 1;
		__asm {
			MOV ecx, MAX_PLAYERS
			MOV loopExterior, ecx

			ciclo_ordenamiento_exterior :
				MOV ECX, MAX_PLAYERS
				MOV loopInterior, ecx

				ciclo_ordenamiento_interior :
					LEA esi, players
					MOV eax, MAX_PLAYERS
					SUB eax, loopInterior

					IMUL eax, 68
					ADD esi, eax

					LEA edi, players
					MOV eax, MAX_PLAYERS
					INC eax
					SUB eax, loopInterior
					IMUL eax, 68
					ADD edi, eax

					CMP edi, 0
					JE final_ordenamiento

					MOV eax, [esi + 64]
					MOV scoreAAuxiliar, eax
					MOV eax, [edi + 64]
					CMP eax, scoreAAuxiliar
				JG swap_elements

				continue_ordenamiento_loop :
			DEC loopInterior
				MOV ecx, loopInterior

				LOOP ciclo_ordenamiento_interior
				DEC loopExterior
				MOV ecx, loopExterior

				LOOP ciclo_ordenamiento_exterior
				JMP final_ordenamiento

				swap_elements :
					LEA esi, players
					MOV eax, MAX_PLAYERS
					SUB eax, loopInterior

					IMUL eax, 68
					ADD esi, eax

					MOV ecx, 64
					MOV eax, 0

					LEA ebx, auxPlayer

					MOV ecx, 64
				copiar_nombre_actual_a_aux:
					MOV al, [esi]
					MOV[ebx], al
					INC esi
					INC ebx

				LOOP copiar_nombre_actual_a_aux

					LEA esi, players
					MOV eax, MAX_PLAYERS
					SUB eax, loopInterior

					IMUL eax, 68
					ADD esi, eax

					LEA ebx, auxPlayer

					ADD ebx, 64
					ADD esi, 64
					;copiar score
					MOV edx, 0
					MOV edx, [esi]
					MOV[ebx], edx

					;copiar siguiente en anterior
					LEA esi, players
					MOV eax, MAX_PLAYERS
					SUB eax, loopInterior

					IMUL eax, 68
					ADD esi, eax

					LEA edi, players
					MOV eax, MAX_PLAYERS
					INC eax
					SUB eax, loopInterior
					IMUL eax, 68
					ADD edi, eax

					MOV ecx, 64
					copiar_nombre_siguiente_a_anterior:
						MOV al, [edi]
						MOV [esi], al
						INC esi
						INC edi
					LOOP copiar_nombre_siguiente_a_anterior

					;copiar score
					MOV edx, 0
					MOV edx, [ edi ]
					MOV[ esi ], edx

					;copiar auxiliar a siguiente

					LEA ebx, auxPlayer

					LEA edi, players
					MOV eax, MAX_PLAYERS
					INC eax
					SUB eax, loopInterior
					IMUL eax, 68
					ADD edi, eax

					MOV ecx, 64
					copiar_nombre_auxiliar_a_siguiente:
						MOV al, [ebx]
						MOV [edi], al
						INC edi
						INC ebx
					LOOP copiar_nombre_auxiliar_a_siguiente

					;copiar score
					MOV edx, 0
					MOV edx, [ ebx ]
					MOV[ edi ], edx

			JMP continue_ordenamiento_loop

			final_ordenamiento :
			nop
		}

		__asm {
			lea eax, name
			ciclar:
				cmp [eax], 0
				je salirCiclo
				mov [eax],'\0'
				add eax,1
				jmp ciclar
			salirCiclo:
				nop
			mov letterCount,0
		}
			SetSoundPitch(hitSound, 2.0f);
			PlaySound(hitSound);
			

			__asm {
				JMP reset_game
			}
			break;
		__asm{
			while_continue:
				NOP
		}
		currentIndex = obtenerTuboAlto(arreglo, bird);
		__asm {
			MOV eax, currentIndex
			CMP eax, prevTubeIndex
			JE continue_checking
			JNE check_if_get_point

			check_if_get_point:
				LEA ebx, arreglo
				MOV ecx, eax
				shl ecx, 4
				add ebx, ecx

				MOV edx, [ ebx ]
				ADD edx, [ ebx + 8 ]
				SUB edx, 4

				CMP bird.posX, edx
				JG give_point
				JMP continue_checking
				give_point:
					INC score
					MOV eax, currentIndex
					MOV prevTubeIndex, eax
		}
		SetSoundPitch(pointSound, 2.0f);
		PlaySound(pointSound);
		__asm {
			continue_checking:
				NOP
		}
				
		DrawTexture(cloud1, cloudX[0], 90, RAYWHITE);
		DrawTexture(cloud2, cloudX[1], 70, RAYWHITE);
		DrawTexture(cloud3, cloudX[2], 120, RAYWHITE);
		DrawTexture(cloud4, cloudX[3], 100, RAYWHITE);
		updateClouds(cloudX, GetMonitorWidth(monitor));
		//DrawTextureRe(sprite, source, (Vector2) { bird.posX,  bird.posY }, RAYWHITE);
		DrawRectangle(arreglo[0].x, arreglo[0].y, arreglo[0].width,arreglo[0].height, DARKGREEN);
		DrawRectangle(arreglo[1].x, arreglo[1].y, arreglo[1].width, arreglo[1].height, DARKGREEN);
		DrawRectangle(arreglo[2].x, arreglo[2].y, arreglo[2].width, arreglo[2].height, DARKGREEN);
		DrawRectangle(arreglo[3].x, arreglo[3].y, arreglo[3].width, arreglo[3].height, DARKGREEN);
		DrawRectangle(arreglo[4].x, arreglo[4].y, arreglo[4].width, arreglo[4].height, DARKGREEN);
		DrawRectangle(arreglo[5].x, arreglo[5].y, arreglo[5].width, arreglo[5].height, DARKGREEN);
		DrawRectangle(arreglo[6].x, arreglo[6].y, arreglo[6].width, arreglo[6].height, DARKGREEN);
		DrawRectangle(arreglo[7].x, arreglo[7].y, arreglo[7].width, arreglo[7].height, DARKGREEN);
		DrawTexture(sprite, bird.posX, bird.posY, RAYWHITE);
		DrawTexture(floor, 0, floorYPosition, RAYWHITE);

		DrawText(scoreToString(score), GetMonitorWidth(monitor) / 2, GetMonitorHeight(monitor) / 2, 48, WHITE);
		__asm {
			mov ecx, mostrarScore
			cmp ecx, 1
			jne saltarScore
		}
		DrawRectangle(leadboardX, leadboardY + 200, leadboardWidth, leadboardHeight, BLACK);
		char buffer[128] = "TU SCORE: ";
		strcat(buffer, scoreToString(tmpScore));
		DrawText(buffer, leadboardX + leadboardWidth / 2 - 100, 350, 30, WHITE);

		int x2 = leadboardX + 20;
		int y2 = 390;
		int rank = 1;
		int recorrer = MAX_PLAYERS;
		char playerName[64] = {0};
		int playerScore = 0;




		//typedef struct {
		//	char name[MAX_INPUT_CHARS];
		//	int score;
		//}Player;
		 
		
		// recorrer ranking para dibujo
		__asm {


			recorrer_ranking:
				LEA esi, players
				MOV eax, rank
				DEC eax
				MOV edx, 0
				IMUL eax, 68
				ADD esi, eax


				LEA edx, playerName

				MOV edi, esi
				copiar_nombre_de_jugador :

						MOV bl, [edi]
						CMP bl, 0
						JE copiar_score
						MOV[edx], bl

						INC edi
						INC edx
						JMP copiar_nombre_de_jugador

						copiar_score:
					LEA edi, [esi]
						ADD edi, 64
						LEA  edx, playerScore
						MOV ebx, [edi]

						MOV[edx], ebx
	}
		char pos[3] = { rank + '0','-','\0' };
		DrawText(pos, x2, y2, 40, WHITE);
		DrawText(playerName, x2 + 50, y2, 40, WHITE);
		char aux[2] = { playerScore + '0','\0' };
		DrawText(aux, x2 + 500, y2, 40, WHITE);
	__asm{
			ADD y2, 50
			INC rank
			MOV playerScore, 0

			MOV ecx, 64
			LEA edx, playerName
			vaciar_player_name: 
				MOV [edx], 0
				INC edx
			LOOP vaciar_player_name
			

			CMP recorrer, 0
			DEC recorrer
			JNE recorrer_ranking
	}
		//for (int i = 0; i < 8; i++) {
		//	char pos[3] = { rank + '0','-','\0' };
		//	DrawText(pos, x2, y2, 40, WHITE);
		//	DrawText(playerName, x2 + 50, y2, 40, WHITE);
		//	char aux[2] = { playerScore + '0','\0' };
		//	DrawText(aux, x2 + 500, y2, 40, WHITE);
		//	y2 = y2 + 50;
		//	rank = rank + 1;
		//	recorrer = recorrer + 1;
		//}

		__asm {
		saltarScore:
			nop
		}
		//DrawRectangleLines(bird.posX, bird.posY, bird.width, bird.height, GREEN);
		__asm {
			mov ecx, nombreIntroducido
			cmp ecx, 1
			je saltarTexto
		}

		DrawRectangle(leadboardX, leadboardY, leadboardWidth, 200, BLACK);
		DrawText("Introduce tu usuario: ", leadboardX + 20, leadboardY + 20, 30, WHITE);
		DrawRectangle(leadboardX + 10, leadboardY + 80, leadboardWidth - 20, 50, WHITE);
		DrawText(name, leadboardX + 20, leadboardY + 80, 30, MAROON);
		DrawText("Pulsa enter o espacio para comenzar", leadboardX + 20, leadboardY + 160, 25, WHITE);

		__asm {
		saltarTexto:
			nop
		}

		while (((IsKeyPressed(KEY_SPACE)) == false && IsKeyPressed(KEY_ENTER) == false) && !juegoIniciado) {
			__asm {
				jmp terminarDibujo
			}
		}
		__asm {
			mov juegoIniciado, 1
			mov nombreIntroducido, 1
		}

		// verificacion nombre
		__asm {
			lea eax, name
			mov ecx, 3
			ciclo:
				mov edx,[eax]
				cmp dl, 0
				je caracterNuloEncontrado
				add eax,1
			loop ciclo 
			jmp conclusion
			caracterNuloEncontrado:
				MOV juegoIniciado,0
				MOV nombreIntroducido, 0
				jmp terminarDibujo
			conclusion:
				nop
		}

		__asm {
			LEA eax, bird.posY
			MOV ebx, [ eax ]


			ADD ebx, 3; gravedad
			MOV [ eax ], ebx
		}
		
		if (IsKeyPressed(KEY_SPACE)) {
			SetSoundPitch(jumpSound, 2.0f);
			PlaySound(jumpSound);
			__asm {
				LEA eax, bird.posY
				MOV ebx, [ eax ]
				SUB ebx, jumpForce
				MOV [ eax ], ebx
			}
		}
		_asm {
			// aumenta la velocidad del 10 al 20 restando 6
			mov eax, score
			cmp eax, 10
			jge continuar
			jmp terminar2
			continuar:
				nop
			cmp eax, 20
			jb cambiar
			jmp terminar2
			cambiar:
				mov ecx, 6
				mov factorResta, ecx
				jmp fin3
			terminar2:
				nop
			//del 20 al 32 restando 8
			cmp eax,20
			jge continuar2
			jmp terminar3
			continuar2:
				nop
			cmp eax, 32
			jb cambiar2
			jmp terminar3
			cambiar2:
				mov ecx,8
				mov factorResta, ecx
				jmp fin3
			terminar3:
				nop
			// de 50 en adelante se le restara 9
			cmp eax, 50
			jg cambiar3
			jmp fin3
			cambiar3:
				mov ecx,9
				mov factorResta,ecx
		}
		_asm {
			fin3:
				nop
		}
		int x = updateTubo(arreglo, GetMonitorWidth(monitor),factorResta);
		factorResta = x;
		__asm {
		terminarDibujo:
			nop
		}
		EndDrawing();
	}
	UnloadSound(jumpSound);
	UnloadSound(hitSound);
	UnloadSound(pointSound);
	CloseAudioDevice();
	CloseWindow();
	
	return 0;
}