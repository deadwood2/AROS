/*
    Copyright © 2024, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <clib/alib_protos.h>

#include "../test-util.h"

#if defined(__AROS__)
#include <CUnit/CUnitCI.h>
#endif

struct Device *TimerBase;
struct MsgPort *TimerMP;
struct timerequest *TimerIO;

CU_SUITE_SETUP()
{
    if (TimerMP = CreatePort(NULL, 0))
    {
        if (TimerIO = (struct timerequest *)CreateExtIO(TimerMP, sizeof(struct timerequest)))
        {
            if (!(OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *)TimerIO, 0)))
            {
                TimerBase = TimerIO->tr_node.io_Device;
                return CUE_SUCCESS;
            }
        }
    }

    CUE_SINIT_FAILED;
}

CU_SUITE_TEARDOWN()
{
    if (TimerBase)
        CloseDevice((struct IORequest *)TimerIO);
    if (TimerIO)
        DeleteExtIO((struct IORequest *)TimerIO);
    if (TimerMP)
        DeletePort(TimerMP);

    return CUE_SUCCESS;
}

static void test_timer_cmptime_basic()
{
    struct timeval t1, t2;
    t1.tv_secs  = 100;
    t1.tv_micro = 100;
    t2.tv_secs  = 100;
    t2.tv_micro = 100;

    CU_ASSERT_EQUAL(CmpTime(&t1, &t1), 0);
    CU_ASSERT_EQUAL(CmpTime(&t1, &t2), 0);

    t1.tv_micro = 99;
    CU_ASSERT_EQUAL(CmpTime(&t1, &t2), 1);

    t1.tv_micro = 101;
    CU_ASSERT_EQUAL(CmpTime(&t1, &t2), -1);

    t1.tv_secs = 99;
    CU_ASSERT_EQUAL(CmpTime(&t1, &t2), 1);

    t1.tv_secs = 101;
    CU_ASSERT_EQUAL(CmpTime(&t1, &t2), -1);
}

static void test_timer_cmptime_large()
{
    struct timeval t1, t2;
    t1.tv_secs  = 0xffff0000;
    t1.tv_micro = 0;
    t2.tv_secs  = 100;
    t2.tv_micro = 0;

    CU_ASSERT_EQUAL(CmpTime(&t1, &t1), 0);

    CU_ASSERT_EQUAL(CmpTime(&t1, &t2), -1);

    CU_ASSERT_EQUAL(CmpTime(&t2, &t1), 1);
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(Timer_Suite, __cu_suite_setup, __cu_suite_teardown, NULL, NULL);
    CUNIT_CI_TEST(test_timer_cmptime_basic);
    CUNIT_CI_TEST(test_timer_cmptime_large);
    return CU_CI_RUN_SUITES();
}
