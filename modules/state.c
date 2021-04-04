
#include <stdlib.h>
#include <stdio.h>
#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"

// int* create_int(int value){
// 	int* pointer = malloc(sizeof(int));		// δέσμευση μνήμης
// 	*pointer = value;						// αντιγραφή του value στον νέο ακέραιο
// 	return pointer;
// }

int compare_entrances(Object a, Object b){
	return *(int*)a - *(int*)b; 
}

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.


// Ζευγάρια πυλών

typedef struct portal_pair {
	Object entrance;		// η πύλη entrance
	Object exit;			// οδηγεί στην exit
}* PortalPair;


// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

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

    // Ο χαρακτήρας (όπως και όλα τα αντικείμενα) έχουν συντεταγμένες x,y σε ένα
    // καρτεσιανό επίπεδο.
	// - Στο άξονα x το 0 είναι η αρχή στης πίστας και οι συντεταγμένες
	//   μεγαλώνουν προς τα δεξιά.
	// - Στον άξονα y το 0 είναι το "δάπεδο" της πίστας, και οι
	//   συντεταγμένες μεγαλώνουν προς τα _κάτω_.
	// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
	// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
	// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
	// στο include/raylib.h).
	// 
	// Προσοχή: τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle, και
	// τα y μεγαλώνουν προς τα κάτω, οπότε πχ ο χαρακτήρας που έχει height=38,
	// αν θέλουμε να "κάθεται" πάνω στο δάπεδο, θα πρέπει να έχει y=-38.

	character->rect.width = 38;
	character->rect.height = 70;
	character->rect.x = 0;
	character->rect.y = SCREEN_HEIGHT - character->rect.height; 

	// Δημιουργία των objects (πύλες / εμπόδια / εχθροί) και προσθήκη στο vector
	// state->objects. Η πίστα περιέχει συνολικά 4*PORTAL_NUM αντικείμενα, από
	// τα οποία τα PORTAL_NUM είναι πύλες, και τα υπόλοια εμπόδια και εχθροί.

	state->objects = vector_create(0, free);		// Δημιουργία του vector
	state->portal_pairs = list_create(free);

	for (int i = 0; i < 4*PORTAL_NUM; i++) {
		// Δημιουργία του Object και προσθήκη στο vector
		Object obj = malloc(sizeof(*obj));
		vector_insert_last(state->objects, obj);

		// Κάθε 4 αντικείμενα υπάρχει μια πύλη. Τα υπόλοιπα αντικείμενα
		// επιλέγονται τυχαία.

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

		// Τα αντικείμενα είναι ομοιόμορφα τοποθετημένα σε απόσταση SPACING
		// μεταξύ τους, και "κάθονται" πάνω στο δάπεδο.

		obj->rect.x = (i+1) * SPACING;                 // το προηγουμενο +700
		obj->rect.y = SCREEN_HEIGHT - obj->rect.height;
		
	}

	// Aρχικοποίηση της λίστας state->portal_pairs

	state->portal_pairs = list_create(free);          //List portal_pairs = list_create(free)

	int t[PORTAL_NUM];                                   // πινακας με περιεχομενα τα exit που εχω πετυχει 
	for(int i = 0; i < PORTAL_NUM; i++) {    
		// αρχικοποιηση των pairs
		PortalPair pair = malloc(sizeof(*pair));
		
		//Object entrance = malloc(sizeof(*entrance));
		//Object exit = malloc(sizeof(*exit));
		pair->entrance = (Object)(i+1);

		int k = rand() % PORTAL_NUM + 1;  
		int j = 0;
		while(j < 100) {
			if(k == t[j]) { k = rand() % PORTAL_NUM + 1;   j = 0;}	
			else	j++;
		}

		pair->exit = (Object)k;
		t[i] = k;

		//printf("%d %d\n",pair->entrance, pair->exit);
	 	list_insert_next(state->portal_pairs, list_last(state->portal_pairs), pair); 
		PortalPair val = list_node_value(state->portal_pairs, list_last(state->portal_pairs));
		//printf("%d %d\n",val->entrance, val->exit); 
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
	if((keys->enter != false) && (state->info.playing == false) ){
		state->info.current_portal = 1;			
		state->info.wins = 0;					
		state->info.playing = true;				
		state->info.paused = false;	
		state->info.character->rect.x = 0;
		state_create();
		return;
		
	}
	if((keys->p != false) && (state->info.playing == false)) {
		state->info.playing = true;
		return;
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
			fast = 1;

			if(state->info.character->rect.y > 220 && up == -1)
				state->info.character->rect.y += 15*(up);
			else{
				up = 0;
				if(state->info.character->rect.y < SCREEN_HEIGHT - 70 )
					state->info.character->rect.y += 15;
			}	
		}	
		else if(keys->right != false) {
			if(forward = -1) 	forward = 1;
			//fast = 2;
		}		
		else if(keys->left != false){
			forward = -1;
		}
		else if(keys->up != false && (state->info.character->rect.y == SCREEN_HEIGHT - state->info.character->rect.height)){
			up = -1;
		} 
		else if(keys->p != false){
			state->info.playing = false;
			//if(keys->n != false)
		}
		
	
		// enemies update
		for(int i = 0; i < 4*PORTAL_NUM; i++) {
			Object obj = vector_get_at(state->objects, i);
			if(obj->type == ENEMY)	{	
				// συγκρουση χαρακτηρα με εχθρο 	
				for(int j = 0; j < 4*PORTAL_NUM; j++){
					Object obj2 = vector_get_at(state->objects, j);   
					// συγκρουση εχθρου με εμποδιο  
					if((CheckCollisionRecs(obj2->rect, obj->rect) == 1) && (obj2->type == OBSTACLE)) {
						//obj->forward = true;
						f = -1;
						//break;
					}
					
					if((CheckCollisionRecs(obj2->rect, obj->rect) == 1) && (obj2->type == PORTAL)) {
						//obj->forward = true;
						f = -1;
						//break;
					}							
				}
				if(CheckCollisionRecs(obj->rect, state->info.character->rect) == 1){ 
					state->info.playing = false;
				}	                                     
				else {
					obj->rect.x -= 5*f*fast;
				}	
			}
			else if(obj->type == OBSTACLE) {
				// συγκρουση εμποδιο με χαρακτηρα
				obj->rect.x -= 7*forward*fast;                                              
				if(CheckCollisionRecs(state->info.character->rect, obj->rect) == 1){
					state->info.playing = false;				
				}									  
			}
			else if(obj->type == PORTAL) {  
				obj->rect.x -= 7*forward*fast;                                              
				// συγκρουση εμποδιου ή εχθρου με πυλη 								
				if((CheckCollisionRecs(obj->rect, state->info.character->rect) == 1)) {     
					obj->rect.x -= 50*forward;
					for(ListNode node = list_first(state->portal_pairs);       
						node != LIST_EOF;                         
						node = list_next(state->portal_pairs, node)) {

						PortalPair pair = list_node_value(state->portal_pairs, node);
						
						if(pair->entrance == state->info.current_portal) {
							state->info.current_portal = pair->exit;
							if(pair->exit == 100) {
								state->info.wins ++;
								state->info.playing = false;
							}
							break;			
						}
					}
				
				}
				// else {
				// 	for(int j = 0; j < 4*PORTAL_NUM; j++) {
				// 		Object obj2 = vector_get_at(state->objects, j);	
				// 		if(obj2->type == ENEMY && (CheckCollisionRecs(obj2->rect, obj->rect) == 1)) {
				// 			for(ListNode node = list_first(state->portal_pairs);       
				// 				node != LIST_EOF;                         
				// 				node = list_next(state->portal_pairs, node)) {

				// 				//PortalPair pair = malloc(sizeof(*pair)); 
				// 				PortalPair pair = list_node_value(state->portal_pairs, node);
								
				// 				if(pair->entrance == state->info.current_portal) {
				// 					//printf("%d %d,",pair->entrance, pair->exit); 
				// 					state->info.current_portal = pair->exit;
				// 					if(pair->exit == 100) {
				// 						state->info.wins ++;
				// 						state->info.playing = false;
				// 					}
				// 					break;
				// 				}
				// 			}	
				// 		}
						
				// 	}
				// } 
					 
				
			}
		}
	}
	

}


// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	//free(state->info.character);
	//free(state->info.character);
	//free(state->info.current_portal);

	ListNode node = list_first(state->portal_pairs);
	while(node != NULL) {
		list_remove_next(state->portal_pairs, node);
		node = list_next(state->portal_pairs, node);
	}
	list_destroy(state->portal_pairs);
	
	while(vector_size(state->objects) > 0) {
		vector_remove_last(state->objects);
	}
	vector_destroy(state->objects);
	
	free(state);
}



