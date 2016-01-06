#ifndef ETHERSHIELD_H
#define ETHERSHIELD_H

#include "stm32f4xx.h"

#define bool _Bool
#define TRUE 1
#define FALSE 0

#include <inttypes.h>
#include "ip_config.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"

void Ethernet_Init(uint8_t* macaddr);
void Ethernet_Clkout(uint8_t clk);
uint8_t Ethernet_Linkup(void);
void Ethernet_PhyWrite(uint8_t address, uint16_t data);
uint16_t Ethernet_PacketReceive(uint16_t len, uint8_t* packet);
void Ethernet_PacketSend(uint16_t len, uint8_t* packet);
uint8_t Ethernet_Revision(void);
uint8_t Ethernet_Read(uint8_t address);
void Ethernet_EnableBroadcast(void);
void Ethernet_DisableBroadcast(void);
void Ethernet_EnableMulticast(void);
void Ethernet_DisableMulticast(void);
void Ethernet_PowerUp();
void Ethernet_PowerDown();

void Ethernet_init_ip_arp_udp_tcp(uint8_t *mymac, uint8_t *myip, uint16_t port);
// for a UDP server:
uint8_t Ethernet_eth_type_is_arp_and_my_ip(uint8_t *buf, uint16_t len);
uint8_t Ethernet_eth_type_is_ip_and_my_ip(uint8_t *buf, uint16_t len);

void Ethernet_make_echo_reply_from_request(uint8_t *buf, uint16_t len);
void Ethernet_make_tcp_synack_from_syn(uint8_t *buf);
void Ethernet_make_tcp_ack_from_any(uint8_t *buf, int16_t datlentoack,
		uint8_t addflags);
void Ethernet_make_tcp_ack_with_data(uint8_t *buf, uint16_t dlen);
void Ethernet_make_tcp_ack_with_data_noflags(uint8_t *buf, uint16_t dlen);
void Ethernet_init_len_info(uint8_t *buf);
uint16_t Ethernet_get_tcp_data_pointer(void);
uint16_t Ethernet_build_tcp_data(uint8_t *buf, uint16_t srcPort);
void Ethernet_send_tcp_data(uint8_t *buf, uint16_t dlen);

// UDP - dirkx
void Ethernet_send_udp_data1(uint8_t *buf, uint16_t dlen, uint16_t source_port,
		uint8_t *dest_ip, uint16_t dest_port);
void Ethernet_send_udp_data2(uint8_t *buf, uint8_t *destmac, uint16_t dlen,
		uint16_t source_port, uint8_t *dest_ip, uint16_t dest_port);

//void Ethernet_fill_buf_p(uint8_t *buf,uint16_t len, const prog_char *progmem_s);
uint16_t Ethernet_checksum(uint8_t *buf, uint16_t len, uint8_t type);
void Ethernet_fill_ip_hdr_checksum(uint8_t *buf);

// return 0 to just continue in the packet loop and return the position
// of the tcp data if there is tcp data part
uint16_t Ethernet_packetloop_icmp_tcp(uint8_t *buf, uint16_t plen);
// functions to fill the web pages with data:
//uint16_t Ethernet_fill_tcp_data_p(uint8_t *buf,uint16_t pos, const prog_char *progmem_s);
uint16_t Ethernet_fill_tcp_data(uint8_t *buf, uint16_t pos, const char *s);
uint16_t Ethernet_fill_tcp_data_len(uint8_t *buf, uint16_t pos, const char *s,
		uint16_t len);
// send data from the web server to the client:
void Ethernet_www_server_reply(uint8_t *buf, uint16_t dlen);

// -- client functions --
uint8_t Ethernet_client_store_gw_mac(uint8_t *buf);	//, uint8_t *gwipaddr);
void Ethernet_client_set_gwip(uint8_t *gwipaddr);
void Ethernet_client_set_wwwip(uint8_t *wwwipaddr);
void Ethernet_client_tcp_set_serverip(uint8_t *ipaddr);
void Ethernet_client_arp_whohas(uint8_t *buf, uint8_t *ip_we_search);
uint8_t Ethernet_client_waiting_gw(void);

#if defined (TCP_CLIENT) || defined (WWW_CLIENT) || defined (NTP_CLIENT)
uint8_t Ethernet_client_tcp_req(
		uint8_t (*result_callback)(uint8_t fd, uint8_t statuscode,
				uint16_t data_start_pos_in_buf, uint16_t len_of_data),
		uint16_t (*datafill_callback)(uint8_t fd), uint16_t port);

void Ethernet_tcp_client_send_packet(uint8_t *buf, uint16_t dest_port,
		uint16_t src_port, uint8_t flags, uint8_t max_segment_size,
		uint8_t clear_seqck, uint16_t next_ack_num, uint16_t dlength,
		uint8_t *dest_mac, uint8_t *dest_ip);
uint16_t Ethernet_tcp_get_dlength(uint8_t *buf);

