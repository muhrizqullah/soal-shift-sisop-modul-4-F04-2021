#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static  const  char *dirpath = "/home/gretzy/Downloads";
static const char *logpath = "/home/gretzy/SinSeiFS.log";

char* chyper(char namaFile[]) {
    char namaTemp[1024] ;
    strcpy(namaTemp, namaFile) ;
    int i ;
    for(i = 0 ; i < strlen(namaTemp) ; i++) {
        if (namaTemp[i] >= 'A' && namaTemp[i] <= 'Z') {
            namaTemp[i] = 'Z' - (namaTemp[i] - 'A') ;
        }
        else if (namaTemp[i] >= 'a' && namaTemp[i] <= 'z') {
            namaTemp[i] = 'z' - (namaTemp[i] - 'a') ;
        }
    }
    char* res = namaTemp ;
    return res ;
}

void printlog(char * args){
	FILE* log;
	log = fopen(logpath,"a+");
	fprintf(log,"%s\n",args);
	fclose(log);
}

void printInfo(char * args){
	char message[10000],timestamp[40];
	char buffer[10000];
    memset(buffer,0,sizeof(buffer));
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(buffer,sizeof(buffer),"%d%m%Y-%X",&tm);
    strcpy(timestamp,buffer);
	sprintf(message,"%s::%s::%s","INFO",timestamp,args);
	printlog(message);
}

void printWarning(char * args){
	char message[10000],timestamp[40];
	char buffer[10000];
    memset(buffer,0,sizeof(buffer));
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(buffer,sizeof(buffer),"%y%m%d-%X",&tm);
    strcpy(timestamp,buffer);
	sprintf(message,"%s::%s::%s","WARNING",timestamp,args);
	printlog(message);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    sprintf(fpath, "%s%s", dirpath, path);

    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
	char logbuffer[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;
	sprintf(logbuffer,"%s::%s","READDIR",path);
	printInfo(logbuffer);

    dp = opendir(fpath);

    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));

        if (res != 0)
            break;
    }
    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];char logbuffer[1000];
	
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;

        sprintf(fpath, "%s", path);
    }
    else{
		sprintf(fpath, "%s%s", dirpath, path);
	}
        
	sprintf(logbuffer,"%s::%s","READ",path);
	printInfo(logbuffer);

    int res = 0;
    int fd = 0;

    (void)fi;

    fd = open(fpath, O_RDONLY);

    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1)
        res = -errno;

    close(fd);

    return res;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    int res;
    char fpath[1000]; char logbuffer[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;

        sprintf(fpath, "%s", path);
    }
    else{
		sprintf(fpath, "%s%s", dirpath, path);
	}
    
	sprintf(logbuffer,"%s::%s","MKNOD",path);
	printInfo(logbuffer);

    if (S_ISREG(mode))
    {
        res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    }
    else if (S_ISFIFO(mode))
        res = mkfifo(fpath, mode);
    else
        res = mknod(fpath, mode, rdev);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000]; char logbuffer[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;

        sprintf(fpath, "%s", path);
    }
    else{
		sprintf(fpath, "%s%s", dirpath, path);
	}
        
	sprintf(logbuffer,"%s::%s","WRITE",path);
	printInfo(logbuffer);
    int fd = 0;
    int res = 0;

    (void)fi;

    fd = open(fpath, O_WRONLY);

    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_unlink(const char *path)
{
    int res;

    char fpath[1000];char logbuffer[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;

        sprintf(fpath, "%s", path);
    }
    else{
        sprintf(fpath, "%s%s", dirpath, path);
    }
    
	sprintf(logbuffer,"%s::%s","UNLINK",path);
	printWarning(logbuffer);
    res = unlink(fpath);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
    int res;

    char fpath[1000]; char logbuffer[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;

        sprintf(fpath, "%s", path);
    }
    else{
		sprintf(fpath, "%s%s", dirpath, path);
	}

	
	sprintf(logbuffer,"%s::%s","TRUNCATE",path);
	printInfo(logbuffer);
    res = truncate(fpath, size);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_rmdir(const char *path) {
	int res;
	char fpath[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else{
		sprintf(fpath, "%s%s", dirpath, path);
	}
	res = rmdir(fpath);
	char logbuffer[1000];
	sprintf(logbuffer,"%s::%s","RMDIR",path);
	printWarning(logbuffer);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res; char logbuffer[1000];
	char fpath[1000];
	if(strcmp(path,"/") == 0){
        path=dirpath;
        sprintf(fpath,"%s",path);
    }else{
        sprintf(fpath, "%s%s",dirpath,path);
    }

	res = mkdir(fpath, mode);
	sprintf(logbuffer,"%s::%s","MKDIR",path);
	printInfo(logbuffer);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;

	res = open(path, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
    printf("\n\nDEBUG rename\n\n");
    char ffrom[1000];
	char fto[1000];
    char logbuffer[1000];
	sprintf(ffrom, "%s%s",dirpath, from);
	sprintf(fto, "%s%s",dirpath, to);

	res = rename(ffrom, fto);
	sprintf(logbuffer,"%s::%s::%s","RENAME",from, to);
	printInfo(logbuffer);
	if (res == -1)
		return -errno;
	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.write = xmp_write,
	.truncate = xmp_truncate,
	.mkdir = xmp_mkdir,
	.unlink = xmp_unlink,
	.rmdir = xmp_rmdir,
	.mknod = xmp_mknod,
	.open = xmp_open,
	.rename = xmp_rename
};

int main(int  argc, char *argv[]){
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
