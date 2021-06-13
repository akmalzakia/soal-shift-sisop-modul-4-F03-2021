# soal-shift-sisop-modul-3-F03-2021

Nama Anggota | NRP
------------------- | --------------		
Dias Tri Kurniasari | 05111940000035
Akmal Zaki Asmara | 05111940000154
M. Fikri Sandi Pratama | 05111940000195

## List of Contents :
- [No 1](#no-1)
  
## No 1
Pada soal no 1, diminta untuk membuat suatu file system yang dapat melakukan encode pada direktori yang dibuat dengan awalan "AtoZ_", proses encode akan terjadi pada seluruh isi direktori yang memiliki awalan "AtoZ_", dan apabila direktori direname menjadi tidak ter-encode, isi direktori akan terdecode semua. Untuk melakukannya diperlukan suatu fungsi yang dapat mengecek nama file di direktori awal dan mengirimkannya ke dalam fungsi utama file system

```cpp
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

```

Fungsi diatas akan dipakai disetiap fungsi file system untuk path awal. Setelah itu diminta juga untuk membuat log pada setiap rename dan pembuatan direktori. Log akan ditulis menggunakan fungsi berikut

```cpp
void writeLog(char* old, char *new){
    FILE* file = fopen("encode.log","a");

    char str[1000];
    sprintf(str,"%s --> %s",old,new);
    fprintf(file,"%s\n",str);
    fclose(file);
}
```

Fungsi ini akan dipanggil setiap kali rename dan mkdir dijalankan. Setelah itu proses rekursi dalam encoding dilakukan menggunakan while loop setiap kali readdir yang akan melakukan enkripsi pada folder dengan awalan 'AtoZ_'.

```cpp
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
```

