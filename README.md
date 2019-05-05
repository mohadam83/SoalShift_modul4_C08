# SoalShift_modul4_C08
## Soal 1
Semua nama file dan folder harus terenkripsi 
Enkripsi yang Atta inginkan sangat sederhana, yaitu Caesar cipher. Namun, Kusuma mengatakan enkripsi tersebut sangat mudah dipecahkan. Dia menyarankan untuk character list diekspansi,tidak hanya alfabet, dan diacak. Berikut character list yang dipakai:

qE1~ YMUR2"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\8s;g<{3.u*W-0

Misalkan ada file bernama “halo” di dalam folder “INI_FOLDER”, dan key yang dipakai adalah 17, maka:
“INI*_*FOLDER/halo” saat belum di-mount maka akan bernama “n,nsbZ]wio/QBE#”, saat telah di-mount maka akan otomatis terdekripsi kembali menjadi “INI_FOLDER/halo” .
Perhatian: Karakter ‘/’ adalah karakter ilegal dalam penamaan file atau folder dalam *NIX, maka dari itu dapat diabaikan.

*Penyelesaian*
```
void Sandi(char *nama)
{
	if(!strcmp(nama,"..")) return;
	if(!strcmp(nama,".")) return;
	int i = 0;
	while(i < strlen(nama)){
		for(int j = 0; j < 94; j++){
			if(daftar_huruf[j] == nama[i]){
				int indeks = (j + 17) % 94;
				if(indeks < 0) indeks += 94;
				nama[i] = daftar_huruf[indeks];
				j = 100;
			}
		}
		i++;
	}
}
```
Fungsi *Sandi* di atas digunakan untuk mengenkripsi nama file *nama* sesuai permintaan di soal dengan *key* yang digunakan adalah 17, sehingga ketika ingin mengubah huruf dari nama file di indeks ke x maka kita harus mengecek di kumpulan huruf yang berjumlah 94 sesuai di soal yang memiliki karakter yang sama kemudian indeks huruf tersebut (y) ditambah 17, dan juga setelah itu di modulo dengan 94 agar ketika y + 17 jumlahnya melebihi 94 maka akan kembali lagi ke indeks di awal.

```
void Buka_sandi(char *nama)
{
	if(!strcmp(nama,"..")) return;
	if(!strcmp(nama,".")) return;
	int i = 0;
	while(i < strlen(nama)){
		for(int j = 0; j < 94; j++){
			if(daftar_huruf[j] == nama[i]){
				int indeks = (j - 17) % 94;
				if(indeks < 0) indeks += 94;
				nama[i] = daftar_huruf[indeks];
				j = 100;
			}
		}
		i++;
	}
}
```
Fungsi *Buka_Sandi* di atas digunakan untuk mendekripsi nama file *nama* yang telah di enkripsi sebelumnya sesuai permintaan di soal dengan *key* yang digunakan adalah 17, sehingga ketika ingin mengubah huruf dari nama file yang telah di enkripsi pada indeks ke x maka kita harus mengecek di kumpulan huruf yang berjumlah 94 sesuai di soal yang memiliki karakter yang sama kemudian indeks huruf tersebut (y) dikurangi 17, dan juga setelah itu di modulo dengan 94 agar ketika y - 17 jumlahnya kurang dari 0 maka akan kembali lagi ke indeks di akhir. Tetapi, hasil dari modulo tersebut ada kemungkinan negatif sehingga ketika hasilnya negatif maka harus ditambah dengan 94.


## Soal 2
Semua file video yang tersimpan secara terpecah-pecah (splitted) harus secara otomatis tergabung (joined) dan diletakkan dalam folder “Videos”
Urutan operasi dari kebutuhan ini adalah:
a. Tepat saat sebelum file system di-mount
   i. Secara otomatis folder “Videos” terbuat di root directory file system
   ii. Misal ada sekumpulan file pecahan video bernama “computer.mkv.000”, “computer.mkv.001”, “computer.mkv.002”, dst. Maka secara otomatis file pecahan tersebut akan di-join menjadi file video “computer.mkv”
Untuk mempermudah kalian, dipastikan hanya video file saja yang terpecah menjadi beberapa file.
   iii. Karena mungkin file video sangat banyak sehingga mungkin saja saat menggabungkan file video, file system akan membutuhkan waktu yang lama untuk sukses ter-mount. Maka pastikan saat akan menggabungkan file pecahan video, file system akan membuat 1 thread/proses(fork) baru yang dikhususkan untuk menggabungkan file video tersebut
   iv. Pindahkan seluruh file video yang sudah ter-join ke dalam folder “Videos”
   v. Jangan tampilkan file pecahan di direktori manapun