#endif
#ifdef DNS_CLIENT
uint8_t Ethernet_dnslkup_haveanswer(void);
uint8_t Ethernet_dnslkup_get_error_info(void);
uint8_t *Ethernet_dnslkup_getip(void);
void Ethernet_dnslkup_set_dnsip(uint8_t *dnsipaddr);
void Ethernet_dnslkup_request(uint8_t *buf, uint8_t *hoststr);
uint8_t Ethernet_udp_client_check_for_dns_answer(uint8_t *buf, uint16_t plen);
uint8_t Ethernet_resolve_hostname(uint8_t *buf, uint16_t buffer_size, uint8_t *hostname);
#endif

#ifdef DHCP_CLIENT
uint8_t Ethernet_dhcp_state(void);
void Ethernet_dhcp_start(uint8_t *buf, uint8_t *macaddrin, uint8_t *ipaddrin,
		uint8_t *maskin, uint8_t *gwipin, uint8_t *dhcpsvrin, uint8_t *dnssvrin);

uint8_t Ethernet_check_for_dhcp_answer(uint8_t *buf, uint16_t plen);
uint8_t Ethernet_allocate_ip_address(uint8_t *buf, uint16_t buffer_size, uint8_t *mymac,
		uint16_t myport, uint8_t *myip, uint8_t *mynetmask, uint8_t *gwip,
		uint8_t *dnsip, uint8_t *dhcpsvrip);
#endif

#define HTTP_HEADER_START ((uint16_t)TCP_SRC_PORT_H_P+(buf[TCP_HEADER_LEN_P]>>4)*4)
#ifdef WWW_CLIENT
// ----- http get
void Ethernet_client_browse_url(char *urlbuf, char *urlbuf_varpart, char *hoststr,
		void (*callback)(uint8_t, uint16_t, uint16_t));
// The callback is a reference to a function which must look like this:
// void browserresult_callback(uint8_t statuscode,uint16_t datapos)
// statuscode=0 means a good webpage was received, with http code 200 OK
// statuscode=1 an http error was received
// statuscode=2 means the other side in not a web server and in this case datapos is also zero
// ----- http post
// client web browser using http POST operation:
// additionalheaderline must be set to NULL if not used.
// method should be used if default POST to be overridden set NULL for dfault
// postval is a string buffer which can only be de-allocated by the caller
// when the post operation was really done (e.g when callback was executed).
// postval must be urlencoded.
void Ethernet_client_http_post(char *urlbuf, char *hoststr,
		char *additionalheaderline, char *method, char *postval,
		void (*callback)(uint8_t, uint16_t, uint16_t));
// The callback is a reference to a function which must look like this:
// void browserresult_callback(uint8_t statuscode,uint16_t datapos)
// statuscode=0 means a good webpage was received, with http code 200 OK
// statuscode=1 an http error was received
// statuscode=2 means the other side in not a web server and in this case datapos is also zero
#endif		// WWW_CLIENT
#ifdef NTP_CLIENT
void Ethernet_client_ntp_request(uint8_t *buf,uint8_t *ntpip,uint8_t srcport);
uint8_t Ethernet_client_ntp_process_answer(uint8_t *buf,uint32_t *time,uint8_t dstport_l);
#endif		// NTP_CLIENT
// you can find out who ping-ed you if you want:
void Ethernet_register_ping_rec_callback(void (*callback)(uint8_t *srcip));

#ifdef PING_CLIENT
void Ethernet_client_icmp_request(uint8_t *buf,uint8_t *destip);
// you must loop over this function to check if there was a ping reply:
uint8_t Ethernet_packetloop_icmp_checkreply(uint8_t *buf,uint8_t *ip_monitoredhost);
#endif // PING_CLIENT
#ifdef WOL_CLIENT
void Ethernet_send_wol(uint8_t *buf,uint8_t *wolmac);
#endif // WOL_CLIENT
#ifdef FROMDECODE_WEBSRV_HELP
uint8_t Ethernet_find_key_val(char *str, char *strbuf, uint16_t maxlen, char *key);
void Ethernet_url_decode(char *urlbuf);
#endif	// FROMDECODE_WEBSRV_HELP
#ifdef URLENCODE_WEBSRV_HELP
void Ethernet_url_encode(char *str, char *urlbuf);
#endif	// URLENCODE_WEBSRV_HELP
uint8_t Ethernet_parse_ip(uint8_t *bytestr, char *str);
void Ethernet_mk_net_str(char *resultstr, uint8_t *bytestr, uint16_t len,
		char separator, uint8_t base);

uint8_t Ethernet_next_tcp_state(uint8_t *buf, uint16_t plen);
uint8_t Ethernet_current_tcp_state();
uint8_t Ethernet_tcp_active_open(uint8_t *buf, uint16_t plen,
		uint8_t (*result_callback)(uint8_t fd, uint8_t statuscode,
				uint16_t data_start_pos_in_buf, uint16_t len_of_data),
		uint16_t (*datafill_callback)(uint8_t fd), uint16_t port);
void Ethernet_tcp_passive_open(uint8_t *buf, uint16_t plen);
void Ethernet_tcp_close(uint8_t *buf, uint16_t plen);

#endif

