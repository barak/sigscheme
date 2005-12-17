/*===========================================================================
 *  FileName : sigscheme.h
 *  About    : main header file
 *
 *  Copyright (C) 2005      by Kazuki Ohta (mover@hct.zaq.ne.jp)
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of authors nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
===========================================================================*/
#ifndef __SIGSCHEME_H
#define __SIGSCHEME_H

#ifdef __cplusplus
extern "C" {
#endif

/*=======================================
   System Include
=======================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*=======================================
   Local Include
=======================================*/
#include "config.h"
#include "encoding.h"

/*=======================================
   Macro Declarations
=======================================*/
#define SCM_ERRMSG_UNHANDLED_EXCEPTION "unhandled exception"
#define SCM_ERRMSG_MEMORY_EXHAUSTED    "memory exhausted"

#ifdef __GNUC__
#define SCM_NOINLINE __attribute__((noinline))
#define SCM_NORETURN __attribute__((noreturn))
#else
#define SCM_NOINLINE
#define SCM_NORETURN
#endif /* __GNUC__ */

/* Casts a machine word mainly a function pointer and (void *) */
#define SCM_WORD_CAST(type, obj) ((type)(unsigned long)(obj))
/*
 * Re-interprets a storage as-is, similarly to reinterpret_cast<type>(obj) of
 * C++. Don't change the semantics. This cast requires that obj is a pointable
 * storage.
 */
#define SCM_REINTERPRET_CAST(type, obj) (*(type *)&(obj))

/* RFC: better names for the debug printing */
#if SCM_DEBUG
#define SCM_CDBG(args) (scm_categorized_debug args)
#define SCM_DBG(args)  (scm_debug args)
#else /* SCM_DEBUG */
#define SCM_CDBG(args)
#define SCM_DBG(args)
#endif /* SCM_DEBUG */

/* FIXME:
 * - separate required validation from optional assertion
 * - support immediate termination to produce core instead of robust recovery
 */
#define SCM_ASSERT(cond)                                                     \
    ((cond) || scm_die("assertion failed", __FILE__, __LINE__))

#define SCM_ASSERT_ALLOCATED(p)                                              \
    ((p) || (scm_fatal_error(SCM_ERRMSG_MEMORY_EXHAUSTED), 1))

#define SCM_ERROBJP(obj)       (NFALSEP(scm_p_error_objectp(obj)))

#define SCM_SYMBOL_BOUNDP(sym) (!SCM_EQ(SCM_SYMBOL_VCELL(sym), SCM_UNBOUND))

#define SCM_CONS(kar, kdr) (scm_make_cons((kar), (kdr)))
#if SCM_USE_STORAGE_ABSTRACTION_LAYER
#define SCM_CAR(kons)  (SCM_CONS_CAR(kons))
#define SCM_CDR(kons)  (SCM_CONS_CDR(kons))
#define SCM_CAAR(kons) (SCM_CAR(SCM_CAR(kons)))
#define SCM_CADR(kons) (SCM_CAR(SCM_CDR(kons)))
#define SCM_CDAR(kons) (SCM_CDR(SCM_CAR(kons)))
#define SCM_CDDR(kons) (SCM_CDR(SCM_CDR(kons)))
#endif /* SCM_USE_STORAGE_ABSTRACTION_LAYER */

#define SCM_LIST_1(elm0) \
    (SCM_CONS((elm0), SCM_NULL))
#define SCM_LIST_2(elm0, elm1) \
    (SCM_CONS((elm0), SCM_LIST_1(elm1)))
#define SCM_LIST_3(elm0, elm1, elm2) \
    (SCM_CONS((elm0), SCM_LIST_2((elm1), (elm2))))
#define SCM_LIST_4(elm0, elm1, elm2, elm3) \
    (SCM_CONS((elm0), SCM_LIST_3((elm1), (elm2), (elm3))))
#define SCM_LIST_5(elm0, elm1, elm2, elm3, elm4) \
    (SCM_CONS((elm0), SCM_LIST_4((elm1), (elm2), (elm3), (elm4))))

#define SCM_LISTP(obj)    (SCM_CONSP(obj) || SCM_NULLP(obj))
#define SCM_LIST_1_P(lst) (SCM_CONSP(lst) && SCM_NULLP(SCM_CDR(lst)))
#define SCM_LIST_2_P(lst) (SCM_CONSP(lst) && SCM_LIST_1_P(SCM_CDR(lst)))
#define SCM_LIST_3_P(lst) (SCM_CONSP(lst) && SCM_LIST_2_P(SCM_CDR(lst)))
#define SCM_LIST_4_P(lst) (SCM_CONSP(lst) && SCM_LIST_3_P(SCM_CDR(lst)))
#define SCM_LIST_5_P(lst) (SCM_CONSP(lst) && SCM_LIST_4_P(SCM_CDR(lst)))

#define SCM_EVAL(obj, env) (scm_eval((obj), (env)))

#if SCM_GCC4_READY_GC
/*
 * Function caller with protecting Scheme objects on stack from GC
 *
 * The protection is safe against with variable reordering on a stack
 * frame performed in some compilers as anti-stack smashing or
 * optimization.
 *
 * Users should only use SCM_GC_PROTECTED_CALL() and
 * SCM_GC_PROTECTED_CALL_VOID().
 */
#define SCM_GC_PROTECTED_CALL(ret, ret_type, func, args)                     \
    SCM_GC_PROTECTED_CALL_INTERNAL(ret = , ret_type, func, args)

#define SCM_GC_PROTECTED_CALL_VOID(func, args)                               \
    SCM_GC_PROTECTED_CALL_INTERNAL((void), void, func, args)

#define SCM_GC_PROTECTED_CALL_INTERNAL(exp_ret, ret_type, func, args)        \
    do {                                                                     \
        /* ensure that func is uninlined */                                  \
        ret_type (*volatile fp)() = (ret_type (*)())&func;                   \
        ScmObj *stack_start;                                                 \
                                                                             \
        if (0) exp_ret func args;  /* compile-time type check */             \
        stack_start = scm_gc_protect_stack(NULL);                          \
        exp_ret (*fp)args;                                                   \
        scm_gc_unprotect_stack(stack_start);                               \
    } while (/* CONSTCOND */ 0)

#endif /* SCM_GCC4_READY_GC */


/*
 * Port I/O Handling macros
 */
#define SCM_CHARPORT_ERROR(cport, msg) (scm_error(msg))
#define SCM_BYTEPORT_ERROR(bport, msg) (scm_error(msg))
#define SCM_PORT_MALLOC(size)          (scm_malloc(size))
#define SCM_PORT_CALLOC(number, size)  (scm_calloc(number, size))
#define SCM_PORT_REALLOC(ptr, size)    (scm_realloc(ptr, size))

#define SCM_ASSERT_LIVE_PORT(port)                                           \
    (SCM_PORT_IMPL(port)                                                     \
     || (scm_error_obj("(unknown)", "operated on closed port", port), 1))

#define SCM_PORT_CLOSE_IMPL(port)                                            \
    (SCM_CHARPORT_CLOSE(SCM_PORT_IMPL(port)), SCM_PORT_SET_IMPL(port, NULL))
#define SCM_PORT_ENCODING(port)                                              \
    (SCM_ASSERT_LIVE_PORT(port), SCM_CHARPORT_ENCODING(SCM_PORT_IMPL(port)))
#define SCM_PORT_INSPECT(port)                                              \
    (SCM_ASSERT_LIVE_PORT(port), SCM_CHARPORT_INSPECT(SCM_PORT_IMPL(port)))
#define SCM_PORT_GET_CHAR(port)                                              \
    (SCM_ASSERT_LIVE_PORT(port), SCM_CHARPORT_GET_CHAR(SCM_PORT_IMPL(port)))
#define SCM_PORT_PEEK_CHAR(port)                                             \
    (SCM_ASSERT_LIVE_PORT(port), SCM_CHARPORT_PEEK_CHAR(SCM_PORT_IMPL(port)))
#define SCM_PORT_CHAR_READYP(port)                                           \
    (SCM_ASSERT_LIVE_PORT(port), SCM_CHARPORT_CHAR_READYP(SCM_PORT_IMPL(port)))
#define SCM_PORT_VPRINTF(port, str, args)                                    \
    (SCM_ASSERT_LIVE_PORT(port),                                             \
     SCM_CHARPORT_VPRINTF(SCM_PORT_IMPL(port), (str), (args)))
#define SCM_PORT_PUTS(port, str)                                             \
    (SCM_ASSERT_LIVE_PORT(port), SCM_CHARPORT_PUTS(SCM_PORT_IMPL(port), (str)))
#define SCM_PORT_PUT_CHAR(port, ch)                                          \
    (SCM_ASSERT_LIVE_PORT(port),                                             \
     SCM_CHARPORT_PUT_CHAR(SCM_PORT_IMPL(port), (ch)))
#define SCM_PORT_FLUSH(port)                                                 \
    (SCM_ASSERT_LIVE_PORT(port), SCM_CHARPORT_FLUSH(SCM_PORT_IMPL(port)))

/*
 * SCM_CHARPORT_ERROR and SCM_BYTEPORT_ERROR must be defined before this
 * inclusion
 */
#include "baseport.h"

#define SCM_WRITESS_TO_PORT(port, obj) ((*scm_writess_func)(port, obj))

/*============================================================================
  Type Definitions
============================================================================*/
enum ScmDebugCategory {
    SCM_DBG_NONE         = 0,
    SCM_DBG_ERRMSG       = 1 << 0,   /* the "Error: foo bar" style msgs */
    SCM_DBG_BACKTRACE    = 1 << 1,
    SCM_DBG_GC           = 1 << 2,
    SCM_DBG_FILE         = 1 << 3,   /* file loading */
    SCM_DBG_PARSER       = 1 << 4,
    SCM_DBG_READ         = 1 << 5,   /* print each parsed expression + misc */
    SCM_DBG_MACRO        = 1 << 6,
    SCM_DBG_ARGS         = 1 << 7,   /* number of arguments, type and so on */
    SCM_DBG_EVAL         = 1 << 8,   /* evaluation-related things */
    SCM_DBG_CONTINUATION = 1 << 9,
    SCM_DBG_EXCEPTION    = 1 << 10,
    SCM_DBG_EXPERIMENTAL = 1 << 11,  /* developed but experimental features */
    SCM_DBG_DEVEL        = 1 << 12,  /* under development */
    SCM_DBG_COMPAT       = 1 << 13,  /* warns compatibility-sensitive code */
    SCM_DBG_ENCODING     = 1 << 14,  /* multibyte handling */
    SCM_DBG_OTHER        = 1 << 30   /* all other messages */
};

#if SCM_USE_STORAGE_ABSTRACTION_LAYER
enum ScmObjType {
    ScmInt          = 0,
    ScmCons         = 1,
    ScmSymbol       = 2,
    ScmChar         = 3,
    ScmString       = 4,
    ScmFunc         = 5,
    ScmClosure      = 6,
    ScmVector       = 7,
    ScmPort         = 8,
    ScmContinuation = 9,
    ScmConstant     = 10,
    ScmValuePacket  = 11,
    ScmFreeCell     = 12,

    ScmCPointer     = 20,
    ScmCFuncPointer = 21
};

/*
 * Function types:
 *
 * Function objects must tag themselves with proper information so
 * that the evaluator can correctly invoke them.  See doc/invocation
 * for details.
 */
enum ScmFuncTypeCode {
    SCM_FUNCTYPE_MAND_BITS = 4,
    SCM_FUNCTYPE_MAND_MASK = (1 << SCM_FUNCTYPE_MAND_BITS)-1,
#define SCM_FUNCTYPE_MAND_MAX 5
    /* SCM_FUNCTYPE_MAND_MAX  = 5, */
    SCM_FUNCTYPE_SYNTAX    = 1 << SCM_FUNCTYPE_MAND_BITS,

    SCM_FUNCTYPE_FIXED     = 0 << (SCM_FUNCTYPE_MAND_BITS+1),
    SCM_FUNCTYPE_VARIADIC  = 1 << (SCM_FUNCTYPE_MAND_BITS+1),
    SCM_FUNCTYPE_TAIL_REC  = 1 << (SCM_FUNCTYPE_MAND_BITS+2),

    SCM_FUNCTYPE_ODDBALL   = 1 << (SCM_FUNCTYPE_MAND_BITS+10),

    /* Compound types. */
    SCM_PROCEDURE_FIXED              = SCM_FUNCTYPE_FIXED,
    SCM_PROCEDURE_FIXED_TAIL_REC     = SCM_FUNCTYPE_TAIL_REC,
    SCM_PROCEDURE_VARIADIC           = SCM_FUNCTYPE_VARIADIC,
    SCM_PROCEDURE_VARIADIC_TAIL_REC  = SCM_FUNCTYPE_VARIADIC | SCM_FUNCTYPE_TAIL_REC,

    SCM_SYNTAX_FIXED          = SCM_PROCEDURE_FIXED | SCM_FUNCTYPE_SYNTAX,
    SCM_SYNTAX_FIXED_TAIL_REC = SCM_PROCEDURE_FIXED_TAIL_REC | SCM_FUNCTYPE_SYNTAX,
    SCM_SYNTAX_VARIADIC       = SCM_PROCEDURE_VARIADIC | SCM_FUNCTYPE_SYNTAX,
    SCM_SYNTAX_VARIADIC_TAIL_REC = SCM_PROCEDURE_VARIADIC_TAIL_REC | SCM_FUNCTYPE_SYNTAX,

    /* Special type. */
    SCM_REDUCTION_OPERATOR = SCM_FUNCTYPE_ODDBALL
};

/* Where we are in a reduction process. */
enum ScmReductionState {
    SCM_REDUCE_0,               /* No argument was given. */
    SCM_REDUCE_1,               /* Only 1 argument was given. */
    SCM_REDUCE_PARTWAY,         /* We have more arguments pending. */
    SCM_REDUCE_LAST,            /* The callee must finalize. */
    SCM_REDUCE_STOP             /* Callee wants to stop. */
};

enum ScmReturnType {
    SCM_RETTYPE_AS_IS           = 0,
    SCM_RETTYPE_NEED_EVAL       = 1
};

enum ScmPortFlag {
    SCM_PORTFLAG_NONE        = 0,
    SCM_PORTFLAG_OUTPUT      = 1 << 0,
    SCM_PORTFLAG_INPUT       = 1 << 1,
    SCM_PORTFLAG_LIVE_OUTPUT = 1 << 2,
    SCM_PORTFLAG_LIVE_INPUT  = 1 << 3,

    SCM_PORTFLAG_DIR_MASK = (SCM_PORTFLAG_OUTPUT | SCM_PORTFLAG_INPUT),
    SCM_PORTFLAG_ALIVENESS_MASK = (SCM_PORTFLAG_LIVE_OUTPUT
                                   | SCM_PORTFLAG_LIVE_INPUT)
};

typedef void (*ScmCFunc)(void);

#if SCM_OBJ_COMPACT
#include "storage-compact.h"
#else /* SCM_OBJ_COMPACT */
#include "storage-fatty.h"
#endif /* SCM_OBJ_COMPACT */
/*
 * A storage implementation defines following types.
 *
 * typedef <hidden> ScmCell;
 * typedef <hidden> ScmObj;
 * typedef <hidden> ScmRef;
 *
 * typedef ScmObj (*ScmFuncType)();
 */

/* The evaluator's state */
typedef struct ScmEvalState_ ScmEvalState;
struct ScmEvalState_ {
    ScmObj env;
    enum ScmReturnType ret_type;
};

/*=======================================
  Object Creators
=======================================*/
#define SCM_MAKE_BOOL(x)                  ((x) ? SCM_TRUE : SCM_FALSE)
/* FIXME: define arguments */
#define SCM_MAKE_INT                      SCM_SAL_MAKE_INT
#define SCM_MAKE_CONS                     SCM_SAL_MAKE_CONS
#define SCM_MAKE_SYMBOL                   SCM_SAL_MAKE_SYMBOL
#define SCM_MAKE_CHAR                     SCM_SAL_MAKE_CHAR
#define SCM_MAKE_STRING                   SCM_SAL_MAKE_STRING
#define SCM_MAKE_STRING_COPYING           SCM_SAL_MAKE_STRING_COPYING
#define SCM_MAKE_IMMUTABLE_STRING         SCM_SAL_MAKE_IMMUTABLE_STRING
#define SCM_MAKE_IMMUTABLE_STRING_COPYING SCM_SAL_MAKE_IMMUTABLE_STRING_COPYING
#define SCM_MAKE_FUNC                     SCM_SAL_MAKE_FUNC
#define SCM_MAKE_CLOSURE                  SCM_SAL_MAKE_CLOSURE
#define SCM_MAKE_VECTOR                   SCM_SAL_MAKE_VECTOR
#define SCM_MAKE_PORT                     SCM_SAL_MAKE_PORT
#define SCM_MAKE_CONTINUATION             SCM_SAL_MAKE_CONTINUATION
#if SCM_USE_NONSTD_FEATURES
#define SCM_MAKE_C_POINTER                SCM_SAL_MAKE_C_POINTER
#define SCM_MAKE_C_FUNCPOINTER            SCM_SAL_MAKE_C_FUNCPOINTER
#endif /* SCM_USE_NONSTD_FEATURES */
#define SCM_MAKE_VALUEPACKET              SCM_SAL_MAKE_VALUEPACKET

/*=======================================
  Object Accessors
=======================================*/
/* ScmObj Global Attribute */
#define SCM_TYPE(o) SCM_SAL_TYPE(o)

/* Type Confirmation */
#if SCM_ACCESSOR_ASSERT
#define SCM_ASSERT_TYPE(cond, o) (SCM_ASSERT(cond), (o))
#else /* SCM_ACCESSOR_ASSERT */
#define SCM_ASSERT_TYPE(cond, o) (o)
#endif /* SCM_ACCESSOR_ASSERT */
#define SCM_AS_INT(o)           (SCM_ASSERT_TYPE(SCM_INTP(o),           (o)))
#define SCM_AS_CONS(o)          (SCM_ASSERT_TYPE(SCM_CONSP(o),          (o)))
#define SCM_AS_SYMBOL(o)        (SCM_ASSERT_TYPE(SCM_SYMBOLP(o),        (o)))
#define SCM_AS_CHAR(o)          (SCM_ASSERT_TYPE(SCM_CHARP(o),          (o)))
#define SCM_AS_STRING(o)        (SCM_ASSERT_TYPE(SCM_STRINGP(o),        (o)))
#define SCM_AS_FUNC(o)          (SCM_ASSERT_TYPE(SCM_FUNCP(o),          (o)))
#define SCM_AS_CLOSURE(o)       (SCM_ASSERT_TYPE(SCM_CLOSUREP(o),       (o)))
#define SCM_AS_VECTOR(o)        (SCM_ASSERT_TYPE(SCM_VECTORP(o),        (o)))
#define SCM_AS_PORT(o)          (SCM_ASSERT_TYPE(SCM_PORTP(o),          (o)))
#define SCM_AS_CONTINUATION(o)  (SCM_ASSERT_TYPE(SCM_CONTINUATIONP(o),  (o)))
#define SCM_AS_VALUEPACKET(o)   (SCM_ASSERT_TYPE(SCM_VALUEPACKETP(o),   (o)))
#define SCM_AS_C_POINTER(o)     (SCM_ASSERT_TYPE(SCM_C_POINTERP(o),     (o)))
#define SCM_AS_C_FUNCPOINTER(o) (SCM_ASSERT_TYPE(SCM_C_FUNCPOINTERP(o), (o)))

#define SCM_INTP(o)                     SCM_SAL_INTP(o)
#define SCM_INT_VALUE(o)                SCM_SAL_INT_VALUE(o)
#define SCM_INT_SET_VALUE(o, val)       SCM_SAL_INT_SET_VALUE((o), (val))

#define SCM_CONSP(o)                    SCM_SAL_CONSP(o)
#define SCM_CONS_CAR(o)                 SCM_SAL_CONS_CAR(o)
#define SCM_CONS_CDR(o)                 SCM_SAL_CONS_CDR(o)
#define SCM_CONS_SET_CAR(o, kar)        SCM_SAL_CONS_SET_CAR((o), (kar))
#define SCM_CONS_SET_CDR(o, kdr)        SCM_SAL_CONS_SET_CDR((o), (kdr))

#define SCM_SYMBOLP(o)                  SCM_SAL_SYMBOLP(o)
#define SCM_SYMBOL_NAME(o)              SCM_SAL_SYMBOL_NAME(o)
#define SCM_SYMBOL_VCELL(o)             SCM_SAL_SYMBOL_VCELL(o)
#define SCM_SYMBOL_SET_NAME(o, name)    SCM_SAL_SYMBOL_SET_NAME((o), (name))
#define SCM_SYMBOL_SET_VCELL(o, val)    SCM_SAL_SYMBOL_SET_VCELL((o), (val))

#define SCM_CHARP(o)                    SCM_SAL_CHARP(o)
#define SCM_CHAR_VALUE(o)               SCM_SAL_CHAR_VALUE(o)
#define SCM_CHAR_SET_VALUE(o, val)      SCM_SAL_CHAR_SET_VALUE((o), (val))

#define SCM_STRINGP(o)                  SCM_SAL_STRINGP(o)
#define SCM_STRING_STR(o)               SCM_SAL_STRING_STR(o)
#define SCM_STRING_LEN(o)               SCM_SAL_STRING_LEN(o)
#define SCM_STRING_SET_STR(o, str)      SCM_SAL_STRING_SET_STR((o), (str))
#define SCM_STRING_SET_LEN(o, len)      SCM_SAL_STRING_SET_LEN((o), (len))
#define SCM_STRING_MUTABLEP(o)          SCM_SAL_STRING_MUTABLEP(o)
#define SCM_STRING_SET_MUTABLE(o)       SCM_SAL_STRING_SET_MUTABLE(o)
#define SCM_STRING_SET_IMMUTABLE(o)     SCM_SAL_STRING_SET_IMMUTABLE(o)

#define SCM_FUNCP(o)                    SCM_SAL_FUNCP(o)
#define SCM_FUNC_TYPECODE(o)            SCM_SAL_FUNC_TYPECODE(o)
#define SCM_FUNC_CFUNC(o)               SCM_SAL_FUNC_CFUNC(o)
#define SCM_FUNC_SET_TYPECODE(o, type)  SCM_SAL_FUNC_SET_TYPECODE((o), (type))
#define SCM_FUNC_SET_CFUNC(o, func)     SCM_SAL_FUNC_SET_CFUNC((o), (func))
#define SCM_SYNTAXP(o)    (SCM_FUNCP(o)                                      \
                           && (SCM_FUNC_TYPECODE(o) & SCM_FUNCTYPE_SYNTAX))
