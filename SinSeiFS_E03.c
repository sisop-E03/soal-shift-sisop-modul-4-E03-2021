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
static const char *dirpath = "/home/iwandp/Downloads";

// Fungsi untuk mengecek apakah string [parameter 1] 
// dimulai dengan string [parameter 2]
int is_starts_with(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

// Enkripsi dan dekripsi string menggunakan atbash
void atbash(char *str){
    int i = 0;
    while(str[i]!='\0')
    {
        if(!((str[i]>=0&&str[i]<65)||(str[i]>90&&str[i]<97)||(str[i]>122&&str[i]<=127)))
        {
            if(str[i]>='A'&&str[i]<='Z')
                str[i] = 'Z' + 'A' - str[i];
            if(str[i]>='a'&&str[i]<='z')
                str[i] = 'z'+'a'-str[i];
        } 
        i++;
    }
}

// Fungsi untuk return path asli
// dan melakukan dekripsi jika path diawali dengan "AtoZ_"
char *get_real_path(const char *path) {
    char *fpath = malloc (sizeof (char) * 1000);
    char real_path[100];

    strcpy(real_path, path);
    if (is_starts_with(path, "/AtoZ_")) {
        for (int i=1; i<strlen(path); i++) {
            if (path[i] == '/'){
                atbash(&real_path[i+1]);
                break;
            }
        }
    }
    sprintf(fpath, "%s%s", dirpath, real_path);
    return fpath;
}

static  int  xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    strcpy(fpath, get_real_path(path));

    res = lstat(fpath, stbuf);

    if (res == -1) return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(get_real_path(path));

    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        char name[100];
        strcpy(name, de->d_name);
        if (is_starts_with(path, "/AtoZ_")) {
            atbash(name);
        }
        res = (filler(buf, name, &st, 0));

        if(res!=0) break;
    }

    closedir(dp);

    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("xmp_read\n");
    char fpath[1000];
    if(strcmp(path,"/") == 0)
    {
        path=dirpath;

        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;
    int fd = 0 ;

    (void) fi;

    fd = open(fpath, O_RDONLY);

    if (fd == -1) return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) res = -errno;

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
    int res;
    res = rename(ffrom, fto);
    if (res == -1)
        return -errno;
    return 0;
}

// Fungsi untuk return path asli saat operasi [mkdir]
// dan melakukan dekripsi pada path folder (nama folder baru tidak didekripsi)
// jika path diawali dengan "AtoZ_"
char *get_real_path_mkdir(const char *path) {
    char *fpath = malloc (sizeof (char) * 1000);
    char real_path[100];

    strcpy(real_path, path);
    if (is_starts_with(path, "/AtoZ_")) {
        for (int i=1; i<strlen(path); i++) {
            if (path[i] == '/') {
                atbash(&real_path[i+1]);
                break;
            }
        }
        for (int i=strlen(path)-1; i>=4; i--) {
            if (path[i] == '/'){
                atbash(&real_path[i+1]);
                break;
            }
        }
    }
    sprintf(fpath, "%s%s", dirpath, real_path);
    return fpath;
}

static int xmp_mkdir(const char *path, mode_t mode) 
{
    printf("xmp_mkdir\n");
    int res;

    res = mkdir(get_real_path_mkdir(path), mode);

    if (res==1)
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

    if (res==1)
        return -errno;
    return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev) 
{
    printf("xmp_mknod\n");
    int res;

    if (S_ISREG(mode)) {
        res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    } else if (S_ISFIFO(mode))
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

    (void) fi;
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
    .write = xmp_write
};



int  main(int  argc, char *argv[])
{
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}