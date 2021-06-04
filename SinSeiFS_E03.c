#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

// static const char *dirpath = "/home/[user]/Donwloads";
static const char *dirpath = "/home/rauf/Downloads";

// Enkripsi dan dekripsi string menggunakan atbash
void atbash(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '.')
            break;
        if (!((str[i] >= 0 && str[i] < 65) || (str[i] > 90 && str[i] < 97) || (str[i] > 122 && str[i] <= 127)))
        {
            if (str[i] >= 'A' && str[i] <= 'Z')
                str[i] = 'Z' + 'A' - str[i];
            if (str[i] >= 'a' && str[i] <= 'z')
                str[i] = 'z' + 'a' - str[i];
        }
        i++;
    }
}

void rot13(char *str)
{
    int i = 0;
    while (str[i] != '\0' && str[i] != '.')
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = ((str[i] - 65) + 13) % 26 + 65;
        }
        else if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] = ((str[i] - 97) + 13) % 26 + 97;
        }

        i++;
    }
}

void write_log(char oldname[], char newname[])
{
    FILE *fp = fopen("encode.log", "a+");

    fprintf(fp, "%s --> %s\n", oldname, newname);

    fclose(fp);
}

// Fungsi untuk return path asli
// dan melakukan dekripsi jika path diawali dengan "AtoZ_" atau "RX_"
char *get_real_path(const char *path)
{
    char *fpath = malloc(sizeof(char) * 1000);
    char real_path[100];
    strcpy(real_path, path);

    char *str = strstr(real_path, "/AtoZ_");
    char *rx = strstr(real_path, "/RX_");
    if (str)
    {
        int index = strlen(real_path) - strlen(str) + 1;
        while (index < strlen(real_path))
        {
            if (real_path[index] == '/')
            {
                atbash(&real_path[index]);
                break;
            }
            index++;
        }
        sprintf(fpath, "%s%s", dirpath, real_path);
    }
    else if (rx)
    {
        int index = strlen(real_path) - strlen(rx) + 1;
        while (index < strlen(real_path))
        {
            if (real_path[index] == '/')
            {
                rot13(&real_path[index]);
                atbash(&real_path[index]);
                break;
            }
            index++;
        }
        sprintf(fpath, "%s%s", dirpath, real_path);
    }
    else
    {
        sprintf(fpath, "%s%s", dirpath, path);
    }

    return fpath;
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    strcpy(fpath, get_real_path(path));

    res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void)offset;
    (void)fi;

    dp = opendir(get_real_path(path));

    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        char name[100];
        strcpy(name, de->d_name);
        if (strstr(path, "/AtoZ_"))
        {
            atbash(name);
        } else if (strstr(path, "/RX_"))
        {
            atbash(name);
            rot13(name);
        }
        res = (filler(buf, name, &st, 0));

        if (res != 0)
            break;
    }

    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("xmp_read\n");
    char fpath[1000];

    strcpy(fpath, get_real_path(path));

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

static int xmp_rename(const char *from, const char *to)
{
    printf("xmp_rename\n");
    char ffrom[1000];
    char fto[1000];
    sprintf(ffrom, "%s%s", dirpath, from);
    sprintf(fto, "%s%s", dirpath, to);

    if (strstr(to, "AtoZ_"))
        write_log(ffrom, fto);

    int res;
    res = rename(ffrom, fto);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    printf("xmp_mkdir\n");
    int res;
    char fpath[1000];

    strcpy(fpath, get_real_path(path));

    if (strstr(fpath, "AtoZ_"))
        write_log("No folder", fpath);

    res = mkdir(fpath, mode);

    if (res == 1)
        return -errno;
    return 0;
}

static int xmp_rmdir(const char *path)
{
    printf("xmp_rmdir\n");
    int res;
    char fpath[1000];

    sprintf(fpath, "%s%s", dirpath, path);

    res = rmdir(fpath);

    if (res == 1)
        return -errno;
    return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    printf("xmp_mknod\n");
    int res;

    if (S_ISREG(mode))
    {
        res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    }
    else if (S_ISFIFO(mode))
        res = mkfifo(path, mode);
    else
        res = mknod(path, mode, rdev);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("xmp_write\n");
    int fd;
    int res;

    (void)fi;
    fd = open(path, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .mkdir = xmp_mkdir,
    .rmdir = xmp_rmdir,
    .mknod = xmp_mknod,
    .rename = xmp_rename,
    .write = xmp_write};

int main(int argc, char *argv[])
{
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}