#define SCM_PROCEDUREP(o) ((SCM_FUNCP(o)                                     \
                            && !(SCM_FUNC_TYPECODE(o) & SCM_FUNCTYPE_SYNTAX)) \
                           || SCM_CLOSUREP(o)                                \
                           || SCM_CONTINUATIONP(o))

#define SCM_CLOSUREP(o)                 SCM_SAL_CLOSUREP(o)
#define SCM_CLOSURE_EXP(o)              SCM_SAL_CLOSURE_EXP(o)
#define SCM_CLOSURE_SET_EXP(o, exp)     SCM_SAL_CLOSURE_SET_EXP((o), (exp))
#define SCM_CLOSURE_ENV(o)              SCM_SAL_CLOSURE_ENV(o)
#define SCM_CLOSURE_SET_ENV(o, env)     SCM_SAL_CLOSURE_SET_ENV((o), (env))

#define SCM_VECTORP(o)                  SCM_SAL_VECTORP(o)
#define SCM_VECTOR_VEC(o)               SCM_SAL_VECTOR_VEC(o)
#define SCM_VECTOR_LEN(o)               SCM_SAL_VECTOR_LEN(o)
#define SCM_VECTOR_SET_VEC(o, vec)      SCM_SAL_VECTOR_SET_VEC((o), (vec))
#define SCM_VECTOR_SET_LEN(o, len)      SCM_SAL_VECTOR_SET_LEN((o), (len))
#define SCM_VECTOR_VALID_INDEXP(o, i)   SCM_SAL_VECTOR_VALID_INDEXP((o), (i))

