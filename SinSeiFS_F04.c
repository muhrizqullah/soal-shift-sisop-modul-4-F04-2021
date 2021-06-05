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
#include <sys/wait.h>
#define MKDIR_STATUS 1
#define MKNOD_STATUS 2

static const char *dirpath = "/home/misdinar/Downloads";
static const char *logpath = "/home/misdinar/SinSeiFS.log";
static int status = 0;

void encrypt(char *enc, int length)
{
    if (strcmp(enc, ".") == 0 || strcmp(enc, "..") == 0)
        return;
    for (int i = length; i >= 0; i--)
    {
        if (enc[i] == '/')
            break;
        if (enc[i] == '.')
        {
            length = i;
            break;
        }
    }
    int start = 0;
    for (int i = 0; i < length; i++)
    {
        if (enc[i] == '/')
        {
            start = i + 1;
            break;
        }
    }
    for (int i = start; i < length; i++)
    {
        if (enc[i] == '/')
            continue;
        if (enc[i] >= 'A' && enc[i] <= 'Z')
        {
            enc[i] = 'Z' - (enc[i] - 'A');
        }
        else if (enc[i] >= 'a' && enc[i] <= 'z')
        {
            enc[i] = 'z' - (enc[i] - 'a');
        }
    }
}

void decrypt(char *enc, int length)
{
	//mencari posisi yg menjadi awal dari nama file/dir
	//AtoZ_sisop/haha.txt
    if (strcmp(enc, ".") == 0 || strcmp(enc, "..") == 0)
        return;
    if (strstr(enc, "/") == NULL)
        return;
    for (int i = length; i >= 0; i--)
    {
        if (enc[i] == '/')
            break;
        if (enc[i] == '.')
        {
            length = i;
            break;
        }
    }
	
    int start = length;
    for (int i = 0; i < length; i++)
    {
        if (enc[i] == '/')
        {
            start = i + 1;
            break;
        }
    }
	//meng-chyper
    for (int i = start; i < length; i++)
    {
        if (enc[i] == '/')
            continue;
        if (enc[i] >= 'A' && enc[i] <= 'Z')
        {
            enc[i] = 'Z' - (enc[i] - 'A');
        }
        else if (enc[i] >= 'a' && enc[i] <= 'z')
        {
            enc[i] = 'z' - (enc[i] - 'a');
        }
    }
}

void printlog(char *args)
{
    FILE *log;
	// dibuka filenya
    log = fopen(logpath, "a");
	// masukin log ke file yg di tuju
    fprintf(log, "%s\n", args);
	// ditutup
    fclose(log);
}

void printInfo(char *args)
{
    char message[10000], timestamp[40];
    char buffer[10000];
    memset(buffer, 0, sizeof(buffer));
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
	// mengambil tanggal dan jam
    strftime(buffer, sizeof(buffer), "%d%m%Y-%X", &tm);
    strcpy(timestamp, buffer);
	// membuat pesan log sesua format soal
    sprintf(message, "%s::%s::%s", "INFO", timestamp, args);
    printlog(message);
}

void printWarning(char *args)
{
    char message[10000], timestamp[40];
    char buffer[10000];
    memset(buffer, 0, sizeof(buffer));
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    strftime(buffer, sizeof(buffer), "%d%m%Y-%X", &tm);
    strcpy(timestamp, buffer);
    sprintf(message, "%s::%s::%s", "WARNING", timestamp, args);
    printlog(message);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

	//memeriksa apakah terdapat AtoZ_
    char *flagAtoz = strstr(path, "AtoZ_");
    if ((status != MKNOD_STATUS || status != MKDIR_STATUS) && flagAtoz != NULL){
            decrypt(flagAtoz, strlen(flagAtoz));
    }
    sprintf(fpath, "%s%s", dirpath, path);

    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    char catatLog[1000];

    char *flagAtoz = strstr(path, "AtoZ_");
    if (status == MKNOD_STATUS && flagAtoz != NULL)
        {
            int length = strlen(flagAtoz);
            for (int i = length; i >= 0; i--)
            {
                if (flagAtoz[i] == '/')
                {
                    length = i;
                    break;
                }
            }
            decrypt(flagAtoz, length);
        
    }
    else if (flagAtoz != NULL)
            decrypt(flagAtoz, strlen(flagAtoz));

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;

        sprintf(fpath, "%s", path);
    }
    else
    {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    sprintf(catatLog, "%s::%s", "READ", path);
    printInfo(catatLog);

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
	status = 0;
    return res;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    char catatLog[1000];
    char *flagAtoz = strstr(path, "AtoZ_");
    if (flagAtoz != NULL)
        decrypt(flagAtoz, strlen(flagAtoz));

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
    {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;
    sprintf(catatLog, "%s::%s", "READDIR", path);
    printInfo(catatLog);

    dp = opendir(fpath);

    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (flagAtoz != NULL)
			encrypt(de->d_name, strlen(de->d_name));
            // encription(de->d_name);
        res = (filler(buf, de->d_name, &st, 0));

        if (res != 0)
            break;
    }
    closedir(dp);
    status = 0;
    return 0;
}

