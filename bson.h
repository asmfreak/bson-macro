#ifndef __BSON_MACRO_H__
#define __BSON_MACRO_H__
#ifdef __cplusplus
extern "C" {
#endif
#define BS_EMPTY()
#define BS_DEFER(id) id BS_EMPTY()
#define BS_OBSTRUCT(...) __VA_ARGS__ BS_DEFER(BS_EMPTY)()
#define BS_EXPAND(...) __VA_ARGS__
#define BS_EVAL(...)  BS_EVAL1(BS_EVAL1(BS_EVAL1(__VA_ARGS__)))
#define BS_EVAL1(...) BS_EVAL2(BS_EVAL2(BS_EVAL2(__VA_ARGS__)))
#define BS_EVAL2(...) BS_EVAL3(BS_EVAL3(BS_EVAL3(__VA_ARGS__)))
#define BS_EVAL3(...) BS_EVAL4(BS_EVAL4(BS_EVAL4(__VA_ARGS__)))
#define BS_EVAL4(...) BS_EVAL5(BS_EVAL5(BS_EVAL5(__VA_ARGS__)))
#define BS_EVAL5(...) __VA_ARGS__
#define BS_CAT(a, ...) BS_PRIMITIVE_CAT(a, __VA_ARGS__)
#define BS_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define BS_INC(x) BS_PRIMITIVE_CAT(BS_INC_, x) 
#define BS_INC_0 1
#include "inc.h"

#define BS_DEC(x) BS_PRIMITIVE_CAT(BS_DEC_, x)
#define BS_DEC_0 0
#include "dec.h"

#define BS_CHECK_N(x, n, ...) n
#define BS_CHECK(...) BS_CHECK_N(__VA_ARGS__, 0,)

#define BS_NOT(x) BS_CHECK(BS_PRIMITIVE_CAT(BS_NOT_, x))
#define BS_NOT_0 ~, 1,

#define BS_COMPL(b) BS_PRIMITIVE_CAT(BS_COMPL_, b)
#define BS_COMPL_0 1
#define BS_COMPL_1 0

#define BS_BOOL(x) BS_COMPL(BS_NOT(x))

#define BS_IIF(c) BS_PRIMITIVE_CAT(BS_IIF_, c)
#define BS_IIF_0(t, ...) __VA_ARGS__
#define BS_IIF_1(t, ...) t

#define BS_IF(c) BS_IIF(BS_BOOL(c))

#define BS_EAT(...)
#define BS_EXPAND(...) __VA_ARGS__
#define BS_WHEN(c) BS_IF(c)(BS_EXPAND, BS_EAT)

#define BS_REPEAT(count, macro, ...) BS_WHEN(count)(\
    BS_OBSTRUCT(BS_REPEAT_INDIRECT) ()(BS_DEC(count), macro, __VA_ARGS__) \
    BS_OBSTRUCT(macro)(BS_DEC(count), __VA_ARGS__) \
)
#define BS_REPEAT_INDIRECT() BS_REPEAT

#define BS_TO_ARRAY_EL(i, v) v[i],
#define BS_TO_ARRAY(v, len) BS_REPEAT(len, BS_TO_ARRAY_EL, v)

#define BS_TO_ARRAY_REV_EL(i, v, len) v[len-i],
#define BS_TO_ARRAY_REV(v, len) BS_REPEAT(len, BS_TO_ARRAY_EL, v, len)

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define BS_TO_ENDIANED_ARRAY BS_TO_ARRAY
#define BS_FROM_ENDIANED_ARRAY BS_TO_ARRAY
#else
#define BS_TO_ENDIANED_ARRAY BS_TO_ARRAY_REV
#define BS_FROM_ENDIANED_ARRAY BS_TO_ARRAY_REV
#endif
#define BS_FIRST1(...) BS_FIRST(__VA_ARGS__)
#define BS_FIRST(e, ...) e
#define BS_NOTFIRST1(...) BS_NOTFIRST(__VA_ARGS__)
#define BS_NOTFIRST(e, ...) __VA_ARGS__
#define BSON_DOCUMENT(len, ...) BS_EVAL(BS_OBSTRUCT(BS_DOCUMENT_IT)(len, 5, __VA_ARGS__) '\x0')
#define BS_DOCUMENT_IT(len, size, ...) BS_IF(len)(BS_DOCUMENT_BODY, BS_DOCUMENT_HEAD)(len, size, __VA_ARGS__)
#define BS_DOCUMENT_BODY(len, size, t, k, klen, v, ...) \
  BS_OBSTRUCT(\
    BS_DOCUMENT_III)()(BS_DEC(len), \
    size+BS_EXPAND(BS_FIRST1(BS_EL(t,k,klen,v))), \
    __VA_ARGS__\
  ) \
  BS_NOTFIRST1(BS_EL(t,k,klen,v))
