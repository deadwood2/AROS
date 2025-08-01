/*

Copyright (C) 2001-2025 Neil Cafferkey

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

#ifndef DEVICE_H
#define DEVICE_H


#include <exec/types.h>
#include <exec/devices.h>
#include <exec/interrupts.h>
#include <exec/semaphores.h>
#include <devices/sana2.h>
#include <devices/sana2specialstats.h>
#include <devices/timer.h>

#include "compatibility.h"
#include "io.h"
#include "ethernet.h"

#define DEVICE_NAME "etherlink3.device"
#define VERSION 1
#define REVISION 3
#define DATE "12.7.2025"

#define UTILITY_VERSION 36
#define PROMETHEUS_VERSION 2
#define POWERPCI_VERSION 2
#define EXPANSION_VERSION 50
#define OPENPCI_VERSION 1
#define PCCARD_VERSION 1

#ifndef __AROS__
#define USE_HACKS
#endif


struct DevBase
{
   struct Device device;
   APTR seg_list;
   struct ExecBase *sys_base;
   struct UtilityBase *utility_base;
   struct Library *prometheus_base;
   struct Library *powerpci_base;
   struct Library *openpci_base;
   struct Library *expansion_base;
   struct Library *pccard_base;
   APTR card_base;
   struct MinList pci_units;
   struct MinList isa_units;
   struct MinList pccard_units;
   struct timerequest timer_request;
   VOID (*wrapper_int_code)();
   VOID (*card_wrapper_int_code)();
};


enum
{
   WRITE_QUEUE,
   ADOPT_QUEUE,
   EVENT_QUEUE,
   GENERAL_QUEUE,
   REQUEST_QUEUE_COUNT
};

enum
{
   PCI_BUS,
   EISA_BUS,
   ISA_BUS,
   PCCARD_BUS
};

enum
{
   FIRST_GEN,
   SECOND_GEN,
   DEMON_GEN,
   VORTEX_GEN,
   BOOMERANG_GEN,
   CYCLONE_GEN,
   TORNADO_GEN
};

#define STAT_COUNT 3

#define TX_SLOT_COUNT 20
#define RX_SLOT_COUNT 20
#define DPD_SIZE 0x20
#define UPD_SIZE 0x20


struct DevUnit
{
   struct MinNode node;
   ULONG index;
   ULONG open_count;
   UWORD flags;
   UWORD transceiver;
   struct Task *task;
   struct MsgPort *request_ports[REQUEST_QUEUE_COUNT];
   struct DevBase *device;
   APTR card;
   BOOL (*insertion_function)(APTR, struct DevBase *);
   VOID (*removal_function)(APTR, struct DevBase *);
   UBYTE (*ByteIn)(APTR, UBYTE);
   ULONG (*LongIn)(APTR, ULONG);
   VOID (*ByteOut)(APTR, ULONG, UBYTE);
   VOID (*WordOut)(APTR, ULONG, UWORD);
   VOID (*LongOut)(APTR, ULONG, ULONG);
   VOID (*LongsIn)(APTR, ULONG, ULONG *, ULONG);
   VOID (*WordsOut)(APTR, ULONG, const UWORD *, ULONG);
   VOID (*LongsOut)(APTR, ULONG, const ULONG *, ULONG);
   VOID (*BEWordOut)(APTR, ULONG, UWORD);
   UWORD (*LEWordIn)(APTR, ULONG);
   ULONG (*LELongIn)(APTR, ULONG);
   VOID (*LEWordOut)(APTR, ULONG, UWORD);
   VOID (*LELongOut)(APTR, ULONG, ULONG);
   APTR (*AllocDMAMem)(APTR, UPINT, UWORD);
   VOID (*FreeDMAMem)(APTR, APTR);
   UBYTE *rx_buffer;
   UBYTE *rx_buffers[RX_SLOT_COUNT];
   UBYTE *tx_buffer;
   UBYTE *tx_buffers[TX_SLOT_COUNT];
   UPINT io_base;
   UPINT window1_offset;
   ULONG card_removed_signal;
   ULONG card_inserted_signal;
   ULONG range_count;
   UBYTE address[ETH_ADDRESSSIZE];
   UBYTE default_address[ETH_ADDRESSSIZE];
   struct MinList openers;
   struct MinList type_trackers;
   struct MinList multicast_ranges;
   struct Interrupt status_int;
   struct Interrupt rx_int;
   struct Interrupt tx_int;
   struct Interrupt tx_end_int;
   struct Interrupt reset_handler;
   struct Sana2DeviceStats stats;
   ULONG special_stats[STAT_COUNT];
   ULONG *dpds;
   ULONG dpds_p;
   ULONG *upds;
   ULONG upds_p;
   UBYTE *headers;
   ULONG speed;
   struct IOSana2Req **tx_requests;
   struct SignalSemaphore access_lock;
   UWORD capabilities;
   UWORD rx_filter_cmd;
   UWORD size_shift;
   UWORD bus;
   UWORD generation;
   UWORD buffer_size;
   UWORD tx_in_slot;
   UWORD tx_out_slot;
   UWORD rx_slot;
   UWORD tx_active_req_count;
   UWORD int_mask;
   UBYTE mii_phy_no;
};


struct Opener
{
   struct MinNode node;
   struct MsgPort read_port;
   BOOL (*rx_function)(REG(a0, APTR), REG(a1, APTR), REG(d0, ULONG));
   BOOL (*tx_function)(REG(a0, APTR), REG(a1, APTR), REG(d0, ULONG));
   UBYTE *(*dma_tx_function)(REG(a0, APTR));
   struct Hook *filter_hook;
   struct MinList initial_stats;
};


struct TypeStats
{
   struct MinNode node;
   ULONG packet_type;
   struct Sana2PacketTypeStats stats;
};


struct TypeTracker
{
   struct MinNode node;
   ULONG packet_type;
   struct Sana2PacketTypeStats stats;
   ULONG user_count;
};


struct AddressRange
{
   struct MinNode node;
   ULONG add_count;
   ULONG lower_bound_left;
   ULONG upper_bound_left;
   UWORD lower_bound_right;
   UWORD upper_bound_right;
};


/* Unit flags */

#define UNITF_SHARED        (1 << 0)
#define UNITF_PROM          (1 << 1)
#define UNITF_CONFIGURED    (1 << 2)
#define UNITF_ONLINE        (1 << 3)
#define UNITF_HAVEADAPTER   (1 << 4)
#define UNITF_TASKADDED     (1 << 5)
#define UNITF_INTADDED      (1 << 6)
#define UNITF_RESETADDED    (1 << 7)
#define UNITF_WASONLINE     (1 << 8)   /* card online at time of removal */
#define UNITF_FULLDUPLEX    (1 << 9)
#define UNITF_RXBUFFERINUSE (1 << 10)
#define UNITF_TXBUFFERINUSE (1 << 11)


/* Library and device bases */

#define SysBase (base->sys_base)
#define CardResource (base->card_base)
#define UtilityBase (base->utility_base)
#define ExpansionBase (base->expansion_base)
#define PrometheusBase (base->prometheus_base)
#define PowerPCIBase (base->powerpci_base)
#define OpenPciBase (base->openpci_base)
#define PCCardBase (base->pccard_base)
#define TimerBase (base->timer_request.tr_node.io_Device)


#endif
