CONTIKI_PROJECT = nullnet-unicast client server
all: $(CONTIKI_PROJECT)

CONTIKI = ../..

PLATFORMS_EXCLUDE = nrf52dk


# MAKE_MAC = MAKE_MAC_TSCH
MAKE_MAC ?= MAKE_MAC_CSMA
MAKE_NET = MAKE_NET_NULLNET
# MODULES += $(CONTIKI_NG_SERVICES_DIR)/tsch-cs
APPS += powertrace collect-view


include $(CONTIKI)/Makefile.include
