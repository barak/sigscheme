/*===========================================================================
 *  FileName : module.c
 *  About    : Code module handlings
 *
 *  Copyright (C) 2005-2006 Kazuki Ohta <mover AT hct.zaq.ne.jp>
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
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 *  IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
===========================================================================*/

#include "config.h"

/*=======================================
  System Include
=======================================*/
#include <stddef.h>
#include <string.h>

/*=======================================
  Local Include
=======================================*/
#include "sigscheme.h"
#include "sigschemeinternal.h"

/*=======================================
  File Local Struct Declarations
=======================================*/
struct module_info {
    const char *name;
    void (*initializer)(void);
};

/*=======================================
  File Local Macro Declarations
=======================================*/

/*=======================================
  Variable Declarations
=======================================*/
static ScmObj features;

static const struct module_info module_info_table[] = {
#if SCM_USE_SSCM_EXTENSIONS
    {"sscm-ext", scm_initialize_sscm_extensions},
#endif
#if SCM_USE_SRFI1
    {"srfi-1", scm_initialize_srfi1},
#endif
#if SCM_USE_SRFI2
    {"srfi-2", scm_initialize_srfi2},
#endif
#if SCM_USE_SRFI6
    {"srfi-6", scm_initialize_srfi6},
#endif
#if SCM_USE_SRFI8
    {"srfi-8", scm_initialize_srfi8},
#endif
#if SCM_USE_SRFI23
    {"srfi-23", scm_initialize_srfi23},
#endif
#if SCM_USE_SRFI28
    {"srfi-28", scm_initialize_srfi28},
#endif
#if SCM_USE_SRFI34
    {"srfi-34", scm_initialize_srfi34},
#endif
#if SCM_USE_SRFI38
    {"srfi-38", scm_initialize_srfi38},
#endif
#if SCM_USE_SRFI48
    {"srfi-48", scm_initialize_srfi48},
#endif
#if SCM_USE_SRFI60
    {"srfi-60", scm_initialize_srfi60},
#endif
#if SCM_COMPAT_SIOD
    {"siod", scm_initialize_siod},
#endif
    {NULL, NULL}
};

/*=======================================
  File Local Function Declarations
=======================================*/
static scm_bool scm_use_internal(const char *feature);

/*=======================================
  Function Implementations
=======================================*/
void
scm_init_module(void)
{
    scm_gc_protect_with_init(&features, SCM_NULL);
}

void
scm_provide(ScmObj feature)
{
    features = CONS(feature, features);
}

scm_bool
scm_providedp(ScmObj feature)
{
    return NFALSEP(scm_p_member(feature, features));
}

scm_bool
scm_use(const char *feature)
{
    scm_bool ok;
#if !SCM_GCC4_READY_GC
    ScmObj stack_start;
#endif

#if SCM_GCC4_READY_GC
    SCM_GC_PROTECTED_CALL(ok, scm_bool, scm_use_internal, (feature));
#else
    scm_gc_protect_stack(&stack_start);

    ok = scm_use_internal(feature);

    scm_gc_unprotect_stack(&stack_start);
#endif

    return ok;
}

static scm_bool
scm_use_internal(const char *feature)
{
    ScmObj ok;

    SCM_ASSERT(feature);

    ok = scm_s_use(scm_intern(feature), SCM_INTERACTION_ENV);
    return NFALSEP(ok);
}

/*
 * TODO:
 * - Make the interface and semantics of 'use' similar to other Scheme
 *   implementations such as Gauche. This is important to make *.scm file
 *   portable
 * - Make a *.scm file loadable via this interface (if necessary to make
 *   similar to other Scheme implementations), and make consistent with
 *   'require'
 * - Make the 'module' concept similar to other Scheme implementations and R6RS
 * - Make the module_info_table dynamically registerable for dynamic loadable
 *   objects (if necessary)
 */
ScmObj
scm_s_use(ScmObj feature, ScmObj env)
{
    const struct module_info *mod;
    ScmObj feature_str;
    const char *c_feature_str;
    DECLARE_FUNCTION("use", syntax_fixed_1);

    ENSURE_SYMBOL(feature);

    c_feature_str = SCM_SYMBOL_NAME(feature);

    for (mod = module_info_table; mod->name; mod++) {
        if (strcmp(c_feature_str, mod->name) == 0) {
            feature_str = CONST_STRING(c_feature_str);
            if (!scm_providedp(feature_str)) {
                (*mod->initializer)();
                scm_provide(feature_str);
            }
            return SCM_TRUE;
        }
    }

    return SCM_FALSE;
}

/*===========================================================================
  Scheme Function Export Related Functions
===========================================================================*/
void
scm_define_alias(const char *newsym, const char *sym)
{
    SCM_SYMBOL_SET_VCELL(scm_intern(newsym),
                         SCM_SYMBOL_VCELL(scm_intern(sym)));
}

