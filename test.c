#include <stdio.h>
#include "bson.h"

int main(){
    unsigned char a[] = {
//EDITHERE EVAL(DOCUMENT(2, INT32, "c1", 2, c1, INT64, "c2", 2, c2))
    };
    for(int i=0; i<sizeof(a); i++ ){
      printf("\\x%x", (int)a[i]);
    }
    //printf("\n");
}
