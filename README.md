# soal-shift-sisop-modul-4-F04-2021

# Anggota Kelompok
- **Thomas Dwi Awaka** (05111940000021)
- **Muhammad Arifiansyah** (05111940000027)
- **Muhammad Rizqullah Akbar** (05111940000178)

# Pembahasan Soal
## Soal 1
Di suatu jurusan, terdapat admin lab baru yang super duper gabut, ia bernama Sin. Sin baru menjadi admin di lab tersebut selama 1 bulan. Selama sebulan tersebut ia bertemu orang-orang hebat di lab tersebut, salah satunya yaitu Sei. Sei dan Sin akhirnya berteman baik. Karena belakangan ini sedang ramai tentang kasus keamanan data, mereka berniat membuat filesystem dengan metode encode yang mutakhir. 
### Soal 1a
Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
### Soal 1b
Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
### Soal 1c
Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.
### Soal 1d
Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]
### Soal 1e
Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)
```bash
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
```
* ```enc[i] = 'Z' - (enc[i] - 'A') dan enc[i] = 'z' - (enc[i] - 'a');``` berguna untuk meng-chyper teks sehingga akan terencrypt secara terbalik , misal A menjadi Z, B menjadi Y, a menjadi z, b menjadi y, dan seterusnya.
* ```if (enc[i] == '/') continue;``` untuk mengabaikan penamaan file ‘/’ sesuai yang di mention di soal.
*  ```void encrypt(char* enc, int length)``` untuk mengenkripsi nama file / direktori. Misalkan apabila mkdir seperti direktori/AtoZ_test, maka yg di enkripsi hanya string AtoZ_test saja, direktori/ tidak akan terenkripsi. Begitu juga dengan ekstensi dari file. Untuk memenuhi persyartan diatas menggunakan

* ```void decrypt(char* enc, int length)``` untuk mengenkripsi nama file / direktori. Misalkan apabila mkdir direktori/AtoZ_test, maka yg di enkripsi hanya string AtoZ_test saja, direktori/ tidak akan di enkripsi. Begitu juga dengan ekstensi dari file. Untuk memenuhi persyartan diatas menggunakan

### Screenshot

### Kendala
Sempat tidak bisa membaca isi dari sebuah file, ternyata karena salah memberikan variabel




## Soal 4
### Soal 4a
Log system yang akan terbentuk bernama “SinSeiFS.log” pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.
### Soal 4b
Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.
### Soal 4c
Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.
### Soal 4d
Sisanya, akan dicatat pada level INFO.
### Soal 4e
Format untuk logging yaitu: [Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]