static int xmp_unlink(const char *path)
{
    int res;

    char *flagAtoz = strstr(path, "AtoZ_");
    if (flagAtoz != NULL)
        decrypt(flagAtoz, strlen(flagAtoz));

    char fpath[1000];
    char catatLog[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;

        sprintf(fpath, "%s", path);
    }
    else
    {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    sprintf(catatLog, "%s::%s", "UNLINK", path);
    printWarning(catatLog);
    res = unlink(fpath);
    if (res == -1)
        return -errno;

    status = 0;

    return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    char catatLog[1000];

    char *flagAtoz = strstr(path, "AtoZ_");
    if (status == MKNOD_STATUS && flagAtoz != NULL){
            int length = strlen(flagAtoz);
            for (int i = length; i >= 0; i--)
            {
                if (flagAtoz[i] == '/')
                {
                    length = i;
                    break;
                }
            }
            decrypt(flagAtoz, length);
    }
    else if (flagAtoz != NULL)
            decrypt(flagAtoz, strlen(flagAtoz));

    if (strcmp(path, "/") == 0)
    {
        path = dirpath;

        sprintf(fpath, "%s", path);
    }
    else
    {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    sprintf(catatLog, "%s::%s", "WRITE", path);
    printInfo(catatLog);
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
	status = 0;
    return res;
}

static int xmp_rename(const char *from, const char *to)
{
    int res;
    char ffrom[1000];
    char fto[1000];
    char catatLog[1000];
    sprintf(ffrom, "%s%s", dirpath, from);
    sprintf(fto, "%s%s", dirpath, to);

    res = rename(ffrom, fto);
    status = 0;

    sprintf(catatLog, "%s::%s::%s", "RENAME", from, to);
    printInfo(catatLog);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    int res;
    status = MKNOD_STATUS;
    char fpath[1000];
    char catatLog[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
    {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    sprintf(catatLog, "%s::%s", "MKNOD", path);
    printInfo(catatLog);

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

static int xmp_mkdir(const char *path, mode_t mode)
{
    int res;
    char catatLog[1000];
    status = MKDIR_STATUS;
    char fpath[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
    {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    res = mkdir(fpath, mode);
    sprintf(catatLog, "%s::%s", "MKDIR", path);
    printInfo(catatLog);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_rmdir(const char *path)
{
    int res;
    char fpath[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
    {
        sprintf(fpath, "%s%s", dirpath, path);
    }
    status = 0;
    char *flagAtoz = strstr(path, "AtoZ_");
    if (flagAtoz != NULL)
        decrypt(flagAtoz, strlen(flagAtoz));
    res = rmdir(fpath);
    char catatLog[1000];
    sprintf(catatLog, "%s::%s", "RMDIR", path);
    printWarning(catatLog);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    int res;
    char *flagAtoz = strstr(path, "AtoZ_");
    if (status == MKNOD_STATUS && flagAtoz != NULL)
        {
            int length = strlen(flagAtoz);
            for (int i = length; i >= 0; i--)
            {
                if (flagAtoz[i] == '/')
                {
                    length = i;
                    break;
                }
            }
            decrypt(flagAtoz, length);
        
    }
    else if (flagAtoz != NULL)
            decrypt(flagAtoz, strlen(flagAtoz));

    res = open(path, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
    int res;
    status = 0;
    char *flagAtoz = strstr(path, "AtoZ_");
    if (flagAtoz != NULL)
        decrypt(flagAtoz, strlen(flagAtoz));

    char fpath[1000];
    char catatLog[1000];
    if (strcmp(path, "/") == 0)
    {
        path = dirpath;
        sprintf(fpath, "%s", path);
    }
    else
    {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    sprintf(catatLog, "%s::%s", "TRUNCATE", path);
    printInfo(catatLog);
    res = truncate(fpath, size);
    if (res == -1)
        return -errno;

    return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .read = xmp_read,
    .readdir = xmp_readdir,
    .write = xmp_write,
    .truncate = xmp_truncate,
    .mknod = xmp_mknod,
    .mkdir = xmp_mkdir,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .open = xmp_open,
    .rename = xmp_rename};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
