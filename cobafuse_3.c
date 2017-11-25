#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <sys/stat.h>

static const char *dirpath = "/home/steve/Downloads";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
 int res;
 char fpath[1000];
 sprintf(fpath,"%s%s",dirpath,path);
 res = lstat(fpath, stbuf);

 if(res == -1)
 {
  return -errno;
 }

 return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
    int res;
    char fpath[1000];
    char direktori[] = "/home/steve/Downloads/file";
    sprintf(fpath,"%s%s", direktori, path);
    res = chmod(fpath, mode);
    if(res == -1)
        return -errno;

    return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s", dirpath, path);
    res = lchown(fpath, uid, gid);
    if(res == -1)
        return -errno;

    return 0;
}

static int xmp_getdir(const char *path, fuse_dirh_t h, fuse_dirfil_t filler)
{
 char fpath[1000];
 if(strcmp(path,"/") == 0)
 {
  path=dirpath;
  sprintf(fpath,"%s",path);
 }
 else sprintf(fpath, "%s%s",dirpath,path);
 int res = 0;
 DIR *dp;
 struct dirent *de;
 dp = opendir(fpath);
 if(dp==NULL)
 {
  return -errno;
 }
 while((de = readdir(dp))!=NULL)
 {
  res = filler(h, de->d_name, de->d_type);
  if(res!=0) break;
 }
 closedir(dp);
 return res;
}

static int xmp_mkdir(const char *path,mode_t mode)
{
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = mkdir (fpath,mode);
    if(res == -1)
        return -errno;
    return 0;
}


static int xmp_truncate(const char *path, off_t size)
{
    int res;
     char fpath[1000];
 sprintf(fpath,"%s%s", dirpath, path);
    res = truncate(fpath, size);
    if(res == -1)
        return -errno;

    return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    int res;
 char fpath[1000];
 sprintf(fpath,"%s%s", dirpath, path);
    res = mknod(fpath, mode, rdev);
    if(res == -1)
        return -errno;

    return 0;
}



static int xmp_read(const char *path, char *buf, size_t size, off_t offset)
{
    int fd;
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s", dirpath, path);
    fd = open(fpath, O_RDONLY);
    if(fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if(res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset)
{
    int fd;
    int res;
 char fpath[1000];
 sprintf(fpath,"%s%s", dirpath, path);
    fd = open(fpath, O_WRONLY);
    if(fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if(res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_rename(const char *from, const char *to)
{
    int res;
    char ffrom[1000];
    char fto[1000];
    //system("mkdir /home/steve/Downloads/simpanan");
    //system("chmod 777 /home/steve/Downloads/simpanan");

    char direktori[] = "/home/steve/Downloads/simpanan";
    mkdir(direktori,0777);

    //system("zenity --info --text=\"INI ADALAH INFO: \" --title=\"INFO\"");
    sprintf(ffrom,"%s%s",dirpath,from);
    sprintf(fto,"%s%s.copy",direktori,to);
    res = rename(ffrom, fto);

    if(res == -1)
    return -errno;

    return 0;
}
//system("zenity --error --text=\"An error occurred!\" --title=\"Warning!\"");

static struct fuse_operations xmp_oper =
{
 .getattr = xmp_getattr,
 .getdir = xmp_getdir,
 .mknod = xmp_mknod,
 .mkdir = xmp_mkdir,
 .rename = xmp_rename,
 .chmod = xmp_chmod,
 .chown = xmp_chown,
 .truncate = xmp_truncate,
 .read = xmp_read,
 .write = xmp_write,
};

int main(int argc, char *argv[])
{
 return fuse_main(argc, argv, &xmp_oper);
}
