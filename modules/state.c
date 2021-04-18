
#include <stdlib.h>
#include <stdio.h>
#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"

int* create_int(int value) {
	int* pointer = malloc(sizeof(int));	
	*pointer = value;						
	return pointer;
}

int compare_entrances(Object a, Object b){
	return *(int*)a - *(int*)b; 
}

// Ζευγάρια πυλών
typedef struct portal_pair {
	Object entrance;		// η πύλη entrance
	Object exit;			// οδηγεί στην exit
}* PortalPair;


struct state {
	Vector objects;			// περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες)
	List portal_pairs;		// περιέχει PortalPair (ζευγάρια πυλών, είσοδος/έξοδος)

	struct state_info info;
};


void state_init(State state) {
	// Γενικές πληροφορίες
	(state_info(state))->current_portal = 1;			// Δεν έχουμε περάσει καμία πύλη
	(state_info(state))->wins = 0;						// Δεν έχουμε νίκες ακόμα
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
	
	state->objects = vector_create(0, free);		// Δημιουργία του vector
	
	for (int i = 0; i < 4*PORTAL_NUM; i++) {
		
		// Δημιουργία του Object και προσθήκη στο vector
		Object obj = malloc(sizeof(*obj));
		vector_insert_last(state->objects, obj);
		
		// Κάθε 4 αντικείμενα υπάρχει μια πύλη. Τα υπόλοιπα αντικείμενα
		// επιλέγονται τυχαία.
		
		if(i % 4 == 3) {							// Το 4ο, 8ο, 12ο κλπ αντικείμενο
			obj->type = PORTAL;						// είναι πύλη.
			obj->rect.width = 80;
			obj->rect.height = 15;
		}
		else if(rand() % 2 == 0) {				    // Για τα υπόλοιπα, με πιθανότητα 50%
			obj->type = OBSTACLE;					// επιλέγουμε εμπόδιο.
			obj->rect.width = 10;
			obj->rect.height = 70;
		} 
		else {
			obj->type = ENEMY;						// Και τα υπόλοιπα είναι εχθροί.
			obj->rect.width = 30;
			obj->rect.height = 30;
			obj->forward = false;					// Οι εχθροί αρχικά κινούνται προς τα αριστερά.
		}

		// Τα αντικείμενα είναι ομοιόμορφα τοποθετημένα σε απόσταση SPACING
		// μεταξύ τους, και "κάθονται" πάνω στο δάπεδο.

		obj->rect.x = (i+1) * SPACING;                 // το προηγουμενο +700
		obj->rect.y = SCREEN_HEIGHT - obj->rect.height;
		
	}

}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));
	state_init(state);
	
	// Aρχικοποίηση της λίστας state->portal_pairs
	state->portal_pairs = list_create(free);        
	
	int t[PORTAL_NUM];                                   // πινακας με περιεχομενα τα exit που εχω πετυχει 
	for(int i = 0; i < PORTAL_NUM; i++) {    
		// αρχικοποιηση των pairs
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

	 	list_insert_next(state->portal_pairs, list_last(state->portal_pairs), pair); 
	}
	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {				
	return &(state->info); 
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {			// Προς υλοποίηση
	List list = list_create(free);
	
 	for(VectorNode vec_node = vector_first(state->objects);        
    	vec_node != VECTOR_EOF;                        
   		vec_node = vector_next(state->objects, vec_node)) {	
		
		Object obj = vector_node_value(state->objects, vec_node);

		// Αν το object ειναι αναμεσα στις συντεταγμενες που με ενδιαφερουν
		if(obj->rect.x >= x_from && obj->rect.x <= x_to) {
			list_insert_next(list, list_last(list), obj);		// Βαλτο στη λιστα
		}
	}
	return list;
}

static int forward = 1, up = 0, f = 1, fast = 1;

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {		
	// character update
	if((keys->enter != false) && ((state_info(state))->playing == false) ){
		state_init(state);
	}

	if((keys->p != false) && ((state_info(state))->paused == true)) {
		(state_info(state))->paused = false;
		return;
	}

	if((state_info(state))->playing == true && (state_info(state))->paused == false){
		fast = 1;
		// σταθεροποιω τον χαρακτηρα στο 1/3 της οθονης και μετακινω ολα τα αλλα αντικειμενα 
		if((state_info(state))->character->rect.x >= SCREEN_WIDTH/3 && forward == 1) {
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
			if((state_info(state))->character->forward == false) {
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
		
		// enemies update
		for(int i = 0; i < 4*PORTAL_NUM; i++) {
			Object obj = vector_get_at(state->objects, i);
			if(obj->type == ENEMY) {	
				
				for(int j = 0; j < 4*PORTAL_NUM; j++) {
					Object obj2 = vector_get_at(state->objects, j);   
					// συγκρουση εχθρου με εμποδιο  
					if((CheckCollisionRecs(obj2->rect, obj->rect)) && (obj2->type == OBSTACLE)) {
						obj->forward = !(obj->forward);	  
					}						
				}
				// συγκρουση χαρακτηρα με εχθρο 
				if(CheckCollisionRecs(obj->rect, (state_info(state))->character->rect)){ 
					(state_info(state))->playing = false;
					return;
				}	                                     
				
				if(obj->forward == false)	f = 1;
				else	f = -1;

				obj->rect.x -= 8*f*fast;	
			}
			else if(obj->type == OBSTACLE) {
				// συγκρουση εμποδιο με χαρακτηρα
				obj->rect.x -= 7*forward*fast;                                              
				if(CheckCollisionRecs((state_info(state))->character->rect, obj->rect)){
					(state_info(state))->playing = false;				
				}									  
			}
			else if(obj->type == PORTAL) {  
				obj->rect.x -= 7*forward*fast;                                              
				// συγκρουση εμποδιου ή εχθρου με πυλη 								
				if((CheckCollisionRecs(obj->rect, (state_info(state))->character->rect))) {  // Portal A->B   
					for(ListNode node = list_first(state->portal_pairs);       
						node != LIST_EOF;                         
						node = list_next(state->portal_pairs, node)) {

						PortalPair pair = list_node_value(state->portal_pairs, node);		
						
						int entr = *(int*)(pair->entrance);
						if(entr == (state_info(state))->current_portal) {
							
							// effects of falling
							if((state_info(state))->character->rect.x < (obj->rect.x)/2)
								(state_info(state))->character->rect.y += 15;
							else if((state_info(state))->character->rect.x > (obj->rect.x)/2 && ((state_info(state))->character->rect.x <= (obj->rect.x)))
								(state_info(state))->character->rect.y -= 15;
	
							(state_info(state))->current_portal = *(int*)(pair->exit);
							if((pair->exit) == (Object)create_int(100)) {
								(state_info(state))->wins += 1;
								(state_info(state))->playing = false;
							}

							if((state_info(state))->character->forward == false) {       // Portal (A->B -->) B->C
								for(ListNode node = list_first(state->portal_pairs);       
									node != LIST_EOF;                         
									node = list_next(state->portal_pairs, node)) {

									PortalPair pair = list_node_value(state->portal_pairs, node);		
									
									int entr = *(int*)(pair->entrance);
									if(entr == (state_info(state))->current_portal) {
										// effects
										if((state_info(state))->character->rect.x < (obj->rect.x)/2)
											(state_info(state))->character->rect.y += 15;
										else if((state_info(state))->character->rect.x > (obj->rect.x)/2 && ((state_info(state))->character->rect.x <= (obj->rect.x)))
											(state_info(state))->character->rect.y -= 15;
				
										(state_info(state))->current_portal = *(int*)(pair->exit);
										if((pair->exit) == (Object)create_int(100)) {
											(state_info(state))->wins += 1;
											(state_info(state))->playing = false;
										}
									}
								}
							}			
						}
					}
				}			 
			}
		}
	}
}


// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {	
	list_destroy(state->portal_pairs);
	vector_destroy(state->objects);
	
	free(state);
}