#define SCM_PORTP(o)                    SCM_SAL_PORTP(o)
#define SCM_PORT_FLAG(o)                SCM_SAL_PORT_FLAG(o)
#define SCM_PORT_IMPL(o)                SCM_SAL_PORT_IMPL(o)
#define SCM_PORT_SET_FLAG(o, flag)      SCM_SAL_PORT_SET_FLAG((o), (flag))
#define SCM_PORT_SET_IMPL(o, impl)      SCM_SAL_PORT_SET_IMPL((o), (impl))

#define SCM_CONTINUATIONP(o)            SCM_SAL_CONTINUATIONP(o)
#define SCM_CONTINUATION_OPAQUE(o)      SCM_SAL_CONTINUATION_OPAQUE(o)
#define SCM_CONTINUATION_TAG(o)         SCM_SAL_CONTINUATION_TAG(o)
#define SCM_CONTINUATION_SET_OPAQUE(o, val)                                  \
    SCM_SAL_CONTINUATION_SET_OPAQUE((o), (val))
#define SCM_CONTINUATION_SET_TAG(o, val)                                     \
    SCM_SAL_CONTINUATION_SET_TAG((o), (val))

#define SCM_VALUEPACKETP(o)             SCM_SAL_VALUEPACKETP(o)
#define SCM_VALUEPACKET_VALUES(o)       SCM_SAL_VALUEPACKET_VALUES(o)

