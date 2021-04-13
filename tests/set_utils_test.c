//////////////////////////////////////////////////////////////////
//
// Test για το set_utils.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "ADTSet.h"
#include "state.h"


int compare(Pointer a, Pointer b) {
	
    Object object1 = (Object)a;
    Object object2 = (Object)b;

    return object1->rect.x - object2->rect.x;
}

Pointer set_find_eq_or_greater(Set set, Pointer value){
    
    if(set_find_node(set, value) != SET_EOF) 
       return value;
 
    int d;

    for(SetNode node = set_first(set);        
        node != SET_EOF;                        
        node = set_next(set, node)) {          
        
        d = compare(set_node_value(set, node), value);
        if(d > 0)  
            return set_node_value(set, node);    
    }

    return NULL;
}

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    
    if(set_find_node(set, value) != SET_EOF) 
       return value;

    for(SetNode node = set_first(set);        
        node != SET_EOF;                        
        node = set_next(set, node)) {          
        
        int d = compare(set_node_value(set, node), value);
        if(d < 0)  
            return set_node_value(set, node);    
    }

    return NULL;
}


int compare_obj(Pointer a, Pointer b) {
	Object obj1 = (Object)a;
    Object obj2 = (Object)b;
    return obj1->rect.x - obj2->rect.x;
}


void test_set_utils() {
	
    Set set = set_create(compare_obj, free);
	
    for(int i = 0; i < 400; i++) {
		Object obj = malloc(sizeof(*obj));
		obj->rect.x = (i+1)*700;
		obj->forward = false;
		set_insert(set, obj);
	}

	Object obj1 = (Object)set_node_value(set, set_first(set)); 
	Object obj2 = malloc(sizeof(*obj2));
    obj2->rect.x = obj1->rect.x - 10;    
    TEST_ASSERT(set_find_eq_or_smaller(set, obj2) == NULL);
	
    obj1 = (Object)set_node_value(set, set_last(set));
    obj2->rect.x = obj1->rect.x + 10;
    TEST_ASSERT(set_find_eq_or_greater(set, obj2) == NULL);

    free(obj1);
    free(obj2);
    set_destroy(set);
}

                                                                                                                                  
// Λίστα με test προς εκτέλεση
TEST_LIST = {
	{ "test_set_utils", test_set_utils},
	{ NULL, NULL }      // τερματίζουμε τη λίστα με NULL
};