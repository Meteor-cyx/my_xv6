#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
void find(char* path, char* name);
char* basename(char *path);

int 
main(int argc, char* argv[]){
    if(argc != 3){
        fprintf(2,"usage: find path name\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}

void 
find(char* path, char* name){
    char buf[512], *p;
    int fd;    //path的文件描述符
    struct dirent de;  //目录项
    struct stat st;    //文件信息，根据fd获得

    if((fd = open(path, 0)) < 0){ //获得该文件的fd
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){  //根据fd 把刚刚打开的文件的信息写入st，应该是个文件夹
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type == T_FILE) {//是文件，直接打印
        if (strcmp(basename(path), name) == 0) {
            printf("%s\n", path);
        }
        close(fd);
        return;
    }

    if(st.type!=T_DIR){//也不是文件夹，静默忽略
        close(fd);
        return;
    }

    if(strlen(path)+1+DIRSIZ+1>sizeof(buf)){//卫语句，确保本次递归路径不溢出
        fprintf(2, "path is too long\n");
        close(fd);
        return;
    }
    
    strcpy(buf,path);
    p = buf+strlen(buf);//位于'\0'处
    *p++ = '/';  //替换为'/'并自增

    //对于所有目录项
    while(read(fd,&de,sizeof(de))==sizeof(de)){
        if(de.inum == 0){
            continue;
        }
        //是.或..不需要获取文件状态信息了
        if(strcmp(".",de.name)==0||strcmp("..",de.name)==0){
           continue; 
        }

        //获取信息，判断类型
        memmove(p,de.name,sizeof(de.name));
        p[DIRSIZ] = 0;
        if(stat(buf,&st)<0){
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;//本轮while跳过
        }
        if(st.type == T_DIR){
            //还是文件夹(除了.和..)
            find(buf,name);
        }
        else if(st.type == T_FILE){
            if(strcmp(basename(buf),name)==0){
                fprintf(1,"%s\n",buf);
            }
        }
    }
    close(fd);
}

char *
basename(char *path)
{
    char* p = path + strlen(path);
    while(p >= path && *p != '/'){
        p--;
    }
    p++;
    return p;
}