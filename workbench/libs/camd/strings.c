/*
    Copyright (C) 1995-2001, The AROS Development Team. All rights reserved.

    Desc: 
*/

#include <proto/exec.h>
#include <proto/utility.h>
#include <exec/rawfmt.h>
#include <stdarg.h>
#include "camd_intern.h"

ULONG mystrlen(char *string){
	ULONG ret=0;
	while(string[ret]!=0) ret++;
	return ret;
}

BOOL mystrcmp(char *one,char *two){
  while(*one==*two){
    if(*one==0) return TRUE;
    one++;
    two++;
  }
  return FALSE;
}

char *findonlyfilename(char *pathfile){
  char *temp=pathfile;
  while(*pathfile!=0){
    if(*pathfile=='/') temp=pathfile+1;
    if(*pathfile==':') temp=pathfile+1;
    pathfile++;
  }
  return temp;
}

#ifndef __amigaos4__
void mysprintf(struct CamdBase *CamdBase,char *string,char *fmt,...){
	va_list args;

	/* The old code did `void *start=&fmt+1; RawDoFmt(fmt,start,...)`, which
	   assumes every vararg sits on the stack right after `fmt`. That only
	   holds on m68k; on x86-64 (and other AROS targets) the first arguments
	   arrive in registers, so `&fmt+1` pointed at the wrong data and a %s read
	   a bogus pointer - which made the DEVS:Midi scan build paths like
	   "devs:Midi/devs:Midi/..." so no driver ever loaded.

	   Use VNewRawDoFmt (compiler fills the va_list correctly for every ABI)
	   with the built-in RAWFMTFUNC_STRING sink, whose engine writes straight
	   into the PutChData buffer (`*(PutChData++) = ch`) and NUL-terminates.
	   This also avoids the custom PutChProc callback: RawDoFmt invokes a
	   user PutChProc as (char D0, data A3) for an array DataStream but as
	   proc(data, char) for a va_list DataStream, so a hand-rolled callback
	   written for one path crashes on the other. RAWFMTFUNC_STRING sidesteps
	   that entirely. */
	va_start(args, fmt);
	VNewRawDoFmt(fmt, RAWFMTFUNC_STRING, string, args);
	va_end(args);
}
#endif

