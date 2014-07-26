#ifndef _SYS_AROSC_H
#define _SYS_AROSC_H

#include <aros/system.h>
#include <setjmp.h>

struct __sFILE;

struct arosc_ctype {
    const unsigned short int *b;
    /* ABI_V0 compatibility */
    const int                   *toupper;
    const int                   *tolower;
};

struct arosc_userdata
{
    /* stdio.h */
    struct __sFILE *acud_stdin;
    struct __sFILE *acud_stdout;
    struct __sFILE *acud_stderr;

    /* errno.h */
    int acud_errno;

    struct arosc_ctype acud_ctype;

    /* ABI_V0 compatibility: int, long int, char * offset */
    int acud_compatibility_1_1;
    long int acud_compatibility_1_2;
    void * acud_compatibility_1_3;

    /* Used by multi-byte functions */
    int acud_mb_cur_max;

    /* ABI_V0 compatibility: environ variable value */
    void * acud_compatibility_2;
};

__BEGIN_DECLS

struct arosc_userdata *__get_arosc_userdata(void) __pure;
const struct arosc_ctype *__get_arosc_ctype(void) __pure;
int __arosc_nixmain(int (*main)(int argc, char *argv[]), int argc, char *argv[]);
int __get_default_file(int file_descriptor, long* file_handle);
void __arosc_program_startup_abiv0(void);
void __arosc_program_end(void);
int *__arosc_set_errorptr(int *errorptr);
int *__arosc_get_errorptr(void);
void __arosc_set_exitjmp(jmp_buf exitjmp, jmp_buf previousjmp);
void __arosc_jmp2exit(int normal, int returncode) __noreturn;
void __arosc_program_startup(jmp_buf exitjmp, int *error_ptr);

__END_DECLS

#endif /* !_SYS_AROSC_H */
