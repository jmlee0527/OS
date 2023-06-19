//Symbolic link test

// #include "types.h"
// #include "stat.h"
// #include "user.h"

// int
// main(int argc, char *argv[]){
//   printf(1,"test[1] : hard link & symbolic link\n");
//   printf(1,"test[2] : hard link & symbolic link\n");
//   printf(1,"test[3] : hard link & symbolic link\n");
//   printf(1,"test[4] : hard link & symbolic link\n");
//   printf(1,"test[5] : hard link & symbolic link\n");
  
//   exit();

// }

//Sync test

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

int main(void) {
  int fd;
  char data[] = "test sync!!!";

  fd = open("testfile.txt", O_CREATE | O_WRONLY);
  if (fd < 0) {
    printf(1, "Failed to open file\n");
    exit();
  }

  if (write(fd, data, sizeof(data)) != sizeof(data)) {
    printf(1, "Failed to write to file\n");
    close(fd);
    exit();
  }

  sync();

  close(fd);

  printf(1, "Data has been written and synchronized to disk\n");
  exit();
}
