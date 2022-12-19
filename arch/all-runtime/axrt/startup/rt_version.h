/*
    Copyright (C) 2020-2022, The AROS Development Team.
*/

#ifndef RT_VERSION_H
#define RT_VERSION_H

#define RT_STARTUP  (6)
#define RT_ABI      (4)
#define RT_VERSION  (41)
#define RT_REVISION (4)

#define RT_VER      ((RT_STARTUP<<28)|(RT_ABI<<24)|(RT_VERSION<<8)|(RT_REVISION<<0))

#define RT_GET_STARTUP(x)   (((x)>>28)&0xF)
#define RT_GET_VERSION(x)   (((x)>>8)&0xFF)
#define RT_GET_REVISION(x)  (((x)>>0)&0xFF)

#endif
