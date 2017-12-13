

#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 1024

//static char BUFFER[BUFFER_SIZE];
char* Dbuffer;

/* Define device_buffer and other global data structures you will need here */
static int openCount = 0;
static int closeCount = 0;
static int bytes_TW = 0;
static int byteLeftW = 0;

int byteLeftR;
int bytesWrote;
int bytesRead;
loff_t new;	


ssize_t simple_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
	printk(KERN_INFO "inside %s function\n",__FUNCTION__);

/*
	deviceBuffer_sizeR = strlen(devBuffer);
	byteLeftR = deviceBuffer_sizeR - *offset;

	if (deviceBuffer_sizeR == 0) { return 0; }
	
	if (byteLeftR < length){
		copy_to_user(buffer,devBuffer + *offset,byteLeftR);
		*offset = (*offset + byteLeftR) % deviceBuffer_sizeR;
		bytesRead = byteLeftR;
	}else {
		copy_to_user(buffer,devBuffer + *offset,length);
		*offset = (*offset + length) % deviceBuffer_sizeR;
		bytesRead = length;
	}

	printk(KERN_ALERT "%d bytes has been read from the device\n",bytesRead);
	return 0;
	*/
	//int devbuffer_size = strlen(Dbuffer);
	if (length == 0){
		printk(KERN_ALERT "Device buffer is empty");
		return 0;
	}
	copy_to_user(buffer, Dbuffer + *offset, length);
	printk(KERN_ALERT "Device has read %d bytes\n", length);
	*offset += length;
	//return bytesRead;
	return 0;
}



ssize_t simple_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */
	printk(KERN_INFO "inside %s function\n",__FUNCTION__);

	
	
	//int devbuffer_size = strlen(Dbuffer);

	byteLeftW = BUFFER_SIZE - *offset;
	if (length == 0) {
		return 0;
	}
	//*offset = devbuffer_size;//offset set to current size of device buffer so we don't overwrite whatevers currently there
	
	if (byteLeftW < length){
		bytes_TW = byteLeftW - copy_from_user(Dbuffer +*offset,buffer,byteLeftW);
	}else {
		copy_from_user(Dbuffer + *offset,buffer,length);
		
	}
	printk(KERN_ALERT "Writing to device\n");
	//copy_from_user((Dbuffer + *offset),buffer,bytes_TW);
	*offset += length;
	//copy_from_user(Dbuffer + *offset, buffer, length);
	printk(KERN_ALERT "Device has written %zu bytes", length);
	
	return length;


	/*
	deviceBuffer_sizeW = strlen(devBuffer);
	byteLeftW = deviceBuffer_sizeW - *offset;
	
	if(length == 0){return 0;}
	*offset = deviceBuffer_sizeW;

	//if (length == 0 || deviceBuffer_sizeW == 0) { return 0; }
	
	if (byteLeftW < length){
		bytes_TW = byteleftW;
	}else {
		bytes_TW = length 
	}
	
	copy_from_user((devBuffer + *offset),buffer,bytes_TW);
		//*offset = *offset + length;
		bytesWrote = length;
		printk(KERN_ALERT "%d bytes has been written on the device",bytesWrote);

	copy_from_user((devBuffer + *offset),buffer,byteLeftW);
		//*offset = *offset + byteLeftW;
		bytesWrote = byteLeftW;
		printk(KERN_ALERT "%d bytes has been written on the device",bytesWrote);
	
	//return bytesWrote;
	*/
}


int simple_char_driver_open (struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "inside %s function\n",__FUNCTION__);

	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	openCount++;
	printk(KERN_ALERT "Opened %d times.\n",openCount);
	return 0;
}

int simple_char_driver_close (struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "inside %s function\n",__FUNCTION__);

	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	closeCount++;
	printk(KERN_ALERT "Closed %d times.\n",closeCount);
	return 0;
}

loff_t simple_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	printk(KERN_INFO "inside %s function\n",__FUNCTION__);

	loff_t new;
	//deviceBuffer_sizeS = strlen(Dbuffer);

	//* Update open file position according to the values of offset and whence */
//	switch(whence) {
//	case 0://current pos is set to the value of the offset
//		new = offset;
//		break;
//	case 1://current pos is incremented by the offset bytes
//		new = pfile->f_pos + offset;
//		break; 
//	case 2://the current pos is set to offset bytes before the end of the file
//		new = BUFFER_SIZE - offset;
//		break;

	//if new buffer is less then 0 or greater then buffer size then dont change anything and return
//	if(new < 0 || new > BUFFER_SIZE)
//	{
//		return pfile->f_pos;
//	} else {

//		pfile->f_pos = new;
//		return new;

//	}
//	return 0;

	if (whence == 1) {
		new = pfile->f_pos + offset;
	}
	else if (whence == 2) {
		new = BUFFER_SIZE - offset;
	}
	else if (whence == 0) {
		new = offset;
	}else {
		return 0;
	}
	
	if(new < 0) return 0;
	if(new > BUFFER_SIZE) return 0;
	pfile->f_pos = new;
	return new;
}

struct file_operations simple_char_driver_file_operations = {

	.owner   = THIS_MODULE,
	.read    = simple_char_driver_read, 
	.write   = simple_char_driver_write,
	.open    = simple_char_driver_open,
	.release = simple_char_driver_close,
	.llseek  = simple_char_driver_seek,
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
};

static int simple_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	/* register the device */

	printk(KERN_INFO "inside %s function\n",__FUNCTION__);


	register_chrdev(3,"simple_character_device", &simple_char_driver_file_operations);
	Dbuffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	return 0;
}

void simple_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	/* unregister  the device using the register_chrdev() function. */
	printk(KERN_INFO "inside %s function\n",__FUNCTION__);
	unregister_chrdev(3,"simple_character_device");
	kfree(Dbuffer);
}

module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);

/* add module_init and module_exit to point to the corresponding init and exit function*/
