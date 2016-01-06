#include "ip_config.h"
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "websrv_help_functions.h"

#ifdef DNS_CLIENT
#include "dnslkup.h"
#endif

#ifdef DHCP_CLIENT
#include "dhcp.h"
#endif

#include "Ethernet.h"

/**
 * Initialize the ENC28J60 using default chip select pin
 * Flash the 2 MagJack LEDs
 */
void Ethernet_Init(uint8_t* macaddr) {
	int f;
	/* initialize enc28j60 */
	ENC28J60_Init(macaddr);
	ENC28J60_Clkout(2); // change clkout from 6.25MHz to 12.5MHz
	ENC28J60_DelayMs(10);

	for (f = 0; f < 3; f++) {
		// 0x880 is PHLCON LEDB=on, LEDA=on
		// ENC28J60_PhyWrite(PHLCON,0b0011 1000 1000 00 00);
		ENC28J60_PhyWrite(PHLCON, 0x3880);
		ENC28J60_DelayMs(500);

		// 0x990 is PHLCON LEDB=off, LEDA=off
		// ENC28J60_PhyWrite(PHLCON,0b0011 1001 1001 00 00);
		ENC28J60_PhyWrite(PHLCON, 0x3990);
		ENC28J60_DelayMs(500);
	}

	// 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
	// ENC28J60_PhyWrite(PHLCON,0b0011 0100 0111 01 10);
	ENC28J60_PhyWrite(PHLCON, 0x3476);
	ENC28J60_DelayMs(100);
}

void Ethernet_Clkout(uint8_t clk) {
	ENC28J60_Clkout(clk);
}

uint8_t Ethernet_Linkup(void) {
	return ENC28J60_Linkup();
}

void Ethernet_EnableBroadcast(void) {
	ENC28J60_EnableBroadcast();
}

void Ethernet_DisableBroadcast(void) {
	ENC28J60_DisableBroadcast();
}

void Ethernet_EnableMulticast(void) {
	ENC28J60_EnableMulticast();
}

void Ethernet_DisableMulticast(void) {
	ENC28J60_DisableMulticast();
}

uint8_t Ethernet_Read(uint8_t address) {
	return ENC28J60_Read(address);
}

uint8_t Ethernet_Revision(void) {
	return ENC28J60_Getrev();
}

void Ethernet_PhyWrite(uint8_t address, uint16_t data) {
	ENC28J60_PhyWrite(address, data);
}

uint16_t Ethernet_PacketReceive(uint16_t len, uint8_t* packet) {
	return ENC28J60_PacketReceive(len, packet);
}

void Ethernet_PacketSend(uint16_t len, uint8_t* packet) {
	ENC28J60_PacketSend(len, packet);
}

void Ethernet_init_ip_arp_udp_tcp(uint8_t *mymac, uint8_t *myip, uint16_t wwwp) {
	init_ip_arp_udp_tcp(mymac, myip, wwwp);
}

uint8_t Ethernet_eth_type_is_arp_and_my_ip(uint8_t *buf, uint16_t len) {
	return eth_type_is_arp_and_my_ip(buf, len);
}

uint8_t Ethernet_eth_type_is_ip_and_my_ip(uint8_t *buf, uint16_t len) {
	return eth_type_is_ip_and_my_ip(buf, len);
}

void Ethernet_make_echo_reply_from_request(uint8_t *buf, uint16_t len) {
	make_echo_reply_from_request(buf, len);
}

void Ethernet_make_tcp_synack_from_syn(uint8_t *buf) {
	make_tcp_synack_from_syn(buf);
}

void Ethernet_make_tcp_ack_from_any(uint8_t *buf, int16_t datlentoack,
		uint8_t addflags) {
	void make_tcp_ack_from_any(uint8_t *buf, int16_t datlentoack,
			uint8_t addflags);
}

void Ethernet_make_tcp_ack_with_data(uint8_t *buf, uint16_t dlen) {
	make_tcp_ack_with_data(buf, dlen);
}

void Ethernet_make_tcp_ack_with_data_noflags(uint8_t *buf, uint16_t dlen) {
	make_tcp_ack_with_data_noflags(buf, dlen);
}

uint16_t Ethernet_build_tcp_data(uint8_t *buf, uint16_t srcPort) {
	return build_tcp_data(buf, srcPort);
}

void Ethernet_send_tcp_data(uint8_t *buf, uint16_t dlen) {
	send_tcp_data(buf, dlen);
}

