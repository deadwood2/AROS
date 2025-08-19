/*
  Copyright © 2025, The AROS Development Team. All rights reserved.
*/

#ifndef _BZ2BASE_H
#define _BZ2BASE_H

#include <exec/libraries.h>

struct BZ2Base
{
   struct Library _lib;
   struct Library *crtBase;
   BOOL           crtBaseClose;
};

#endif /* _BZ2BASE_H */
