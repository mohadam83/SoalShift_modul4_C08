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

static const char *lokasi_asal = "/home/salsha/modul4";
char daftar_huruf[1024] = "qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\\8s;g<{3.u*W-0";
void Buka_sandi(char *nama);
char folder[1000];

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
	char fs[1000], folder[1000];
	sprintf(fs,"%s",path);
	Sandi(fs);
	sprintf(folder,"%s%s",lokasi_asal, fs);
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(folder);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		struct stat tiga;
		FILE *yap;
		char name[4096];
		char haha[4096];
		memset(name,0,4096);
		memset(haha,0,4096);
		strcpy(name,de->d_name);
		sprintf(haha,"%s%s", folder, name);
		Buka_sandi(name);
		
		stat(haha,&tiga);
		struct passwd *pass;
		pass = getpwuid(tiga.st_uid);
    	struct group *grouptiga;
		grouptiga = getgrgid(tiga.st_gid);
		
		if((strcmp(pass->pw_name,"chipset") == 0 || strcmp(pass->pw_name,"ic_controller") == 0) && strcmp(grouptiga->gr_name,"rusak")==0)
		{
			if((tiga.st_mode & S_IRUSR) == 0 || (tiga.st_mode & S_IRGRP) == 0 || (tiga.st_mode & S_IROTH) == 0)
			{
				char isi[1024];
				char isifile[1024];
				char simpan[1024]="/filemiris.txt";
				char asli[1024]="/home/salsha/modul4";
				time_t waktu =time(NULL);
				Sandi(simpan);
				strcat(asli,simpan);
				yap=fopen(asli,"a+");
				strftime(isi, 20 ,"%Y-%m-%d %H:%M:%S", localtime(&waktu));
				snprintf(isifile,sizeof(isifile), "%s%s-%d-%d-%s",path,name,pass->pw_uid,grouptiga->gr_gid,isi);
				fprintf(yap,"%s",isifile);
				remove(haha);
				fclose(yap);
				chown(asli,1000,1000);
			}
		}
		else
		{
			memset(&st, 0, sizeof(st));
			st.st_ino = de->d_ino;
			st.st_mode = de->d_type << 12;
			if (filler(buf, name, &st, 0))
				break;			
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
	if(strstr(fpath, "/@ZA>AXio/") != NULL)
    {
    	res = mkdir(fpath, 0750);
    }
    else res = mkdir(fpath, mode);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
	(void) fi;

	int res, res2;

	char fpath[1000];
	char sementara[1000];
	char file_baru[1000];
	sprintf(sementara,"%s",path);

	Sandi(sementara);

	sprintf(fpath, "%s%s",lokasi_asal,sementara);

	if(strstr(fpath, "/@ZA>AXio/") != NULL)
    {
    	res = creat(fpath, 0640);
    }
    else res = creat(fpath, mode);

    if(res == -1)
    {
    	return -errno;
    }
	
	close(res);

	if(strstr(fpath, "/@ZA>AXio/") != NULL)
    {
    	strcpy(file_baru, fpath);
    	strcat(file_baru, "`[S%");
    	res2 = rename(fpath, file_baru);
    	if(res2==-1)
        {
                return -errno;
        }
    }

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

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];

    strcpy(folder, path);
    Sandi(folder);
    sprintf(fpath, "%s%s", lokasi_asal, folder);

    if(strstr(fpath, "/@ZA>AXio/") != NULL)
    {	
    	size_t ext = strlen(fpath), exts = strlen("`[S%");
    	if(ext >= exts && !strcmp(fpath + ext - exts, "`[S%"))
    	{
    		pid_t anak;

            anak = fork();
            if(anak == 0)
            {
                char *argv[5] = {"zenity", "--error", "--title=Error", "--text=File ekstensi iz1 tidak boleh diubah permisionnya", NULL};
                        execv("/usr/bin/zenity", argv);

                        return -errno;
            }
            return 0;
    	}
    	else res = chmod(fpath, mode);
    }
    else res = chmod(fpath, mode);
    
    if(res == -1) return -errno; 

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
	.truncate 	= xmp_truncate,
	.chmod		= xmp_chmod,
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
