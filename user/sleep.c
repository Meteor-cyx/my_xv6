#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]){
    //参数不是2个（包括命令sleep），用法不对，提醒一下
    if(argc != 2){
        fprintf(2,"usage:sleep ticks\n");//第一个参数是文件描述符，往标准错误输出错误信息（file descriptor 2）
        exit(1);
    }
    //秒数不能小于0
    int time = atoi(argv[1]);
    if(time<0){
        fprintf(2,"warn:ticks should >=0\n");
        exit(1);
    }

    sleep(time);
    exit(0);
}