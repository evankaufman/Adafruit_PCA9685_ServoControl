/*
 * ServoControl.c
 * This Arduino code for servo control (https://github.com/adafruit/
 * Adafruit-PWM-Servo-Driver-Library) by Limor Fried/Ladyada is
 * adapted for Gumstix Overo COM by Evan Kaufman (tested on AirStorm).
 * This code is used to run servos on a PCA9685 Adafruit 12-bit
 * PWM/Servo Driver (http://www.adafruit.com/products/815)
 * with datasheet (http://www.adafruit.com/datasheets/PCA9685.pdf).
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// Pulse ranges:
#define ServoMin 150
#define ServoMax 600

// Other LED -> Servo info
#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

// i2c_addr set for board 0, but can be changed/included
// in functions for different/multiple boards
#define i2c_addr 0x40

#include "PCA9685.h"

main() {

    // Open i2c:
    int fh = open("/dev/i2c-3", O_RDWR);
    printf("Opening i2c port...\n");
    if(fh!=3)
        printf("ERROR opening i2c port.\n");
    else
        printf("The i2c port is open.\n");

    // Reset Device
    printf("Reseting device...\n");
    if(resetDev(fh)!=1)
        printf("ERROR: Device not reset.\n");
    else
        printf("Device is reset and ready for use.\n");

    // Set pwm frequency to 60 Hz (different depending on servo):
    float freq = 60;
    if(pwmFreq(fh, freq)!=1)
        printf("ERROR: setting pwm frequency\n");
    else
        printf("Frequency set to %f Hz.\n", freq);

    // Make loop that rotates servos up and down simultaneously, lapping 10 times
    uint16_t pl1 = ServoMin;// small pulse length (may need editind depending on servo)
    uint16_t pl2 = ServoMax;// large pulse length (may need editind depending on servo)
    int switcher = 0;// Switches between 0 and 1 to change servo directions
    int loops = 1;// For loops 1-10
    int servo, servo1, servo2;// for calling every other servo inside while() loop

    int servonum = 0;
    uint16_t pl;

    while(loops<=10){
        printf("Pulse %d:\n", loops);

        for(servo = 0; servo < 8; servo++) {
            servo1 = 2*servo;// 0, 2, 4,...14
            servo2 = 2*servo+1;// 1, 3, 5,...15
            if(switcher == 0) {
                pwmPulse(fh, servo1, 0, pl1);
                pwmPulse(fh, servo2, 0, pl2);
            }
            else {
                pwmPulse(fh, servo1, 0, pl2);
                pwmPulse(fh, servo2, 0, pl1);
            }
        }
        if(switcher == 0)
        switcher = 1;
        else
        switcher = 0;

        usleep(1000000);// 0.5 sec delay

        loops++;
    }
    resetDev(fh);// Stop pulses
}
