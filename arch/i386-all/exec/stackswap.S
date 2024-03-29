/*
    Copyright � 1995-2023, The AROS Development Team. All rights reserved.
    $Id$
*/

/*****************************************************************************
 
    NAME
 
 	AROS_LH1(void, StackSwap,
 
    SYNOPSIS
 	AROS_LHA(struct StackSwapStruct *, newStack, A0),
 
    LOCATION
 	struct ExecBase *, SysBase, 122, Exec)
 
    FUNCTION
 	This function switches to the new stack given by the parameters in the
 	stackswapstruct structure. The old stack parameters are returned in
 	the same structure so that the stack can be restored later
 
    INPUTS
 	newStack - parameters for the new stack
 
    RESULT
 
    NOTES
 
    EXAMPLE
 
    BUGS
 
    SEE ALSO
 
    INTERNALS
 
    HISTORY
 
******************************************************************************/
#include "aros/i386/asm.h"
#include <aros/config.h>

	.text
	.globl	AROS_SLIB_ENTRY(StackSwap,Exec,122)
	_FUNCTION(AROS_SLIB_ENTRY(StackSwap,Exec,122))
AROS_SLIB_ENTRY(StackSwap,Exec,122):
	/* Save %ebx content */
	pushl	%ebx

	/* Take the function's parameters; SysBase is in %eax */
	movl	8(%esp),  %ebx  //newStack

	/* Disable interrupts, to be sure they always find a good stackframe */
	call	*Disable(%eax)

	/* FindTask(NULL) in %edx */
	pushl	%eax		/* Arg1: Sysbase */
	pushl	$0			/* Arg0: NULL */
	call	*FindTask(%eax)
	movl	%eax, %edx	/* ThisTask to %edx */
	movl	4(%esp), %eax	/* Restore SysBase from stack */
	addl	$8, %esp	/* Move stack pointer back */
	/* movl	ThisTask(%eax), %edx */

	/* Swap Lower boundaries, %ecx is used as temp register */
	movl    tc_SPLower(%edx), %ecx
	xchgl	%ecx,             stk_Lower(%ebx)
	movl    %ecx,             tc_SPLower(%edx)

	/* Swap Upper boundaries, %ecx is used as temp register */
	movl    tc_SPUpper(%edx), %ecx
	xchgl	%ecx,             stk_Upper(%ebx)
	movl    %ecx,             tc_SPUpper(%edx)

	movb	tc_Flags(%edx), %cl
	test	$TF_STACKCHK, %cx
	je	noStackSnoop

	/* Fill [stk_Lower .. stk_Pointer - 16] with 0xE1 */

	pushl	%edi
	pushl	%eax // SysBase

	/* Destination register = %edi = NEW SPLower.
	   Which was already swapped above, so to be found
	   in task->tc_SPLower */
	movl	tc_SPLower(%edx), %edi

	/* %ecx = count register = NEW SP register - NEW SP_Lower - 16 */
	movl	stk_Pointer(%ebx), %ecx
	subl	$16, %ecx
	subl	%edi, %ecx

	/* byte value to store */
	movb	$0xE1, %al

	/* direction to store: forward */
	cld

	/* do the store operation: put %ecx times %al into memory starting at %edi. */
	rep
	stosb

	popl	%eax // SysBase
	popl	%edi

noStackSnoop:
	/* Now restore %ebx */
	popl	%ebx

	/* Pop function's return address and arguments before we swap stacks.
	   This is needed for proper stack balancing (saved value of sp must be
	   caller's stack pointer, right before StackSwap() was called. This way
	   second StackSwap() call will restore the stack to exactly the same state. */
	popl    %ecx    // return address
	popl    %edx    // newStack

	/* Swap stack pointers */
	xchgl	%esp, stk_Pointer(%edx) 

	/* Reenable interrupts. */
	call	*Enable(%eax)

	/*
	 * Now we should make the top of new stack to look exactly like the
	 * old one. This means it should contain arguments passed to StackSwap(),
	 * because the caller now will want to clear the stack and remove arguments.
	 * Arguments were: newStack. So we need to push it to the new stack.
	 */
	pushl %edx

	/* Return. %ecx contains the return address, and the stack is already
	   adjusted, we don't need, or even can, use ret here.  */
        jmp	*%ecx
