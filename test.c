#include "bson.h"

int main(){
    sint32_t c1 = {.value=0x0EADBEEF};
    sint64_t c2 = {.value=0x0EADBEEFFEEDBEAD};
    unsigned char a[] = {
//EDITHERE EVAL(DOCUMENT(2, INT32, "c1", 2, c1, INT64, "c2", 2, c2))
    };
    for(int i=0; i<sizeof(a); i++ ){
      printf("\\x%x", (int)a[i]);
    }
    printf("\n");
}
