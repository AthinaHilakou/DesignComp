
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
int end_game = 0;

int compare_entrances(Pointer a, Pointer b){
	return *(int*)a - *(int*)b; 
}

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Vector objects;			// περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες)
	List portal_pairs;		// περιέχει PortalPair (ζευγάρια πυλών, είσοδος/έξοδος)

	struct state_info info;
};

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
	state->info.current_portal = 0;			// Δεν έχουμε περάσει καμία πύλη
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

	character->rect.width = 70;
	character->rect.height = 38;
	character->rect.x = 0;
	character->rect.y = - character->rect.height;

	// Δημιουργία των objects (πύλες / εμπόδια / εχθροί) και προσθήκη στο vector
	// state->objects. Η πίστα περιέχει συνολικά 4*PORTAL_NUM αντικείμενα, από
	// τα οποία τα PORTAL_NUM είναι πύλες, και τα υπόλοια εμπόδια και εχθροί.

	state->objects = vector_create(0, free);		// Δημιουργία του vector

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
		else if(rand() % 2 == 0) {				// Για τα υπόλοιπα, με πιθανότητα 50%
			obj->type = OBSTACLE;					// επιλέγουμε εμπόδιο.
			obj->rect.width = 10;
			obj->rect.height = 80;
		} 
		else {
			obj->type = ENEMY;						// Και τα υπόλοιπα είναι εχθροί.
			obj->rect.width = 30;
			obj->rect.height = 30;
			obj->forward = false;					// Οι εχθροί αρχικά κινούνται προς τα αριστερά.
		}

		// Τα αντικείμενα είναι ομοιόμορφα τοποθετημένα σε απόσταση SPACING
		// μεταξύ τους, και "κάθονται" πάνω στο δάπεδο.

		obj->rect.x = (i+1) * SPACING;                 //το προηγουμενο +700
		obj->rect.y = - obj->rect.height;
	}

	// TODO: αρχικοποίηση της λίστας state->portal_pairs

	List portal_pairs = list_create(NULL);

	int t[PORTAL_NUM/2];                                   // πινακας με περιεχομενα τα exit που εχω πετυχει 
	for(int i = 0; i < PORTAL_NUM/2; i++) {       // odd !?
		
		PortalPair pair = malloc(sizeof(*pair));
		//αρχικοποιηση των pairs
		Object entrance = malloc(sizeof(*entrance));
		Object exit = malloc(sizeof(*exit));
		pair->entrance = i+1;                      // cast !

		int k = rand() % 100 + 1;  
		int j = 0;
		while(j < 50)
			if(k == t[j]) { k = rand() % 100 + 1;   j = 0; }	
			else	j++;

		pair->exit = k;								// cast !
		t[i] = k;

		//printf("%d, %d\n",pair->entrance,pair->exit);
	 	list_insert_next(portal_pairs, list_last(portal_pairs), pair); 
	}

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {				// Προς υλοποίηση
	return &(state->info); 
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {			// Προς υλοποίηση
	Vector vec = state->objects;
	List list = list_create(free);
	ListNode list_node = list_first(list);

 	for(VectorNode vec_node = vector_first(vec);        
    	vec_node != VECTOR_EOF;                        
   		vec_node = vector_next(vec, vec_node)) {	
		Object obj = vector_node_value(vec, vec_node);
		// Αν το object ειναι αναμεσα στις συντεταγμενες που με ενδιαφερουν
		if(obj->rect.x >= x_from && obj->rect.x <= x_to) {
			list_insert_next(list, list_node, obj);		// Βαλτο στη λιστα
			list_node = list_next(list, list_node);
		} 
	}
	return list;
}


// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {		
	// character update
	if(keys->enter == false && keys->left == false && keys->right == false && keys->up == false && keys->n == false && keys->p == false )
		state->info.character->rect.x +=7;
	else if(keys->right != false)
		state->info.character->rect.x +=12;
	else if(keys->left != false){
		state->info.character->forward = false;
		//state->info.character->rect.x -=7;
	}
	else if(keys->up != false){
		if(state->info.character->rect.y < 220)
			state->info.character->rect.y -= 15;
		else	
			state->info.character->rect.y += 15; 
	}
	else if(keys->enter != false && end_game == 1){
		end_game = 0;
		state_create();
	}
	else if(keys->p != false){
		if(state->info.playing == 1)	state->info.playing == 0;
		else 	state->info.playing == 1;
		//if(keys->n != false)
	}
		

	// enemies update
	for(int i = 0; i < 4*PORTAL_NUM; i++){
		Object obj = vector_get_at(state->objects, i);
		if(obj->type == ENEMY)	{	
			obj->rect.x += 5;
			if(state->info.character->rect.x == obj->rect.x) { // συγκρουση εχθρου με εμποδιο 
				end_game = 1; // Μηνυμα για enter !!
				exit(1);
			}	                                     
			else {
				for(int j = 0; j < 4*PORTAL_NUM; j++){
					Object obj2 = vector_get_at(state->objects, i);
					if(obj2->type == OBSTACLE && obj->rect.x == obj2->rect.x)
						obj->forward = true;
				}
			}	
		}
		else if(obj->type == OBSTACLE) {
			if(state->info.character->rect.x == obj->rect.x){  // συγκρουση χαρακτηρα με εμποδιο
				end_game = 1;
				exit(1);
			}									  // Μηνυμα για enter !!
		}
		else {                                                // συγκρουση εμποδιου ή εχθρου με πυλη 
			int count = 0;       							  // συνολο νικων                                          
			Object obj = vector_get_at(state->objects, i);    // portal
			for(int j = 0; j < 4*PORTAL_NUM; j++) {
				Object obj2 = vector_get_at(state->objects, j);									
				if((obj2->type != PORTAL) && (obj2->rect.x == obj->rect.x)) {
					if(obj->forward == true) {
						ListNode node = list_first(state->portal_pairs);
						for(int k = 0; k < PORTAL_NUM; k++) {
							PortalPair pair = malloc(sizeof(*pair)); 
							pair = list_find(state->portal_pairs, node, compare_entrances);
							if(pair->entrance == (j % 4) + 1) {
								obj->rect.x = pair->exit->rect.x;
								if((obj->type == CHARACTER) && (pair->exit == PORTAL_NUM)) {
									count++;
									state->info.character->rect.x = 0;				
								}
							}
							node = list_next(state->portal_pairs, node);
						}
					}
					
				}
			}
		}
	}

}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	free(state);
}



