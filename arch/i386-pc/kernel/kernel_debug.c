/*
    Copyright © 1995-2018, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <bootconsole.h>
#include <string.h>

#include "kernel_base.h"
#include "kernel_debug.h"
#include "kernel_intern.h"

__attribute__((section(".data"))) static unsigned int debug_y_resolution = 0;
__attribute__((section(".data"))) static void *debug_framebuffer = NULL;

#define MIN_WIDTH   480

int krnPutC(int c, struct KernelBase *KernelBase)
{
    unsigned long flags;

    __save_flags(flags);

    /*
     * Don't use Disable/Enable, because we want the interrupt enabled flag
     * to stay the same as it was before the Disable() call
     */
    __cli();

    /*
     * If we got 0x03, this means graphics driver wants to take over the screen.
     * If VESA hack is activated, it will use only upper half of the screen
     * because y resolution was adjusted.
     * In our turn, we need to switch over to lower half.
     * VESA hack is supported only on graphical console of course. And do not
     * expect it to work with native mode video driver. :)
     */
    if ((c == 0x03) && (scr_Type == SCR_GFX) && debug_framebuffer)
    {
        /* Reinitialize boot console with decreased height */
    	scr_FrameBuffer = debug_framebuffer;
    	fb_Resize(debug_y_resolution);
    }
    else
	con_Putc(c);

    /*
     * Interrupt flag is stored in flags - if it was enabled before,
     * it will be re-enabled when the flags are restored
     */
    __restore_flags(flags);

    return 1;
}

void vesahack_Init(char *cmdline, struct vbe_mode *vmode)
{
    if (cmdline && vmode && vmode->phys_base && strstr(cmdline, "vesahack"))
    {
	bug("[Kernel] VESA debugging hack activated\n");

	/*
	 * VESA hack.
	 * It divides screen into two parts.
	 * This allows VESA driver to use only upper part of the screen, while
	 * lower part will still be used for debug output. At minimum, 480
	 * lines are needed to boot into Wanderer.
	 */

	debug_y_resolution = vmode->y_resolution - MIN_WIDTH;
	vmode->y_resolution = MIN_WIDTH;
	debug_framebuffer  = (void *)(unsigned long)vmode->phys_base + vmode->y_resolution * vmode->bytes_per_scanline;
    }
}
