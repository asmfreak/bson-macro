# bson-macro

A C macros library to encode bson strings at compile time. 

## Limitations
It can encode only simple values - bson documents with `int32_t` and `int64_t`. All keys must be known at compile time. Keys and values will appear in resulting bson in reversed order they were specified.

## Usage 
`DOCUMENT` is a variadic macro. It accepts number of keys in document and groups of arguments - type (`INT32` or `INT64`), key string (can be both a variable and literal), key length without trailing zero (must be a literal) and value(can be both variable and literal).
```c
#include <stdio.h>
#include <stdint.h>
#include "bson.h"

int main(){
    int32_t val = 50;
    uint8_t a[] = {DOCUMENT(2, INT32, val2, 4, val, INT64, val1, 4, 100)};
}
```

## Acknoledgements
Big part of `bson.h` was written from [this stackoverflow answer](http://stackoverflow.com/a/10540654) and [Cloak library](https://github.com/pfultz2/Cloak) by Paul Fultz II.
