/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  Minor modifications and note by Andy Sayler (2012) <www.andysayler.com>

  Source: fuse-2.8.7.tar.gz examples directory
  http://sourceforge.net/projects/fuse/files/fuse-2.X/

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall `pkg-config fuse --cflags` fusexmp.c -o fusexmp `pkg-config fuse --libs`

  Note: This implementation is largely stateless and does not maintain
        open file handels between open and release calls (fi->fh).
        Instead, files are opened and closed as necessary inside read(), write(),
        etc calls. As such, the functions that rely on maintaining file handles are
        not implmented (fgetattr(), etc). Those seeking a more efficient and
        more complete implementation may wish to add fi->fh support to minimize
        open() and close() calls and support fh dependent functions.

*/
// learned the process for this lab from jon5477 github page

#define FUSE_USE_VERSION 28
#define HAVE_SETXATTR

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#include <stdlib.h>
#include <limits.h>
#include "aes-crypt.h"


#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

//encryption 


#define INPUTDATA ((struct inputData *) fuse_get_context()->private_data)



typedef struct inputData 
{
	
	char *pass;
	char *dir;
}inputData;


int addXattr(const char *path){
	
	setxattr(path, "Encrypt","true",(sizeof(char)*5),0);
	return 0;
}

char XattrValue(const char *path){
	char retBuffer[5];
	getxattr(path,"Encrypt",retBuffer,(sizeof(char)*5));
	
	
	return retBuffer;
}


static void newpath(char newPATH[PATH_MAX], const char *path)
{
	strcpy(newPATH, INPUTDATA->dir);
    	strncat(newPATH, path, PATH_MAX);
	
}


static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);

	res = lstat(newPATH, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	res = access(newPATH, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
		
	res = readlink(newPATH, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(newPATH);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(newPATH, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(newPATH, mode);
	else
		res = mknod(newPATH, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	res = mkdir(newPATH, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	res = unlink(newPATH);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	res = rmdir(newPATH);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;
	
	
	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
	
	
	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;
	
	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	res = chmod(newPATH, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	res = lchown(newPATH, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	res = truncate(newPATH, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(newPATH, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	res = open(newPATH, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	int res;
	(void) fi;

	//rewriting path 
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	FILE *fp,*temp;
	
	//open file 
	fp = fopen(newPATH, "r");

	if (fp == NULL)
		return -errno;
	
/*
	char* Tpath = malloc(sizeof(char)*(strlen(newPATH) + strlen(".Mirror") + 1));
	Tpath[0]= '\0';
	strcat(Tpath,newPATH);
	strcat(Tpath,".Mirror");
	*/

	
	//fp = fopen(newPATH,"rb");
	//temp = fopen(Tpath,"wb+");

	temp = tmpfile();
	
/* //-----------------------------------------------------------------------//	
	//testing if file is encrypted
	int action = -1;
	if(!(strcmp(XattrValue(newPATH),"true"))){
		action = 0;
	}
	
	do_crypt(fp,temp,action,INPUTDATA->pass); 

//-----------------------------------------------------------------------//	*/
	do_crypt(fp,temp,0,INPUTDATA->pass);
	
	fflush(temp);
	fseek(temp,0,SEEK_SET);

	res = fread(buf,1,size,temp);
	if (res == -1)
		res = -errno;

	
	
	
	
	fclose(temp);
	fclose(fp);
	//remove(Tpath);


/*
	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;
*/
	//fclose(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	
	char newPATH[PATH_MAX];
	newpath(newPATH,path);

	FILE *fp,*temp;
	
	(void) fi;
	
	
	temp = tmpfile();
	fp = fopen(newPATH,"rb+");
	int tempD = fileno(temp);

	//fd = open(newPATH, O_WRONLY);
	if (fp == NULL)
		return -errno;

	
/*
	char* Tpath = malloc(sizeof(char)*(strlen(newPATH) + strlen(".temp") + 1));
	Tpath[0]= '\0';
	strcat(Tpath,newPATH);
	strcat(Tpath,".temp");

	printf("\n New path = %s \n",newPATH);
	printf("\n Temp path = %s \n",Tpath);
	
*/
	
	//temp = fopen(Tpath,"wb+");
	
	
	//fseek(fp,0,SEEK_SET);
	//fseek(temp,0,SEEK_SET);
	
/* //-----------------------------------------------------------------------//	
	//testing if file is encrypted
	int action = -1;
	if(!(strcmp(XattrValue(newPATH),"true"))){
		action = 0;
	}
	
	do_crypt(fp,temp,action,INPUTDATA->pass); 

//-----------------------------------------------------------------------//	*/
	
	do_crypt(fp,temp,0,INPUTDATA->pass); 

	fseek(fp,0,SEEK_SET);
	fseek(temp,0,SEEK_SET);
	
	//res = fwrite(buf,1,size,temp);
	res = pwrite(tempD,buf,size,offset);
	if (res == -1)
		res = -errno;

	fseek(fp,0,SEEK_SET);
	fseek(temp,0,SEEK_SET);
	
/* //-----------------------------------------------------------------------//	
	//testing if file is encrypted
	int action = -1;
	if(!(strcmp(XattrValue(newPATH),"true"))){
		action = 0;
	}
	
	do_crypt(fp,temp,action,INPUTDATA->pass); 

//-----------------------------------------------------------------------//	*/
	do_crypt(temp,fp,1,INPUTDATA->pass); 
	
	fseek(fp,0,SEEK_SET);
	fseek(temp,0,SEEK_SET);

	fclose(fp);
	fclose(temp);
	//remove(Tpath);

	
	//res = pwrite(fd, buf, size, offset);
	//if (res == -1)
		//res = -errno;

	//close(fd);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	res = statvfs(newPATH, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {

    	(void) fi;
	
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
    	int res;
   	 res = creat(newPATH, mode);
   	 if(res == -1)
	return -errno;

    	close(res);
	//addXattr(path);
    	return 0;
}

/* ORIGINAL 

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {

    	(void) fi;
	
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
    	int res;
   	 res = creat(newPATH, mode);
   	 if(res == -1)
	return -errno;

    	close(res);
	
    	return 0;
}

*/
static int xmp_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	
	(void) path;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */
	

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_SETXATTR
static int xmp_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	int res = lsetxattr(newPATH, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	int res = lgetxattr(newPATH, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_listxattr(const char *path, char *list, size_t size)
{
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	int res = llistxattr(newPATH, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_removexattr(const char *path, const char *name)
{
	char newPATH[PATH_MAX];
	newpath(newPATH,path);
	
	int res = lremovexattr(newPATH, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create         = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
#ifdef HAVE_SETXATTR
	.setxattr	= xmp_setxattr,
	.getxattr	= xmp_getxattr,
	.listxattr	= xmp_listxattr,
	.removexattr	= xmp_removexattr,
#endif
};


int 
main(int argc, char *argv[])
{
	umask(0);

	struct inputData *Data = malloc(sizeof(struct inputData));
	
	Data->pass = argv[1];
	Data->dir = realpath(argv[2],NULL);
	
	argv[2] = argv[0];
	
	printf("\nData->dir : %s \n",Data->dir);
	printf("Data->pass : %s \n",Data->pass);
	
	//printf("\nINPUTDATA->dir : %s \n",INPUTDATA->dir);
	//printf("INPUTDATA->pass : %s \n",INPUTDATA->pass);	
	
	
	return fuse_main(argc-2, argv+2, &xmp_oper, Data);
}



