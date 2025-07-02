/*
    Copyright © 2025, The AROS Development Team. All rights reserved.
    $Id$
*/
#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>
 
 /* Test printing an ASCII wide string with wprintf */
static void test_wprintf_1(void) {
 
    FILE * temp_file = tmpfile();

    // Redirect stdout to temp_file to capture output
    int original_stdout_fd = dup(fileno(stdout));
    int tmp_fd = fileno(temp_file);
    if (dup2(tmp_fd, fileno(stdout)) == -1) {
      fclose(temp_file);
    }

    // Test wide character string printing
    const wchar_t* test_string = L"Hello, World!";
    int result = wprintf(L"Test: %ls\n", test_string);

    // Restore stdout
    if (dup2(original_stdout_fd, fileno(stdout)) == -1) {
      fclose(temp_file);
    }

    // Check if print was successful
    CU_ASSERT(result > 0);

    // Rewind the temp file to read its contents
    rewind(temp_file);

    // Buffer to read the output
    wchar_t buffer[100];
    fgetws(buffer, sizeof(buffer), temp_file); 
    
    // Verify the output
    CU_ASSERT(wcscmp(buffer, L"Test: Hello, World!\n") == 0);

    // Close the temporary file
    fclose(temp_file);
 }
 
 // Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE("wprintf_Suite", NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wprintf_1);

    return CU_CI_RUN_SUITES();
}
 