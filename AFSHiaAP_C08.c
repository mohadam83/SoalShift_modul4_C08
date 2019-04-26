#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

char daftar_huruf[2048] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
static const char *lokasi_asal = "/home/adam/shift4";

void Buka_sandi(char *nama);
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

static int xmp_getattr(const char *path, struct stat *stbuf){
	char fs[1000];
	sprintf(fs,"%s",path);
	Sandi(fs);

	char folder[1000];
	sprintf(folder,"%s%s",lokasi_asal, fs);

	int cek;
	cek = lstat(folder, stbuf);

	if(cek == -1){
		return -errno;
	} 

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char directory_readdir[1000], fs_readdir[1000];
	sprintf(fs_readdir, "%s",path);
	Sandi(fs_readdir);
	
	sprintf(directory_readdir, "%s%s", lokasi_asal, fs_readdir);
	
	DIR *directory;
	struct dirent *berkas;

	(void) offset;
	(void) fi;

	directory = opendir(directory_readdir);
	if (directory != NULL){
		berkas = readdir(directory);
		while (berkas != NULL) {
			struct stat st;
			memset(&st, 0, sizeof(st));
			st.st_ino = de->d_ino;
			st.st_mode = de->d_type << 12;
			
			char temp[1000];
			for(int i = 0; i < 1000; i++){
				temp[i] = 0;
			}
			strcpy(temp, de->d_name);
	
			Buka_sandi(temp);

			if (filler(buf, temp, &st, 0)) break;
		}
		closedir(directory);
		return 0;
	} 
	else{
		return -errno;
	}		
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi){
	
	char fs_read[1000], directory_read[1000];;
	sprintf(fs_read, "%s", path);
	Sandi(fs_read);
	
	sprintf(directory_read,"%s%s", lokasi_asal, fs_read);

	int cek, cek_fd;
	(void) fi;
	cek_fd = open(directory_read, O_RDONLY);
	if(cek_fd == -1) return -errno;
	
	cek = pread(cek_fd, buf, size, offset);
	if(cek == -1) cek = -errno;
	
	close(cek_fd);
	return cek;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}

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


