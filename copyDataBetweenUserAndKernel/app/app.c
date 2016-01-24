#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char **argv) {
   int i, fd, len;
   char ch, write_buf[100], read_buf[100];

   if (argc != 3 && argc != 2) {
      printf("Usage: %s <read|write> [data]\n",argv[0]);
      exit(0);
   }


   fd = open("/dev/copy_data0", O_RDWR); // perform ops, read, write
   
   if (fd == -1) {
      printf("Error in opening file\n");
      exit(-1);
   }

   if (argc == 3) len = strlen(argv[2]); else len = 0;

   if (!strcmp("read", argv[1])) {
      read (fd, read_buf, sizeof(read_buf));
      printf ("Device data: %s\n", read_buf);
   } else if (!strcmp("write", argv[1])) {
      strncpy (write_buf, argv[2], len);
      write(fd, write_buf, len);
   }

   close(fd);
}

