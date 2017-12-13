README 

Contact Info
	Toshal Ghimire
	toshal.ghimire@colorado.edu

Files:
 Makefile	/home/kernel/linux-hwe-4.10.0/arch/x86/kernel/
 simple_add.c	/home/kernel/linux-hwe-4.10.0/arch/x86/kernel/ 
 syscall_64.tbl	/home/kernel/linux-hwe-4.10.0/arch/x86/entry/syscalls/
 syslog		/var/log/
 syscalls.h	/home/kernel/linux-hwe-4.10.0/include/linux/sys.h
 SimpleAddTest.c 
 helloworldTest.c

Details:
 Makefile: 		contains all the obj-y+= files for helloworld and 				simple_add
 
 syscalls.h: 		contains all the prototype functions

 simple_add.c: 		is the custom system call function for the lab, 			witch adds 2 numbers

 syscall_64.tbl:	is the trap table witch has all the system call for 				the linux version
 
 SimpleAddTest.c:  	is the source code to test simple_add function
 helloworldTest.c: 	is the source code to test helloworld function


To run the test program:
 1) open terminal in the directory of the file and run these commands
 	
	gcc -o test SimpleAddTest.c
 	./test

 2)run either dmesg or sudo tail /var/log/syslog to view the output 
