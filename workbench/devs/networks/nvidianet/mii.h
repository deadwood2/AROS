/*

Copyright (C) 2003-2022 Neil Cafferkey

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA.

*/

#ifndef MII_H
#define MII_H


/* Registers */
/* ========= */

#define MII_CONTROL         0
#define MII_STATUS          1
#define MII_PHYID1          2
#define MII_PHYID2          3
#define MII_AUTONEGADVERT   4
#define MII_AUTONEGABILITY  5
#define MII_1GCONTROL       9
#define MII_1GSTATUS       10
#define MII_EXTSTATUS      15


/* Register Details */
/* ================ */

/* Control Register */

#define MII_CONTROLB_RESET 15

#define MII_CONTROLF_RESET (1 << MII_CONTROLB_RESET)

/* Status Register */

#define MII_STATUSB_100BASET4   15
#define MII_STATUSB_100BASEXFD  14
#define MII_STATUSB_100BASEXHD  13
#define MII_STATUSB_10BASEFD    12
#define MII_STATUSB_10BASEHD    11
#define MII_STATUSB_100BASET2FD 10
#define MII_STATUSB_100BASET2HD  9
#define MII_STATUSB_EXTSTATUS    8
#define MII_STATUSB_AUTONEGDONE  5
#define MII_STATUSB_AUTONEGABLE  3
#define MII_STATUSB_LINK         2
#define MII_STATUSB_EXTREGSET    0

#define MII_STATUSF_100BASET4   (1 << MII_STATUSB_100BASET4)
#define MII_STATUSF_100BASEXFD  (1 << MII_STATUSB_100BASEXFD)
#define MII_STATUSF_100BASEXHD  (1 << MII_STATUSB_100BASEXHD)
#define MII_STATUSF_100BASEFD   (1 << MII_STATUSB_100BASEFD)
#define MII_STATUSF_100BASEHD   (1 << MII_STATUSB_100BASEHD)
#define MII_STATUSF_100BASET2FD (1 << MII_STATUSB_100BASET2FD)
#define MII_STATUSF_100BASET2HD (1 << MII_STATUSB_100BASET2HD)
#define MII_STATUSF_EXTSTATUS   (1 << MII_STATUSB_EXTSTATUS)
#define MII_STATUSF_AUTONEGDONE (1 << MII_STATUSB_AUTONEGDONE)
#define MII_STATUSF_AUTONEGABLE (1 << MII_STATUSB_AUTONEGABLE)
#define MII_STATUSF_LINK        (1 << MII_STATUSB_LINK)
#define MII_STATUSF_EXTREGSET   (1 << MII_STATUSB_EXTREGSET)

/* Autonegotiation Registers */

#define MII_AUTONEGB_100BASETXFD 8
#define MII_AUTONEGB_100BASETX   7
#define MII_AUTONEGB_10BASETFD   6
#define MII_AUTONEGB_10BASET     5

#define MII_AUTONEGF_100BASETXFD (1 << MII_AUTONEGB_100BASETXFD)
#define MII_AUTONEGF_100BASETX   (1 << MII_AUTONEGB_100BASETX)
#define MII_AUTONEGF_10BASETFD   (1 << MII_AUTONEGB_10BASETFD)
#define MII_AUTONEGF_10BASET     (1 << MII_AUTONEGB_10BASET)

/* Gigabit Control Register */

#define MII_1GCONTROLB_1000BASETFD  9
#define MII_1GCONTROLB_1000BASET    8

#define MII_1GCONTROLF_1000BASETFD (1 << MII_1GCONTROLB_1000BASETFD)
#define MII_1GCONTROLF_1000BASET   (1 << MII_1GCONTROLB_1000BASET)

/* Gigabit Status Register */

#define MII_1GSTATUSB_1000BASETFD 11
#define MII_1GSTATUSB_1000BASET   10

#define MII_1GSTATUSF_1000BASETFD (1 << MII_1GSTATUSB_1000BASETFD)
#define MII_1GSTATUSF_1000BASET   (1 << MII_1GSTATUSB_1000BASET)

#endif