#define BS_DOCUMENT_III() BS_DOCUMENT_IT
#define BS_EL(t, k, klen, v) BS_PRIMITIVE_CAT(BS_E, t)(#k, klen, v)
#define BS_DOCUMENT_HEAD(len, size, ...) BS_TO_ENDIANED_ARRAY(((sint32_t)(int32_t)(size)).string, 4)

#define BS_EINT32(k, klen, v)  \
  sizeof(k)+5, '\x10', \
  BS_TO_ARRAY(k, klen)  '\x0', \
  BS_TO_ENDIANED_ARRAY(((sint32_t)(int32_t)v).string, 4)
#define BS_EINT64(k, klen, v)  \
  sizeof(k)+9, '\x12', \
  BS_TO_ARRAY(k, klen) '\x0', \
  BS_TO_ENDIANED_ARRAY(((sint64_t)(int64_t)v).string, 8)

#define BS_EDIND(k) sizeof(k)+1

#include <stdint.h>
#ifdef __cplusplus
#define stringify(t) union BS_PRIMITIVE_CAT(ss,t){\
  BS_PRIMITIVE_CAT(ss,t)(t in):value(in){};\
  t value;\
  uint8_t string[sizeof(t)];\
}
#else
#define stringify(t) union BS_PRIMITIVE_CAT(ss,t){\
  t value;\
  uint8_t string[sizeof(t)];\
}
#endif

#define BSON_VERIFY(name, len, ...) \
const int32_t BS_PRIMITIVE_CAT(name,_size) = BS_EVAL(BS_VERSIZE(len, __VA_ARGS__));\
uint8_t BS_PRIMITIVE_CAT(verify_, name)(const uint8_t* input, int32_t input_len BS_EVAL(BS_VERARGS(len, __VA_ARGS__))){\
    const int32_t exp_len = BS_EVAL(BS_VERSIZE(len, __VA_ARGS__));\
    const uint8_t exp[] = {BS_EVAL(BS_VERDOC(len, __VA_ARGS__))};\
    const int32_t indexes[] = {BS_EVAL(BS_OBSTRUCT(BS_VREVERSE)(len, BS_VERIND(len,__VA_ARGS__)))};\
    const int8_t types[] = {BS_EVAL(BS_VERTYPE(len, __VA_ARGS__))};\
    int i;\
    const uint8_t *ptr;\
    if(input_len!=exp_len||input[exp_len-1]!='\x0')return -1;\
    for(i=0;i<exp_len;i++){\
       BS_EVAL(BS_REPEAT(len,BS_NOTINDATA,))\
       if(exp[i]!=input[i]){return -2;}\
    }\
    BS_EVAL(BS_VER_GETVARS(len, __VA_ARGS__))\
    return(0);\
}
#define BS_NOTINDATA(c, ...) if(i>=indexes[c] && i<(indexes[c]+types[c])){continue;};
#define BS_VER_GETVARS(len, ...) BS_VER_GETVARS_IT(len, len, __VA_ARGS__)
#define BS_VER_GETVARS_IT(len, mlen, ...) BS_IF(len)(BS_VER_GETVARS_I, BS_EAT)(len, mlen, __VA_ARGS__)
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define BS_VER_GETVARS_I(len, mlen, t, k, klen,...)\
  ptr=input+indexes[len-1];\
  *k = (*((BS_TYP(t)(sint32_t*, sint64_t*))ptr)).value;\
  BS_OBSTRUCT(BS_VER_GETVARS_II)()(BS_DEC(len), mlen, __VA_ARGS__)
#else
#error "Unimplemented for big-endian ABI"
#endif
#define BS_VER_GETVARS_II() BS_VER_GETVARS_IT

