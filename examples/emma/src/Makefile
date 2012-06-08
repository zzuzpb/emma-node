OPRJDIR=../

prepare:
#	cp $(OPRJDIR)/dd.c 

all: prepare emma-node emma-node-shell emma-node-ping emma-node-border 

ifndef NODE_ID
	NODE_ID=1
endif

CONTIKI=../../..

WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS += -Os
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

ifdef BORDER
CFLAGS += -DRPL_BORDER_ROUTER=1
	ifneq ($(TARGET),cooja)
		CFLAGS += -DTAP=\"tap0\"
	endif
else
	CFLAGS += -DUIP_CONF_IPV6_RPL=1

		CFLAGS += -DTAP=\"tap$(NODE_ID)\"
		CFLAGS += -DHARD_CODED_ADDRESS=\"bbbb::$(NODE_ID)\"	

endif

CFLAGS += -DWITH_COAP
	
APPS += emma
APPS += rest-coap

ifdef EMMA_SHELL
	CFLAGS += -DEMMA_SHELL
	APPS += serial-shell
endif

include $(CONTIKI)/Makefile.include


#ifeq ($(TARGET),avr-zigbit)
#avr-size -C --mcu=atmega1281 emma-example.avr-zigbit
#avr-objcopy -O ihex -R .eeprom -R .fuse -R .signature emma-example.avr-zigbit emma-example.avr-zigbit.hex
#endif

# make emma-node-shell TARGET=avr-atmega128rfa1 EMMA_SHELL=1
# avr-objcopy -O ihex -R .eeprom -R .fuse -R .signature emma-node-shell.avr-atmega128rfa1 emma-node-shell.avr-atmega128rfa1.hex
$(CONTIKI)/tools/tunslip6:	$(CONTIKI)/tools/tunslip6.c
	(cd $(CONTIKI)/tools && $(MAKE) tunslip6)

connect-router:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 aaaa::1/64

connect-router-cooja:	$(CONTIKI)/tools/tunslip6
	sudo $(CONTIKI)/tools/tunslip6 -a 127.0.0.1 aaaa::1/64
