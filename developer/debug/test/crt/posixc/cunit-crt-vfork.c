/*
    Copyright © 2022, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <CUnit/CUnitCI.h>

int child_1_enter;
int child_1_fail;
int child_2_enter;
int child_3_enter;
int parent__enter;
int parent__fail;

CU_TEST_SETUP()
{
    parent__enter   = 0;
    parent__fail    = 0;
    child_1_enter   = 0;
    child_1_fail    = 0;
    child_2_enter   = 0;
    child_3_enter   = 0;
}

static void test_parent_child()
{
    pid_t pid;

    pid = vfork();
    if((int) pid > 0)
    {
        int status = -1;
        parent__enter = 1;
        waitpid(pid, &status, 0);
        if (status != 1)
            parent__fail = 1;
    }
    else if(pid == 0)
    {
        child_1_enter = 1;
        _exit(1);
    }
    else
    {
        CU_FAIL("vfork returned < 0");
    }

    CU_ASSERT_EQUAL(1, parent__enter);
    CU_ASSERT_EQUAL(0, parent__fail);

    CU_ASSERT_EQUAL(1, child_1_enter);
}

static void test_parent_child_child_child()
{
    pid_t pid = vfork();
    if((int) pid > 0)
    {
        pid_t pid2 = vfork();
        if((int) pid2 > 0)
        {
            int status = -1;
            parent__enter = 1;
            waitpid(pid2, &status, 0);
            if (status != 2)
                parent__fail = 1;
        }
        else if(pid2 == 0)
        {
            child_2_enter = 1;
            _exit(2);
        }
        waitpid(pid, NULL, 0);
    }
    else if(pid == 0)
    {
        pid_t pid2 = vfork();
        int retval = 3;
        if((int) pid2 > 0)
        {
            int status = -1;
            child_1_enter = 1;
            waitpid(pid2, &status, 0);
            if (status != 4)
                child_1_fail = 1;
        }
        else if(pid2 == 0)
        {
            child_3_enter = 1;
            _exit(4);
        }
        _exit(retval);
    }
    else
    {
        CU_FAIL("vfork returned < 0");
    }

    CU_ASSERT_EQUAL(1, parent__enter);
    CU_ASSERT_EQUAL(0, parent__fail);

    CU_ASSERT_EQUAL(1, child_1_enter);
    CU_ASSERT_EQUAL(0, child_1_fail);

    CU_ASSERT_EQUAL(1, child_2_enter);
    CU_ASSERT_EQUAL(1, child_3_enter);
}

static void test_vfork_execl_share_filedescriptors()
{
    int fd;
    FILE *f;
    char line[10];

    fd = open("T:execl2_out", O_RDWR|O_CREAT, 00700);
    CU_ASSERT_FATAL(fd != -1);

    f = fdopen(fd, "w");
    CU_ASSERT(f != NULL);

    fputs("OK\n", f);
    fflush(f);

    pid_t pid = vfork();
    if((int) pid > 0)
    {
        int status = -1;
        waitpid(pid, &status, 0);
    }
    else if (pid == 0)
    {
        char arg[10];
        sprintf(arg, "%d", fd);
        execl("execl2_slave", "execl2_slave", arg, NULL);
        CU_FAIL("execl did not execute"); /* Should not be reached */
        exit(20);
    }
    else
    {
        CU_FAIL("vfork returned < 0");
    }

    fseek(f, 0, SEEK_SET);
    fgets(line, 10, f);
    CU_ASSERT_EQUAL(0, strcmp(line, "CHILD\n"));
    fclose(f);
}

static void test_vfork_non_existing_program()
{
    pid_t pid = vfork();
    if((int) pid > 0)
    {
        waitpid(pid, NULL, 0);
    }
    else if(pid == 0)
    {
        CU_ASSERT_EQUAL(-1, execl(":XYZ/NotExist", "NotExist", "I'm child", NULL));
        _exit(0);
    }
    else
    {
        CU_FAIL("vfork returned < 0");
    }
}

int main(int argc, char** argv)
{
    CU_CI_DEFINE_SUITE(vfork_Suite, NULL, NULL, __cu_test_setup, NULL);
    CUNIT_CI_TEST(test_parent_child);
    CUNIT_CI_TEST(test_parent_child_child_child);
    CUNIT_CI_TEST(test_vfork_execl_share_filedescriptors);
    CUNIT_CI_TEST(test_vfork_non_existing_program);
    return CU_CI_RUN_SUITES();
}