#define SCM_CONSTANTP(o)                SCM_SAL_CONSTANTP(o)

#define SCM_C_POINTERP(o)               SCM_SAL_C_POINTERP(o)
#define SCM_C_POINTER_VALUE(o)          SCM_SAL_C_POINTER_VALUE(o)
#define SCM_C_POINTER_SET_VALUE(o, ptr) SCM_SAL_C_POINTER_SET_VALUE((o), (ptr))

#define SCM_C_FUNCPOINTERP(o)           SCM_SAL_C_FUNCPOINTERP(o)
#define SCM_C_FUNCPOINTER_VALUE(o)      SCM_SAL_C_FUNCPOINTER_VALUE(o)
#define SCM_C_FUNCPOINTER_SET_VALUE(o, funcptr)                              \
    SCM_SAL_C_FUNCPOINTER_SET_VALUE((o), (funcptr))

/*============================================================================
  Environment Specifiers
============================================================================*/
#define SCM_INTERACTION_ENV SCM_SAL_INTERACTION_ENV
/*
 * Current implementation cannot handle scheme-report-environment and
 * null-environment properly. Be careful to use these environemnts.
 */
#define SCM_R5RS_ENV        SCM_SAL_R5RS_ENV
#define SCM_NULL_ENV        SCM_SAL_NULL_ENV

#define SCM_ENVP(env)       SCM_SAL_ENVP(env)

/*============================================================================
  Abstract ScmObj Reference For Storage-Representation Independent Efficient
  List Operations
============================================================================*/
#define SCM_INVALID_REF       SCM_SAL_INVALID_REF

#define SCM_REF_CAR(kons)     SCM_SAL_REF_CAR(kons)
#define SCM_REF_CDR(kons)     SCM_SAL_REF_CDR(kons)
#define SCM_REF_OFF_HEAP(obj) SCM_SAL_REF_OFF_HEAP(obj)

/* SCM_DEREF(ref) is not permitted to be used as lvalue */
#define SCM_DEREF(ref)        SCM_SAL_DEREF(ref)

/* RFC: Is there a better name? */
#define SCM_SET(ref, obj)     SCM_SAL_SET((ref), (obj))

