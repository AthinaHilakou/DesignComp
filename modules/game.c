#include "interface.h"
#include "state.h"
#include <stdlib.h>

State state;

void update_and_draw() {
	struct key_state keys = { false, false, false, false, false, false };
	if(IsKeyPressed(KEY_UP))	keys.up = true;
	else if(IsKeyPressed(KEY_LEFT))	keys.left = true;
	else if(IsKeyPressed(KEY_RIGHT))	keys.right = true;
	else if(IsKeyPressed(KEY_ENTER)){
		keys.enter = true;
		// state_destroy(state);
	}	
	else if(IsKeyPressed(KEY_P)) keys.p = true;
	else if(IsKeyPressed(KEY_N)) keys.n = true;
	state_update(state, &keys);
	interface_draw_frame(state);
}

int main() {

	state = state_create();
	interface_init();
	// Η κλήση αυτή καλεί συνεχόμενα την update_and_draw μέχρι ο χρήστης να κλείσει το παράθυρο
	start_main_loop(update_and_draw);
	
	interface_close();
	state_destroy(state);
	return 0;
}