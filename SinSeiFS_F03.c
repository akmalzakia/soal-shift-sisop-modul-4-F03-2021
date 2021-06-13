#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
bool isAZ = false;
static const char *dirpath = "/home/zaki/Downloads";


char* atBash_cipher(char* string){
    char res[strlen(string) + 1];

    strcpy(res,string);
    
    for(int i = 0; i < strlen(res); i++){
        if(res[i] >= 'A' && res[i] <= 'Z'){
            res[i] = 'A' + 'Z' - res[i];
        }
        else if(res[i] >= 'a' && res[i] <= 'z'){
            res[i] = 'a' + 'z' - res[i];
        }

    }
    char* ret = res;
    return ret;
}

char* check_Path(char* path){
    char fpath[1000];
    memset(fpath,0,sizeof(fpath));
    isAZ = false;
    char *ret;

    if(strcmp(path, "/") != 0){
        ret = strstr(path,"/AtoZ_");
        if(ret){
            isAZ = true;
            ret++;
        }
    }

    if(strcmp(path,"/") == 0){
        path = dirpath;
        sprintf(fpath,"%s",path);
    }
    else if(isAZ){
        char filePath[1000];
        memset(filePath,0,sizeof(filePath));
        strncpy(filePath,path,strlen(path) - strlen(ret));

        char temp[1000];
        char temp2[1000];
        char *token;
        char *savePtr;

        strcpy(temp,ret);
        token  = strtok_r(temp,"/",savePtr);

        int i = 0;
        while(token){
            memset(temp2,0,sizeof(temp2));
            if(i == 0){
                strcat(filePath,token);
                i++;
                continue;
            }

            char checkType[1000];
            strcpy(checkType,filePath);
            strcat(checkType,"/");
            strcat(filePath,"/");
            strcat(checkType,token);

            if(strlen(checkType) == strlen(path)){
                char folderPath[1024];
                sprintf(folderPath,"%s%s%s",dirpath,filePath,token);

                DIR *dp = opendir(folderPath);
                if(!dp){
                    char* dot;
                    dot = strchr(token,'.');

                    char fileName[1024];
                    if(dot){
                        strncpy(fileName,token,strlen(token) - strlen(dot));
                        strcpy(fileName,atBash_cipher(fileName));
                        strcat(fileName,dot);
                    }
                    else{
                        strcpy(fileName,token);
                        strcpy(fileName,atBash_cipher(fileName));
                    }
                    strcat(filePath,fileName);
                }
                else{
                    char folderName[1024];
                    strcpy(folderName, token);
                    strcpy(folderName,atBash_cipher(folderName));
                    strcat(filePath,folderName);
                }
            }
            else{
                char folderName[1024];
                strcpy(folderName, token);
                strcpy(folderName,atBash_cipher(folderName));
                strcat(filePath,folderName);
            }

            token = strtok_r(NULL,"/",savePtr);
            
        }
        sprintf(fpath,"%s%s",dirpath,filePath);
    }
    else{
        sprintf(fpath,"%s%s",dirpath,path);
    }

    char* ret_path = fpath;
    return ret_path;
}



static  int  xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];
    strcpy(fpath,check_Path(path));

    res = lstat(fpath, stbuf);

    if (res == -1) return -errno;

    return 0;
}



static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    isAZ = false;
    strcpy(fpath,check_Path(path));

    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(fpath);

    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if(strcmp(de->d_name,".") == 0 || strcmp(de->d_name,"..") == 0){
            res = filler(buf,de->d_name,&st,0);
        }
        else if(isAZ){
            if(de->d_type & DT_DIR){
                char temp[1000];
                strcpy(temp,de->d_name);
                strcpy(temp,atBash_cipher(temp));
                res = filler(buf,temp,&st,0);
            }
            else{
                char *dot;
                dot = strchr(de->d_name,'.');

                char fileName[1024];
                if(dot){
                    strncpy(fileName,de->d_name,strlen(de->d_name) - strlen(dot));
                    strcpy(fileName,atBash_cipher(fileName));
                    strcat(fileName,dot);
                }   
                else{
                    strcpy(fileName,de->d_name);
                    strcpy(fileName,atBash_cipher(fileName));
                }

                res = filler(buf,fileName,&st,0);
            }
        }
        else{
            res = filler(buf,de->d_name,&st,0);
        }

        if(res != 0){
            break;
        }

    }

    closedir(dp);

    return 0;
}



static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    strcpy(fpath,check_Path(path));

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

static int xmp_mkdir(const char *path, mode_t mode){
    char fpath[1000];
    char *slash = strchr(path,'/');
    if(strstr(slash,"/AtoZ_")){
        char temp[1000];
        sprintf(temp,"%s%s",dirpath,path);
    }

    int res;
    strcpy(fpath,check_Path(fpath));
	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    char ffrom[1000];
    char fto[1000];


    strcpy(ffrom,check_Path(from));
    strcpy(fto,check_Path(to));

	int res;

	res = rename(ffrom, fto);
	if (res == -1)
		return -errno;

	return 0;
}



static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
    .mkdir = xmp_mkdir, 
    .rename = xmp_rename,
};



int  main(int  argc, char *argv[])
{
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}