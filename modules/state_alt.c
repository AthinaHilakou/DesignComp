
#include <stdlib.h>
#include <stdio.h>
#include "ADTList.h"
#include "ADTMap.h"
#include "ADTSet.h"
#include "state.h"


typedef struct portal_pair {
	Object entrance;		// η πύλη entrance
	Object exit;			// οδηγεί στην exit
}* PortalPair;


int compare_obj(Pointer a, Pointer b) {
    Object obj1 = (Object)a;
    Object obj2 = (Object)b;
	// if(obj1->rect.x == obj2->rect.x)
	//  	return -1;
    return obj2->rect.x - obj1->rect.x;
}

int compare_port(Pointer a, Pointer b) {
	Object entr1 = (Object)a;
	Object entr2 = (Object)b;
	//return *(int*)entr - *(int*)entr2;
	//PortalPair portal1 = (PortalPair)a;
	//PortalPair portal2 = (PortalPair)b;
	//printf("%d %d\n",entr1, entr2);
	return entr2 - entr1;//*(int*)(portal1->entrance) - *(int*)(portal2->entrance);
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

	state->objects = set_create(compare_obj, NULL);
	state->portal_pairs = map_create(compare_port, NULL, NULL);

	for (int i = 0; i < 4*PORTAL_NUM; i++) {
 
 		Object obj = malloc(sizeof(*obj));

		if(i % 4 == 3) {							// Το 4ο, 8ο, 12ο κλπ αντικείμενο
			obj->type = PORTAL;						// είναι πύλη.
			obj->rect.width = 100;
			obj->rect.height = 5;
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
     
	// Object obj = malloc(sizeof(*obj)); 
	// for(SetNode node = set_first(state->objects);        
    // 	node != SET_EOF;                       
    // 	node = set_next(state->objects, node)) {          
		
	// 	obj = set_node_value(state->objects, node);
	// 	printf("%d ",obj->type);
	// }	

	int t[PORTAL_NUM];  
    for(int i = 0; i < PORTAL_NUM; i++) {    
		PortalPair pair = malloc(sizeof(*pair));
		Object entrance = malloc(sizeof(*entrance));
		Object exit = malloc(sizeof(*exit));

		pair->entrance = (Pointer)(i+1);
		int k = rand() % PORTAL_NUM + 1;  
		int j = 0;
		while(j < 100) {
			if(k == t[j]) { k = rand() % PORTAL_NUM + 1;   j = 0;}	
			else	j++;
		}
		pair->exit = (Pointer)k;
		t[i] = k;
		//printf("%d %d ",pair->entrance, pair->exit); 		
		map_insert(state->portal_pairs, (Pointer)pair->entrance, (Pointer)pair->exit);
		//printf("%d %d\n",pair->entrance ,map_find(state->portal_pairs, (Pointer)pair->entrance));
	}
	
	//printf("\n%d\n", map_size(state->portal_pairs));

	// int i = 1;
	// for(MapNode node = map_first(state->portal_pairs);          // ξενικάμε από τον πρώτο κόμβο
    // 	node != MAP_EOF;                        // μέχρι να φτάσουμε στο EOF
    // 	node = map_next(state->portal_pairs, node)) {           // μετάβαση στον επόμενο κόμβο
		
	// 	PortalPair pair = malloc(sizeof(*pair));
	// 	Object entrance = malloc(sizeof(*entrance));
	// 	pair->entrance = (Pointer)i;
	// 	//MapNode nodee = map_find_node(state->portal_pairs, (Pointer)pair->entrance);
	// 	//printf("%d\n", map_node_value(state->portal_pairs, nodee));
	// 	printf("%d %d\n",pair->entrance ,map_find(state->portal_pairs, (Pointer)pair->entrance));
	// 	i++;
	// }

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {				
	return &(state->info); 
}

List state_objects(State state, float x_from, float x_to) {			
	List list = list_create(free);
	
	Object obj = malloc(sizeof(*obj));
	// for(SetNode node = set_first(state->objects);          // ξενικάμε από τον πρώτο κόμβο
    // 	node != SET_EOF;                        // μέχρι να φτάσουμε στο EOF
    // 	node = set_next(state->objects, node)) {           // μετάβαση στον επόμενο κόμβο
		
	// 	obj = set_node_value(state->objects, node);
	// }

	for(float x = x_from; x <= x_to; x++) {
		obj->rect.x = x;
		Object object = set_find(state->objects, obj);
		if(object != NULL) {
			list_insert_next(list, list_last(list), object);
			//printf("%d: %f\n",object->type, object->rect.x);
		}
			
	}
 	//printf("%d",list_size(list));
	return list;
}

static int forward = 1, up = 0, f=1 ;

void state_update(State state, KeyState keys) {		
	// character update
	if((keys->enter != false) && (state->info.playing == false) ){
		state->info.playing = true;
		state->info.character->rect.x = 0; 
	}
	if(state->info.playing == true){
		if(state->info.character->rect.x >= SCREEN_WIDTH/2 && forward == 1) {
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
				if(state->info.character->rect.y < SCREEN_HEIGHT - 70 )
					state->info.character->rect.y += 15;
			}	
		}	
		else if(keys->right != false) {
			if(forward == 1)	state->info.character->rect.x += 12;
			else forward = 1;
		}		
		else if(keys->left != false){
			forward = -1;
		}
		else if(keys->up != false && (state->info.character->rect.y == SCREEN_HEIGHT - state->info.character->rect.height)){
			up = -1;
		} 
		else if(keys->p != false){
			if(state->info.playing == true)	state->info.playing = false;
			else 	state->info.playing = true;
			//if(keys->n != false)
		}

		// List list = state_objects(state, 0, PORTAL_NUM*700);
		// for(ListNode node = list_first(list);         
   		// 	node != LIST_EOF;                         
    	// 	node = list_next(list, node)) {          
			
		// 	Object obj = list_node_value(state->objects, node);//obj->rect.x - SCREEN_WIDTHobj->rect.x + SCREEN_WIDTH
		// 	if(obj->type == ENEMY) {
		// 		List list2 = state_objects(state, 0, PORTAL_NUM*700);
		// 		for(ListNode node2 = list_first(list);         
   		// 			node2 != LIST_EOF;                         
    	// 			node = list_next(list, node2)) {          
					
		// 			Object obj2 = list_node_value(state->objects, node);		
		// 			if((CheckCollisionRecs(obj2->rect, obj->rect) == 1) && (obj2->type == OBSTACLE)) {
		// 				obj->forward = true;
		// 				f = -1;
		// 			}
		// 		}
		// 		if(CheckCollisionRecs(obj->rect, state->info.character->rect) == 1){ 
		// 			state->info.playing = false;
		// 			//printf("LOST");
		// 		}	                                     
		// 		else {
		// 			obj->rect.x -= 5*f;
		// 		}	

		// 	}
		// 	else if(obj->type == OBSTACLE) {
		// 		// συγκρουση εμποδιο με χαρακτηρα
		// 		obj->rect.x -= 7*forward;
		// 		if(CheckCollisionRecs(state->info.character->rect, obj->rect) == 1){
		// 			state->info.playing = false;
					
		// 		}									  
		// 	}
		// 	else if(obj->type == PORTAL) {  // συγκρουση εμποδιου ή εχθρου με πυλη 
		// 		obj->rect.x -= 7*forward;
		// 		if((CheckCollisionRecs(obj->rect, state->info.character->rect) == 1)) { 
		// 			forward = -1;
		// 		}
		// 	}
		// }


		for(SetNode node = set_first(state->objects);        
    		node != SET_EOF;                       
    		node = set_next(state->objects, node)) {          
		
			Object obj = set_node_value(state->objects, node);
			if(obj->type == ENEMY) {
				
				for(SetNode node2 = set_first(state->objects);         
    				node2 != SET_EOF;                       
    				node2 = set_next(state->objects, node2)) {         
					
					Object obj2 = set_node_value(state->objects, node2);
					if((CheckCollisionRecs(obj2->rect, obj->rect) == 1) && (obj2->type == OBSTACLE)) {
					 	obj->forward = true;
					 	f = -1;
					}
				}
				if(CheckCollisionRecs(obj->rect, state->info.character->rect) == 1){ 
					state->info.playing = false;
				}	                                     
				else {
					obj->rect.x -= 5*f;
				}
					
			}
			else if(obj->type == OBSTACLE) {
				// συγκρουση εμποδιο με χαρακτηρα
				obj->rect.x -= 7*forward;
				if(CheckCollisionRecs(state->info.character->rect, obj->rect) == 1){
					state->info.playing = false;					
				}									  
			}
			else if(obj->type == PORTAL) {  // συγκρουση εμποδιου ή εχθρου με πυλη 
				obj->rect.x -= 7*forward;    
				if((CheckCollisionRecs(obj->rect, state->info.character->rect) == 1)) { 
					forward = -1;
				}                         
			}
			
		}

		
	}
	

}