void Ethernet_send_udp_data2(uint8_t *buf, uint8_t *destmac, uint16_t dlen,
		uint16_t source_port, uint8_t *dest_ip, uint16_t dest_port) {
	uint8_t i;
	send_udp_prepare(buf, source_port, dest_ip, dest_port);
	for (i = 0; i < 6; i++)
		buf[ETH_DST_MAC + i] = destmac[i];
	send_udp_transmit(buf, dlen);
}

void Ethernet_send_udp_data1(uint8_t *buf, uint16_t dlen, uint16_t source_port,
		uint8_t *dest_ip, uint16_t dest_port) {
	send_udp_prepare(buf, source_port, dest_ip, dest_port);
	send_udp_transmit(buf, dlen);
}

void Ethernet_init_len_info(uint8_t *buf) {
	init_len_info(buf);
}

/*void Ethernet_fill_buf_p(uint8_t *buf,uint16_t len, const prog_char *progmem_s) {
 fill_buf_p(buf, len, progmem_s);
 }*/

uint16_t Ethernet_checksum(uint8_t *buf, uint16_t len, uint8_t type) {
	return checksum(buf, len, type);

}

void Ethernet_fill_ip_hdr_checksum(uint8_t *buf) {
	fill_ip_hdr_checksum(buf);
}

uint16_t Ethernet_get_tcp_data_pointer(void) {
	return get_tcp_data_pointer();
}

uint16_t Ethernet_packetloop_icmp_tcp(uint8_t *buf, uint16_t plen) {
	return packetloop_icmp_tcp(buf, plen);
}

/*uint16_t Ethernet_fill_tcp_data_p(uint8_t *buf,uint16_t pos, const prog_char *progmem_s){
 return fill_tcp_data_p(buf, pos, progmem_s);
 }*/

uint16_t Ethernet_fill_tcp_data(uint8_t *buf, uint16_t pos, const char *s) {
	return fill_tcp_data(buf, pos, s);
}

uint16_t Ethernet_fill_tcp_data_len(uint8_t *buf, uint16_t pos, const char *s,
		uint16_t len) {
	return fill_tcp_data_len(buf, pos, s, len);
}

void Ethernet_www_server_reply(uint8_t *buf, uint16_t dlen) {
	www_server_reply(buf, dlen);
}

uint8_t Ethernet_client_store_gw_mac(uint8_t *buf) {
	return client_store_gw_mac(buf);
}

void Ethernet_client_set_gwip(uint8_t *gwipaddr) {
	client_set_gwip(gwipaddr);
}

void Ethernet_client_set_wwwip(uint8_t *wwwipaddr) {
	//client_set_wwwip(wwwipaddr);
	client_tcp_set_serverip(wwwipaddr);
}

void Ethernet_client_tcp_set_serverip(uint8_t *ipaddr) {
	client_tcp_set_serverip(ipaddr);
}

void Ethernet_client_arp_whohas(uint8_t *buf, uint8_t *ip_we_search) {
	client_arp_whohas(buf, ip_we_search);
}

#if defined (TCP_CLIENT) || defined (WWW_CLIENT) || defined (NTP_CLIENT)
uint8_t Ethernet_client_tcp_req(
		uint8_t (*result_callback)(uint8_t fd, uint8_t statuscode,
				uint16_t data_start_pos_in_buf, uint16_t len_of_data),
		uint16_t (*datafill_callback)(uint8_t fd), uint16_t port) {
	return client_tcp_req(result_callback, datafill_callback, port);
}

void Ethernet_tcp_client_send_packet(uint8_t *buf, uint16_t dest_port,
		uint16_t src_port, uint8_t flags, uint8_t max_segment_size,
		uint8_t clear_seqck, uint16_t next_ack_num, uint16_t dlength,
		uint8_t *dest_mac, uint8_t *dest_ip) {

	tcp_client_send_packet(buf, dest_port, src_port, flags, max_segment_size,
			clear_seqck, next_ack_num, dlength, dest_mac, dest_ip);
}

uint16_t Ethernet_tcp_get_dlength(uint8_t *buf) {
	return tcp_get_dlength(buf);
}

#endif		// TCP_CLIENT WWW_Client etc

#ifdef WWW_CLIENT

// ----- http get

void Ethernet_client_browse_url(char *urlbuf, char *urlbuf_varpart, char *hoststr,
		void (*callback)(uint8_t, uint16_t, uint16_t)) {
	client_browse_url(urlbuf, urlbuf_varpart, hoststr, callback);
}

void Ethernet_client_http_post(char *urlbuf, char *hoststr,
		char *additionalheaderline, char *method, char *postval,
		void (*callback)(uint8_t, uint16_t, uint16_t)) {
	client_http_post(urlbuf, hoststr, additionalheaderline, method, postval,
			callback);
}

