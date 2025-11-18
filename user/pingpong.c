#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

int main(int argc, char* argv[]){
    int fd_c2p[2];//child_to_parent
    int fd_p2c[2];//parent_to_child
    pipe(fd_c2p);
    pipe(fd_p2c);

    char buff[8];//用户缓冲区，接受read的结果

    int pid = fork();
    //  | 
    //  | ----|          开始双进程
    //  |     |
    //  V     V
    if(pid > 0){//父进程
        close(fd_p2c[RD]);//关闭 父->子 的读
        close(fd_c2p[WR]);//关闭 子->父 的写
        if(4!=write(fd_p2c[WR],"ping",4)){
            fprintf(2,"父进程:write_error");
            exit(1);
        }
        if(4!=read(fd_c2p[RD],buff,4)){
            fprintf(2,"父进程:read_error");
            exit(1);
        }
        printf("%d: received %s\n",getpid(),buff);
        
    }
    else if(pid == 0){//子进程
        close(fd_p2c[WR]);//关闭 父->子 的写
        close(fd_c2p[RD]);//关闭 子->父 的读

        if(4!=read(fd_p2c[RD],buff,4)){
            fprintf(2,"子进程:read_error");
            exit(1);
        }
        printf("%d: received %s\n",getpid(),buff);
        if(4!=write(fd_c2p[WR],"pong",4)){
            fprintf(2,"子进程:write_error");
            exit(1);
        }
    }
    else{//pid<0
        fprintf(2,"fork_error");
        close(fd_p2c[WR]);
        close(fd_p2c[RD]);
        close(fd_c2p[WR]);
        close(fd_c2p[RD]);
        exit(1);
    }

    exit(0);
}