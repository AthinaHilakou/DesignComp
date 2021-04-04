#include <stdlib.h>
#include <stdio.h>
#include "ADTVector.h"
#include "ADTSet.h"
#include "set_utils.h"

int Compare_ints(Pointer a, Pointer b){
	return *(int*)a - *(int*)b;
}
int Compare_strings(Pointer a,Pointer b) {
    return strcmp(a, b);
}

Pointer set_find_eq_or_greater(Set set, Pointer value){
    
    if(set_find_node(set, value) != SET_EOF) 
       return value;
 
    int d;

    for(SetNode node = set_first(set);        
        node != SET_EOF;                        
        node = set_next(set, node)) {          
        
        d = Compare_strings(set_node_value(set, node), value);
        if(d > 0)  
            return set_node_value(set, node);    
    }

    return NULL;
}



Pointer set_find_eq_or_smaller(Set set, Pointer value){
    
    if(set_find_node(set, value) != SET_EOF) 
       return value;
    Pointer f = set_find_eq_or_greater(set, value);
    if(f != NULL) {
        return set_node_value(set, set_previous(set, set_find_node(set, f)));
    }
    return NULL;
}