b. Tepat saat file system akan di-unmount
   i. Hapus semua file video yang berada di folder “Videos”, tapi jangan hapus file pecahan yang terdapat di root directory file system
   ii. Hapus folder “Videos” 


## Soal 3
Sebelum diterapkannya file system ini, Atta pernah diserang oleh hacker LAPTOP*_*RUSAK yang menanamkan user bernama “chipset” dan “ic_controller” serta group “rusak” yang tidak bisa dihapus. Karena paranoid, Atta menerapkan aturan pada file system ini untuk menghapus “file bahaya” yang memiliki spesifikasi:
    • Owner Name 	: ‘chipset’ atau ‘ic_controller’
    • Group Name	: ‘rusak’
    • Tidak dapat dibaca
Jika ditemukan file dengan spesifikasi tersebut ketika membuka direktori, Atta akan menyimpan nama file, group ID, owner ID, dan waktu terakhir diakses dalam file “filemiris.txt” (format waktu bebas, namun harus memiliki jam menit detik dan tanggal) lalu menghapus “file bahaya” tersebut untuk mencegah serangan lanjutan dari LAPTOP_RUSAK.

*Penyelesaian*
Untuk menyelesaikan soal ini, maka perlu ditambahkan beberapa syntax tambahan pada fungsi readdir seperti dibawah ini.

```
	struct passwd *p = getpwuid(st.st_uid);
	struct group *g = getgrgid(st.st_gid);
	//time_t wktu = time(NULL);
	struct tm *waktu = localtime(&st.st_atime);
	//st.st_atime

	//printf("%s %s %s\n",dpath,p->pw_name,g->gr_name);
if( p != NULL && g != NULL )
{
	//comparing username and group (YES!!!!)
	if( (strcmp(p->pw_name, "chipset") == 0 || strcmp(p->pw_name, "ic_controller") == 0) && strcmp(g->gr_name, "rusak") == 0 )
	{
		//check if file readable
		if(fopen(show, "r") == NULL)
		{
			//error access??
			if(errno == EACCES)
			{
				//it's time for record
				FILE *teks;
				int tahun = waktu->tm_year + 1900;
				char sumber[1000];
				char ketikan[2064];
				Buka_sandi(dpath);
				sprintf(sumber, "%s/V[EOr[c[Y`HDH", lokasi_asal);
				sprintf(ketikan, "%s %d %d | waktu : %02d:%02d:%02d [%02d %02d %04d]\n", dpath, st.st_uid, st.st_gid, waktu->tm_hour, waktu->tm_min, waktu->tm_sec, waktu->tm_mday, waktu->tm_mon, tahun);
				teks = fopen(sumber, "a");
				fputs(ketikan, teks);
				fclose(teks);

				remove(show);
			}
		}	

		//no.1
		else 
		{
			Buka_sandi(dpath);
			res = (filler(buf, dpath, &st, 0));
			if(res!=0) break;
		}
	}

	//no.1
	else 
	{
		Buka_sandi(dpath);
		res = (filler(buf, dpath, &st, 0));
		if(res!=0) break;
	}
}
```
Setelah itu, dilakukan pengecekan untuk masing-masing file di folder tersebut apakah terdapat file yang memiliki ketentuan seperti di bawah ini.
```
Owner Name	: ‘chipset’ atau ‘ic_controller’
Group Name	: ‘rusak’
Tidak dapat dibaca
```
Kemudian, untuk menyimpan nama file, group id dan lain sebagainya sebagaimana ditentukan di soal, maka kita menggunakan *struct stat* yang mana data-datanya tersebut akan disimpan di file *filemiris.txt*. Terakhir menghapus file yang memiliki kriteria seperti di atas. 

## Soal 4
Pada folder YOUTUBER, setiap membuat folder permission foldernya akan otomatis menjadi 750. Juga ketika membuat file permissionnya akan otomatis menjadi 640 dan ekstensi filenya akan bertambah “.iz1”. File berekstensi “.iz1” tidak bisa diubah permissionnya dan memunculkan error bertuliskan “File ekstensi iz1 tidak boleh diubah permissionnya.”


## Soal 5
Ketika mengedit suatu file dan melakukan save, maka akan terbuat folder baru bernama Backup kemudian hasil dari save tersebut akan disimpan pada backup dengan nama *namafile_[timestamp].ekstensi*. Dan ketika file asli dihapus, maka akan dibuat folder bernama RecycleBin, kemudian file yang dihapus beserta semua backup dari file yang dihapus tersebut (jika ada) di zip dengan nama namafile_deleted_[timestamp].zip dan ditaruh kedalam folder RecycleBin (file asli dan backup terhapus). Dengan format [timestamp] adalah yyyy-MM-dd_HH:mm:ss
