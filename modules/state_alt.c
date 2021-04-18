#include <stdlib.h>
#include <stdio.h>
#include "ADTMap.h"
#include "ADTSet.h"
#include "state.h"
#include "set_utils.c"

List list_update;

struct state {
	Set objects;			// περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες)
	Map portal_pairs;		// περιέχει PortalPair (ζευγάρια πυλών, είσοδος/έξοδος)

	struct state_info info;
};

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
}

int* create_int(int value) {
	int* pointer = malloc(sizeof(int));	
	*pointer = value;						
	return pointer;
}

void state_init(State state) {
	// Γενικές πληροφορίες
	(state_info(state))->current_portal = 1;			// Δεν έχουμε περάσει καμία πύλη
	(state_info(state))->wins = 0;					// Δεν έχουμε νίκες ακόμα
	(state_info(state))->playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	(state_info(state))->paused = false;				// Χωρίς να είναι paused.

	// Πληροφορίες για το χαρακτήρα.
	Object character = (state_info(state))->character = malloc(sizeof(*character));
	character->type = CHARACTER;
	character->forward = true;
	character->jumping = false;

	character->rect.width = 38;
	character->rect.height = 70;
	character->rect.x = 0;
	character->rect.y = SCREEN_HEIGHT - character->rect.height;  	
	
	state->objects = set_create(compare_obj, NULL);

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
		
		set_insert(state->objects, obj);
    }
}

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));
	state_init(state);

	state->portal_pairs = map_create(compare_port, free, free);
	srand(0);

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
		Object object = set_find(state->objects, obj);
		if(object != NULL) 
			list_insert_next(list, list_last(list), object);	
	}
	return list;
}

float* create_float(float value) {
	float* pointer = malloc(sizeof(float));	
	*pointer = value;						
	return pointer;
}



int forward = 1, f, up = 0, flag = 1, fast;

