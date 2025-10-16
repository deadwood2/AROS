/*
 * initgroups.c - group initialization function
 *
 * Original Author: ppessi <Pekka.Pessi@hut.fi>
 *
 * Based upon usergroup.library from AmiTCP/IP.
 *
 * Copyright � 2025 The AROS Dev Team.
 * Copyright � 1993 AmiTCP/IP Group, <AmiTCP-Group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 */

#include <aros/libcall.h>
#include <proto/exec.h>
#include <sys/time.h>

#include "base.h"

/****** usergroup.library/initgroups ***************************************

    NAME */
#include <proto/usergroup.h>

AROS_LH2(int, initgroups,

/*  SYNOPSIS */
        AROS_LHA(const char *, name, A1),
        AROS_LHA(gid_t, basegroup, D0),
        struct UserGroupBase *, UserGroupBase, 18, Usergroup)

/*  FUNCTION
        The initgroups() function reads through the group file and sets up,
        the group access list for the user specified in name. The basegid is
        automatically included in the groups list.  Typically this value is
        given as the group number from the password file.

     RESULT
        The initgroups() function returns -1 if the process has got no
        necessary privileges, zero if the call is succesful.

     FILES
         AmiTCP:db/group

     SEE ALSO
         setgroups()

     HISTORY
         The initgroups function appeared in 4.2BSD.

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    struct NetInfoReq *nreq;
    short error = -1;

    ObtainSemaphore(ni_lock);
    if (nreq = OpenNIUnit(NETINFO_GROUP_UNIT)) {
        gid_t *groups = nreq->io_Data;
        short ngroups, i, j;

        groups[0] = basegroup;
        nreq->io_Data = groups + 1;
#warning: TODO (AROS) io_Offset is not large enough on 64bit...
        nreq->io_Offset = (LONG) name;
        nreq->io_Command = NI_MEMBERS;

        if (myDoIO(nreq) == 0 || nreq->io_Error == NIERR_TOOSMALL) {
            ngroups = nreq->io_Actual / sizeof(gid_t) + 1;
            /* search for duplicate of basegroup */
            for (i = j = 1; i < ngroups; i++) {
                if (groups[i] != basegroup)
                    groups[j++] = groups[i];
            }
            if (j > NGROUPS)
                j = NGROUPS;
            error = setgroups(j, groups);
        } else {
            SetErrno((nreq->io_Error < 0) ? ENOENT : nreq->io_Error);
        }
    } else {
        SetErrno(ENOENT);
    }

    ReleaseSemaphore(ni_lock);

    return error;

    AROS_LIBFUNC_EXIT
}
