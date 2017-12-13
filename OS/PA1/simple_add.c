#include <linux/kernel.h>
#include <linux/linkage.h>
#include <asm/uaccess.h>

asmlinkage long sys_simple_add(int num1,int num2,int *result)
{

 //*result = num1 + num2;

 int num3;
 num3 = num1 + num2;
 copy_to_user(result,&num3,sizeof(int));

 printk(KERN_EMERG "Number 1: %d \n",num1);
 printk(KERN_EMERG "Number 2: %d \n",num2);
 printk(KERN_EMERG "Number 1 PLUS Number 2 = %d \n",num3);
 return 0;

}

