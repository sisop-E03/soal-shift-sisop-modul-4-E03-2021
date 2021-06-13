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
char *key = "SISOP";

static const char *LOG = "/home/rauf/SinSeiFS.log";
char *leveli ="INFO";
char *levelw ="WARNING";

//untuk level warning rmdir dan unlink
void Levellog(char *level, char* desc, const char* path) {
	FILE *file_log = fopen(LOG, "a");

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	int tahun = tm.tm_year+1900;
	int bulan = tm.tm_mon+1;
	int hari = tm.tm_mday;
	int jam = tm.tm_hour;
	int menit = tm.tm_min;
	int detik = tm.tm_sec;

	fprintf(file_log, "%s::%d%d%d-%02d:%02d:%02d::%s::%s\n", level, hari, bulan, tahun, jam, menit, detik, desc, path);
	fclose(file_log);
}

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

void encryptViginere(char *str)
{
    int i = 0;
    int j = 0;
    while (str[i] != '\0' && str[i] != '.')
    {  
        if(str[i] == '/')
        {
            i++;
            j = 0;
            continue;
        }
        else if(str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = ((str[i] - 65) + (key[j % strlen(key)] - 65)) % 26 + 65;
        }
        else if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] = ((str[i] - 97) + (key[j % strlen(key)] - 65)) % 26 + 97;
        }

        j++;
        i++;
    }
}

void decryptViginere(char *str)
{
    int i = 0;
    int j = 0;
    while (str[i] != '\0' && str[i] != '.')
    {
        if(str[i] == '/')
        {
            i++;
            j = 0;
            continue;
        }
        else if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = ((str[i] - 65) - (key[j % strlen(key)] - 65) + 26) % 26 + 65;
            j++;
        }
        else if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] = ((str[i] - 97) - (key[j % strlen(key)] - 65) + 26) % 26 + 97;
            j++;
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

void write_log2(char method[], char oldname[], char newname[])
{
    FILE *fp = fopen("no2.log", "a+");

    if (strcmp(method, "mkdir") == 0)
        fprintf(fp, "%s %s\n", method, newname);
    else if (strcmp(method, "rename") == 0)
        fprintf(fp, "%s %s to %s\n", method, oldname, newname);
    fclose(fp);
}

int cek_log2(char *str)
{
    char line[1000];

    FILE *fp = fopen("no2.log", "r");

    if(!fp) {
        return 0;
    }

    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, "rename", 6))
        {
            continue;
        }

        line[strcspn(line, "\n")] = '\0';
        char *path = strrchr(line, ' ') + 1;
        char *name = strstr(path, "/RX_");

        if (!name)
        {
            continue;
        }

        char name2[1000];
        strcpy(name2, str);

        if (!strcmp(strtok(name, "/"), strtok(name2, "/")))
        {
            return 1;
        }
    }

    return 0;
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
                if (cek_log2(rx))
                {
                    decryptViginere(&real_path[index]);
                    atbash(&real_path[index]);
                }
                else
                {
                    rot13(&real_path[index]);
                    atbash(&real_path[index]);
                }

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

    //bikin lognya
    Levellog(leveli, "LS", fpath);

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
        }
        else if (strstr(path, "/RX_"))
        {
            if (cek_log2(strstr(path, "/RX_")))
            {
                atbash(name);
                encryptViginere(name);
            }
            else
            {
                atbash(name);
                rot13(name);
            }
        }
        res = (filler(buf, name, &st, 0));

        if (res != 0)
            break;
    }

    closedir(dp);

    //bikin lognya
    Levellog(leveli, "CD", path);


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

    //bikin lognya
    Levellog(leveli, "READ", fpath);

    return res;
}

static int xmp_rename(const char *from, const char *to)
{
    printf("xmp_rename\n");
    char ffrom[1000];
    char fto[1000];
    strcpy(ffrom, get_real_path(from));
        strcpy(fto, get_real_path(to));

    if (strstr(to, "AtoZ_"))
        write_log(ffrom, fto);
    else if (strstr(to, "RX_"))
        write_log2("rename", ffrom, fto);

    int res;
    res = rename(ffrom, fto);
    if (res == -1)
        return -errno;

    //bikin lognya
    Levellog(leveli, "MOVE", ffrom);

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
    else if (strstr(fpath, "RX_"))
        write_log2("mkdir", NULL, fpath);

    res = mkdir(fpath, mode);

    if (res == 1)
        return -errno;

    //bikin lognya BUAT NO 4
    Levellog(leveli, "MKDIR", fpath);

    return 0;
}

static int xmp_rmdir(const char *path)
{
    printf("xmp_rmdir\n");
    int res;
    char fpath[1000];

    strcpy(fpath, get_real_path(path));

    res = rmdir(fpath);

    if (res == 1)
        return -errno;

    //untuk di log no4
    Levellog(levelw, "RMDIR", fpath);

    return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    printf("xmp_mknod\n");
    int res;

    char fpath[1000];

    strcpy(fpath, get_real_path(path));

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

    //untuk di log no4
    Levellog(leveli, "CREATE", fpath);

    return 0;
}

static int xmp_write(const char *fpath, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    printf("xmp_write\n");
    int fd;
    int res;

    (void)fi;
    fd = open(fpath, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);

    //untuk di log no4
    Levellog(leveli, "WRITE", fpath);

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
