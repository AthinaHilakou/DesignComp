#include <stdlib.h>
#include <stdio.h>
#include "ADTVector.h"
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
 
    for(SetNode node = set_first(set);        
        node != SET_EOF;                        
        node = set_next(set, node)) {          
        
        int d = compare(set_node_value(set, node), value);
        if(d > 0)  
            return set_node_value(set, node);    
    }

    return NULL;
}

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    
    if(set_find_node(set, value) != SET_EOF) 
       return value;
    
    Pointer d = NULL;
    for(SetNode node = set_first(set);        
        node != SET_EOF;                        
        node = set_next(set, node)) {          
        
        if(compare(set_node_value(set, node), value) < 0)  
           d = set_node_value(set, node);
        else    
            return d;    
    }

    return NULL;
}