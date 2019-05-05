#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#define TRUE 1
#define FALSE 0

static const char *lokasi_asal = "/home/adam/shift4";
char daftar_huruf[1024] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
char epath[1000], dpath[1000];
void Buka_sandi(char *nama);

void Sandi(char *nama)
{
	if(!strcmp(nama,"..")) return;
	if(!strcmp(nama,".")) return;
	int i = 0;
	while(i < strlen(nama)){
		for(int j = 0; j < 94; j++){
			if(daftar_huruf[j] == nama[i]){
				int indeks = (j + 31) % 94;
				if(indeks < 0) indeks += 94;
				nama[i] = daftar_huruf[indeks];
				j = 100;
			}
		}
		i++;
	}
}


static int xmp_getattr(const char *path, struct stat *stbuf)
{
	char fs[1000], folder[1000];
	sprintf(fs, "%s",path);
	Sandi(fs);
	sprintf(folder,"%s%s", lokasi_asal, fs);
	
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
	char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=lokasi_asal;
		sprintf(fpath,"%s",path);
	}
	else
	{
		//no.1
		strcpy(epath, path);
		if(strcmp(epath, ".") != 0 && strcmp(epath, "..") != 0)Sandi(epath);
		sprintf(fpath, "%s%s", lokasi_asal, epath);
	}
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL)
	{
		struct stat st;
		memset(&st, 0, sizeof(st));

		//show
		char show[2000];
		snprintf(show, 2000, "%s/%s", fpath, de->d_name);
		//de->d_name
		stat(show, &st);

		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;

		//no.1 dan 3
		strcpy(dpath, de->d_name);
		//printf("directory : %s\n",show);

		if(strcmp(dpath, ".") != 0 && strcmp(dpath, "..") != 0)
		{
			//no.3
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
		}

		
	}

	closedir(dp);
return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int res,fd;
	char fs[1000], folder[1000];
	sprintf(fs,"%s",path);
	Sandi(fs);
	sprintf(folder,"%s%s",lokasi_asal,fs);

	(void) fi;
	fd=open(folder, O_RDONLY);
	res=pread(fd, buf, size, offset);
	close(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int res,fd;
	char fs[1000], folder[1000];
	sprintf(fs,"%s",path);
	Sandi(fs);
	sprintf(folder,"%s%s",lokasi_asal,fs);

	(void) fi;
	fd=open(folder, O_WRONLY);
	res=pwrite(fd, buf, size, offset);
	close(fd);
	return res;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char fs[1000], folder[1000];
	sprintf(fs,"%s",path);
	Sandi(fs);
	sprintf(folder,"%s%s",lokasi_asal,fs);

	res=open(folder, fi->flags);
	
	close(res);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	char fs[1000], folder[1000];
	sprintf(fs,"%s",path);
	Sandi(fs);
	sprintf(folder,"%s%s",lokasi_asal,fs);
	int res;
	if (S_ISREG(mode)) {
		res = open(folder, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(folder, mode);
	else
		res = mknod(folder, mode, rdev);
		
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	char fs[1000], folder[1000];
	sprintf(fs,"%s",path);
	Sandi(fs);
	sprintf(folder,"%s%s",lokasi_asal,fs);
	lchown(folder, uid, gid);
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	char fpath[1000];
	char sementara[1000];
	sprintf(sementara,"%s",path);

	Sandi(sementara);

	sprintf(fpath, "%s%s",lokasi_asal,sementara);

	int res;
	res = mkdir(fpath, mode);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {

	(void) fi;

	int res;

	char fpath[1000];
	char sementara[1000];
	sprintf(sementara,"%s",path);

	Sandi(sementara);

	sprintf(fpath, "%s%s",lokasi_asal,sementara);
	//

	res = creat(fpath, mode);
	if(res == -1) return -errno;
	
	close(res);

	return 0;
}


static int xmp_utimeandstart(const char *path, const struct timespec ts[2])
{
	struct timeval timevalue[2];
	char fs[1000], folder[1000];
	sprintf(fs,"%s",path);
	Sandi(fs);
	sprintf(folder,"%s%s",lokasi_asal,fs);

	timevalue[0].tv_sec = ts[0].tv_sec;
	timevalue[0].tv_usec = ts[0].tv_nsec / 1000;
	timevalue[1].tv_sec = ts[1].tv_sec;
	timevalue[1].tv_usec = ts[1].tv_nsec / 1000;

	utimes(folder, timevalue);

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
    //
    char fpath[1000];
	char sementara[1000];
    sprintf(sementara,"%s",path);

    Sandi(sementara);

	sprintf(fpath, "%s%s",lokasi_asal,sementara);
    //

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
	int res;
    //
    char fpath[1000];
	char sementara[1000];
    sprintf(sementara,"%s",path);

    Sandi(sementara);

	sprintf(fpath, "%s%s",lokasi_asal,sementara);
    //

	res = unlink(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.write		= xmp_write,
	.open		= xmp_open,
	.mkdir		= xmp_mkdir,
	.mknod		= xmp_mknod,
	.chown		= xmp_chown,
	.create 	= xmp_create,
	.utimens	= xmp_utimeandstart,
	.unlink		= xmp_unlink,
	.truncate 	= xmp_truncate,
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
				int indeks = (j - 31) % 94;
				if(indeks < 0) indeks += 94;
				nama[i] = daftar_huruf[indeks];
				j = 100;
			}
		}
		i++;
	}
}

