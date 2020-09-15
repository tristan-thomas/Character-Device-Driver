

#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define BUFFER_SIZE 1024

/* Define device_buffer and other global data structures you will need here */
#define device_name "pa2_character_device"
void *device_buffer;
static const int device_file_major_number = 245;
int openCount = 0;
int closeCount = 0;


ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
	printk(KERN_ALERT "Read from File Called.\n");
	if(length>BUFFER_SIZE-*offset){
		length=BUFFER_SIZE-*offset;
		printk(KERN_ALERT "Error: bytes uncopied\n");
	}
	copy_to_user(buffer,device_buffer+*offset,length);
	printk(KERN_ALERT "%zd bytes read\n",length);
	*offset=*offset+length;
	return 0;
}



ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */
	printk(KERN_ALERT "Write to File Called.\n");
	if(length>BUFFER_SIZE-*offset){
		length=BUFFER_SIZE-*offset;
		printk(KERN_ALERT "Error: bytes uncopied\n");
	}
	copy_from_user(device_buffer+*offset,buffer,length);
	printk(KERN_ALERT "Bytes Writen: %zd\n",length);
	*offset=*offset+length;
	return length;
}


int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	openCount++;
	printk(KERN_ALERT "Open file has been called %d times.\n",openCount);
	return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	closeCount++;
	printk(KERN_ALERT "Close file has been called %d times.\n",closeCount);
	return 0;
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	loff_t updatePos=0;
	printk(KERN_ALERT "Seek File Called.\n");
	switch(whence)
	{
		case 0:
			// the current position is set to the value of the offset
			updatePos = offset;
			break;
		case 1:
			// the current position is incremented by offset bytes
			updatePos=pfile->f_pos+offset;
			break;
		case 2:
			// the current position is set to offset bytes before the end of the file
			updatePos=BUFFER_SIZE-offset;
			break;
		default:
			printk(KERN_ALERT "Invalid value passed as whence.\n");
			break;
	}
	if(updatePos>BUFFER_SIZE-1)
	{
		updatePos=pfile->f_pos;
		printk(KERN_ALERT "Err: Cannot seek past end of file.\n");
	}
	else if(updatePos<0)
	{
		updatePos=pfile->f_pos;
		printk(KERN_ALERT "Err: Cannot seek before beginning of file.\n");
	}
	pfile->f_pos=updatePos;
	return 0;
}

struct file_operations pa2_char_driver_file_operations = {

	.owner   = THIS_MODULE,
	/* add the function pointers to point to the corresponding file operations. 			look at the file fs.h in the linux souce code*/
	.open	 = pa2_char_driver_open,
	.release = pa2_char_driver_close,
	.read	 = pa2_char_driver_read,
	.write	 = pa2_char_driver_write,
	.llseek	 = pa2_char_driver_seek,
};

static int pa2_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	/* register the device */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	if (register_chrdev(device_file_major_number, device_name, &pa2_char_driver_file_operations)==0)
	{
		printk(KERN_ALERT "Registration Successful.\n Device File Major Number: %d\n",device_file_major_number);
	}
	else
	{
		printk(KERN_ALERT "ERR: Registration Unsuccessful.\n");
	}

	return 0;
}

static void pa2_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	/* unregister  the device using the register_chrdev() function. 	*/
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	unregister_chrdev(device_file_major_number, device_name);
	kfree(device_buffer);
}

/* add module_init and module_exit to point to the corresponding init and exit function*/
module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);