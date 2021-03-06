#ifndef DHCP_H
#define DHCP_H

#include "stm32f4xx.h"

// to use this you need to enable UDP_CLIENT in the file ip_config.h
#if defined (UDP_CLIENT)

// Some defines

#define DHCP_BOOTREQUEST 1
#define DHCP_BOOTRESPONSE 2

extern void dhcp_start(uint8_t *buf, uint8_t *macaddrin, uint8_t *ipaddrin,
		uint8_t *maskin, uint8_t *gwipin, uint8_t *dhcpsvrin,
		uint8_t *dnssvrin );

extern uint8_t dhcp_state( void );

void dhcp_send(uint8_t *buf, uint8_t requestType);

uint8_t check_for_dhcp_answer(uint8_t *buf,uint16_t plen);

uint8_t have_dhcpoffer(uint8_t *buf,uint16_t plen);
uint8_t have_dhcpack(uint8_t *buf,uint16_t plen);

#endif /* UDP_CLIENT */
#endif /* DHCP_H */
//@}
