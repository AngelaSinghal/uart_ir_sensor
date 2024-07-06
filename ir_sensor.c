//#############################################################################
//
// FILE:   empty_driverlib_main.c
//
//! \addtogroup driver_example_list
//! <h1>Empty Project Example</h1> 
//!
//! This example is an empty project setup for Driverlib development.
//!
//
//#############################################################################
//
//
// $Copyright:
// Copyright (C) 2022 Texas Instruments Incorporated - http://www.ti.com
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "c2000ware_libraries.h"
#include "string.h"

uint32_t obj;
uint32_t cmd;
uint32_t clk_no;
uint32_t dist;
char receivedChar;
char *msg1,*msg2,*msg3,*msg4,*msg5;
//
// Main
//


void display_menu(){
    msg1 = "\nPress 1 to start the motor \0";
    SCI_writeCharArray(mySCI0_BASE, (uint16_t*)msg1,strlen(msg1));
    msg2 = "\r\nPress 2 to stop the motor \0";
    SCI_writeCharArray(mySCI0_BASE, (uint16_t*)msg2, strlen(msg2));
    msg3 = "\r\nPress 3 to get speed of the motor \0";
    SCI_writeCharArray(mySCI0_BASE, (uint16_t*)msg3, strlen(msg3));
    msg4 = "\r\nPress 4 for IR sensor data the motor \0";
    SCI_writeCharArray(mySCI0_BASE, (uint16_t*)msg4, strlen(msg4));
    msg5 = "\r\nPress 5 for ultra sonic data of the motor \0";
    SCI_writeCharArray(mySCI0_BASE, (uint16_t*)msg5, strlen(msg5));

}

uint32_t get_command(){
    uint32_t  num=0;
    receivedChar = SCI_readCharBlockingFIFO(mySCI0_BASE);

              //Turns character to digit
    num = receivedChar - '0';
    return num;
}

void print_integer(int msg)

{

    char arr[64], arr2[64];
    int length = 0, i;

    while(msg != 0)
    {
        arr[length] = msg%10 + '0';
        msg/=10;
        length++;
    }
    for(i=0; i<length; i++)
    {
        arr2[i] = arr[length-i-1];
    }
    arr2[length] = '\r';
    arr2[length+1] = '\n';
    arr2[length+2] = '\0';

    SCI_writeCharArray(mySCI0_BASE, (uint16_t*)arr2, length+2);

}

