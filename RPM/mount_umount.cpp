#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>

void unmountUSBDrive() {
	int unmountResult = system("sudo umount -l /home/pi/images/usbdisk1.d");
	if(unmountResult != 0){
		perror("Unmount failed");
	}else{
		printf("USB drive unmounted successfully.\n");
	}
}

void remountUSBDrive(){
	int mountResult = system("sudo mount -o loop,ro, -t vfat /home/pi/images/usbdisk1.img /home/pi/images/usbdisk1.d");
	if(mountResult != 0){
		perror("Mount failed");
	}else{
		printf("USB drive remounted successfully.\n");
	}
}

int main(){
	const char* usbDevicePath = "/dev/loop0";
	const char* mountPoint = "/home/pi/images/usbdisk1.d";

	unmountUSBDrive();

	sleep(5);

	remountUSBDrive();

	return 0;
}
