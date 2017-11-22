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
    //sprintf(fpath,"%s%s", dirpath, path);
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
   int i;
   for(i = 0; i < strlen(fpath) && fpath[i] != '.'; i++); //mengekstrak ekstensi file
   strcpy(ext, fpath+i);
  
   char cmd[1000];
  
   fd = open(fpath, O_RDONLY);
   if (fd == -1)
                  return -errno;
   else
   {
      if(strcmp(ext, ".pdf") == 0 || strcmp(ext, ".txt") == 0 || strcmp(ext, ".doc") == 0)
      {
         pid_t child_id;
         child_id=fork();
         int status;
         if(child_id==0){
         //system("zenity --width 400 --error --title 'Error' --text 'Terjadi Kesalahan! File berisi konten berbahaya.'");
         char *argv[]= {"zenity","--error","--text","\"ha haha\"",NULL};
         execv("/usr/bin/zenity", argv);       

    
        }
        else{
            while ((wait(&status)) > 0);

             char from[1000], to[1000], newFolder[1000], filename[1000];
            
             //mengambil direktori dari file
             strcpy(newFolder, fpath);
            
             //menghapus filename dan menambahkan 'rahasia'
             for(i = strlen(newFolder) - 1; newFolder[i] != '/'; newFolder[i--] = 0);
             strcat(newFolder, "rahasia");
            
             //mengambil filename
             for(i = strlen(fpath) - 1; fpath[i] != '/'; i--);
             strcpy(filename, fpath+(i+1) ); //+1 untuk menghilangkan tanda ‘/’
            
             //Mengecek ada tidaknya folder rahasia
             struct stat s;
             if (stat(newFolder, &s) != 0)  //0 berarti folder tersebut tidak ada
                    mkdir(newFolder, 0777); //membuat folder 'rahasia' folder ke direktori mountnya
            
             sprintf(from, "%s", fpath);  //mengambil path file awal
             sprintf(to, "%s/%s.ditandai", newFolder, filename); //membuat path tujuan pemindahan file ke folder ‘rahasia’ dan menambahkan ekstensi ‘.ditandai’
            
             sprintf(cmd, "mv %s %s", from, to); //Merename dan memindah file
             system(cmd);
             sprintf(cmd, "chmod 000 %s", to);
             system(cmd);
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
