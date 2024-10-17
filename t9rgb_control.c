/*
__author__ = "Sergey Fedotov"
__copyright__ = "Copyright 2024 Sergey Fedotov <servifed2@gmail.com>"
__license__ = "GPLv3-or-later"
__email__ = "servifed2@gmail.com"
__version__ = "0.3"
*/

//#define _GNU_SOURCE  // For BOTHER flag in termios2 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <asm/termbits.h>   // For termios2 structure 
#include <sys/ioctl.h> 
#include <errno.h> 
#include <time.h> 
 
const unsigned char *modes_name[]={"Rainbow", "Breathing", "Cycle", "Off", "Auto"};

#define DefaultDevice "/dev/ttyUSB0"
#define Brightness BS
#define Speed BS

// Function get mode code
unsigned char  getModeCode(const unsigned char* mode_name){
    for ( unsigned char  i=0; i<5; i++ ){
        if ( ! strcmp( mode_name, modes_name[i])) {
            return i+1;
        }
    }
    return 0;
}

// Function to set a custom baud rate 
int set_custom_baudrate(const int fd, const int baudrate) { 
    struct termios2 tio; 
 
    // Get current terminal I/O settings 
    if (ioctl(fd, TCGETS2, &tio) < 0) { 
        perror("ioctl TCGETS2"); 
        return -1; 
    } 
 
    // Set custom baud rate using BOTHER flag 
    tio.c_cflag &= ~CBAUD; 
    tio.c_cflag |= BOTHER;  // Allow custom baud rate 
    tio.c_ispeed = baudrate; 
    tio.c_ospeed = baudrate; 
 
    // Apply the new terminal settings 
    if (ioctl(fd, TCSETS2, &tio) < 0) { 
        perror("ioctl TCSETS2"); 
        return -1; 
    } 
 
    return 0; 
} 
 
// Function to set up the serial port 
int setup_serial(const char *device, const int baudrate) { 
    // Open the serial device 
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC); 
    if (fd < 0) { 
        perror("open"); 
        return -1; 
    } 
 
    // Set the custom baud rate 
    if (set_custom_baudrate(fd, baudrate) < 0) { 
        close(fd); 
        return -1; 
    } 
 
    return fd; 
} 
 
// Function to calculate the check digit (as per original Python code) 
unsigned char calculate_check_digit(const unsigned char mode, const unsigned char brightness, const unsigned char speed) { 
    return (0xfa + mode + brightness + speed) & 0xff; 
} 
 
// Function to send the LED command 
int send_led_status(const int fd, const unsigned char mode, const unsigned char brightness, const unsigned char speed) { 
    unsigned char check_digit = calculate_check_digit(mode, brightness, speed); 
    unsigned char buffer[] = {0xFA, mode, brightness, speed, check_digit}; 
 
    // Send data with 5ms delays between each byte 
    for (int i = 0; i < sizeof(buffer); i++) { 
        if (write(fd, &buffer[i], 1) < 0) { 
            perror("write"); 
            return -1; 
        } 
        usleep(5000);  // 5ms delay 
    } 
 
    return 0; 
} 
 
void usage(char *argv[]){
    printf("Usage: %s [device] <mode> [brightness speed]\n", argv[0]); 
    printf("\twhere mode is one from:");
    for (int i=4; i>=0; i--){
        printf(" '%s'", modes_name[i]);
    }
    printf(".\n\tbrightness and speed value from 1 to 5 (optional, default=1).\n");
    printf("\tDefault device is '/dev/ttyUSB0'\n") ;
};

int main(int argc, char *argv[]) { 
    const unsigned char BS[] = {0x05, 0x04, 0x03, 0x02, 0x01};  // Brightness and Speed levels 1 to 5 

    const char *device;
    const int baudrate = 10000; 

    int brightness_index = 0;
    int speed_index = 0;
    int mode;

    if ( argc < 2 || argc > 5 ){
        usage(argv);
        return 1; 
    }

    if ( argc == 4 || argc == 5 ){    
        brightness_index = atoi(argv[argc-2]) - 1; 
        speed_index = atoi(argv[argc-1]) - 1; 
    } 
    if ( argc == 2 || argc == 4 ){
        device = DefaultDevice; 
        mode = getModeCode( argv[1] );
    } else 
    if ( argc == 3 || argc == 5 ){
        device = argv[1];
        mode = getModeCode( argv[2] );
    } else {
        usage(argv);
        return 1;
    }

    if ( mode < 1 || mode > 5 ){
        if ( argc == 4 || argc == 3 ){ 
            printf("Both brightness and speed must be specified.\n");
        } else {
            printf("Invalid mode: %s\n", argv[1]);
        }
        usage(argv);
        return 1;
    }
    

     if (brightness_index < 0 || brightness_index >= 5 ) { 
        printf("Invalid brightness value. Use values from 1 to 5.\n"); 
        return 1; 
    }
    if ( speed_index < 0 || speed_index >= 5) { 
        printf("Invalid speed value. Use values from 1 to 5.\n"); 
        return 1; 
    } 
 
    // Open and set up the serial port 
    int fd = setup_serial(device, baudrate); 
    if (fd < 0) { 
        printf("Failed to set up serial port\n"); 
        return 1; 
    } 
 
    // Send the LED command 
    if (send_led_status(fd, mode, Brightness[brightness_index], Speed[speed_index]) < 0) { 
        printf("Failed to send LED status\n"); 
        close(fd); 
        return 1; 
    } 
 
    printf("LED status sent successfully.\n"); 
 
    // Close the serial port 
    close(fd); 
    return 0; 
}
