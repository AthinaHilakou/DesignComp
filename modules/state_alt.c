#include <stdlib.h>
#include <stdio.h>
#include "ADTMap.h"
#include "ADTSet.h"
#include "state.h"
#include "set_utils.c"

List list_update;

typedef struct portal_pair {
	Object entrance;		// η πύλη entrance
	Object exit;			// οδηγεί στην exit
}* PortalPair;

int compare_port(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}


int compare_obj(Pointer a, Pointer b) {
	Object obj1 = (Object)a;
    Object obj2 = (Object)b;
    return obj1->rect.x - obj2->rect.x;
	//return *(int*)a - *(int*)b;
}

int* create_int(int value) {
	int* pointer = malloc(sizeof(int));	
	*pointer = value;						
	return pointer;
}

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));
 
	// Γενικές πληροφορίες
	state->info.current_portal = 1;			// Δεν έχουμε περάσει καμία πύλη
	state->info.wins = 0;					// Δεν έχουμε νίκες ακόμα
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.

	// Πληροφορίες για το χαρακτήρα.
	Object character = state->info.character = malloc(sizeof(*character));
	character->type = CHARACTER;
	character->forward = true;
	character->jumping = false;

	character->rect.width = 38;
	character->rect.height = 70;
	character->rect.x = 0;
	character->rect.y = SCREEN_HEIGHT - character->rect.height; 

	state->objects = set_create(compare_obj, free);
	state->portal_pairs = map_create(compare_port, free, free);

	for (int i = 0; i < 4*PORTAL_NUM; i++) {
 
 		Object obj = malloc(sizeof(*obj));

		if(i % 4 == 3) {							// Το 4ο, 8ο, 12ο κλπ αντικείμενο
			obj->type = PORTAL;						// είναι πύλη.
			obj->rect.width = 100;
			obj->rect.height = 20;
		}
		else if(rand() % 2 == 0) {				    // Για τα υπόλοιπα, με πιθανότητα 50%
			obj->type = OBSTACLE;					// επιλέγουμε εμπόδιο.
			obj->rect.width = 10;
			obj->rect.height = 60;
		} 
		else {
			obj->type = ENEMY;						// Και τα υπόλοιπα είναι εχθροί.
			obj->rect.width = 30;
			obj->rect.height = 30;
			obj->forward = false;					// Οι εχθροί αρχικά κινούνται προς τα αριστερά.
		}

		obj->rect.x = (i+1) * SPACING;                
		obj->rect.y = SCREEN_HEIGHT - obj->rect.height;
		
		//printf("%d, %f\n",obj->type,obj->rect.x);
		set_insert(state->objects, obj);
    }

	int t[PORTAL_NUM];  
    for(int i = 0; i < PORTAL_NUM; i++) {    
		
		PortalPair pair = malloc(sizeof(*pair));
		pair->entrance = (Object)(create_int(i+1));
		
		int k = rand() % PORTAL_NUM + 1;  
		int j = 0;
		while(j < 100) {
			if(k == t[j]) { k = rand() % PORTAL_NUM + 1;   j = 0;}	
			else	j++;
		}
		pair->exit = (Object)(create_int(k));
		printf("%d %d\n", *(int*)(pair->entrance),*(int*)(pair->exit));
		t[i] = k;
		map_insert(state->portal_pairs, pair->entrance, pair->exit);
	}

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {				
	return &(state->info); 
}

List state_objects(State state, float x_from, float x_to) {			
	List list = list_create(free);
	
	Object obj = malloc(sizeof(*obj));

	for(float x = x_from; x <= x_to; x++) {
		obj->rect.x = x;
		// Object object = set_find(state->objects, obj);
		//printf("%d ", list_size(list_update));
		Object object = list_find(list_update, obj, compare_obj);
		if(object != NULL) {
			list_insert_next(list, list_last(list), object);
			//printf("%d: %f\n",object->type, object->rect.x);
		}
			
	}
 	// printf("%d",list_size(list));
	return list;
}

float* create_float(float value) {
	float* pointer = malloc(sizeof(float));	
	*pointer = value;						
	return pointer;
}



int forward = 1, f = 1, up = 0, flag2 = 1;

