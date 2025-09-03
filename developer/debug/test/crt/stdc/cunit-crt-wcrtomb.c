/*
    Copyright © 2025, The AROS Development Team. All rights reserved.
    $Id$
*/
#include <stdio.h>
#include <wchar.h>
#include <CUnit/CUnitCI.h>
 
 /* Test conversion of an ASCII character */
static void test_wcrtomb_1(void) {
 
     mbstate_t state;
     char buf[1];
     wchar_t wc = L'A'; /* US-ASCII character */
     
     /* Initialize the conversion state */
     for (size_t i = 0; i < sizeof(state); i++) {
         ((unsigned char *)&state)[i] = 0;
     }
     
     /* convert wc to a multibyte sequence */
     size_t ret = wcrtomb(buf, wc, &state);
     
     CU_ASSERT_NOT_EQUAL(ret, (size_t)-1);
     if (ret != (size_t)-1) {
         CU_ASSERT_EQUAL(ret, 1);
         CU_ASSERT_EQUAL(buf[0], 'A');
     }
 }
 
 /* Test conversion of the null wide character
    Note: When wc == L'\0', wcrtomb writes a null byte. */
static void test_wcrtomb_2(void) {
 
     mbstate_t state;
     char buf[1];
     wchar_t wc = L'\0';
     
     /* Reset state */
     for (size_t i = 0; i < sizeof(state); i++) {
         ((unsigned char *)&state)[i] = 0;
     }
     
     size_t ret = wcrtomb(buf, wc, &state);
     
     CU_ASSERT_NOT_EQUAL(ret, (size_t)-1);
     if (ret != (size_t)-1) {
         CU_ASSERT_EQUAL(ret, 1);
         CU_ASSERT_EQUAL(buf[0], '\0');
     }
 }
 
 /* Test conversion using a NULL pointer for s
    In this case, wcrtomb should encode only the shift state. */
static void test_wcrtomb_3(void) {
 
     mbstate_t state;
     wchar_t wc = L'A';
 
     /* Reset state */
     for (size_t i = 0; i < sizeof(state); i++) {
         ((unsigned char *)&state)[i] = 0;
     }
     
     /*
      * When the destination pointer s is NULL, wcrtomb is supposed to behave as if s
      * pointed to a dummy array. Thus, a valid conversion should still occur.
      */
     size_t ret = wcrtomb(NULL, wc, &state);
     CU_ASSERT_NOT_EQUAL(ret, (size_t)-1);
 }
 
 // Main function to run the tests
int main(int argc, char** argv) {
    //Define test suite
    CU_CI_DEFINE_SUITE(wcrtomb_Suite, NULL, NULL, NULL, NULL);
    CUNIT_CI_TEST(test_wcrtomb_1);
    CUNIT_CI_TEST(test_wcrtomb_2);
    CUNIT_CI_TEST(test_wcrtomb_3);

    return CU_CI_RUN_SUITES();
}
 