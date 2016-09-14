#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()
#define EXPAND(...) __VA_ARGS__
#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL5(...) __VA_ARGS__
/* #define EVAL5(...) EVAL6(EVAL6(EVAL6(__VA_ARGS__)))
#define EVAL6(...) EVAL7(EVAL7(EVAL7(__VA_ARGS__)))
#define EVAL7(...) __VA_ARGS__*/
#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define INC(x) PRIMITIVE_CAT(INC_, x) 
#define INC_0 1
#include "inc.h"

#define DEC(x) PRIMITIVE_CAT(DEC_, x)
#define DEC_0 0
#include "dec.h"

#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)

#define NOT(x) CHECK(PRIMITIVE_CAT(NOT_, x))
#define NOT_0 ~, 1,

#define COMPL(b) PRIMITIVE_CAT(COMPL_, b)
#define COMPL_0 1
#define COMPL_1 0

#define BOOL(x) COMPL(NOT(x))

#define IIF(c) PRIMITIVE_CAT(IIF_, c)
#define IIF_0(t, ...) __VA_ARGS__
#define IIF_1(t, ...) t

#define IF(c) IIF(BOOL(c))

#define EAT(...)
#define EXPAND(...) __VA_ARGS__
#define WHEN(c) IF(c)(EXPAND, EAT)

#define REPEAT(count, macro, ...) WHEN(count)(\
    OBSTRUCT(REPEAT_INDIRECT) ()(DEC(count), macro, __VA_ARGS__) \
    OBSTRUCT(macro)(DEC(count), __VA_ARGS__) \
)
#define REPEAT_INDIRECT() REPEAT

#define TO_ARRAY_EL(i, v) v[i],
#define TO_ARRAY(v, len) REPEAT(len, TO_ARRAY_EL, v)

#define TO_ARRAY_REV_EL(i, v, len) v[len-i],
#define TO_ARRAY_REV(v, len) REPEAT(len, TO_ARRAY_EL, v, len)

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define TO_ENDIANED_ARRAY TO_ARRAY
#else
#define TO_ENDIANED_ARRAY TO_ARRAY_REV
#endif
#define FIRST1(...) FIRST(__VA_ARGS__)
#define FIRST(e, ...) e
#define NOTFIRST1(...) NOTFIRST(__VA_ARGS__)
#define NOTFIRST(e, ...) __VA_ARGS__
#define DOCUMENT(len, ...) OBSTRUCT(DOCUMENT_IT)(len, 5, __VA_ARGS__) '\x0'
#define DOCUMENT_IT(len, size, ...) IF(len)(DOCUMENT_BODY, DOCUMENT_HEAD)(len, size, __VA_ARGS__)
#define DOCUMENT_BODY(len, size, t, k, klen, v, ...) OBSTRUCT(DOCUMENT_III)()(DEC(len), size+EXPAND(FIRST1(EL(t,k,klen,v))), __VA_ARGS__)  NOTFIRST1(EL(t,k,klen,v))
#define DOCUMENT_III() DOCUMENT_IT
#define EL(t, k, klen, v) PRIMITIVE_CAT(E, t)(k, klen, v)
#define DOCUMENT_HEAD(len, size, ...) TO_ENDIANED_ARRAY(((sint32_t){.value=(size)}).string, 4)

#define EINT32(k, klen, v)  sizeof(k)+5, '\x10', TO_ARRAY(k, klen)  '\x0', TO_ENDIANED_ARRAY(v.string, 4)
#define EINT64(k, klen, v)  sizeof(k)+9, '\x12', TO_ARRAY(k, klen) '\x0', TO_ENDIANED_ARRAY(v.string, 8)

#include <stdio.h>
#include <stdint.h>
#define stringify(t) union{\
  t value;\
  uint8_t string[sizeof(t)];\
}
typedef stringify(int32_t) sint32_t;
typedef stringify(int64_t) sint64_t;