void process_command(uint32_t num){

    if (num==1){
        EPWM_setTimeBaseCounterMode(myEPWM0_BASE, EPWM_COUNTER_MODE_UP);
        EPWM_setTimeBaseCounterMode(myEPWM1_BASE, EPWM_COUNTER_MODE_UP);
        EPWM_setClockPrescaler(myEPWM0_BASE, EPWM_CLOCK_DIVIDER_4, EPWM_HSCLOCK_DIVIDER_10);
        EPWM_setClockPrescaler(myEPWM1_BASE, EPWM_CLOCK_DIVIDER_4, EPWM_HSCLOCK_DIVIDER_10);
        }
    if (num==2){
        EPWM_setTimeBaseCounterMode(myEPWM0_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
        EPWM_setTimeBaseCounterMode(myEPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
        }
    if (num==3){

//        if ((dist)>=250000){
//           speed=93.75;
//        }
//        if(200000<=(dist) && dist<250000){
//            speed=46.875;
////        }
//        if(150000<=(dist) && dist<200000){
//            speed=23.44;
//        }
//        if(100000<=(dist) && dist<150000){
//            speed=11.72;
//        }
//        if(50000<=(dist) && dist<100000){
//            speed=5.86;
//        }
//        if(dist<50000){
//            speed=0;
//        }
        print_integer(94);
    }
    if (num==4){
        print_integer(obj);

        }
    if (num==5){
        print_integer(dist);
        }
}

void main(){

    Device_init();
    Device_initGPIO();
    Interrupt_initModule();
    Interrupt_initVectorTable();
    Board_init();
    C2000Ware_libraries_init();
    EINT;
    ERTM;

    while(1)
    {
       obj = GPIO_readPin(OUT);

       GPIO_writePin(LED1, 0);
       GPIO_writePin(TRIG, 0);

       DEVICE_DELAY_US(2000000);

       GPIO_writePin(LED1, 1);
       GPIO_writePin(TRIG, 1);

       DEVICE_DELAY_US(2000000);
       /* Display the options */
       display_menu() ;
           /* Read the command */
       cmd = get_command() ;
           /* Process the command */
       process_command(cmd) ;

    }
}


interrupt void INT_ECAP_ECHO_ISR(void){
    GPIO_writePin(LED2, 0);
    clk_no = ECAP_getEventTimeStamp(ECAP_ECHO_BASE, ECAP_EVENT_2);

    dist = (343*clk_no)/(2*160); //in micrometers

    DEVICE_DELAY_US(200000);

    GPIO_writePin(LED2, 1);

    ECAP_clearInterrupt(ECAP_ECHO_BASE,ECAP_ISR_SOURCE_CAPTURE_EVENT_2);
    ECAP_clearGlobalInterrupt(ECAP_ECHO_BASE);
    ECAP_reArm(ECAP_ECHO_BASE);
    Interrupt_clearACKGroup(INT_ECAP_ECHO_INTERRUPT_ACK_GROUP);

//    if ((dist)>=250000){
//        EPWM_setTimeBaseCounterMode(myEPWM0_BASE, EPWM_COUNTER_MODE_UP);
//        EPWM_setTimeBaseCounterMode(myEPWM1_BASE, EPWM_COUNTER_MODE_UP);
//        EPWM_setClockPrescaler(myEPWM0_BASE, EPWM_CLOCK_DIVIDER_4, EPWM_HSCLOCK_DIVIDER_10);
//        EPWM_setClockPrescaler(myEPWM1_BASE, EPWM_CLOCK_DIVIDER_4, EPWM_HSCLOCK_DIVIDER_10);
//        }
//    if(200000<=(dist) && dist<250000){
//            EPWM_setTimeBaseCounterMode(myEPWM0_BASE, EPWM_COUNTER_MODE_UP);
//            EPWM_setTimeBaseCounterMode(myEPWM1_BASE, EPWM_COUNTER_MODE_UP);
//            EPWM_setClockPrescaler(myEPWM0_BASE, EPWM_CLOCK_DIVIDER_8, EPWM_HSCLOCK_DIVIDER_10);
//            EPWM_setClockPrescaler(myEPWM1_BASE, EPWM_CLOCK_DIVIDER_8, EPWM_HSCLOCK_DIVIDER_10);
//            }
//    if(150000<=(dist) && dist<200000){
//            EPWM_setTimeBaseCounterMode(myEPWM0_BASE, EPWM_COUNTER_MODE_UP);
//            EPWM_setTimeBaseCounterMode(myEPWM1_BASE, EPWM_COUNTER_MODE_UP);
//            EPWM_setClockPrescaler(myEPWM0_BASE, EPWM_CLOCK_DIVIDER_16, EPWM_HSCLOCK_DIVIDER_10);
//            EPWM_setClockPrescaler(myEPWM1_BASE, EPWM_CLOCK_DIVIDER_16, EPWM_HSCLOCK_DIVIDER_10);
//            }
//    if(100000<=(dist) && dist<150000){
//            EPWM_setTimeBaseCounterMode(myEPWM0_BASE, EPWM_COUNTER_MODE_UP);
//            EPWM_setTimeBaseCounterMode(myEPWM1_BASE, EPWM_COUNTER_MODE_UP);
//            EPWM_setClockPrescaler(myEPWM0_BASE, EPWM_CLOCK_DIVIDER_32, EPWM_HSCLOCK_DIVIDER_10);
//            EPWM_setClockPrescaler(myEPWM1_BASE, EPWM_CLOCK_DIVIDER_32, EPWM_HSCLOCK_DIVIDER_10);
//            }
//
//    if(50000<=(dist) && dist<100000){
//        EPWM_setTimeBaseCounterMode(myEPWM0_BASE, EPWM_COUNTER_MODE_UP);
//        EPWM_setTimeBaseCounterMode(myEPWM1_BASE, EPWM_COUNTER_MODE_UP);
//        EPWM_setClockPrescaler(myEPWM0_BASE, EPWM_CLOCK_DIVIDER_64, EPWM_HSCLOCK_DIVIDER_10);
//        EPWM_setClockPrescaler(myEPWM1_BASE, EPWM_CLOCK_DIVIDER_64, EPWM_HSCLOCK_DIVIDER_10);
//    }
//    if(dist<50000){
//        EPWM_setTimeBaseCounterMode(myEPWM0_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
//        EPWM_setTimeBaseCounterMode(myEPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
//
//    }
}
//
// End of File
//
