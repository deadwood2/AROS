/*
    Copyright © 2024, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/locale.h>
#include <proto/exec.h>

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <CUnit/CUnitCI.h>


static void test_gmtime_r_should_convert_directly()
{
    time_t _zero = 0;
    time_t _7200 = 7200;
    struct tm _tm;

    gmtime_r(&_zero, &_tm);

    CU_ASSERT_EQUAL(_tm.tm_sec,     0);
    CU_ASSERT_EQUAL(_tm.tm_min,     0);
    CU_ASSERT_EQUAL(_tm.tm_hour,    0);
    CU_ASSERT_EQUAL(_tm.tm_mday,    1);
    CU_ASSERT_EQUAL(_tm.tm_mon,     0);
    CU_ASSERT_EQUAL(_tm.tm_year,    70);
    CU_ASSERT_EQUAL(_tm.tm_wday,    4);
    CU_ASSERT_EQUAL(_tm.tm_yday,    0);

    CU_ASSERT_EQUAL(_tm.tm_isdst,   0);
    CU_ASSERT_EQUAL(_tm.tm_gmtoff,  0);
    CU_ASSERT_PTR_NULL(_tm.tm_zone);

    gmtime_r(&_7200, &_tm);

    CU_ASSERT_EQUAL(_tm.tm_sec,     0);
    CU_ASSERT_EQUAL(_tm.tm_min,     0);
    CU_ASSERT_EQUAL(_tm.tm_hour,    2);
    CU_ASSERT_EQUAL(_tm.tm_mday,    1);
    CU_ASSERT_EQUAL(_tm.tm_mon,     0);
    CU_ASSERT_EQUAL(_tm.tm_year,    70);
    CU_ASSERT_EQUAL(_tm.tm_wday,    4);
    CU_ASSERT_EQUAL(_tm.tm_yday,    0);

    CU_ASSERT_EQUAL(_tm.tm_isdst,   0);
    CU_ASSERT_EQUAL(_tm.tm_gmtoff,  0);
    CU_ASSERT_PTR_NULL(_tm.tm_zone);
}

static void test_localtime_r_should_apply_gmtoffset()
{
    time_t _zero = 0;
    time_t _7200 = 7200;
    struct tm _tm;
    struct Library *LocaleBase = NULL;
    struct Locale *loc;
    LONG origOffset;

    LocaleBase = OpenLibrary("locale.library", 0L);
    loc = OpenLocale(NULL);
    origOffset = loc->loc_GMTOffset;
    loc->loc_GMTOffset = -120;

    localtime_r(&_zero, &_tm);

    CU_ASSERT_EQUAL(_tm.tm_sec,     0);
    CU_ASSERT_EQUAL(_tm.tm_min,     0);
    CU_ASSERT_EQUAL(_tm.tm_hour,    2);
    CU_ASSERT_EQUAL(_tm.tm_mday,    1);
    CU_ASSERT_EQUAL(_tm.tm_mon,     0);
    CU_ASSERT_EQUAL(_tm.tm_year,    70);
    CU_ASSERT_EQUAL(_tm.tm_wday,    4);
    CU_ASSERT_EQUAL(_tm.tm_yday,    0);

    CU_ASSERT_EQUAL(_tm.tm_isdst,   0);
    CU_ASSERT_EQUAL(_tm.tm_gmtoff,  7200);
    CU_ASSERT_PTR_NULL(_tm.tm_zone);

    localtime_r(&_7200, &_tm);

    CU_ASSERT_EQUAL(_tm.tm_sec,     0);
    CU_ASSERT_EQUAL(_tm.tm_min,     0);
    CU_ASSERT_EQUAL(_tm.tm_hour,    4);
    CU_ASSERT_EQUAL(_tm.tm_mday,    1);
    CU_ASSERT_EQUAL(_tm.tm_mon,     0);
    CU_ASSERT_EQUAL(_tm.tm_year,    70);
    CU_ASSERT_EQUAL(_tm.tm_wday,    4);
    CU_ASSERT_EQUAL(_tm.tm_yday,    0);

    CU_ASSERT_EQUAL(_tm.tm_isdst,   0);
    CU_ASSERT_EQUAL(_tm.tm_gmtoff,  7200);
    CU_ASSERT_PTR_NULL(_tm.tm_zone);

    loc->loc_GMTOffset = origOffset;
    CloseLocale(loc);
    CloseLibrary(LocaleBase);
}

static void test_asctime_r()
{
    time_t _7200 = 7200;
    struct tm _tm;
    char buff[32];

    gmtime_r(&_7200, &_tm);
    asctime_r(&_tm, buff);

    CU_ASSERT_STRING_EQUAL(buff, "Thu Jan  1 02:00:00 1970\n");
}

static void test_different_time_should_be_consistent()
{
    time_t tnow;
    struct timespec tsnow;
    struct timeval tvnow;

    time(&tnow);
    clock_gettime(CLOCK_REALTIME, &tsnow);
    gettimeofday(&tvnow, NULL);

    /* Since we are measuring live time, allow for up to 1 second difference */
    if (tnow != tsnow.tv_sec) tsnow.tv_sec -= 1;
    CU_ASSERT(tnow == tsnow.tv_sec);

    /* Since we are measuring live time, allow for up to 1 second difference */
    if (tsnow.tv_sec != tvnow.tv_sec) tvnow.tv_sec -=1;
    CU_ASSERT(tsnow.tv_sec == tvnow.tv_sec);
}

static void test_unix_timestamps_should_be_consistent()
{
    time_t tnow;
    FILE *f;
    struct stat buff;

    f = fopen("T:timestamp", "w");
    fprintf(f, "T");
    fclose(f);

    time(&tnow);

    stat("T:timestamp", &buff);

    /* Since we are measuring live time, allow for up to 1 second difference */
    if (buff.st_mtime != tnow) tnow -= 1;
    CU_ASSERT(buff.st_mtime == tnow);
}


int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE("time_Suite", NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_gmtime_r_should_convert_directly);
    CUNIT_CI_TEST(test_localtime_r_should_apply_gmtoffset);
    CUNIT_CI_TEST(test_asctime_r);
    CUNIT_CI_TEST(test_different_time_should_be_consistent);
    CUNIT_CI_TEST(test_unix_timestamps_should_be_consistent);

    return CU_CI_RUN_SUITES();
}
