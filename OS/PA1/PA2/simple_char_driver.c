

#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<asm/uaccess.h>

#define BUFFER_SIZE 1024

/* Define device_buffer and other global data structures you will need here */
static char device_buffer[BUFFER_SIZE];
static void* abuffer;

static int ocount = 0;
static int ccount = 0;

ssize_t simple_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */ 
	//int bytesRead;
	//int bytesToRead = BUFFER_SIZE - *offset;
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);

	int devbuffer_size = strlen(abuffer);
	//if (bytesToRead  == 0){
	//	printk(KERN_ALERT "End of read");
	//	return bytesToRead;
	//}
	//bytesRead = bytesToRead - copy_to_user(buffer, device_buffer + *offset, bytesToRead);
	//printk(KERN_ALERT "Reading %d bytes\n", bytesRead);
	//*offset += bytesRead;
	if (devbuffer_size == 0){
		printk(KERN_ALERT "Device buffer is empty");
		return 0;
	}
	copy_to_user(buffer, abuffer, devbuffer_size);
	printk(KERN_ALERT "Device has read %d bytes\n", devbuffer_size);
	//return bytesRead;
	return 0;
}



ssize_t simple_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	int devbuffer_size = strlen(abuffer);
	if (length == 0) {
		return 0;
	}
	*offset = devbuffer_size;//offset set to current size of device buffer so we don't overwrite whatevers currently there
	printk(KERN_ALERT "Writing to device\n");
	copy_from_user(abuffer + *offset, buffer, length);
	printk(KERN_ALERT "Device has written %zu bytes", strlen(buffer));
	return length;
}


int simple_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	ocount++;
	printk(KERN_ALERT "device opened %d times\n", ocount++);
	return 0;
}

int simple_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	ccount++;
	printk(KERN_ALERT "device opened %d times\n", ccount++);
	return 0;
}

loff_t simple_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);

	loff_t new_pos = 0;
	switch(whence)
	{
	case 0:
		new_pos = offset;
		break;
	case 1:
		new_pos = pfile->f_pos + offset;
		break;
	case 2:
		new_pos = abuffer - offset;
		break;

	}
	if (new_pos < 0) return new_pos;
	if (new_pos > device_buffer) return new_pos;
	pfile->f_pos = new_pos;
	return new_pos;
}

struct file_operations simple_char_driver_file_operations = {

	.owner   = THIS_MODULE,
	.read	 = simple_char_driver_read,
	.write	 = simple_char_driver_write,
	.open	 = simple_char_driver_open,
	.release = simple_char_driver_close,
	.llseek	 = simple_char_driver_seek
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
};

static int simple_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	/* register the device */
	register_chrdev (3, "simple_char_device", &simple_char_driver_file_operations);
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	abuffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	//void* kmalloc(device_buffer, GFP_KERNEL);
	//abuffer = *(char*)kmalloc;
	return 0;
}

void simple_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	/* unregister  the device using the register_chrdev() function. */
	unregister_chrdev (3, "simple_char_device");
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	kfree(abuffer);
	//return 0;
}

module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);
/* add module_init and module_exit to point to the corresponding init and exit function*/
