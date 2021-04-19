//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "ADTSet.h"
#include "ADTList.h"
#include "state.h"

typedef struct portal_pair {
	Object entrance;		// η πύλη entrance
	Object exit;			// οδηγεί στην exit
}* PortalPair;

struct state {
	Vector objects;			// περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες)
	List portal_pairs;		// περιέχει PortalPair (ζευγάρια πυλών, είσοδος/έξοδος)

	struct state_info info;
};

void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(info->current_portal == 1);
	TEST_ASSERT(info->wins == 0);
	TEST_ASSERT(info->playing == true);
	TEST_ASSERT(info->paused == false);

	bool check[100];
	for(int i = 1; i <= 100 ; i++) 	
		check[i] = 0;
	for(ListNode node = list_first(state->portal_pairs);       
		node != LIST_EOF;                         
		node = list_next(state->portal_pairs, node)) {
		
		PortalPair pair = (PortalPair)list_node_value(state->portal_pairs, node);
		int exit = *(int*)(pair->exit);
		TEST_ASSERT(check[exit] == 0);
	}
}

 void test_state_update() {
 	State state = state_create();
 	TEST_ASSERT(state != NULL && state_info(state) != NULL);

 	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
 	struct key_state keys = { false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, ο χαρακτήρας μετακινείται 7 pixels μπροστά
	Rectangle old_rect = state_info(state)->character->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->character->rect;

	TEST_ASSERT( new_rect.x == old_rect.x + 7 && new_rect.y == old_rect.y );

	//Με πατημένο το δεξί βέλος, ο χαρακτήρας μετακινείται 12 pixes μπροστά
	// keys.right = true; 
	// old_rect = state_info(state)->character->rect;
	// state_update(state, &keys);
	// new_rect = state_info(state)->character->rect;

	// TEST_ASSERT( new_rect.x == old_rect.x + 12 && new_rect.y == old_rect.y );

    keys.up = true; 
    old_rect = state_info(state)->character->rect;
    state_update(state, &keys);
    new_rect = state_info(state)->character->rect; 
    TEST_ASSERT(new_rect.y == old_rect.y);
    
    keys.up = false;                             // character goes up at next frame 
    state_update(state, &keys);
    new_rect = state_info(state)->character->rect; 
    TEST_ASSERT(new_rect.y == old_rect.y - 15);

	state_destroy(state);
}


                                                                                                                                  
// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};