void
scm_register_funcs(const struct scm_func_registration_info *table)
{
    const struct scm_func_registration_info *info;

    for (info = &table[0]; info->funcname; info++) {
        scm_register_func(info->funcname, info->c_func, info->typecode);
    }
}

ScmObj
scm_register_func(const char *name, ScmFuncType c_func,
                  enum ScmFuncTypeCode type)
{
    ScmObj sym, func;

    sym  = scm_intern(name);
    func = MAKE_FUNC(type, c_func);

    /* TODO: reject bad TYPE */
    SCM_SYMBOL_SET_VCELL(sym, func);
    return func;
}

/* Not implemented yet. */
void scm_register_reduction_operator(const char *name, ScmObj (*func)(ScmObj, ScmObj, enum ScmReductionState *))
{
    scm_register_func(name, func, SCM_REDUCTION_OPERATOR);
}

/* So, yeah, um... this isn't really such a big deal if you think
 * about W32.... */
void scm_register_syntax_fixed_0(const char *name, ScmObj (*func)(ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED | 0);
}

#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_syntax_fixed_1(const char *name, ScmObj (*func)(ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED | 1);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_syntax_fixed_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED | 2);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_syntax_fixed_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED | 3);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_syntax_fixed_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED | 4);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_syntax_fixed_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED | 5);
}
#endif

void scm_register_syntax_fixed_tailrec_0(const char *name, ScmObj (*func)(ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED_TAIL_REC | 0);
}

#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_syntax_fixed_tailrec_1(const char *name, ScmObj (*func)(ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED_TAIL_REC | 1);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_syntax_fixed_tailrec_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED_TAIL_REC | 2);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_syntax_fixed_tailrec_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED_TAIL_REC | 3);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_syntax_fixed_tailrec_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED_TAIL_REC | 4);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_syntax_fixed_tailrec_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_FIXED_TAIL_REC | 5);
}
#endif

void scm_register_syntax_variadic_0(const char *name, ScmObj (*func)(ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC | 0);
}

#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_syntax_variadic_1(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC | 1);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_syntax_variadic_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC | 2);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_syntax_variadic_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC | 3);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_syntax_variadic_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC | 4);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_syntax_variadic_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC | 5);
}
#endif

void scm_register_syntax_variadic_tailrec_0(const char *name, ScmObj (*func)(ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC_TAIL_REC | 0);
}

#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_syntax_variadic_tailrec_1(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC_TAIL_REC | 1);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_syntax_variadic_tailrec_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC_TAIL_REC | 2);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_syntax_variadic_tailrec_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC_TAIL_REC | 3);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_syntax_variadic_tailrec_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC_TAIL_REC | 4);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_syntax_variadic_tailrec_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_SYNTAX_VARIADIC_TAIL_REC | 5);
}
#endif

void scm_register_procedure_fixed_0(const char *name, ScmObj (*func)())
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED | 0);
}

#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_procedure_fixed_1(const char *name, ScmObj (*func)(ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED | 1);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_procedure_fixed_2(const char *name, ScmObj (*func)(ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED | 2);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_procedure_fixed_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED | 3);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_procedure_fixed_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED | 4);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_procedure_fixed_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED | 5);
}
#endif

void scm_register_procedure_fixed_tailrec_0(const char *name, ScmObj (*func)(ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED_TAIL_REC | 0);
}

#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_procedure_fixed_tailrec_1(const char *name, ScmObj (*func)(ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED_TAIL_REC | 1);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_procedure_fixed_tailrec_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED_TAIL_REC | 2);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_procedure_fixed_tailrec_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED_TAIL_REC | 3);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_procedure_fixed_tailrec_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED_TAIL_REC | 4);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_procedure_fixed_tailrec_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_FIXED_TAIL_REC | 5);
}
#endif

void scm_register_procedure_variadic_0(const char *name, ScmObj (*func)(ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC | 0);
}

#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_procedure_variadic_1(const char *name, ScmObj (*func)(ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC | 1);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_procedure_variadic_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC | 2);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_procedure_variadic_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC | 3);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_procedure_variadic_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC | 4);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_procedure_variadic_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC | 5);
}
#endif

void scm_register_procedure_variadic_tailrec_0(const char *name, ScmObj (*func)(ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC_TAIL_REC | 0);
}

#if SCM_FUNCTYPE_MAND_MAX >= 1
void scm_register_procedure_variadic_tailrec_1(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC_TAIL_REC | 1);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 2
void scm_register_procedure_variadic_tailrec_2(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC_TAIL_REC | 2);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 3
void scm_register_procedure_variadic_tailrec_3(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC_TAIL_REC | 3);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 4
void scm_register_procedure_variadic_tailrec_4(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC_TAIL_REC | 4);
}
#endif

#if SCM_FUNCTYPE_MAND_MAX >= 5
void scm_register_procedure_variadic_tailrec_5(const char *name, ScmObj (*func)(ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmObj, ScmEvalState *))
{
    scm_register_func(name, func, SCM_PROCEDURE_VARIADIC_TAIL_REC | 5);
}
#endif
