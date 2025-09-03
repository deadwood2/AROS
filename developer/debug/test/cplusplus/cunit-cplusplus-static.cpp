/*
    Copyright © 2024, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdlib.h>

#include <CUnit/CUnitCI.h>

/* Purpose of this program is to test that atexit can already be called at CTOR stage (after opening libraries, but
   before running __progonly_program_startup via PROGRAM_ENTRIES set)
*/

/* START simple case */
class Foo
{
public:
    Foo();
    ~Foo();
};

Foo::Foo()
{
    /* This will only be called after program has started via the Bar::func call in unit test,
       so calling (by C++ code) atexit here is always safe */
}

Foo::~Foo()
{
    /* This is called from registers atexit handler */
}

class Bar
{
    public:
        static Foo & func();
};

Foo & Bar::func()
{
    static Foo obj;
    return obj;
}
/* END simple case */

/* START complex case */
class Foo2
{
public:
    Foo2();
    ~Foo2();
};

Foo2::Foo2()
{
    /* This will be called via the constructor of Bar2 global object and part of CTORS set before program has
       started. */
}

Foo2::~Foo2()
{
    /* This is called from registers atexit handler */
}

class Bar2
{
public:
    Bar2();
};

Bar2::Bar2()
{
    static Foo2 obj;
}

Bar2 globalBar2;
/* END complex case */

/* Capture atexit to allow tracking of actions */
int atexit_captured     = 0;
int atexit_call_count   = 0;

AROS_IMPORT_ASM_SYM(int, dummy, __includecrtprogramstartuphandling);
extern "C" int __progonly_atexit(void (*func)(void));
extern "C" int atexit(void (*func)(void))
{
    atexit_captured = 1;
    atexit_call_count++;

    return __progonly_atexit(func);
}

static void test_atexit_called_for_contructor_of_static_object()
{
    CU_ASSERT_EQUAL_FATAL(atexit_captured, 1);
    CU_ASSERT_EQUAL(atexit_call_count, 1);
    Bar::func();
    CU_ASSERT_EQUAL(atexit_call_count, 2);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(CPlusPlus_Static_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_atexit_called_for_contructor_of_static_object);

    return CU_CI_RUN_SUITES();
}
