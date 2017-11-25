#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

static const char *dirpath = "/home/steve/Downloads";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

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


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
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

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
   int fd;
   int res;
   char fpath[1000];
   sprintf(fpath,"%s%s",dirpath,path);
  
   (void) fi;
  
   char ext[1000];
   int i,u;
   for(i = 0; i < strlen(fpath); i++){
        if(fpath[i]=='.') u=i;
    } //mengekstrak ekstensi file
   strcpy(ext, fpath+u);
  
  
   fd = open(fpath, O_RDONLY);
   if (fd == -1)
                  return -errno;
   else
   {
      if(strcmp(ext, ".copy") == 0 )
      {
         pid_t child_id;
         child_id=fork();
         int status;
         if(child_id==0){
         
         char *argv[]= {"zenity","--error","--text","File yang anda buka adalah file hasil salinan. File tidak bisa diubah maupun disalin kembali!",NULL};
         execv("/usr/bin/zenity", argv);       

    
        }
        else{
            while ((wait(&status)) > 0);

            
             return -errno;                                   
            }

          res = pread(fd, buf, size, offset);
          if (res == -1)
                         res = -errno;

          close(fd);
       }
    }
   return res;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
    .chmod      = xmp_chmod,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
