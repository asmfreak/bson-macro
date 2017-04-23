# bson-macro

A C macros library to encode bson strings at compile time. 

## Limitations
It can encode only simple values - bson documents with `int32_t` and `int64_t`. All keys must be known at compile time. Keys and values will appear in resulting bson in reversed order they were specified.

## Usage 
`BSON_DOCUMENT` is a variadic macro. It accepts number of keys in document and groups of arguments - type (`INT32` or `INT64`), key string (can be both a variable and literal), key length without trailing zero (must be a literal) and value(can be both variable and literal).
```c
#include <stdio.h>
#include <stdint.h>
#include "bson.h"

int main(){
    int32_t val = 50;
    uint8_t a[] = {BSON_DOCUMENT(2, INT32, val2, 4, val, INT64, val1, 4, 100)};
}
```

## Acknoledgements
Big part of `bson.h` was written from [this stackoverflow answer](http://stackoverflow.com/a/10540654) and [Cloak library](https://github.com/pfultz2/Cloak) by Paul Fultz II.


Copyright (C) 2016-2017 Pavel Pletenev <cpp.create@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
