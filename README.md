# soal-shift-sisop-modul-4-E03-2021

## NO 1

Di suatu jurusan, terdapat admin lab baru yang super duper gabut, ia bernama Sin. Sin baru menjadi admin di lab tersebut selama 1 bulan. Selama sebulan tersebut ia bertemu orang-orang hebat di lab tersebut, salah satunya yaitu Sei. Sei dan Sin akhirnya berteman baik. Karena belakangan ini sedang ramai tentang kasus keamanan data, mereka berniat membuat filesystem dengan metode encode yang mutakhir. Berikut adalah filesystem rancangan Sin dan Sei :
	
``NOTE : 
Semua file yang berada pada direktori harus ter-encode menggunakan Atbash cipher(mirror).
Misalkan terdapat file bernama kucinglucu123.jpg pada direktori DATA_PENTING
“AtoZ_folder/DATA_PENTING/kucinglucu123.jpg” → “AtoZ_folder/WZGZ_KVMGRMT/pfxrmtofxf123.jpg”
Note : filesystem berfungsi normal layaknya linux pada umumnya, Mount source (root) filesystem adalah directory /home/[USER]/Downloads, dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di-encode.
Referensi : https://www.dcode.fr/atbash-cipher``


- a. Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- b. Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- c. Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.
- d. Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]
- e. Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

### Penyelesaian
Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
```c
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

```

Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.

Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]
```c
void write_log2(char method[], char oldname[], char newname[])
{
    FILE *fp = fopen("no2.log", "a+");

    if (strcmp(method, "mkdir") == 0)
        fprintf(fp, "%s %s\n", method, newname);
    else if (strcmp(method, "rename") == 0)
        fprintf(fp, "%s %s to %s\n", method, oldname, newname);
    fclose(fp);
}
```
- e. Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

## NO 2
Selain itu Sei mengusulkan untuk membuat metode enkripsi tambahan agar data pada komputer mereka semakin aman. Berikut rancangan metode enkripsi tambahan yang dirancang oleh Sei
- a. Jika sebuah direktori dibuat dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai kasus nomor 1  dengan algoritma tambahan ROT13 (Atbash + ROT13).
- b. Jika sebuah direktori di-rename dengan awalan “RX_[Nama]”, maka direktori tersebut akan menjadi direktori terencode beserta isinya dengan perubahan nama isi sesuai dengan kasus nomor 1 dengan algoritma tambahan Vigenere Cipher dengan key “SISOP” (Case-sensitive, Atbash + Vigenere).
- c. Apabila direktori yang terencode di-rename (Dihilangkan “RX_” nya), maka folder menjadi tidak terencode dan isi direktori tersebut akan terdecode berdasar nama aslinya.
- d. Setiap pembuatan direktori terencode (mkdir atau rename) akan tercatat ke sebuah log file beserta methodnya (apakah itu mkdir atau rename).
- e. Pada metode enkripsi ini, file-file pada direktori asli akan menjadi terpecah menjadi file-file kecil sebesar 1024 bytes, sementara jika diakses melalui filesystem rancangan Sin dan Sei akan menjadi normal. Sebagai contoh, Suatu_File.txt berukuran 3 kiloBytes pada directory asli akan menjadi 3 file kecil yakni:

``Suatu_File.txt.0000
Suatu_File.txt.0001
Suatu_File.txt.0002``

Ketika diakses melalui filesystem hanya akan muncul Suatu_File.txt

### Penyelesaian
Karena soal 1 dan 2 ada beberapa kesamaan, maka solusi dari soal 1 dapat dimodifikasi untuk menyelesaikan soal 2. Pertama, fungsi-fungsi untuk enkripsi dan dekripsi tambahan dibuat terlebih dahulu.

```c
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
```
Fungsi `rot13` akan melakukan looping setiap karakter sampai ditemukan null terminator atau tanda titik. Kemudian bila karakter saat ini berupa huruf, maka huruf tersebut akan digeser 13 kali. Enkripsi ini bersifat simetris sehingga dapat digunakan juga untuk dekripsi. Fungsi ini akan digunakan untuk soal 2a

```c
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
```
Mirip seperti `rot13`, fungsi `encryptViginere` akan melakukan looping setiap karakter sampai ditemukan null terminator atau tanda titik. BIla karakter saat ini huruf, huruf tersebut akan digeser sesuai dengan karakter di index kunci saat ini kemudian index kunci akan bergeser 1. Apabila karakter saat ini slash, maka karakter dilompati dan index kunci direset. 

```c
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
```
Cara kerja fungsi `decryptViginere` mirip dengan `encryptViginere`, hanya saja hurufnya digeser ke arah kebalikannya. Artinya, karena di enkripsi nilai hurufnya ditambah, di dekripsi nilai hurufnya dikurangi. Fungsi `decryptViginere` dan `encryptViginere` akan digunakan untuk soal 2b.

```c
void write_log2(char method[], char oldname[], char newname[])
{
    FILE *fp = fopen("no2.log", "a+");

    if (strcmp(method, "mkdir") == 0)
        fprintf(fp, "%s %s\n", method, newname);
    else if (strcmp(method, "rename") == 0)
        fprintf(fp, "%s %s to %s\n", method, oldname, newname);
    fclose(fp);
}
```
Kemudian, soal untuk menuliskan log bila melakukan `mkdir` dan `rename` juga dibuat. Fungsi `write_log2` akan menerima argumen method, nama lama, dan nama baru. Kemudian argumen tadi akan ditulis ke file `no2.log`. Fungsi ini selain digunakan untuk soal 2d, tapi juga 2a dan 2b.