/*============================================================================
  Special Constants and Predicates
============================================================================*/
#define SCM_INVALID SCM_SAL_INVALID
#define SCM_NULL    SCM_SAL_NULL
#define SCM_TRUE    SCM_SAL_TRUE
#define SCM_FALSE   SCM_SAL_FALSE
#define SCM_EOF     SCM_SAL_EOF
#define SCM_UNBOUND SCM_SAL_UNBOUND
#define SCM_UNDEF   SCM_SAL_UNDEF

#define SCM_EQ(a, b)   (SCM_SAL_EQ((a), (b)))
#define SCM_NULLP(a)   (SCM_EQ((a),  SCM_NULL))
#define SCM_FALSEP(a)  (SCM_EQ((a),  SCM_FALSE))
#define SCM_NFALSEP(a) (!SCM_EQ((a), SCM_FALSE))
#define SCM_EOFP(a)    (SCM_EQ((a),  SCM_EOF))

/*============================================================================
  Predefined Symbols
============================================================================*/
/* for list construction */
#define SCM_SYM_QUOTE            SCM_SAL_SYM_QUOTE
#define SCM_SYM_QUASIQUOTE       SCM_SAL_SYM_QUASIQUOTE
#define SCM_SYM_UNQUOTE          SCM_SAL_SYM_UNQUOTE
#define SCM_SYM_UNQUOTE_SPLICING SCM_SAL_SYM_UNQUOTE_SPLICING

#else /* SCM_USE_STORAGE_ABSTRACTION_LAYER */

/* type declaration */
#if SCM_OBJ_COMPACT
#include "sigschemetype-compact.h"
#else
#include "sigschemetype.h"
#endif
#endif /* SCM_USE_STORAGE_ABSTRACTION_LAYER */

/*=======================================
   Variable Declarations
=======================================*/
/* storage-gc.c */
#if SCM_GCC4_READY_GC
/*
 * The variable to ensure that a call of scm_gc_protect_stack() is
 * uninlined in portable way through (*f)().
 *
 * Don't access this variables directly. Use SCM_GC_PROTECTED_CALL*() instead.
 */
extern ScmObj *(*volatile scm_gc_protect_stack)(ScmObj *);
#endif /* SCM_GCC4_READY_GC */

/*=======================================
   Function Declarations
=======================================*/
/*===========================================================================
   SigScheme : Core Functions
===========================================================================*/
/* sigscheme.c */
void scm_initialize(void);
void scm_finalize(void);
void scm_define_alias(const char *newsym, const char *sym);
void scm_provide(ScmObj feature);
int  scm_providedp(ScmObj feature);
int  scm_use(const char *feature);
ScmObj scm_s_use(ScmObj feature, ScmObj env);
ScmObj scm_eval_c_string(const char *exp);
#if SCM_COMPAT_SIOD
ScmObj scm_return_value(void);
#endif

/* Procedure/Syntax Registration */
void scm_register_reduction_operator(const char *name, ScmObj (*func)(ScmObj, ScmObj, enum ScmReductionState*));
void scm_register_syntax_fixed_0(const char *name, ScmObj (*func)(ScmObj));
#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_syntax_fixed_1(const char *name, ScmObj (*func)(ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_syntax_fixed_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_syntax_fixed_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_syntax_fixed_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_syntax_fixed_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj));
#endif
void scm_register_syntax_fixed_tailrec_0(const char *name, ScmObj (*func)(ScmEvalState*));
#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_syntax_fixed_tailrec_1(const char *name, ScmObj (*func)(ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_syntax_fixed_tailrec_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_syntax_fixed_tailrec_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_syntax_fixed_tailrec_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_syntax_fixed_tailrec_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
void scm_register_syntax_variadic_0(const char *name, ScmObj (*func)(ScmObj, ScmObj));
#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_syntax_variadic_1(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_syntax_variadic_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_syntax_variadic_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_syntax_variadic_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_syntax_variadic_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj));
#endif
void scm_register_syntax_variadic_tailrec_0(const char *name, ScmObj (*func)(ScmObj, ScmEvalState*));
#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_syntax_variadic_tailrec_1(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_syntax_variadic_tailrec_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_syntax_variadic_tailrec_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_syntax_variadic_tailrec_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_syntax_variadic_tailrec_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
void scm_register_procedure_fixed_0(const char *name, ScmObj (*func)());
#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_procedure_fixed_1(const char *name, ScmObj (*func)(ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_procedure_fixed_2(const char *name, ScmObj (*func)(ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_procedure_fixed_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_procedure_fixed_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_procedure_fixed_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj));
#endif
void scm_register_procedure_fixed_tailrec_0(const char *name, ScmObj (*func)(ScmEvalState*));
#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_procedure_fixed_tailrec_1(const char *name, ScmObj (*func)(ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_procedure_fixed_tailrec_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_procedure_fixed_tailrec_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_procedure_fixed_tailrec_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_procedure_fixed_tailrec_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
void scm_register_procedure_variadic_0(const char *name, ScmObj (*func)(ScmObj));
#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_procedure_variadic_1(const char *name, ScmObj (*func)(ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_procedure_variadic_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_procedure_variadic_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_procedure_variadic_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_procedure_variadic_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj));
#endif
void scm_register_procedure_variadic_tailrec_0(const char *name, ScmObj (*func)(ScmObj, ScmEvalState*));
#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_procedure_variadic_tailrec_1(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_procedure_variadic_tailrec_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_procedure_variadic_tailrec_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_procedure_variadic_tailrec_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif
#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_procedure_variadic_tailrec_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState*));
#endif

/* alloc.c */
void *scm_malloc_aligned(size_t size);
void *scm_malloc(size_t size);
void *scm_calloc(size_t number, size_t size);
void *scm_realloc(void *ptr, size_t size);

/* storage.c */
/* Don't use these functions directly. Use SCM_MAKE_*() or MAKE_*() instead to
 * allow flexible object allocation. */
/* FIXME: rename to scm_make_*() */
ScmObj scm_make_cons(ScmObj a, ScmObj b);
ScmObj scm_make_int(int val);
ScmObj scm_make_symbol(char *name, ScmObj v_cell);
ScmObj scm_make_char(int val);
ScmObj scm_make_immutable_string(char *str);
ScmObj scm_make_immutable_string_copying(const char *str);
ScmObj scm_make_string(char *str);
ScmObj scm_make_string_copying(const char *str);
ScmObj scm_make_func(enum ScmFuncTypeCode type, ScmFuncType func);
ScmObj scm_make_closure(ScmObj exp, ScmObj env);
ScmObj scm_make_vector(ScmObj *vec, int len);
ScmObj scm_make_port(ScmCharPort *cport, enum ScmPortFlag flag);
ScmObj scm_make_continuation(void);
#if !SCM_USE_VALUECONS
ScmObj scm_make_value_packet(ScmObj values);
#endif
#if SCM_USE_NONSTD_FEATURES
ScmObj scm_make_cpointer(void *data);
ScmObj scm_make_cfunc_pointer(ScmCFunc func);
#endif

