#include "kernel/types.h"
#include "user/user.h"
#define MAXARG 32
int
main(int argc, char* argv[]){
    if(argc<2){
        fprintf(2,"usage:xargs cmd ...");
        exit(1);
    }
    int basec = argc - 1;//原始参数-1
    char* base[MAXARG];
    for(int i = 1; i < argc; i++){  //记录原始参数
        base[i-1] = argv[i];
    }
//用一个大buff全部接收
    char buf[512];
    char perc;
    int n = 0;
    while(read(0,&perc,1)==1){
        if(n < sizeof(buf) - 1){
            buf[n++] = perc;
        }
    }
    buf[n] = '\0';
//切分大buff
    char* p = buf;
    char* extrav[MAXARG];//记录这一行的额外参数
    int extrac = 0;   //额外参数的数目

    while(*p != '\0'){

        while(*p == ' ' || *p == '\n'){//跳过前面空白
            p++;
        }
        if(*p == '\0'){//直接到头了，啥也没有
            break;
        }

        //这是一个token的开始
        char* start = p;//备份起点
        while(*p&&*p != '\n'&&*p != ' '){//不是换行，不是空格，那就是参数
            p++;
        }
        if(*p == ' '){//一个参数结束
            *p++ = '\0';  //覆盖这一位，这样start就是到此结束的字符串
            extrav[extrac] = start; //记录
            extrac++; //
        }
        else if(*p == '\n'){//一行结束，该输出了
            *p++ = '\0';  //覆盖这一位，这样start就是到此结束的字符串
            extrav[extrac] = start; //记录
            extrac++; //
            //拿出我的原始参数base
            //和额外记录的extrav
            //按顺序拼
            char* cmd[MAXARG];
            for(int i = 0; i < basec; i++){
                cmd[i] = base[i];
            }
            for(int i = 0; i < extrac; i++){
                cmd[i+basec] = extrav[i];
            }
            cmd[basec+extrac] = 0;

            if(fork()==0){//子进程
                exec(base[0],cmd);
                fprintf(2, "xargs: exec failed\n");
                exit(1);
            }
            //父进程
            wait(0);
            extrac = 0;
            
        }
        else if(*p == '\0'){
            extrav[extrac++] = start;
            break;
        }

    }

    if(extrac>0){
        char* cmd[MAXARG];
        for(int i = 0; i < basec; i++){
            cmd[i] = base[i];
        }
        for(int i = 0; i < extrac; i++){
            cmd[i+basec] = extrav[i];
        }
        cmd[basec+extrac] = 0;
        if(fork()==0){//子进程
            exec(base[0],cmd);
            fprintf(2, "xargs: exec failed\n");
            exit(1);
        }
        wait(0);
    }
    exit(0);
}