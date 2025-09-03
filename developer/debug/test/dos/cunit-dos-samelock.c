/*
    Copyright © 2024, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>

#include "../test-util.h"

#if defined(__AROS__)
#include <CUnit/CUnitCI.h>
#endif

void test_samelock_same_different(void)
{
    BPTR lock1 = Lock("SYS:Libs/asl.library", SHARED_LOCK);
    BPTR lock2 = Lock("SYS:Libs/asl.library", SHARED_LOCK);
    BPTR lock3 = Lock("RAM:ENV/Language", SHARED_LOCK);
    BPTR lock4 = BNULL;
    LONG res;

    CU_ASSERT_NOT_EQUAL_FATAL(lock1, BNULL);
    CU_ASSERT_NOT_EQUAL_FATAL(lock2, BNULL);

    res = SameLock(lock1, lock2);
    CU_ASSERT_EQUAL(res, LOCK_SAME);

    res = SameLock(lock1, BNULL);
    CU_ASSERT_EQUAL(res, LOCK_DIFFERENT);

    res = SameLock(lock1, lock3);
    CU_ASSERT_EQUAL(res, LOCK_DIFFERENT);

    lock4 = DupLock(lock1);
    res = SameLock(lock2, lock4);
    CU_ASSERT_EQUAL(res, LOCK_SAME);

    UnLock(lock1);
    UnLock(lock2);
    UnLock(lock3);
    UnLock(lock4);
}

void test_samelock_same_volume(void)
{
    BPTR lock1 = Lock("SYS:Libs/asl.library", SHARED_LOCK);
    BPTR lock2 = Lock("SYS:Libs/locale.library", SHARED_LOCK);
    LONG res;

    CU_ASSERT_NOT_EQUAL_FATAL(lock1, BNULL);
    CU_ASSERT_NOT_EQUAL_FATAL(lock2, BNULL);

    res = SameLock(lock1, lock2);
    CU_ASSERT_EQUAL(res, LOCK_SAME_VOLUME);

    UnLock(lock1);
    UnLock(lock2);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(SameLock_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_samelock_same_different);
    CUNIT_CI_TEST(test_samelock_same_volume);
    return CU_CI_RUN_SUITES();
}