```c
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
```
Fungsi `cek_log2` akan mengecek apakah sebuah folder dibuat dengan rename atau mkdir dengan cara membaca file `no2.log`. File tersebut akan dibaca baris per baris, kemudian dibandingkan 6 karakter pertamanya dengan `rename`, jika tidak sama maka akan melanjutkan ke baris berikutnya. Kemudian fungsi akan mengecek apakah path hasil rename mengandung `RX_`, jika tidak maka juga akan melanjutkan ke baris berikutnya. Terakhir, argumen yang diterima akan dibandingkan dengan nama folder dari log, bila sama maka akan fungsi mengembalikan nilai 1 yang artinya folder dibuat dengan rename. Jika file `no2.log` tidak ada atau sampai baris akhir tidak ditemukan nama folder yang sama, maka fungsi mengembalikan nilai 0. Fungsi ini akan digunakan di soal 2a dan 2b.

```c
char *get_real_path(const char *path)
{
    ...

    char *rx = strstr(real_path, "/RX_");
    
	...

    else if (rx)
    {
        int index = strlen(real_path) - strlen(rx) + 1;
        while (index < strlen(real_path))
        {

            if (real_path[index] == '/')
            {
                if (cek_log2(real_path))
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
    
	...
}
```
Di fungsi `get_real_path` yang juga digunakan di soal 1, ditambahkan pengecekan apabila `path` mengandung substring `/RX_`. Bila iya, maka akan kembali dicek dengan fungsi `cek_log2` apa path tersebut dibuat dengan rename atau mkdir, bila iya maka akan dilakukan dekripsi menggunakan vigenere dan atbash. Bila tidak, maka akan menggunakan rot13 dan atbash. Terakhir, `dirpath` dan `real_path` disambungkan di `fpath`.

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    ...

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

	...

}
```
Di fungsi `xmp_readdir` juga ditambahkan pengecekan apabila `path` mengandung substring `/RX_` dan juga dicek dengan fungsi `cek_log2`. Bila dibuat dengan rename, maka akan dienkripsi menggunakan atbash dan vigenere. Bila bukan, maka dienkripsi dengan rot13 dan atbash. Setelah itu `buf` akan diisi dengan `name`.

Dengan kedua fungsi diatas, selain merupakan penyelesaian 2a dan 2b. 2c juga dapat terselesaikan karena apabila di `path` tidak ada `RX_`, maka tidak akan dilakukan enkripsi ataupun dekripsi.

```c
static int xmp_rename(const char *from, const char *to)
{
    ...

    else if (strstr(to, "RX_"))
        write_log2("rename", ffrom, fto);

    ...
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    ...

    else if (strstr(fpath, "RX_"))
        write_log2("mkdir", NULL, fpath);

    ...
}
```
Terakhir untuk 2d, di fungsi `xmp_rename` dan `xmp_mkdir` juga dilakukan pengecekan apakah `to` di rename atau `fpath` di mkdir mengandung `RX_`. Jika iya, maka akan memanggil fungsi `write_log2` untuk menuliskan log.

Untuk soal 2e tidak dikerjakan.

![file asli](https://github.com/sisop-E03/soal-shift-sisop-modul-4-E03-2021/blob/master/images/no3-1.png)
Folder dan file di dalam folder RX_coba di direktori Downloads

![file mount](https://github.com/sisop-E03/soal-shift-sisop-modul-4-E03-2021/blob/master/images/no3-2.png)
Folder dan file di dalam folder RX_coba di folder mount program

### Kendala
1. Hanya sedikit example untuk FUSE File System
2. Tidak dibolehkan menggunakan exec() sehingga tidak bisa mengerjakan soal 2e dengan `split`

## NO 3
### Penyelesaian

## NO 4
Untuk memudahkan dalam memonitor kegiatan pada filesystem mereka Sin dan Sei membuat sebuah log system dengan spesifikasi sebagai berikut.
- a. Log system yang akan terbentuk bernama “SinSeiFS.log” pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.
- b. Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.
- c. Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.
- d. Sisanya, akan dicatat pada level INFO.
- e. Format untuk logging yaitu:


[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]

Level : Level logging, dd : 2 digit tanggal, mm : 2 digit bulan, yyyy : 4 digit tahun, HH : 2 digit jam (format 24 Jam),MM : 2 digit menit, SS : 2 digit detik, CMD : System Call yang terpanggil, DESC : informasi dan parameter tambahan

INFO::28052021-10:00:00:CREATE::/test.txt
INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt

### Penyelesaian
 untuk lokasi file SinSeiFS.log sendiri berada di (/home/[user]/SinSeiFS.log) `` static const char *LOG = "/home/rauf/SinSeiFS.log"; ``
 lalu untuk level sendiri berdasarkan soal
 level warning--> untuk rmdir dan unlink
 level info: sisanya
 
 log level dibuat varibel global:
 ``char *leveli ="INFO";
char *levelw ="WARNING";``

lalu `void Levellog` berisi format log yang akan masukkan ke dalam file di SinSeiFS.log
waktu yang digunakan adalah waktu yang terdapat dalam komputer kita ``struct tm tm = *localtime(&t)``. isi void Levellog adala sebagi berikut:
```c
void Levellog(char *level, char* desc, const char* path) 
{
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
```

dan contoh memanggil lognya :
- untuk level info-->
```c
//bikin lognya
    Levellog(leveli, "CD", path);
```

- untuk level warning -->
```c
//untuk di log no4
    Levellog(levelw, "RMDIR", fpath);
```


![alt text](https://github.com/sisop-E03/soal-shift-sisop-modul-4-E03-2021/blob/master/images/no%204.jpg)
