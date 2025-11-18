#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int left_fd);  // 提前声明，等会儿去实现它


int main(int argc, char *argv[]){
    int fd[2];
    if(pipe(fd)<0){
        fprintf(2,"main: pipe failed\n");
        exit(1);
    }
    int pid = fork();
    if(pid<0){
        fprintf(2,"main: fork failed\n");
        exit(1);
    }
    if(pid > 0){//*父进程*
        close(fd[0]);//不读，只写
        int num = 2;
        while (num<=35){
            if(4!=write(fd[1],&num,4)){
                fprintf(2,"main: write failed\n");
                exit(1);
            }
            num++;
        }
        close(fd[1]);
        wait(0);
        exit(0);
    }
    else{//*子进程*
        close(fd[1]); //不写
        //read并处理
        sieve(fd[0]);
    }

    exit(0);
}

void sieve(int left_fd){
    int prime;
    int red_res = read(left_fd,&prime,4);//从左管道读4个字节，也就是一个int
    if(red_res == 0){//读完了
        exit(0);
    }
    else if(red_res<4){//不足4，且不为0，出问题了
        fprintf(2,"%d: read failed\n",getpid());
        exit(1);
    }
    //打印素数
    printf("prime %d\n",prime);
    //建立管道
    int fd[2];
    if(pipe(fd)<0){
        fprintf(2,"%d: pipe failed\n",getpid());
        exit(1);
    }
    //fork
    int pid = fork();
    if(pid<0){
        fprintf(2,"%d: fork failed\n",getpid());
        exit(1);
    }
    if(pid>0){
        close(fd[0]);
        int rcv;//从左管道读到的num

        //**过滤**，不是重新发
        while(read(left_fd,&rcv,4)==4){//rcv是左管道传过来的
            if(rcv%prime!=0){
                if(4!=write(fd[1],&rcv,4)){
                    fprintf(2,"%d: write failed\n",getpid());
                    exit(1);
                }
            }            
        }
        close(left_fd);//左管道读完了
        close(fd[1]);//往右发的写完了
        wait(0);
        exit(0);
    }
    else{
        //子
        close(left_fd); //要关，为什么？？
        close(fd[1]);
        sieve(fd[0]);
    }
}