#endif		// WWW_CLIENT

#ifdef NTP_CLIENT
void Ethernet_client_ntp_request(uint8_t *buf,uint8_t *ntpip,uint8_t srcport) {
	client_ntp_request(buf,ntpip,srcport);
}

uint8_t Ethernet_client_ntp_process_answer(uint8_t *buf,uint32_t *time,uint8_t dstport_l) {
	return client_ntp_process_answer(buf,time,dstport_l);
}
#endif		// NTP_CLIENT

void Ethernet_register_ping_rec_callback(void (*callback)(uint8_t *srcip)) {
	register_ping_rec_callback(callback);
}

#ifdef PING_CLIENT
void Ethernet_client_icmp_request(uint8_t *buf,uint8_t *destip) {
	client_icmp_request(buf,destip);
}

uint8_t Ethernet_packetloop_icmp_checkreply(uint8_t *buf,uint8_t *ip_monitoredhost) {
	return packetloop_icmp_checkreply(buf,ip_monitoredhost);
}
#endif // PING_CLIENT

#ifdef WOL_CLIENT
void Ethernet_send_wol(uint8_t *buf,uint8_t *wolmac) {
	send_wol(buf,wolmac);
}
#endif // WOL_CLIENT

#ifdef FROMDECODE_WEBSRV_HELP
uint8_t Ethernet_find_key_val(char *str, char *strbuf, uint16_t maxlen, char *key) {
	return find_key_val(str, strbuf, maxlen, key);
}

void Ethernet_urldecode(char *urlbuf) {
	urldecode(urlbuf);
}
#endif

#ifdef URLENCODE_WEBSRV_HELP
void Ethernet_urlencode(char *str, char *urlbuf) {
	urlencode(str, urlbuf);
}
#endif

uint8_t Ethernet_parse_ip(uint8_t *bytestr, char *str) {
	return parse_ip(bytestr, str);
}

void Ethernet_mk_net_str(char *resultstr, uint8_t *bytestr, uint16_t len,
		char separator, uint8_t base) {
	mk_net_str(resultstr, bytestr, len, separator, base);
}

uint8_t Ethernet_client_waiting_gw() {
	return (client_waiting_gw());
}

#ifdef DNS_CLIENT
uint8_t Ethernet_dnslkup_haveanswer(void) {
	return (dnslkup_haveanswer());
}

uint8_t Ethernet_dnslkup_get_error_info(void) {
	return (dnslkup_get_error_info());
}

uint8_t * Ethernet_dnslkup_getip(void) {
	return (dnslkup_getip());
}

void Ethernet_dnslkup_set_dnsip(uint8_t *dnsipaddr) {
	dnslkup_set_dnsip(dnsipaddr);
}

void Ethernet_dnslkup_request(uint8_t *buf, uint8_t *hostname) {
	dnslkup_request(buf, hostname);
}

uint8_t Ethernet_udp_client_check_for_dns_answer(uint8_t *buf, uint16_t plen) {
	return udp_client_check_for_dns_answer(buf, plen);
}

// Perform all processing to resolve a hostname to IP address.
// Returns 1 for successful Name resolution, 0 otherwise
uint8_t Ethernet_resolve_hostname(uint8_t *buf, uint16_t buffer_size, uint8_t *hostname) {
	uint16_t dat_p;
	int plen = 0;
	uint32_t lastDnsRequest = ENC28J60_GetMs();
	uint8_t dns_state = DNS_STATE_INIT;
	bool gotAddress = FALSE;
	uint8_t dnsTries = 3;// After 10 attempts fail gracefully so other action can be carried out

	while (!gotAddress) {
		// handle ping and wait for a tcp packet
		plen = ENC28J60_PacketReceive(buffer_size, buf);
		dat_p = packetloop_icmp_tcp(buf, plen);

		// We have a packet
		// Check if IP data
		if (dat_p == 0) {
			if (client_waiting_gw()) {
				// No ARP received for gateway
				continue;
			}
			// It has IP data
			if (dns_state == DNS_STATE_INIT) {
				dns_state = DNS_STATE_REQUESTED;
				lastDnsRequest = ENC28J60_GetMs();
				dnslkup_request(buf, hostname);
				continue;
			}
			if (dns_state != DNS_STATE_ANSWER) {
				// retry every minute if dns-lookup failed:
				if (ENC28J60_GetMs() > (lastDnsRequest + 60000L)) {
					if (--dnsTries <= 0)
						return 0;		// Failed to allocate address

					dns_state = DNS_STATE_INIT;
					lastDnsRequest = ENC28J60_GetMs();
				}
				// don't try to use client before
				// we have a result of dns-lookup

				continue;
			}
		} else {
			if (dns_state == DNS_STATE_REQUESTED
					&& udp_client_check_for_dns_answer(buf, plen)) {
				dns_state = DNS_STATE_ANSWER;
				//client_set_wwwip(dnslkup_getip());
				client_tcp_set_serverip(dnslkup_getip());
				gotAddress = TRUE;
			}
		}
	}
	return 1;
}

