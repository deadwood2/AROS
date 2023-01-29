/*
    Copyright © 2022-2023, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <aros/cpu.h>

#if 0
#include <CUnit/CUnitCI.h>
#else
#include <exec/types.h>
#include <proto/dos.h>

#define CU_SUITE_SETUP      static int __cu_suite_setup
#define CU_SUITE_TEARDOWN   static int __cu_suite_teardown
#define CU_TEST_SETUP       static void __cu_test_setup
#define CU_TEST_TEARDOWN    static void __cu_test_teardown
#define CUE_SUCCESS         0

#define CU_ASSERT_EQUAL(expected, actual) \
    CU_ASSERT(expected == actual)

#define CU_ASSERT(expr)                                 \
    if (!(expr))                                        \
    {                                                   \
        CONST_STRPTR f = __FILE__;                      \
        ULONG _tags[] = { (ULONG)f, __LINE__};          \
        VPrintf("Assertion failed %s:%ld\n", (RAWARG)_tags);    \
    }

#define CU_CI_DEFINE_SUITE(...) \
    __cu_suite_setup();         \

#define CUNIT_CI_TEST(func)                 \
    __cu_test_setup();                      \
    {                                       \
        CONST_STRPTR f = #func;             \
        ULONG _tags[] = { (ULONG) f};       \
        VPrintf("Running: %s\n", (RAWARG)_tags);    \
    }                                       \
    func();                                 \
    __cu_test_teardown();                   \

#define CU_CI_RUN_SUITES()      \
    __cu_suite_teardown();      \

#endif

CU_SUITE_SETUP()
{
    return CUE_SUCCESS;
}

CU_SUITE_TEARDOWN()
{
    return CUE_SUCCESS;
}

CU_TEST_SETUP()
{
}

CU_TEST_TEARDOWN()
{
}

/* Values validated on GNU Linux x86_64 */
#if (__WORDSIZE==64)
#if defined(__USE_LARGEFILE64)
 /* gcc main.c -o main -D_LARGEFILE64_SOURCE */
const int expectedF4[] = {8,  8, 8, 8};
const int expectedF2[] = {8, 8};
#else
 /* gcc main.c -o main.o */
const int expectedF4[] = {8, -1, 8, 8};
const int expectedF2[] = {8, 8};
#endif
#else
#if defined(__USE_FILE_OFFSET64)
 /* gcc main.c -o main -m32 -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 */
const int expectedF4[] = {8,  8, 4, 8};
const int expectedF2[] = {4, 4};
#elif defined(__USE_LARGEFILE64)
 /* gcc main.c -o main -m32 -D_LARGEFILE64_SOURCE */
const int expectedF4[] = {4,  8, 4, 8};
const int expectedF2[] = {4, 4};
#else
 /* gcc main.c -o main -m32 */
const int expectedF4[] = {4, -1, 4, 8};
const int expectedF2[] = {4, 4};
#endif
#endif

static void test_ino_t()
{
    CU_ASSERT_EQUAL(expectedF4[0], sizeof(ino_t));
#if defined(__USE_LARGEFILE64)
    CU_ASSERT_EQUAL(expectedF4[1], sizeof(ino64_t));
#endif
    CU_ASSERT_EQUAL(expectedF4[2], sizeof(__ino_t));
    CU_ASSERT_EQUAL(expectedF4[3], sizeof(__ino64_t));
}

static void test_off_t()
{
    CU_ASSERT_EQUAL(expectedF4[0], sizeof(off_t));
#if defined(__USE_LARGEFILE64)
    CU_ASSERT_EQUAL(expectedF4[1], sizeof(off64_t));
#endif
    CU_ASSERT_EQUAL(expectedF4[2], sizeof(__off_t));
    CU_ASSERT_EQUAL(expectedF4[3], sizeof(__off64_t));
}

static void test_blkcnt_t()
{
    CU_ASSERT_EQUAL(expectedF4[0], sizeof(blkcnt_t));
#if defined(__USE_LARGEFILE64)
    CU_ASSERT_EQUAL(expectedF4[1], sizeof(blkcnt64_t));
#endif
    CU_ASSERT_EQUAL(expectedF4[2], sizeof(__blkcnt_t));
    CU_ASSERT_EQUAL(expectedF4[3], sizeof(__blkcnt64_t));
}

static void test_fsblkcnt_t()
{
    CU_ASSERT_EQUAL(expectedF4[0], sizeof(fsblkcnt_t));
#if defined(__USE_LARGEFILE64)
    CU_ASSERT_EQUAL(expectedF4[1], sizeof(fsblkcnt64_t));
#endif
    CU_ASSERT_EQUAL(expectedF4[2], sizeof(__fsblkcnt_t));
    CU_ASSERT_EQUAL(expectedF4[3], sizeof(__fsblkcnt64_t));
}

static void test_fsfilcnt_t()
{
    CU_ASSERT_EQUAL(expectedF4[0], sizeof(fsfilcnt_t));
#if defined(__USE_LARGEFILE64)
    CU_ASSERT_EQUAL(expectedF4[1], sizeof(fsfilcnt64_t));
#endif
    CU_ASSERT_EQUAL(expectedF4[2], sizeof(__fsfilcnt_t));
    CU_ASSERT_EQUAL(expectedF4[3], sizeof(__fsfilcnt64_t));
}

static void test_fpos_t()
{
    CU_ASSERT_EQUAL(expectedF4[0], sizeof(fpos_t));
#if defined(__USE_LARGEFILE64)
    CU_ASSERT_EQUAL(expectedF4[1], sizeof(fpos64_t));
#endif
    CU_ASSERT_EQUAL(expectedF4[2], sizeof(__fpos_t));
    CU_ASSERT_EQUAL(expectedF4[3], sizeof(__fpos64_t));
}

static void test_blksize_t()
{
    CU_ASSERT_EQUAL(expectedF2[0], sizeof(blksize_t));
    CU_ASSERT_EQUAL(expectedF2[1], sizeof(__blksize_t));
}

static void test_time_t()
{
    CU_ASSERT_EQUAL(4, sizeof(time_t));
    CU_ASSERT_EQUAL(4, sizeof(__time_t));
}

static void test_dev_t()
{
#if (__WORDSIZE==64)
    CU_ASSERT_EQUAL(8, sizeof(dev_t));
#else
    /* Note: dev_t is always 8-byte on GNU Linux, AROS differs here */
    CU_ASSERT_EQUAL(4, sizeof(dev_t));
#endif
}

#if defined(__USE_FILE_OFFSET64)
const char *suitename = "types64-lfs-fob_Suite";
#elif defined(__USE_LARGEFILE64)
const char *suitename = "types64-lfs_Suite";
#else
const char *suitename = "types64_Suite";
#endif

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(suitename, __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);
    CUNIT_CI_TEST(test_ino_t);
    CUNIT_CI_TEST(test_off_t);
    CUNIT_CI_TEST(test_blkcnt_t);
    CUNIT_CI_TEST(test_fsblkcnt_t);
    CUNIT_CI_TEST(test_fsfilcnt_t);
    CUNIT_CI_TEST(test_fpos_t);
    CUNIT_CI_TEST(test_time_t);
    CUNIT_CI_TEST(test_blksize_t);
    CUNIT_CI_TEST(test_dev_t);
    return CU_CI_RUN_SUITES();
}
