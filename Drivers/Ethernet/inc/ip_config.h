/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher 
 * Copyright: GPL V2
 *
 * This file can be used to decide which functionallity of the
 * TCP/IP stack shall be available.
 *
 *********************************************/
//@{
#ifndef IP_CONFIG_H
#define IP_CONFIG_H

#include "stm32f4xx.h"

//------------- functions in ip_arp_udp_tcp.c --------------
// an NTP client (ntp clock):
//#define NTP_client
// a spontaneous sending UDP client
#define UDP_CLIENT

// to send out a ping:
//#define PING_CLIENT
//#define PINGPATTERN 0x42

// a UDP wake on lan sender:
//#define WOL_CLIENT

// a "web browser". This can be use to upload data
// to a web server on the Internet by encoding the data
// into the url (like a Form action of type GET):
//#define WWW_CLIENT
//#define TCP_CLIENT
// if you do not need a browser and just a server:
//#undef WWW_CLIENT

// functions to decode cgi-form data:
#define FROMDECODE_WEBSRV_HELP

// function to encode a URL (mostly needed for a web client)
#define URLENCODE_WEBSRV_HELP

// DNS lookup support
//#define DNS_CLIENT

// DHCP support
#define DHCP_CLIENT

/* Some dependencies */

#ifdef DHCP_CLIENT
#define UDP_CLIENT
#endif

#endif /* IP_CONFIG_H */
//@}
