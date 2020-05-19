#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include <string.h>
#include <stdio.h> /* For printf() */
#include "lib/random.h"
#include "sys/energest.h"
#include "lib/random.h"
/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (60 * CLOCK_SECOND)

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"

#include "tsch-cs.h"
static linkaddr_t coordinator_addr = {{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
#endif /* MAC_CONF_WITH_TSCH */

/*---------------------------------------------------------------------------*/
PROCESS(jamming_project_client, "Jamming Project Client");
AUTOSTART_PROCESSES(&jamming_project_client);

/*---------------------------------------------------------------------------*/
static unsigned long
to_seconds(uint64_t time)
{
  return (unsigned long)(time / ENERGEST_SECOND);
}

unsigned int random_number(unsigned int MaxValue)
{
  return random_rand() % MaxValue;
}

void input_callback(const void *data, uint16_t len,
                    const linkaddr_t *src, const linkaddr_t *dest)
{
  char payload[32];
  memcpy(&payload, data, sizeof(payload));
  printf("Received: %.*s ", (int)sizeof(payload), payload);
  LOG_INFO_LLADDR(src);
  printf("\n");
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(jamming_project_client, ev, data)
{
  static struct etimer alive_timer;
  static struct etimer event_timer;
  static char payload[32] = {0};
  static unsigned alive_count = 0;
  static unsigned event_count = 0;

  PROCESS_BEGIN();

#if MAC_CONF_WITH_TSCH
  tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));

  NETSTACK_MAC.on();
#endif /* MAC_CONF_WITH_TSCH */

  /* Initialize NullNet */

  nullnet_set_input_callback(input_callback);

  etimer_set(&alive_timer, SEND_INTERVAL);
  etimer_set(&event_timer, random_number(60) * CLOCK_SECOND);

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&alive_timer) || etimer_expired(&event_timer));

    if (etimer_expired(&alive_timer))
    {
      printf("Sending Alive %u to ", alive_count);
      LOG_INFO_LLADDR(NULL);
      printf("\n");

      snprintf(payload, 32, "Alive %d", alive_count);

      nullnet_buf = (uint8_t *)&payload;
      memcpy(nullnet_buf, &payload, sizeof(payload));
      nullnet_len = sizeof(payload);

      NETSTACK_NETWORK.output(NULL);
      alive_count++;
      etimer_reset(&alive_timer);

      printf("\nEnergest:\n");
      printf(" CPU          %4lus LPM      %4lus DEEP LPM %4lus  Total time %lus\n",
             to_seconds(energest_type_time(ENERGEST_TYPE_CPU)),
             to_seconds(energest_type_time(ENERGEST_TYPE_LPM)),
             to_seconds(energest_type_time(ENERGEST_TYPE_DEEP_LPM)),
             to_seconds(ENERGEST_GET_TOTAL_TIME()));
      printf(" Radio LISTEN %4lus TRANSMIT %4lus OFF      %4lus\n",
             to_seconds(energest_type_time(ENERGEST_TYPE_LISTEN)),
             to_seconds(energest_type_time(ENERGEST_TYPE_TRANSMIT)),
             to_seconds(ENERGEST_GET_TOTAL_TIME() - energest_type_time(ENERGEST_TYPE_TRANSMIT) - energest_type_time(ENERGEST_TYPE_LISTEN)));
    }
    if (etimer_expired(&event_timer))
    {
      printf("Sending Event %u  ", event_count);
      LOG_INFO_LLADDR(NULL);
      printf("\n");

      snprintf(payload, 32, "Event %d", event_count);

      nullnet_buf = (uint8_t *)&payload;
      memcpy(nullnet_buf, &payload, sizeof(payload));
      nullnet_len = sizeof(payload);

      NETSTACK_NETWORK.output(NULL);
      event_count++;

      etimer_set(&event_timer, SEND_INTERVAL / 6);
      etimer_reset(&event_timer);

      energest_flush();
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
