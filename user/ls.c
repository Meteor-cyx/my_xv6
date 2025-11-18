#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char*//format filename？？从路径提取文件名
fmtname(char *path)//就是返回路径末端的名字
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.

  //path是路径起点，加strlen(path)就到了尾部“\0”的位置
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
    //停在 “/“
    
  p++;
  //往前走一字节
  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)   //大于14直接返回
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p)); //空格填充到 14 字节长
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }
  //打开文件fd，信息存入&st
  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE: //文件
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR: //文件夹
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){//两次+1分别是路径分隔的'/'和'\0'
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);//把路径path写入buf
    p = buf+strlen(buf);//到末端：'\0'的地方
    *p++ = '/';  //覆盖  \0 ，写入'/'，自增向后走
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;   //"name-占位-\0"，合法字符串
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}


//  "stat.h"
// struct stat {
//   int dev;     // File system's disk device 文件所在设备号
//   uint ino;    // Inode number              与 dirent.inum 是对应的
//   short type;  // Type of file              看上面的宏
//   short nlink; // Number of links to file
//   uint64 size; // Size of file in bytes     文件大小/字节数
// };
