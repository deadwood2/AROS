/*
 * Group database functions
 *
 * Original Author: ppessi <Pekka.Pessi@hut.fi>
 *
 * Based upon usergroup.library from AmiTCP/IP.
 *
 * Copyright � 2025 The AROS Dev Team.
 * Copyright � 1993 AmiTCP/IP Group, <AmiTCP-Group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 */

/****** usergroup.library/getgrent *******************************************

    NAME
        getgrgid, getgrnam, getgrent, setgrent, endgrent
         - group database operations

    SYNOPSIS
        #include <grp.h>

        groupent = getgrgid(gid)
           D0               D0
        struct group *getgrgid(gid_t);


        groupent = getgrnam(name)
           D0                A1          
        struct group *getgrnam(const char *);


        groupent = getgrent()
           D0
        struct group *getgrent(void);

        setgrent()
        void setgrent(void);

        endgrent()
        void endgrent(void);

    FUNCTION
        These functions operate on the group database via netinfo.device
        interface. They provide a convenient unix-compatible interface to
        the group unit of the netinfo.device.

        The local group database is stored in the file AmiTCP:db/group, its
        format is described in netinfo.device/group.  The entry returned by
        each reading function is defined by the structure group found in the
        include file <grp.h>:

              struct group
              {
                char   *gr_name;              \* Group name.  *\
                char   *gr_passwd;            \* Password.    *\
                gid_t   gr_gid;               \* Group ID.    *\
                char  **gr_mem;               \* Member list. *\
               };

        The functions getgrnam() and getgrgid() search the group database
        for the given group name pointed to by name or the group id pointed
        to by gid, respectively, returning the first one encountered.
        Identical group names or group gids may result in undefined
        behavior.

        The getgrent() function sequentially reads the group database and is
        intended for programs that wish to step through the complete list of
        groups.

        All three routines will open the group unit of netinfo.device for
        reading, if necesssary.

        The setgrent() function opens the group unit of netinfo.device.  The
        endgrent() function closes the group unit of netinfo.device.  It is
        recommended to call endgrent() if the program won't access group
        database any more.

    RESULTS
        The functions getgrent(), getgrnam(), and getgrgid(), return a
        pointer to the group entry if successful; if the end of database is
        reached or an error occurs a null pointer is returned.  The
        functions endgrent() and setgrent() have no return value.

    ERRORS
        [ENOENT] -- the netinfo.device could not be opened.

        Other netinfo.device IO errors can be retrieved by ug_GetErr().

    FILES
        AmiTCP:db/group     The group database file

    SEE ALSO

        getpwnam(), netinfo.device/group

    HISTORY

        The functions getgrgid(), getgrnam(), getgrent(), setgrent() and
        endgrent() appeared in Version 7 AT&T UNIX.

    BUGS

        These functions leave their results in an internal static object and
        return a pointer to that object. Subsequent calls to the same
        function will modify the same object. If you need re-entrant
        operation, you can use directly the netinfo.device commands.

    COMPATIBILITY

        The BSD passwd database handling routines setgrfile() and
        setgroupent() are fairly useless in a networked environment and they
        are not implemented.

*****************************************************************************
*
*/

#include <aros/libcall.h>
#include <proto/exec.h>
#include <sys/time.h>

#include "base.h"

#include <proto/usergroup.h>

static short done_set_ent = 0;

AROS_LH1(struct group *, getgrnam,
        AROS_LHA(const char *, name, A1),
        struct UserGroupBase *, UserGroupBase, 24, Usergroup)
{
    AROS_LIBFUNC_INIT

    struct NetInfoReq *nreq;
    struct group *gr = NULL;

    if (name == NULL) {
        SetErrno(EFAULT);
        return NULL;
    }

    ObtainSemaphore(ni_lock);
    if (nreq = OpenNIUnit(NETINFO_GROUP_UNIT)) {

        gr = (struct group *)nreq->io_Data;
        gr->gr_name = (char *)name;
        nreq->io_Command = NI_GETBYNAME;

        if (myDoIO(nreq) != 0) {
            gr = NULL;
            SetDeviceErr();
        }
    } else {
        SetDeviceErr();
    }

    ReleaseSemaphore(ni_lock);

    return gr;

    AROS_LIBFUNC_EXIT
}

AROS_LH1(struct group *, getgrgid,
        AROS_LHA(gid_t, gid, D0),
        struct UserGroupBase *, UserGroupBase, 25, Usergroup)
{
    AROS_LIBFUNC_INIT

    struct NetInfoReq *nreq;
    struct group *gr = NULL;

    ObtainSemaphore(ni_lock);
    if (nreq = OpenNIUnit(NETINFO_GROUP_UNIT)) {
        gr = (struct group *)nreq->io_Data;
        gr->gr_gid = gid;
        nreq->io_Command = NI_GETBYID;

        if (myDoIO(nreq) != 0) {
            gr = NULL;
            SetDeviceErr();
        }
    } else {
        SetDeviceErr();
    }

    ReleaseSemaphore(ni_lock);

    return gr;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, setgrent,
                struct UserGroupBase *, UserGroupBase, 26, Usergroup)
{
    AROS_LIBFUNC_INIT

    struct NetInfoReq *nreq;

    ObtainSemaphore(ni_lock);

    if (nreq = OpenNIUnit(NETINFO_GROUP_UNIT)) {
        nreq->io_Command = CMD_RESET;
        myDoIO(nreq);
        done_set_ent = 1;
    } else {
        SetDeviceErr();
    }

    ReleaseSemaphore(ni_lock);

    AROS_LIBFUNC_EXIT
}

AROS_LH0(struct group *, getgrent,
                struct UserGroupBase *, UserGroupBase, 27, Usergroup)
{
    AROS_LIBFUNC_INIT

    struct NetInfoReq *nreq;
    struct group *gr = NULL;

    ObtainSemaphore(ni_lock);
    if (nreq = OpenNIUnit(NETINFO_GROUP_UNIT)) {
         /* do setgrent() if necessary */
        if (!done_set_ent) {
            nreq->io_Command = CMD_RESET;
            myDoIO(nreq);
            done_set_ent = 1;
        }

        nreq->io_Command = CMD_READ;
        if (myDoIO(nreq) == 0) {
            gr = (struct group *)nreq->io_Data;
        } else {
            SetDeviceErr();
        }
    } else {
        SetDeviceErr();
    }

    ReleaseSemaphore(ni_lock);

    return gr;

    AROS_LIBFUNC_EXIT
}

AROS_LH0(void, endgrent,
                struct UserGroupBase *, UserGroupBase, 28, Usergroup)
{
    AROS_LIBFUNC_INIT

    ObtainSemaphore(ni_lock);
    done_set_ent = 0;
    CloseNIUnit(NETINFO_GROUP_UNIT);
    ReleaseSemaphore(ni_lock);

    AROS_LIBFUNC_EXIT
}