/* storage-gc.c */
void scm_gc_protect(ScmObj *var);
void scm_gc_unprotect(ScmObj *var);
#if SCM_GCC4_READY_GC
/*
 * Ordinary programs should not call these functions directly. Use
 * SCM_GC_PROTECTED_CALL*() instead.
 */
#ifdef __GNUC__
#define scm_gc_protect_stack scm_gc_protect_stack_internal
#else /* __GNUC__ */
#define scm_gc_protect_stack (*scm_gc_protect_stack)
#endif /* __GNUC__ */

ScmObj *scm_gc_protect_stack_internal(ScmObj *designated_stack_start) SCM_NOINLINE;
#else /* SCM_GCC4_READY_GC */
void   scm_gc_protect_stack(ScmObj *stack_start);
#endif /* SCM_GCC4_READY_GC */
void   scm_gc_unprotect_stack(ScmObj *stack_start);

/* storage-symbol.c */
ScmObj scm_intern(const char *name);
ScmObj scm_symbol_bound_to(ScmObj obj);

/* eval.c */
ScmObj scm_p_eval(ScmObj obj, ScmObj env);
ScmObj scm_p_apply(ScmObj proc, ScmObj arg0, ScmObj rest, ScmEvalState *eval_state);
ScmObj scm_s_quote(ScmObj datum, ScmObj env);
ScmObj scm_s_lambda(ScmObj formals, ScmObj body, ScmObj env);
ScmObj scm_s_if(ScmObj test, ScmObj conseq, ScmObj rest, ScmEvalState *eval_state);
ScmObj scm_s_setd(ScmObj var, ScmObj val, ScmObj env);
ScmObj scm_s_cond(ScmObj args, ScmEvalState *eval_state);
ScmObj scm_s_case(ScmObj key, ScmObj args, ScmEvalState *eval_state);
ScmObj scm_s_and(ScmObj args, ScmEvalState *eval_state);
ScmObj scm_s_or(ScmObj args, ScmEvalState *eval_state);
ScmObj scm_s_let(ScmObj args, ScmEvalState *eval_state);
ScmObj scm_s_letstar(ScmObj bindings, ScmObj body, ScmEvalState *eval_state);
ScmObj scm_s_letrec(ScmObj bindings, ScmObj body, ScmEvalState *eval_state);
ScmObj scm_s_begin(ScmObj args, ScmEvalState *eval_state);
ScmObj scm_s_do(ScmObj bindings, ScmObj testframe, ScmObj commands, ScmEvalState *eval_state);
ScmObj scm_s_delay(ScmObj expr, ScmObj env);
ScmObj scm_s_quasiquote(ScmObj datum, ScmObj env);
ScmObj scm_s_unquote(ScmObj dummy, ScmObj env);
ScmObj scm_s_unquote_splicing(ScmObj dummy, ScmObj env);
ScmObj scm_s_define(ScmObj var, ScmObj rest, ScmObj env);
ScmObj scm_p_scheme_report_environment(ScmObj version);
ScmObj scm_p_null_environment(ScmObj version);
ScmObj scm_p_interaction_environment(void);

ScmObj scm_call(ScmObj proc, ScmObj args);

