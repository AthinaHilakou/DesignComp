#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include "state.h"
#include "interface.h"
//#include "state_alt.c"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 460

Texture mario_img;
Texture mario2_img;
Texture enemy_img;
//Sound game_over_snd;


void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
	SetTargetFPS(60);    
	//InitAudioDevice();

	mario_img = LoadTextureFromImage(LoadImage("../modules/assets/mario.png"));
	mario2_img = LoadTextureFromImage(LoadImage("../modules/assets/mario2.png"));
	enemy_img = LoadTextureFromImage(LoadImage("../modules/assets/enemy.png"));
	//game_over_snd = LoadSound("../modules/assets/game_over.mp3");

}

void interface_close() {
	//CloseAudioDevice();
	CloseWindow();
}


static int wins = 0;

void interface_draw_frame(State state) {
	BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(SKYBLUE);
	
	int x1,x2;
	if(state->info.character->rect.x < (SCREEN_WIDTH/2)) { 
		x1 = 0;; 
		x2 = SCREEN_WIDTH;
	}
	else {
		x1 = state->info.character->rect.x - SCREEN_WIDTH/2;
		x2 = state->info.character->rect.x + 10 + SCREEN_WIDTH/2;
	}

	List list = state_objects(state, x1 ,x2);        // θα μου δωσει τα αντικειμενα στο frame που θελω καθε φορα 

	for(ListNode node = list_first(list);          
    	node != LIST_EOF;                          
    	node = list_next(list, node)) {            

		Object obj = list_node_value(list, node);
		//printf("type %d ", obj->type);
		if(obj->type == OBSTACLE) {
			DrawRectangle(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, RED);
			// printf("y %f ", obj->rect.y);
		}
		if(obj->type == ENEMY)
			DrawTexture(enemy_img,obj->rect.x , obj->rect.y, WHITE);
			//DrawRectangle(, , obj->rect.width, obj->rect.height, WHITE);
		if(obj->type == PORTAL)
			DrawRectangle(obj->rect.x, obj->rect.y + 10, obj->rect.width, obj->rect.height, GRAY);

	}

	// Σχεδιάζουμε τον χαρακτήρα
	//DrawRectangle(state->info.character->rect.x, state->info.character->rect.y, state->info.character->rect.width, state->info.character->rect.height, BLACK);
	if(state->info.character->forward == true)
		DrawTexture(mario_img, state->info.character->rect.x, state->info.character->rect.y, WHITE);
	else 
		DrawTexture(mario2_img, state->info.character->rect.x, state->info.character->rect.y, WHITE);


	DrawRectangle(0, 445, 800, 15, DARKGREEN);
	DrawFPS(SCREEN_WIDTH - 80, 0);
	DrawText(TextFormat("PORTAL %03i", state->info.current_portal), 20, 20, 40, GRAY);
	DrawText(TextFormat("WINS %01i", state->info.wins), 20 , 60, 40, GRAY);
	
	if(state->info.wins > wins) {
		wins++;
		DrawText(
			"YOU WON! PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("YOU WON! PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}
	else if (!state->info.playing) {
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
		//PlaySound(game_over_snd);
	}

	EndDrawing();
}