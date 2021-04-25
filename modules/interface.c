#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include "state.h"
#include "interface.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

Texture mario_img;
Texture mario2_img;
Texture enemy_img;


void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game");
	SetTargetFPS(60); 

	mario_img = LoadTextureFromImage(LoadImage("../../mario.png"));
	mario2_img = LoadTextureFromImage(LoadImage("../../mario2.png"));
	enemy_img = LoadTextureFromImage(LoadImage("../../enemy.png"));
}

void interface_close() {
	CloseWindow();
}


static int wins = 0;

void interface_draw_frame(State state) {
	BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(SKYBLUE);
		
	int x1,x2;
	if((state_info(state))->character->rect.x < (SCREEN_WIDTH/3)) { 
		x1 = 0;
		x2 = SCREEN_WIDTH;
	}
	else {
		x1 = (state_info(state))->character->rect.x - SCREEN_WIDTH/3;
		x2 = (state_info(state))->character->rect.x + 10 + SCREEN_WIDTH*2/3;
	}
	
	DrawRectangle(0, 435, 800, 15, DARKGREEN);
	
	List list = state_objects(state, x1 ,x2);        // θα μου δωσει τα αντικειμενα στο frame που θελω καθε φορα 

	for(ListNode node = list_first(list);          
    	node != LIST_EOF;                          
    	node = list_next(list, node)) {            

		Object obj = list_node_value(list, node);
		if(obj->type == OBSTACLE) 
			DrawRectangle(obj->rect.x, obj->rect.y, obj->rect.width, obj->rect.height, MAROON);
		if(obj->type == ENEMY)
			DrawTexture(enemy_img,obj->rect.x , obj->rect.y, WHITE);
		if(obj->type == PORTAL)
			DrawRectangle(obj->rect.x, obj->rect.y , obj->rect.width, obj->rect.height, SKYBLUE);
	}

	// Σχεδιάζουμε τον χαρακτήρα
	if((state_info(state))->character->forward == true)
		DrawTexture(mario_img, (state_info(state))->character->rect.x, (state_info(state))->character->rect.y, WHITE);
	else 
		DrawTexture(mario2_img, (state_info(state))->character->rect.x, (state_info(state))->character->rect.y, WHITE);


	
	DrawFPS(SCREEN_WIDTH - 80, 0);
	
    
	DrawText(TextFormat("PORTAL %03i", (state_info(state))->current_portal), 20, 20, 40, GRAY);
	DrawText(TextFormat("WINS %01i", (state_info(state))->wins), 20 , 60, 40, GRAY);
	
	if((state_info(state))->wins > wins) {
		wins++;
		DrawText(
			"YOU WON! PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("YOU WON! PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}
	else if (!(state_info(state))->playing) {
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
			 GetScreenHeight() / 2 - 50, 20, GRAY
		);
	}
	EndDrawing();
}