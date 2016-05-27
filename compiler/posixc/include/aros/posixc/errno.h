#ifndef _POSIXC_ERRNO_H_
#define _POSIXC_ERRNO_H_

/*
    Copyright © 1995-2016, The AROS Development Team. All rights reserved.
    $Id$

    POSIX.1-2008 header file errno.h

*/

/*
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)errno.h	8.5 (Berkeley) 1/21/94
 */

/* C99 */
#include <aros/stdc/errno.h>

/* ABI_V0 compatibility */
/* Error codes reverted to ABI V0 values */

/* Other codes defined by POSIX.1-2008 */
#define     E2BIG           7       /* Argument list too long */
#define     EADDRINUSE      98      /* Address already in use */
#define     EADDRNOTAVAIL   99      /* Can't assign requested address */
#define     EAFNOSUPPORT    97      /* Address family not supported by protocol family */
#define     EAGAIN          11      /* Resource temporarily unavailable */
#define     EALREADY        114     /* Operation already in progress */
#define     EBADF           9       /* Bad file descriptor */
#define     EBADMSG         74      /* Bad or Corrupt message */
/* NOTIMPL ECANCELED */
#define     ECHILD          10      /* No child processes */
#define     ECONNABORTED    103     /* Software caused connection abort */
#define     ECONNREFUSED    111     /* Connection refused */
#define     ECONNRESET      104     /* Connection reset by peer */
#define     EDEADLK         35      /* Resource deadlock avoided */
#define     EDESTADDRREQ    89      /* Destination address required */
#define     EPROCLIM        66      /* Too many processes */
#define     EUSERS          68      /* Too many users */
#define     EDQUOT          122     /* Disc quota exceeded */
#define     EFAULT          14      /* Bad address */
#define     EFBIG           27      /* File too large */
#define     EHOSTUNREACH    113     /* No route to host */
#define     EIDRM           43      /* Identifier removed */
#define     EINPROGRESS     115     /* Operation now in progress */
#define     EIO             5       /* Input/output error */
#define     EISCONN         106     /* Socket is already connected */
#define     EISDIR          21      /* Is a directory */
#define     ELOOP           40      /* Too many levels of symbolic links */
#define     EMFILE          24      /* Too many open files */
#define     EMLINK          31      /* Too many links */
#define     EMSGSIZE        90      /* Message too long */
#define     EMULTIHOP       72      /* Multihop attempted */
#define     ENAMETOOLONG    36      /* File name too long */
#define     ENETDOWN        100     /* Network is down */
#define     ENETRESET       102     /* Network dropped connection on reset */
#define     ENETUNREACH     101     /* Network is unreachable */
#define     ENFILE          23      /* Too many open files in system */
#define     ENODATA         61      /* No message available */
#define     ENODEV          19      /* Operation not supported by device */
#define     ENOLCK          37      /* No locks available */
#define     ENOLINK         67      /* Link has been severed */
#define     ENOMSG          42      /* No message of desired type */
#define     ENOPROTOOPT     92      /* Protocol not available */
#define     ENOSPC          28      /* No space left on device */
#define     ENOSR           63      /* No STREAM resources */
#define     ENOSTR          60      /* Not a STREAM */
#define     ENOSYS          38      /* Function not implemented */
#define     ENOTCONN        107     /* Socket is not connected */
#define     ENOTEMPTY       39      /* Directory not empty */
/* NOTIMPL ENORECOVERABLE */
#define     ENOTSOCK        88      /* Socket operation on non-socket */
#define     ENOTSUP         95      /* Not supported */
#define     ENOTTY          25      /* Inappropriate ioctl for device */
#define     ENXIO           6       /* Device not configured */
#define     EOPNOTSUPP      95      /* Operation not supported on socket */
#define     EOVERFLOW       75      /* Value too large to be stored in data type */
/* NOTIMPL EOWNERDEAD */
#define     EPERM           1       /* Operation not permitted */
#define     EPIPE           32      /* Broken pipe */
#define     EPROTO          71      /* Protocol error */
#define     EPROTONOSUPPORT 93      /* Protocol not supported */
#define     EPROTOTYPE      91      /* Protocol wrong type for socket */
#define     EROFS           30      /* Read-only file system */
#define     ESPIPE          29      /* Illegal seek */
#define     ESRCH           3       /* No such process */
#define     ESTALE          116     /* Stale NFS file handle */
#define     ETIME           62      /* STREAM ioctl timeout */
#define     ETIMEDOUT       110     /* Connection timed out */
#define     ETXTBSY         26      /* Text file busy */
#define     EWOULDBLOCK     EAGAIN  /* Operation would block */


/* Some compatibility defines mainly for AROSTCP */
#ifdef __BSD_VISIBLE
#define     EHOSTDOWN       112     /* Host is down */
#define     EPFNOSUPPORT    96      /* Protocol family not supported */
#define     ERESTART        85      /* restart syscall */
#define     ESOCKTNOSUPPORT 94      /* Socket type not supported */
#define     ESHUTDOWN       108     /* Can't send after socket shutdown */
#endif

#define     __POSIXC_ELAST  EDQUOT  /* Points to highest used errno in this include */

#endif /* _POSIXC_ERRNO_H_ */