void state_update(State state, KeyState keys) {	
	
	if(flag == 1) {                                    // αρχικοποιηση της λιστας πανω στην οποια θα γινει το iteration
		list_update = list_create(free);
		for(SetNode node = set_first(state->objects);        
			node != SET_EOF;                       
			node = set_next(state->objects, node)) {
			
			Object temp = set_node_value(state->objects, node);
			Object obj_to_insert = malloc(sizeof(*obj_to_insert));
			obj_to_insert->rect.x = temp->rect.x; 
			obj_to_insert->rect.y = temp->rect.y;
			obj_to_insert->type = temp->type;
			obj_to_insert->forward = temp->forward;
			
			list_insert_next(list_update, list_last(list_update), obj_to_insert);
		}
		flag = 0;		
	}
	
	if((keys->enter == true) && ((state_info(state))->playing == false) ){
		set_destroy(state->objects);
		state_init(state);
	}

	if((keys->p == true) && ((state_info(state))->paused == true)) {
		(state_info(state))->paused = false;
		return;
	}

	if((state_info(state))->playing == true && (state_info(state))->paused == false) {	
		fast = 1;
		if((state_info(state))->character->rect.x >= SCREEN_WIDTH/3) {
			(state_info(state))->character->rect.x = SCREEN_WIDTH/3;
		}
		else if((state_info(state))->character->rect.x < SCREEN_WIDTH/3 && forward == -1){
			(state_info(state))->character->rect.x = SCREEN_WIDTH/3;
		}
		if(keys->enter == false && keys->left == false && keys->right == false && keys->up == false && keys->n == false && keys->p == false ){
			(state_info(state))->character->rect.x += 7*(forward);
			
			if((state_info(state))->character->rect.y > 220 && up == -1)
				(state_info(state))->character->rect.y += 15*(up);
			else{
				up = 0;
				if((state_info(state))->character->rect.y < SCREEN_HEIGHT - (state_info(state))->character->rect.height )
					(state_info(state))->character->rect.y += 15;
			}	
		}	
		else if(keys->right != false) {
			if(forward == -1) {
				(state_info(state))->character->forward = true;
				forward = 1;
			}
			else
			fast = 2;
		}		
		else if(keys->left != false){
			forward = -1;
			(state_info(state))->character->forward = false;

		}
		else if(keys->up != false && ((state_info(state))->character->rect.y == SCREEN_HEIGHT - (state_info(state))->character->rect.height)){
			up = -1;
		}
		else if(keys->p != false){
			(state_info(state))->paused = true;
		}
		
		// θα ενημερωσω τα οbjects της λιστας, θα κοιταξω μεσω των set_find του set_utils
		// αν προκειται να υπαρξει collision και στο τελος θα κανω remove/ insert στο set
		for(ListNode node = list_first(list_update);        
			node != LIST_EOF;                       
			node = list_next(list_update, node)) {
			
			Object obj = list_node_value(list_update, node);

			if(obj->type == ENEMY) {
				
				if(obj->forward == false)
					f = 1;
				else 
					f = -1;	

				// συγκρουση χαρακτηρα με εχθρο
				if(CheckCollisionRecs(obj->rect, (state_info(state))->character->rect)){ 
					(state_info(state))->playing = false;
					return;
				}

				// ψαχνω το προηγουμενο και το επομενο αντικειμενο
				Object greater_obj = (Object)set_find_eq_or_greater(state->objects, obj);
				Object smaller_obj = (Object)set_find_eq_or_smaller(state->objects, obj);

				if(smaller_obj != NULL && greater_obj != NULL) {
			
					Object temp = malloc(sizeof(*temp));
					
					// κοιταω αν θα υπαρξει collision  αν προχωρησει
					temp->rect.x = obj->rect.x - 5*f;       
					if(CheckCollisionRecs(temp->rect, smaller_obj->rect)) {
						obj->forward = !(obj->forward);
						return;
					}

					temp->rect.x = obj->rect.x - 5*f;
					if(CheckCollisionRecs(temp->rect, smaller_obj->rect)) {
						obj->forward = !(obj->forward);
						return;
					}
				}
				obj->rect.x -= 5*f;
			}
			else if(obj->type == OBSTACLE) {
				
				obj->rect.x -= 7*forward;  
				
				// συγκρουση χαρακτηρα με εμποδιο
				if(CheckCollisionRecs(obj->rect, (state_info(state))->character->rect)){ 
					(state_info(state))->playing = false;
					return;
				}  
			}
			else if(obj->type == PORTAL){
				
				obj->rect.x -= 7*forward;
				
				// συγκρουση χαρακτηρα με πυλη
				if((CheckCollisionRecs(obj->rect, (state_info(state))->character->rect))) { 
					
					PortalPair pair = malloc(sizeof(*pair));
					pair->entrance = (Object)(create_int((state_info(state))->current_portal));    
					
					MapNode map_node = map_find_node(state->portal_pairs, pair->entrance);
					if(map_node != MAP_EOF) {
						(state_info(state))->current_portal = *(int*)map_node_value(state->portal_pairs, map_node);
						if((pair->exit) == (Object)create_int(100)) {
							(state_info(state))->wins += 1;
							(state_info(state))->playing = false;
						}
					}

					if((state_info(state))->character->forward == false) {
						pair->entrance = (Object)(create_int((state_info(state))->current_portal));    
					
						MapNode map_node = map_find_node(state->portal_pairs, pair->entrance);
						if(map_node != MAP_EOF) {
							(state_info(state))->current_portal = *(int*)map_node_value(state->portal_pairs, map_node);
							if((pair->exit) == (Object)create_int(100)) {
								(state_info(state))->wins += 1;
								(state_info(state))->playing = false;
							}
						}
					}		
					
				}
			}
		}
		
		// αλλαγη της διαταξης του set
		ListNode lnode = list_first(list_update);
		for(SetNode node = set_first(state->objects);        
			node != SET_EOF;                       
			node = set_next(state->objects, node)) {
			
			set_remove(state->objects, set_node_value(state->objects, node));
			set_insert(state->objects, list_node_value(list_update, lnode));	
			lnode = list_next(list_update, lnode);
		}
	}
	
}

void state_destroy(State state) {
	set_destroy(state->objects);
	map_destroy(state->portal_pairs);
	list_destroy(list_update);
	free(state);
}