/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *       Improved example of an interfering node
 * \author
 *        Sergio Diaz
 */

/*INCLUDES*/
#include "contiki.h"
#include "dev/cc2420/cc2420.h"
#include "dev/cc2420/cc2420_const.h" // Include the CC2420 constants
#include "dev/spi-legacy.h"          // Include basic SPI macros
#include "dev/leds.h"                // Include Leds to debbug
#include "sys/rtimer.h"              //Include the real time library
#include "lib/random.h"
#include <stdio.h> /* For printf() */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/*DEFINES*/
#define CONSTANT_MICROS 300 // 300 us was defined by the paper [1]
#define TIME_TICK 31        // The time of 1 tick is 30,51 us in rtimer for sky motes in Contiki v3.0. TIME_TICK = 1 / RTIMER_ARCH_SECOND = 1 / 32768 s

static struct rtimer rtimer; // Create the rtimer variable
uint32_t counter;
/*---------------------------------------------------------------------------*/
/** 
 * Writes to a register.
 * Note: the SPI_WRITE(0) seems to be needed for getting the
 * write reg working on the Z1 / MSP430X platform
 */
static void
setreg(enum cc2420_register regname, uint16_t value)
{
    CC2420_SPI_ENABLE();
    SPI_WRITE_FAST(regname);
    SPI_WRITE_FAST((uint8_t)(value >> 8));
    SPI_WRITE_FAST((uint8_t)(value & 0xff));
    SPI_WAITFORTx_ENDED();
    SPI_WRITE(0);
    CC2420_SPI_DISABLE();
}

/* Sends a strobe */
static void
strobe(enum cc2420_register regname)
{
    CC2420_SPI_ENABLE();
    SPI_WRITE(regname);
    CC2420_SPI_DISABLE();
}

/*
* Function called by the rtimer to turn the carrier on and off.
*/
static void carrier_OnOff(struct rtimer *timer, void *ptr)
{
    uint32_t num_ticks; // Number of ticks of the time_next_period

    if (counter < 500 && cc2420_get_channel() < 26)
    {
        printf("\nSweeper: Setting channel to: %u from %u", cc2420_get_channel() + 1, cc2420_get_channel());
        cc2420_set_channel(cc2420_get_channel() + 1);
        counter++;
    }
    else
    {
        printf("\nSweeper: Setting channel to: 11 from %u", cc2420_get_channel());
        cc2420_set_channel(11);
        counter = 0;
    }

    //The node must generate interference. Turn the carrier on.
    // Creates an unmodulated carrier by setting the appropiate registers in the CC2420
    setreg(CC2420_MANOR, 0x0100);
    setreg(CC2420_TOPTST, 0x0004);
    setreg(CC2420_MDMCTRL1, 0x0508);
    setreg(CC2420_DACTST, 0x1800);
    strobe(CC2420_STXON);

    // Set the rtimer to the time_next_period (num_ticks) by compute the next time period according to the paper [1]
    num_ticks = 10 * CONSTANT_MICROS / TIME_TICK;

    rtimer_set(&rtimer, RTIMER_NOW() + num_ticks, 1, carrier_OnOff, NULL); // Set the rtimer again to the time_next_period (num_ticks)
}

/*---------------------------------------------------------------------------*/

PROCESS(turn_carrier_OnOff, "Turn Carrier On Off"); // Declares the process to turn the carrier on and off
AUTOSTART_PROCESSES(&turn_carrier_OnOff);           // Load the process on boot

PROCESS_THREAD(turn_carrier_OnOff, ev, data) // Process to turn carrier on and off
{
    counter = 0;
    PROCESS_BEGIN(); // Says where the process starts

    rtimer_set(&rtimer, RTIMER_NOW() + RTIMER_ARCH_SECOND, 1, carrier_OnOff, NULL); //Initiates the rtimer 1 second after boot

    PROCESS_END(); //Says where the process ends
}

/*---------------------------------------------------------------------------*/