//////////////////////////////////////////////////////////////////
//
// Test για το state_alt.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "ADTSet.h"
#include "ADTList.h"
#include "ADTMap.h"
#include "state.h"


void test_state_alt() {
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
	for(MapNode node = map_first(state->portal_pairs);       
		node != MAP_EOF;                         
		node = map_next(state->portal_pairs, node)) {
		
		int val = *(int*)map_node_value(state->portal_pairs, node);
		TEST_ASSERT(check[val] == 0);
	}


	struct key_state keys = { false, false, false, false, false, false };
    Rectangle old_rect = state_info(state)->character->rect;
	state_update(state, &keys);
    Rectangle new_rect = state_info(state)->character->rect;
	TEST_ASSERT(new_rect.x == old_rect.x + 7 && new_rect.y == old_rect.y);

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


                                                                                                                                  
// Λίστα με όλα test προς εκτέλεση
TEST_LIST = {
	{ "test_state_alt", test_state_alt},
	{ NULL, NULL }        // τερματίζουμε τη λίστα με NULL
};