/*
    Copyright (C) 2023, The AROS Development Team. All rights reserved.

    crt.library 1.0 backwards compatibility
*/

#include <sys/stat.h>

struct stat_ino_t_32
{
    dev_t           st_dev;	    /* inode's device */
    int32_t         st_ino;	    /* inode's number */
    mode_t          st_mode;	    /* inode protection mode */
    nlink_t         st_nlink;	    /* number of hard links */
    uid_t           st_uid;	    /* user ID of the file's owner */
    gid_t           st_gid;	    /* group ID of the file's group */
    dev_t           st_rdev;	    /* device type */
    __off_t         st_size;	    /* file size, in bytes */
    struct timespec st_atim;	    /* time of last access */
    struct timespec st_mtim;	    /* time of last data modification */
    struct timespec st_ctim;	    /* time of last file status change */
    __blksize_t     st_blksize;	    /* optimal blocksize for I/O */
    __blkcnt_t      st_blocks;      /* blocks allocated for file */
    unsigned long   st_flags;	    /* user defined flags for file */
    unsigned long   st_gen;         /* file generation number */
};

int __fstat_ino_t_32(int fd, struct stat_ino_t_32 *sb)
{
    int res;
    struct stat _tmp;

    res = __posixc_fstat(fd, &_tmp);

    sb->st_dev      = _tmp.st_dev;
    sb->st_ino      = (int32_t)_tmp.st_ino;
    sb->st_mode     = _tmp.st_mode;
    sb->st_nlink    = _tmp.st_nlink;
    sb->st_uid      = _tmp.st_uid;
    sb->st_gid      = _tmp.st_gid;
    sb->st_rdev     = _tmp.st_rdev;
    sb->st_size     = _tmp.st_size;
    sb->st_atim     = _tmp.st_atim;
    sb->st_mtim     = _tmp.st_mtim;
    sb->st_ctim     = _tmp.st_ctim;
    sb->st_blksize  = _tmp.st_blksize;
    sb->st_blocks   = _tmp.st_blocks;
    sb->st_flags    = _tmp.st_flags;
    sb->st_gen      = _tmp.st_gen;

    return res;
}

int __stat_ino_t_32(const char * restrict path, struct stat_ino_t_32 * restrict sb)
{
    int res = 0;
    struct stat _tmp;

    res = __posixc_stat(path, &_tmp);

    sb->st_dev      = _tmp.st_dev;
    sb->st_ino      = (int32_t)_tmp.st_ino;
    sb->st_mode     = _tmp.st_mode;
    sb->st_nlink    = _tmp.st_nlink;
    sb->st_uid      = _tmp.st_uid;
    sb->st_gid      = _tmp.st_gid;
    sb->st_rdev     = _tmp.st_rdev;
    sb->st_size     = _tmp.st_size;
    sb->st_atim     = _tmp.st_atim;
    sb->st_mtim     = _tmp.st_mtim;
    sb->st_ctim     = _tmp.st_ctim;
    sb->st_blksize  = _tmp.st_blksize;
    sb->st_blocks   = _tmp.st_blocks;
    sb->st_flags    = _tmp.st_flags;
    sb->st_gen      = _tmp.st_gen;

    return res;
}

int __lstat_ino_t_32(const char * restrict path, struct stat_ino_t_32 * restrict sb)
{
    int res;
    struct stat _tmp;

    res = __posixc_lstat(path, &_tmp);

    sb->st_dev      = _tmp.st_dev;
    sb->st_ino      = (int32_t)_tmp.st_ino;
    sb->st_mode     = _tmp.st_mode;
    sb->st_nlink    = _tmp.st_nlink;
    sb->st_uid      = _tmp.st_uid;
    sb->st_gid      = _tmp.st_gid;
    sb->st_rdev     = _tmp.st_rdev;
    sb->st_size     = _tmp.st_size;
    sb->st_atim     = _tmp.st_atim;
    sb->st_mtim     = _tmp.st_mtim;
    sb->st_ctim     = _tmp.st_ctim;
    sb->st_blksize  = _tmp.st_blksize;
    sb->st_blocks   = _tmp.st_blocks;
    sb->st_flags    = _tmp.st_flags;
    sb->st_gen      = _tmp.st_gen;

    return res;
}

#include <dirent.h>

struct dirent_ino_t_32
{
    int32_t d_ino;
    int32_t PAD;
    __off_t d_off;

    unsigned short int d_reclen;
    unsigned char d_type;
    char    d_name[256];
};

struct dirent *__readdir_ino_t_32(DIR *dir)
{
    struct dirent *ret;

    ret = __posixc_readdir(dir);
    if (ret)
    {
        /* This is a no-op. Left for reference. */
        struct dirent_ino_t_32 *ret2;
        ret2 = (struct dirent_ino_t_32 *)ret;
        return (struct dirent *)ret2;
    }

    return ret;
}