#define BS_VERDOC(len, ...) BS_EVAL(BS_OBSTRUCT(BS_VERDOC_IT)(len, 5, __VA_ARGS__) '\x0')
#define BS_VERDOC_IT(len, size, ...) BS_IF(len)(BS_VERDOC_BODY, BS_VERDOC_HEAD)(len, size, __VA_ARGS__)
#define BS_VERDOC_BODY(len, size, t, k, klen, ...) \
  BS_OBSTRUCT(BS_VERDOC_III)()( \
    BS_DEC(len), \
    size+BS_EXPAND(BS_FIRST1(BS_VEL(t,k,klen))), \
    __VA_ARGS__\
  ) \
  BS_NOTFIRST1(BS_VEL(t,k,klen))
#define BS_VERDOC_III() BS_VERDOC_IT
#define BS_VEL(t, k, klen) BS_PRIMITIVE_CAT(BS_E, t)(#k, klen, 0)
#define BS_VERDOC_HEAD(len, size, ...) BS_TO_ENDIANED_ARRAY(((sint32_t)(int32_t)(size)).string, 4)

#define BS_VERSIZE(len, ...) 5 BS_VERSIZE_I(len, __VA_ARGS__)
#define BS_VERSIZE_I(len, ...) BS_IF(len)(BS_VERSIZE_II, BS_EAT)(len, __VA_ARGS__)
#define BS_VERSIZE_II(len, t, k, klen, ...) +BS_FIRST1(BS_VEL(t, k, klen)) BS_OBSTRUCT(BS_VERSIZE_III)()(BS_DEC(len), __VA_ARGS__)
#define BS_VERSIZE_III() BS_VERSIZE_I

#define BS_VERARGS(...) BS_VERARGS_I(__VA_ARGS__)
#define BS_VERARGS_I(len, ...) BS_IF(len)(BS_VERARGS_II, BS_EAT)(len, __VA_ARGS__)
#define BS_VERARGS_II(len, t, k, klen, ...) BS_OBSTRUCT(BS_VERARGS_III)()(BS_DEC(len), __VA_ARGS__), BS_TYP(t)(int32_t, int64_t)* k
#define BS_VERARGS_III() BS_VERARGS_I
#define BS_TINT32 1
#define BS_TINT64 0
#define BS_TYP(t) BS_IF(BS_PRIMITIVE_CAT(BS_T, t))

#define BS_VREVERSE(len, ...) BS_IF(len)(BS_VREVERSE_I, BS_EAT)(len, __VA_ARGS__)
#define BS_VREVERSE_I(len, t, ...) BS_OBSTRUCT(BS_VREVERSE_II)()(BS_DEC(len),__VA_ARGS__) t,
#define BS_VREVERSE_II() BS_VREVERSE

#define BS_VERIND(len, ...) BS_IF(len)(BS_VERIND_II, BS_EAT)(len, __VA_ARGS__)
#define BS_VERIND_II(len, t, k, klen, ...) BS_VERSIZE(BS_DEC(len), __VA_ARGS__)-1+BS_EDIND(#k),  BS_OBSTRUCT(BS_VERIND_III)()(BS_DEC(len), __VA_ARGS__)
#define BS_VERIND_III() BS_VERIND

#define BS_VERTYPE(len, ...) BS_IF(len)(BS_VERTYPE_I, BS_EAT)(len, __VA_ARGS__)
#define BS_VERTYPE_I(len, t, k, klen, ...) BS_PRIMITIVE_CAT(BS_VERTYPE_,t), BS_OBSTRUCT(BS_VERTYPE_II)()(BS_DEC(len),__VA_ARGS__)
#define BS_VERTYPE_II() BS_VERTYPE
#define BS_VERTYPE_INT32 4
#define BS_VERTYPE_INT64 8

typedef stringify(int32_t) sint32_t;
typedef stringify(int64_t) sint64_t;
typedef stringify(int16_t) sint16_t;

#define BS_TO_ARRI64(v) BS_EVAL(BS_TO_ENDIANED_ARRAY( ((sint64_t)(int64_t)v).string, 8 ))
#define BS_TO_ARRI32(v) BS_EVAL(BS_TO_ENDIANED_ARRAY( ((sint32_t)(int32_t)v).string, 4 ))
#define BS_TO_ARRI16(v) BS_EVAL(BS_TO_ENDIANED_ARRAY( ((sint16_t)(int16_t)v).string, 2 ))
#define BS_TO_ARRI8(v) (uint8_t)(v),

#ifdef __cplusplus
}
#endif

#endif