void state_update(State state, KeyState keys) {	
	
	if(state->info.character->rect.x >= SCREEN_WIDTH/2 ) {
		state->info.character->rect.x = SCREEN_WIDTH/2;
	}
	else if(state->info.character->rect.x < SCREEN_WIDTH/2 && forward == -1){
		state->info.character->rect.x = SCREEN_WIDTH/2;
	}
	if(keys->enter == false && keys->left == false && keys->right == false && keys->up == false && keys->n == false && keys->p == false ){
		state->info.character->rect.x += 7*(forward);
		
		if(state->info.character->rect.y > 220 && up == -1)
			state->info.character->rect.y += 15*(up);
		else{
			up = 0;
			if(state->info.character->rect.y < SCREEN_HEIGHT - state->info.character->rect.height )
				state->info.character->rect.y += 15;
		}	
	}	
	else if(keys->right != false) {
		if(forward == -1) {
			state->info.character->forward = true;
			forward = 1;
		}
	}		
	else if(keys->left != false){
		forward = -1;
		state->info.character->forward = false;

	}
	else if(keys->up != false && (state->info.character->rect.y == SCREEN_HEIGHT - state->info.character->rect.height)){
		up = -1;
	}
	
	if(flag2 == 1) {
		printf("--------\n");
		list_update = list_create(free);
		for(SetNode node = set_first(state->objects);        
			node != SET_EOF;                       
			node = set_next(state->objects, node)) {
			
			Object temp = set_node_value(state->objects, node);
			float x  = temp->rect.x;
			Object obj_to_insert = malloc(sizeof(*obj_to_insert));
			obj_to_insert->rect.x = x; 
			obj_to_insert->rect.y = temp->rect.y;
			obj_to_insert->type = temp->type;
			obj_to_insert->forward = temp->forward;
			list_insert_next(list_update, list_last(list_update), obj_to_insert);
		}
	}
	flag2 = 0;

	for(ListNode node = list_first(list_update);       
		node != LIST_EOF;                         
		node = list_next(list_update, node)) {
		
		//printf("%f ", ((Object)(list_node_value(list_update, node)))->rect.x);	

		int flag = 0;
		Object t_obj = list_node_value(list_update, node);
		//printf("%d ", t_obj->type);

		if(t_obj->type == ENEMY) {
			
			// if(CheckCollisionRecs(t_obj->rect, state->info.character->rect) == 1){ 
			// 	state->info.playing = false;
			// 	exit(1);
			// }

			if(t_obj->forward == true)	f = -1;
			else 	f = 1;
							
			Object existing_object = malloc(sizeof(*existing_object));

			if(t_obj->forward == false) {
				
				existing_object = (Object)set_find_eq_or_smaller(state->objects, t_obj);
				
				if(existing_object != NULL){
					//printf("%d %f,", existing_object->type, existing_object->rect.x);
					if((CheckCollisionRecs(existing_object->rect, t_obj->rect) == 1) && (existing_object->type == OBSTACLE)) {	
						printf("1 ");					
						flag = 1;
						f = f*(-1);
						t_obj->forward = !(t_obj->forward); 
						//printf("-%f %f\n",existing_object->rect.x, t_obj->rect.x);
						//exit(1);
						//set_remove(state->objects, obj);
						//obj->rect.x += 5;
						//set_insert(state->objects, obj);
						//exit(1);
					}
					else {
						t_obj->rect.x -= 5;
					}
				}
				else {
					t_obj->rect.x -= 5;
				}	
			}
			else {
				
				//Object existing_object = malloc(sizeof(*existing_object));
				existing_object = (Object)set_find_eq_or_greater(state->objects, t_obj);
				
				if(existing_object != NULL){
					if(CheckCollisionRecs(existing_object->rect, t_obj->rect) == 1 && (existing_object->type == OBSTACLE)) {	
						printf("2 ");
						flag = 1;
						f = f*(-1);					
						t_obj->forward = !(t_obj->forward); 
						//printf("--%f %f\n",existing_object->rect.x, obj->rect.x);
						//exit(1);
						//set_remove(state->objects, obj);
						//obj->rect.x += 5;
						//set_insert(state->objects, obj);
						//exit(1);
					}
					else {
						t_obj->rect.x += 5;
					}
				}
				else {
					t_obj->rect.x += 5;
				}
			}
			if(flag == 1) {
				Object to_remove = (Object)set_find(state->objects, t_obj);
				set_remove(state->objects, to_remove);
				to_remove->rect.x -= 5*f;
				set_insert(state->objects, to_remove);
			}
			//free(existing_object);		

		}
		else if(t_obj->type == OBSTACLE) {
			t_obj->rect.x -= 7*forward;
			// if(CheckCollisionRecs(state->info.character->rect, t_obj->rect) == 1){
			// 	state->info.playing = false;
			// 	exit(1);				
			// }
		}
		else if(t_obj->type == PORTAL){
			
			t_obj->rect.x -= 7*forward; 
			
			if((CheckCollisionRecs(t_obj->rect, state->info.character->rect) == 1)) { 
				
				PortalPair pair = malloc(sizeof(*pair));
				pair->entrance = (Object)(create_int(state->info.current_portal));    
				
				MapNode map_node = map_find_node(state->portal_pairs, pair->entrance);
				if(map_node != MAP_EOF) {
					state->info.current_portal = *(int*)map_node_value(state->portal_pairs, map_node);
					if((pair->exit) == (Object)create_int(100)) {
						state->info.wins += 1;
						state->info.playing = false;
					}
				}

				if(state->info.character->forward == false) {
					pair->entrance = (Object)(create_int(state->info.current_portal));    
				
					MapNode map_node = map_find_node(state->portal_pairs, pair->entrance);
					if(map_node != MAP_EOF) {
						state->info.current_portal = *(int*)map_node_value(state->portal_pairs, map_node);
						if((pair->exit) == (Object)create_int(100)) {
							state->info.wins += 1;
							state->info.playing = false;
						}
					}
				}		
				
			}

		}
		//printf("%f ", t_obj->rect.x);

	}
	//printf("\n\n");
	
}

void state_destroy(State state) {
	set_destroy(state->objects);
	map_destroy(state->portal_pairs);
	list_destroy(list_update);
	free(state);
}