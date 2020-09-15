#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define buffer_size 1024
#define device_name "/dev/pa2_character_device"

char write_buf[buffer_size];
int main()
{
  int dev;
  dev = open(device_name, O_RDWR);
  if(dev < 0) {
    printf("Cannot open device file...\n");
    return 0;
  }
  else{
    printf("Opened device file...\n");
    char choice = 'p';
    int bytes;
    char* read_buffer;
    int offset;
    int whence;
    while(choice!='e'){
      printf("\nEnter r to read from device\n");
  		printf("Enter w to write to device\n");
  		printf("Enter s to seek into the device\n");
  		printf("Enter e to exit from the device\n");
  		printf("All other input will continue to reading or writing from this device\n");
  		printf("Enter option:\n");
      scanf("%c",&choice);
      getchar();
      switch(choice)
      {
        case 'r':
          printf("Enter the number of bytes you want to read: ");
          scanf("%d",&bytes);
          getchar();

          read_buffer=(char*)malloc(bytes);
          read(dev,read_buffer,bytes);
          printf("Data read from the device: %s\n",read_buffer);
          memset(read_buffer,'\0',bytes);
          free(read_buffer);
          break;
        case 'w':
          printf("Write\n");
    			printf("Enter data you want to write to the device: ");
    			scanf("%s", write_buf);
    			write(dev,write_buf,strlen(write_buf));
          getchar();
          break;
        case 's':
          printf("Seek\n");
          printf("Enter an offset value: ");
    			scanf("%d",&offset);
    			getchar();
    			printf("Enter a value for whence (third parameter): ");
    			scanf("%d",&whence);
    			getchar();

    			lseek(dev,offset,whence);
    			break;
        case 'e':
          printf("Exiting... Goodbye!\n");
          break;
        default:
          printf("BAD\n");
          break;
      }
    }


    printf("Done...\n");

	}
  close(dev);
}