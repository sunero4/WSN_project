#include "contiki.h"
#include "dev/cc2420/cc2420.h"
#include "dev/cc2420/cc2420_const.h"
#include "dev/spi-legacy.h"
#include "sys/rtimer.h"
#include "lib/random.h"
#include <stdio.h>
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define CONSTANT_MICROS 300 // 300 us was defined by the paper [1]
#define TIME_TICK 31        // The time of 1 tick is 30,51 us in rtimer for sky motes in Contiki v3.0. TIME_TICK = 1 / RTIMER_ARCH_SECOND = 1 / 32768 s

static struct rtimer rtimer; // Create the rtimer variable

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
    cc2420_set_channel(26);
    PROCESS_BEGIN(); // Says where the process starts

    rtimer_set(&rtimer, RTIMER_NOW() + RTIMER_ARCH_SECOND, 1, carrier_OnOff, NULL); //Initiates the rtimer 1 second after boot

    PROCESS_END(); //Says where the process ends
}

/*---------------------------------------------------------------------------*/