#endif  // DNS_CLIENT

#ifdef DHCP_CLIENT
void Ethernet_dhcp_start(uint8_t *buf, uint8_t *macaddrin, uint8_t *ipaddrin,
		uint8_t *maskin, uint8_t *gwipin, uint8_t *dhcpsvrin, uint8_t *dnssvrin) {
	dhcp_start(buf, macaddrin, ipaddrin, maskin, gwipin, dhcpsvrin, dnssvrin);
}
uint8_t Ethernet_dhcp_state(void) {
	return (dhcp_state());
}

uint8_t Ethernet_check_for_dhcp_answer(uint8_t *buf, uint16_t plen) {
	return (check_for_dhcp_answer(buf, plen));
}

// Perform all processing to get an IP address plus other addresses returned, e.g. gw, dns, dhcp server.
// Returns 1 for successful IP address allocation, 0 otherwise
uint8_t Ethernet_allocate_ip_address(uint8_t *buf, uint16_t buffer_size, uint8_t *mymac,
		uint16_t myport, uint8_t *myip, uint8_t *mynetmask, uint8_t *gwip,
		uint8_t *dnsip, uint8_t *dhcpsvrip) {
	uint16_t dat_p;
	int plen = 0;
	uint32_t lastDhcpRequest = ENC28J60_GetMs();
	uint8_t dhcpState = 0;
	bool gotIp = FALSE;
	uint8_t dhcpTries = 10;	// After 10 attempts fail gracefully so other action can be carried out

	dhcp_start(buf, mymac, myip, mynetmask, gwip, dnsip, dhcpsvrip);

	while (!gotIp) {
		// handle ping and wait for a tcp packet
		plen = ENC28J60_PacketReceive(buffer_size, buf);
		dat_p = packetloop_icmp_tcp(buf, plen);
		if (dat_p == 0) {
			Ethernet_check_for_dhcp_answer(buf, plen);
			dhcpState = dhcp_state();
			// we are idle here
			if (dhcpState != DHCP_STATE_OK) {
				if (ENC28J60_GetMs() > (lastDhcpRequest + 10000L)) {
					lastDhcpRequest = ENC28J60_GetMs();
					if (--dhcpTries <= 0)
						return 0;		// Failed to allocate address
					// send dhcp
					dhcp_start(buf, mymac, myip, mynetmask, gwip, dnsip,
							dhcpsvrip);
				}
			} else {
				if (!gotIp) {
					gotIp = TRUE;

					//init the ethernet/ip layer:
					init_ip_arp_udp_tcp(mymac, myip, myport);

					// Set the Router IP
					client_set_gwip(gwip);  // e.g internal IP of dsl router
#ifdef DNS_CLIENT
					// Set the DNS server IP address if required, or use default
					dnslkup_set_dnsip(dnsip);
#endif
				}
			}
		}
	}
	return 1;
}

#endif // DHCP_CLIENT

void Ethernet_PowerUp() {
	ENC28J60_PowerUp();
}

void Ethernet_PowerDown() {
	ENC28J60_PowerDown();
}

// TCP functions broken out here for testing
uint8_t Ethernet_nextTcpState(uint8_t *buf, uint16_t plen) {
	return nextTcpState(buf, plen);
}

uint8_t Ethernet_currentTcpState() {
	return currentTcpState();
}

uint8_t Ethernet_tcpActiveOpen(uint8_t *buf, uint16_t plen,
		uint8_t (*result_callback)(uint8_t fd, uint8_t statuscode,
				uint16_t data_start_pos_in_buf, uint16_t len_of_data),
		uint16_t (*datafill_callback)(uint8_t fd), uint16_t port) {
	return tcpActiveOpen(buf, plen, result_callback, datafill_callback, port);
}

void Ethernet_tcpPassiveOpen(uint8_t *buf, uint16_t plen) {
	tcpPassiveOpen(buf, plen);
}

void Ethernet_tcpClose(uint8_t *buf, uint16_t plen) {
	tcpClose(buf, plen);
}

// End
