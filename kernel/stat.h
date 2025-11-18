#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device
//文件状态信息
struct stat {
  int dev;     // File system's disk device  文件所在设备号
  uint ino;    // Inode number     与 dirent.inum 是对应的
  short type;  // Type of file    看上面的宏
  short nlink; // Number of links to file
  uint64 size; // Size of file in bytes     文件大小/字节数
};
