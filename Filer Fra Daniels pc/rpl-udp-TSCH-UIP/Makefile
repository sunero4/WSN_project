CONTIKI_PROJECT = udp-client udp-server
all: $(CONTIKI_PROJECT)

MODULES += $(CONTIKI_NG_SERVICES_DIR)/tsch-cs

APPS += powertrace collect-view
CONTIKI=../..
MAKE_MAC = MAKE_MAC_TSCH
MODULES += os/services/shell
CFLAGS += -w

include $(CONTIKI)/Makefile.include


