/*
    Copyright © 2020, The AROS Development Team.
    $Id$
*/

#ifndef RT_VERSION_H
#define RT_VERSION_H

#define RT_STARTUP  (3)
#define RT_ABI      (2)
#define RT_VERSION  (40)
#define RT_REVISION (0)

#define RT_VER      ((RT_STARTUP<<28)|(RT_ABI<<24)|(RT_VERSION<<8)|(RT_REVISION<<0))

#define RT_GET_VERSION(x)   (((x)>>8)&0xFF)
#define RT_GET_REVISION(x)  (((x)>>0)&0xFF)

#endif