/* operations.c */
ScmObj scm_p_eqvp(ScmObj obj1, ScmObj obj2);
ScmObj scm_p_eqp(ScmObj obj1, ScmObj obj2);
ScmObj scm_p_equalp(ScmObj obj1, ScmObj obj2);
ScmObj scm_p_add(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_subtract(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_multiply(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_divide(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_equal(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_less(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_less_eq(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_greater(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_greater_eq(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_numberp(ScmObj obj);
ScmObj scm_p_zerop(ScmObj scm_num);
ScmObj scm_p_positivep(ScmObj scm_num);
ScmObj scm_p_negativep(ScmObj scm_num);
ScmObj scm_p_oddp(ScmObj scm_num);
ScmObj scm_p_evenp(ScmObj scm_num);
ScmObj scm_p_max(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_min(ScmObj left, ScmObj right, enum ScmReductionState *state);
ScmObj scm_p_abs(ScmObj scm_num);
ScmObj scm_p_quotient(ScmObj scm_n1, ScmObj scm_n2);
ScmObj scm_p_modulo(ScmObj scm_n1, ScmObj scm_n2);
ScmObj scm_p_remainder(ScmObj scm_n1, ScmObj scm_n2);
ScmObj scm_p_number2string (ScmObj num, ScmObj args);
ScmObj scm_p_string2number(ScmObj str, ScmObj args);
ScmObj scm_p_not(ScmObj obj);
ScmObj scm_p_booleanp(ScmObj obj);
ScmObj scm_p_car(ScmObj obj);
ScmObj scm_p_cdr(ScmObj obj);
ScmObj scm_p_pairp(ScmObj obj);
ScmObj scm_p_cons(ScmObj car, ScmObj cdr);
ScmObj scm_p_set_card(ScmObj pair, ScmObj car);
ScmObj scm_p_set_cdrd(ScmObj pair, ScmObj cdr);
ScmObj scm_p_caar(ScmObj lst);
ScmObj scm_p_cadr(ScmObj lst);
ScmObj scm_p_cdar(ScmObj lst);
ScmObj scm_p_cddr(ScmObj lst);
ScmObj scm_p_caddr(ScmObj lst);
ScmObj scm_p_cdddr(ScmObj lst);
ScmObj scm_p_list(ScmObj args);
ScmObj scm_p_nullp(ScmObj obj);
ScmObj scm_p_listp(ScmObj obj);
ScmObj scm_p_length(ScmObj obj);
ScmObj scm_p_append(ScmObj args);
ScmObj scm_p_reverse(ScmObj lst);
ScmObj scm_p_list_tail(ScmObj lst, ScmObj scm_k);
ScmObj scm_p_list_ref(ScmObj lst, ScmObj scm_k);
ScmObj scm_p_memq(ScmObj obj, ScmObj lst);
ScmObj scm_p_memv(ScmObj obj, ScmObj lst);
ScmObj scm_p_member(ScmObj obj, ScmObj lst);
ScmObj scm_p_assq(ScmObj obj, ScmObj alist);
ScmObj scm_p_assv(ScmObj obj, ScmObj alist);
ScmObj scm_p_assoc(ScmObj obj, ScmObj alist);
ScmObj scm_p_symbolp(ScmObj obj);
ScmObj scm_p_symbol2string(ScmObj obj);
ScmObj scm_p_string2symbol(ScmObj str);

ScmObj scm_p_charp(ScmObj obj);
ScmObj scm_p_charequalp(ScmObj ch1, ScmObj ch2);
/* TODO : many comparing functions around char is unimplemented */
ScmObj scm_p_char_alphabeticp(ScmObj obj);
ScmObj scm_p_char_numericp(ScmObj obj);
ScmObj scm_p_char_whitespacep(ScmObj obj);
ScmObj scm_p_char_upper_casep(ScmObj obj);
ScmObj scm_p_char_lower_casep(ScmObj obj);
ScmObj scm_p_char2integer(ScmObj obj);
ScmObj scm_p_integer2char(ScmObj obj);
ScmObj scm_p_char_upcase(ScmObj obj);
ScmObj scm_p_char_downcase(ScmObj obj);

ScmObj scm_p_stringp(ScmObj obj);
ScmObj scm_p_make_string(ScmObj length, ScmObj args);
ScmObj scm_p_string(ScmObj args);
ScmObj scm_p_string_length(ScmObj str);
ScmObj scm_p_string_ref(ScmObj str, ScmObj k);
ScmObj scm_p_string_setd(ScmObj str, ScmObj k, ScmObj ch);
ScmObj scm_p_stringequalp(ScmObj str1, ScmObj str2);
/* TODO : many comparing functions around string is unimplemented */
ScmObj scm_p_substring(ScmObj str, ScmObj start, ScmObj end);
ScmObj scm_p_string_append(ScmObj args);
ScmObj scm_p_string2list(ScmObj str);
ScmObj scm_p_list2string(ScmObj lst);
ScmObj scm_p_string_copy(ScmObj str);
ScmObj scm_p_string_filld(ScmObj str, ScmObj ch);
ScmObj scm_p_vectorp(ScmObj obj);
ScmObj scm_p_make_vector(ScmObj vector_len, ScmObj args);
ScmObj scm_p_vector(ScmObj args);
ScmObj scm_p_vector_length(ScmObj vec);
ScmObj scm_p_vector_ref(ScmObj vec, ScmObj scm_k);
ScmObj scm_p_vector_setd(ScmObj vec, ScmObj scm_k, ScmObj obj);
ScmObj scm_p_vector2list(ScmObj vec);
ScmObj scm_p_list2vector(ScmObj lst);
ScmObj scm_p_vector_filld(ScmObj vec, ScmObj fill);
ScmObj scm_p_procedurep(ScmObj obj);
ScmObj scm_p_map(ScmObj proc, ScmObj args);
ScmObj scm_p_for_each(ScmObj proc, ScmObj args);
ScmObj scm_p_force(ScmObj closure);
ScmObj scm_p_call_with_current_continuation(ScmObj proc, ScmEvalState *eval_state);
ScmObj scm_p_values(ScmObj args);
ScmObj scm_p_call_with_values(ScmObj producer, ScmObj consumer, ScmEvalState *eval_state);
ScmObj scm_p_dynamic_wind(ScmObj before, ScmObj thunk, ScmObj after);

/* operations-r5rs-deepcadrs.c */
#if SCM_USE_DEEP_CADRS
ScmObj scm_p_caaar(ScmObj lst);
ScmObj scm_p_caadr(ScmObj lst);
ScmObj scm_p_cadar(ScmObj lst);
ScmObj scm_p_cdaar(ScmObj lst);
ScmObj scm_p_cdadr(ScmObj lst);
ScmObj scm_p_cddar(ScmObj lst);
ScmObj scm_p_caaaar(ScmObj lst);
ScmObj scm_p_caaadr(ScmObj lst);
ScmObj scm_p_caadar(ScmObj lst);
ScmObj scm_p_caaddr(ScmObj lst);
ScmObj scm_p_cadaar(ScmObj lst);
ScmObj scm_p_cadadr(ScmObj lst);
ScmObj scm_p_caddar(ScmObj lst);
ScmObj scm_p_cadddr(ScmObj lst);
ScmObj scm_p_cdaaar(ScmObj lst);
ScmObj scm_p_cdaadr(ScmObj lst);
ScmObj scm_p_cdadar(ScmObj lst);
ScmObj scm_p_cdaddr(ScmObj lst);
ScmObj scm_p_cddaar(ScmObj lst);
ScmObj scm_p_cddadr(ScmObj lst);
ScmObj scm_p_cdddar(ScmObj lst);
ScmObj scm_p_cddddr(ScmObj lst);
#endif /* SCM_USE_DEEP_CADRS */

/* operations-nonstd.c */
#if SCM_USE_NONSTD_FEATURES
void scm_initialize_nonstd_features(void);
ScmObj scm_p_symbol_boundp(ScmObj sym, ScmObj rest);
ScmObj scm_p_load_path(void);
/* FIXME: add ScmObj scm_require(const char *c_filename); */
ScmObj scm_p_require(ScmObj filename);
ScmObj scm_p_provide(ScmObj feature);
ScmObj scm_p_providedp(ScmObj feature);
ScmObj scm_p_file_existsp(ScmObj filepath);
ScmObj scm_p_delete_file(ScmObj filepath);
#endif

/* io.c */
void   scm_set_lib_path(const char *path);
ScmObj scm_make_shared_file_port(FILE *file, const char *aux_info,
                              enum ScmPortFlag flag);
void scm_port_printf(ScmObj port, const char *fmt, ...);
void scm_port_vprintf(ScmObj port, const char *fmt, va_list args);
void scm_port_newline(ScmObj port);
void scm_error_printf(const char *fmt, ...);
void scm_error_vprintf(const char *fmt, va_list args);
void scm_error_newline(void);

ScmObj scm_p_call_with_input_file(ScmObj filepath, ScmObj proc);
ScmObj scm_p_call_with_output_file(ScmObj filepath, ScmObj proc);
ScmObj scm_p_input_portp(ScmObj obj);
ScmObj scm_p_output_portp(ScmObj obj);
ScmObj scm_p_current_input_port(void);
ScmObj scm_p_current_output_port(void);
ScmObj scm_p_with_input_from_file(ScmObj filepath, ScmObj thunk);
ScmObj scm_p_with_output_to_file(ScmObj filepath, ScmObj thunk);
ScmObj scm_p_open_input_file(ScmObj filepath);
ScmObj scm_p_open_output_file(ScmObj filepath);
ScmObj scm_p_close_input_port(ScmObj port);
ScmObj scm_p_close_output_port(ScmObj port);

ScmObj scm_p_read(ScmObj args);
ScmObj scm_p_read_char(ScmObj args);
ScmObj scm_p_peek_char(ScmObj args);
ScmObj scm_p_eof_objectp(ScmObj obj);
ScmObj scm_p_char_readyp(ScmObj args);
ScmObj scm_p_write(ScmObj obj, ScmObj args);
ScmObj scm_p_display(ScmObj obj, ScmObj args);
ScmObj scm_p_newline(ScmObj args);
ScmObj scm_p_write_char(ScmObj obj, ScmObj args);

ScmObj scm_load(const char *c_filename);
ScmObj scm_p_load(ScmObj filename);

/* read.c */
ScmObj scm_read(ScmObj port);
ScmObj scm_read_char(ScmObj port);

/* error.c */
int  scm_debug_categories(void);
void scm_set_debug_categories(int categories);
int  scm_predefined_debug_categories(void);
void scm_categorized_debug(int category, const char *msg, ...);
void scm_debug(const char *msg, ...);
int  scm_die(const char *msg, const char *filename, int line);
void scm_error(const char *msg, ...) SCM_NORETURN;
void scm_error_obj(const char *funcname, const char *msg, ScmObj obj) SCM_NORETURN;
void scm_show_backtrace(ScmObj trace_stack);
ScmObj scm_make_error_obj(ScmObj reason, ScmObj objs);
void scm_raise_error(ScmObj err_obj) SCM_NORETURN;
void scm_fatal_error(const char *msg) SCM_NORETURN;
void scm_set_fatal_error_callback(void (*cb)(void));
ScmObj scm_p_error_objectp(ScmObj obj);
ScmObj scm_p_fatal_error(ScmObj err_obj) SCM_NORETURN;
ScmObj scm_p_inspect_error(ScmObj err_obj);
ScmObj scm_p_backtrace(void);

/* print.c */
void scm_display(ScmObj obj);
void scm_write_to_port(ScmObj port, ScmObj obj);
void scm_display_to_port(ScmObj port, ScmObj obj);
#if SCM_USE_SRFI38
void scm_write_to_port_with_shared_structure(ScmObj port, ScmObj obj);
#endif


/*===========================================================================
   SigScheme : Optional Funtions
===========================================================================*/
#if SCM_USE_SRFI1
/* operations-srfi1.c */
void   scm_initialize_srfi1(void);
ScmObj scm_p_srfi1_xcons(ScmObj a, ScmObj b);
ScmObj scm_p_srfi1_consstar(ScmObj args);
ScmObj scm_p_srfi1_make_list(ScmObj length, ScmObj args);
ScmObj scm_p_srfi1_list_tabulate(ScmObj scm_n, ScmObj args);
ScmObj scm_p_srfi1_list_copy(ScmObj lst);
ScmObj scm_p_srfi1_circular_list(ScmObj args);
ScmObj scm_p_srfi1_iota(ScmObj scm_count, ScmObj args);
ScmObj scm_p_srfi1_proper_listp(ScmObj lst);
ScmObj scm_p_srfi1_circular_listp(ScmObj lst);
ScmObj scm_p_srfi1_dotted_listp(ScmObj lst);
ScmObj scm_p_srfi1_not_pairp(ScmObj pair);
ScmObj scm_p_srfi1_null_listp(ScmObj lst);
ScmObj scm_p_srfi1_listequal(ScmObj eqproc, ScmObj args);
ScmObj scm_p_srfi1_first(ScmObj lst);
ScmObj scm_p_srfi1_second(ScmObj lst);
ScmObj scm_p_srfi1_third(ScmObj lst);
ScmObj scm_p_srfi1_fourth(ScmObj lst);
ScmObj scm_p_srfi1_fifth(ScmObj lst);
ScmObj scm_p_srfi1_sixth(ScmObj lst);
ScmObj scm_p_srfi1_seventh(ScmObj lst);
ScmObj scm_p_srfi1_eighth(ScmObj lst);
ScmObj scm_p_srfi1_ninth(ScmObj lst);
ScmObj scm_p_srfi1_tenth(ScmObj lst);
ScmObj scm_p_srfi1_carpluscdr(ScmObj lst);
ScmObj scm_p_srfi1_take(ScmObj lst, ScmObj scm_idx);
ScmObj scm_p_srfi1_drop(ScmObj lst, ScmObj scm_idx);
ScmObj scm_p_srfi1_take_right(ScmObj lst, ScmObj scm_elem);
ScmObj scm_p_srfi1_drop_right(ScmObj lst, ScmObj scm_elem);
ScmObj scm_p_srfi1_taked(ScmObj lst, ScmObj scm_idx);
ScmObj scm_p_srfi1_drop_rightd(ScmObj lst, ScmObj scm_idx);
ScmObj scm_p_srfi1_split_at(ScmObj lst, ScmObj idx);
ScmObj scm_p_srfi1_split_atd(ScmObj lst, ScmObj idx);
ScmObj scm_p_srfi1_last(ScmObj lst);
ScmObj scm_p_srfi1_last_pair(ScmObj lst);
ScmObj scm_p_srfi1_lengthplus(ScmObj lst);
ScmObj scm_p_srfi1_concatenate(ScmObj args);
#endif
#if SCM_USE_SRFI2
/* operations-srfi2.c */
void   scm_initialize_srfi2(void);
ScmObj scm_s_srfi2_and_letstar(ScmObj claws, ScmObj body, ScmEvalState *eval_state);
#endif
#if SCM_USE_SRFI6
/* operations-srfi6.c */
void   scm_initialize_srfi6(void);
ScmObj scm_p_srfi6_open_input_string(ScmObj str);
ScmObj scm_p_srfi6_open_output_string(void);
ScmObj scm_p_srfi6_get_output_string(ScmObj port);
#endif
#if SCM_USE_SRFI8
/* operations-srfi8.c */
void   scm_initialize_srfi8(void);
ScmObj scm_s_srfi8_receive(ScmObj formals, ScmObj expr, ScmObj body, ScmEvalState *eval_state);
#endif
#if SCM_USE_SRFI23
/* operations-srfi23.c */
void   scm_initialize_srfi23(void);
ScmObj scm_p_srfi23_error(ScmObj reason, ScmObj args);
#endif
#if SCM_USE_SRFI34
/* operations-srfi34.c */
void  scm_initialize_srfi34(void);
ScmObj scm_p_srfi34_with_exception_handler(ScmObj handler, ScmObj thunk);
ScmObj scm_s_srfi34_guard(ScmObj cond_catch, ScmObj body,
                           ScmEvalState *eval_state);
ScmObj scm_p_srfi34_raise(ScmObj obj);
#endif
#if SCM_USE_SRFI38
/* operations-srfi38.c */
void   scm_initialize_srfi38(void);
ScmObj scm_p_srfi38_write_with_shared_structure(ScmObj obj, ScmObj args);
#endif
#if SCM_USE_SRFI60
/* operations-srfi60.c */
void   scm_initialize_srfi60(void);
ScmObj scm_p_srfi60_logand(ScmObj left, ScmObj right,
                           enum ScmReductionState *state);
ScmObj scm_p_srfi60_logior(ScmObj left, ScmObj right,
                           enum ScmReductionState *state);
ScmObj scm_p_srfi60_logxor(ScmObj left, ScmObj right,
                           enum ScmReductionState *state);
ScmObj scm_p_srfi60_lognot(ScmObj n);
ScmObj scm_p_srfi60_bitwise_if(ScmObj mask, ScmObj n0, ScmObj n1);
ScmObj scm_p_srfi60_logtest(ScmObj j, ScmObj k);
#endif
#if SCM_COMPAT_SIOD
/* operations-siod.c */
void   scm_initialize_siod(void);
ScmObj scm_p_symbol_value(ScmObj var);
ScmObj scm_p_set_symbol_valued(ScmObj var, ScmObj val);
ScmObj scm_p_siod_equal(ScmObj obj1, ScmObj obj2);
ScmObj scm_p_the_environment(ScmEvalState *eval_state);
ScmObj scm_p_closure_code(ScmObj closure);
ScmObj scm_p_verbose(ScmObj args);
ScmObj scm_p_eof_val(void);
ScmObj scm_s_undefine(ScmObj var, ScmObj env);
long   scm_get_verbose_level(void);
void   scm_set_verbose_level(long level);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SIGSCHEME_H */
