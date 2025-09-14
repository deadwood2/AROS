#ifndef __STDLIB_INTTYPES_H
#define __STDLIB_INTTYPES_H

/*
 * This definition is for supporting applications compiled before intmax_t
 * switched to 64 bits on 32 bit architectures.
 */
typedef struct {
	long quot;		/* quotient */
	long rem;		/* remainder */
} legacy_imaxdiv_t;

#endif //__STDLIB_INTTYPES_H
