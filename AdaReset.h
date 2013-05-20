#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <math.h>

#define PCA9685_MODE1 0x0

// Resets Adafruit Driver as if it were turned off
main() {
    int fh = open("/dev/i2c-3", O_RDWR);
    if(ioctl(fh, I2C_SLAVE, PCA9685_MODE1)<0){
        printf("ERROR in reset: ioctl\n");
    }
    int reset = 0x06;
    if(write(fh, &reset, 1)!=1){
        printf("ERROR in reset: write\n");
    }
    else
        printf("Adafruit board reset. Thank you.\n");
}
