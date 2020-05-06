#include <stdint.h>
#include <stdbool.h>

#define LLSEC802154_CONF_ENABLED 1

/* IEEE802.15.4 PANID */
#define IEEE802154_CONF_PANID 0x81a5

/* Do not start TSCH at init, wait for NETSTACK_MAC.on() */
#define TSCH_CONF_AUTOSTART 0

/* 6TiSCH minimal schedule length.
 * Larger values result in less frequent active slots: reduces capacity and saves energy. */
#define TSCH_SCHEDULE_CONF_DEFAULT_LENGTH 3

#if WITH_SECURITY

/* Enable security */
#define LLSEC802154_CONF_ENABLED 1

#endif /* WITH_SECURITY */

/* Enable TSCH statistics: must be on for channel selection to work */
#define TSCH_STATS_CONF_ON 1

/* Enable periodic RSSI sampling for TSCH statistics */
#define TSCH_STATS_CONF_SAMPLE_NOISE_RSSI 1

/* Reduce the TSCH stat "decay to normal" period to get printouts more often */
#define TSCH_STATS_CONF_DECAY_INTERVAL (60 * CLOCK_SECOND)

/* For adaptive channel selection */
extern void tsch_cs_channel_stats_updated(uint8_t updated_channel, uint16_t old_busyness_metric);
extern bool tsch_cs_process(void);
/* These will be called from the core TSCH code */
#define TSCH_CALLBACK_CHANNEL_STATS_UPDATED tsch_cs_channel_stats_updated
#define TSCH_CALLBACK_SELECT_CHANNELS tsch_cs_process

/* The coordinator will update the network nodes with new hopping sequences */
#define TSCH_PACKET_CONF_EB_WITH_HOPPING_SEQUENCE 1

/* Reduce the EB period in order to update the network nodes with more agility */
#define TSCH_CONF_EB_PERIOD (4 * CLOCK_SECOND)
#define TSCH_CONF_MAX_EB_PERIOD (4 * CLOCK_SECOND)