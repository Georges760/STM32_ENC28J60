#include "enc28j60.h"

static uint8_t CurrentBank;
static uint16_t gNextPacketPtr;
static uint8_t erxfcon;

__weak void ENC28J60_DelayUs(uint32_t delay) {

}

__weak void ENC28J60_DelayMs(uint32_t delay) {

}

__weak uint32_t ENC28J60_GetMs(void) {
	return 0;
}

__weak void ENC28J60_EnableChip(void) {

}

__weak void ENC28J60_DisableChip(void) {

}

__weak uint8_t ENC28J60_TransceiveByte(uint8_t data) {
	return 0;
}

uint8_t ENC28J60_ReadOp(uint8_t op, uint8_t address) {
	uint8_t temp;
	ENC28J60_EnableChip();
	ENC28J60_TransceiveByte(op | (address & ADDR_MASK));
	temp = ENC28J60_TransceiveByte(0xFF);
	if (address & 0x80)
		temp = ENC28J60_TransceiveByte(0xFF);
	ENC28J60_DisableChip();
	return temp;
}

void ENC28J60_WriteOp(uint8_t op, uint8_t address, uint8_t data) {
	ENC28J60_EnableChip();
	ENC28J60_TransceiveByte(op | (address & ADDR_MASK));
	ENC28J60_TransceiveByte(data);
	ENC28J60_DisableChip();
}

void ENC28J60_PowerDown() {
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXEN);
	while (ENC28J60_Read(ESTAT) & ESTAT_RXBUSY)
		;
	while (ENC28J60_Read(ECON1) & ECON1_TXRTS)
		;
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PWRSV);
}

void ENC28J60_PowerUp() {
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON2, ECON2_PWRSV);
	while (!ENC28J60_Read(ESTAT) & ESTAT_CLKRDY)
		;
}

void ENC28J60_ReadBuffer(uint16_t len, uint8_t *data) {
	ENC28J60_EnableChip();
	ENC28J60_TransceiveByte(ENC28J60_READ_BUF_MEM);
	while (len--) {
		*data++ = ENC28J60_TransceiveByte(0x00);
	}
	ENC28J60_DisableChip();
}

static uint16_t ENC28J60_ReadBufferWord() {
	uint16_t result;
	ENC28J60_ReadBuffer(2, (uint8_t*) &result);
	return result;
}

void ENC28J60_WriteBuffer(uint16_t len, uint8_t* data) {
	ENC28J60_EnableChip();
	ENC28J60_TransceiveByte(ENC28J60_WRITE_BUF_MEM);
	while (len--)
		ENC28J60_TransceiveByte(*data++);
	ENC28J60_DisableChip();
}

void ENC28J60_SetBank(uint8_t address) {
	if ((address & BANK_MASK) != CurrentBank) {
		ENC28J60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1 | ECON1_BSEL0);
		CurrentBank = address & BANK_MASK;
		ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, CurrentBank >> 5);
	}
}

uint8_t ENC28J60_Read(uint8_t address) {
	// set the bank
	ENC28J60_SetBank(address);
	// do the read
	return ENC28J60_ReadOp(ENC28J60_READ_CTRL_REG, address);
}

void ENC28J60_WriteWord(uint8_t address, uint16_t data) {
	ENC28J60_Write(address, data & 0xff);
	ENC28J60_Write(address + 1, data >> 8);
}

// read upper 8 bits
uint16_t ENC28J60_PhyReadH(uint8_t address) {
	// Set the right address and start the register read operation
	ENC28J60_Write(MIREGADR, address);
	ENC28J60_Write(MICMD, MICMD_MIIRD);
	ENC28J60_DelayUs(15);

	// wait until the PHY read completes
	while (ENC28J60_Read(MISTAT) & MISTAT_BUSY)
		;

	// reset reading bit
	ENC28J60_Write(MICMD, 0x00);

	return (ENC28J60_Read(MIRDH));
}

void ENC28J60_Write(uint8_t address, uint8_t data) {
	// set the bank
	ENC28J60_SetBank(address);
	// do the write
	ENC28J60_WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

void ENC28J60_PhyWrite(uint8_t address, uint16_t data) {
	// set the PHY register address
	ENC28J60_Write(MIREGADR, address);
	// write the PHY data
	ENC28J60_Write(MIWRL, data);
	ENC28J60_Write(MIWRH, data >> 8);
	// wait until the PHY write completes
	while (ENC28J60_Read(MISTAT) & MISTAT_BUSY) {
		ENC28J60_DelayUs(15);
	}
}
/*
 static void ENC28J60_PhyWriteWord(byte address, word data) {
 ENC28J60_Write(MIREGADR, address);
 //ENC28J60_WriteByte(MIREGADR, address);
 ENC28J60_WriteWord(MIWRL, data);
 while (ENC28J60_ReadByte(MISTAT) & MISTAT_BUSY)
 ;
 }
 */
void ENC28J60_Clkout(uint8_t clk) {
	//setup clkout: 2 is 12.5MHz:
	ENC28J60_Write(ECOCON, clk & 0x7);
}

void ENC28J60_Init(uint8_t* macaddr) {
	ENC28J60_EnableChip();

	// perform system reset
	ENC28J60_WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	ENC28J60_DelayMs(50);
	// check CLKRDY bit to see if reset is complete
	// The CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.
	//while(!(ENC28J60_Read(ESTAT) & ESTAT_CLKRDY));
	// do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers, must write low byte first
	// set receive buffer start address
	gNextPacketPtr = RXSTART_INIT;
	// Rx start
	ENC28J60_WriteWord(ERXSTL, RXSTART_INIT);
	// set receive pointer address
	ENC28J60_WriteWord(ERXRDPTL, RXSTART_INIT);
	// RX end
	ENC28J60_WriteWord(ERXNDL, RXSTOP_INIT);
	// TX start
	ENC28J60_WriteWord(ETXSTL, TXSTART_INIT);
	// TX end
	ENC28J60_WriteWord(ETXNDL, TXSTOP_INIT);
	// do bank 1 stuff, packet filter:
	// For broadcast packets we allow only ARP packtets
	// All other packets should be unicast only for our mac (MAADR)
	//
	// The pattern to match on is therefore
	// Type     ETH.DST
	// ARP      BROADCAST
	// 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
	// in binary these poitions are:11 0000 0011 1111
	// This is hex 303F->EPMM0=0x3f,EPMM1=0x30

	//ENC28J60_Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	//Change to add ERXFCON_BCEN recommended by epam
	//ENC28J60_Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN|ERXFCON_BCEN);
	erxfcon = ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN | ERXFCON_BCEN;
	ENC28J60_Write(ERXFCON, erxfcon);
	ENC28J60_WriteWord(EPMM0, 0x303f);
	ENC28J60_WriteWord(EPMCSL, 0xf7f9);
	//
	// do bank 2 stuff
	// enable MAC receive
	ENC28J60_Write(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	// bring MAC out of reset
	ENC28J60_Write(MACON2, 0x00);
	// enable automatic padding to 60bytes and CRC operations
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, MACON3,
	MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN); //|MACON3_FULDPX);
	// set inter-frame gap (non-back-to-back)
	ENC28J60_WriteWord(MAIPGL, 0x0C12);
	// set inter-frame gap (back-to-back)
	ENC28J60_Write(MABBIPG, 0x12);
	// Set the maximum packet size which the controller will accept
	// Do not send packets longer than MAX_FRAMELEN:
	ENC28J60_WriteWord(MAMXFLL, MAX_FRAMELEN);
	// do bank 3 stuff
	// write MAC address
	// NOTE: MAC address in ENC28J60 is byte-backward
	ENC28J60_Write(MAADR5, macaddr[0]);
	ENC28J60_Write(MAADR4, macaddr[1]);
	ENC28J60_Write(MAADR3, macaddr[2]);
	ENC28J60_Write(MAADR2, macaddr[3]);
	ENC28J60_Write(MAADR1, macaddr[4]);
	ENC28J60_Write(MAADR0, macaddr[5]);
	// no loopback of transmitted frames
	ENC28J60_PhyWrite(PHCON2, PHCON2_HDLDIS);
	// switch to bank 0
	ENC28J60_SetBank(ECON1);
	// enable interrutps
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
	// enable packet reception
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

// read the revision of the chip:
uint8_t ENC28J60_Getrev(void) {
	uint8_t rev;
	rev = ENC28J60_Read(EREVID);
	// microchip forgot to step the number on the silcon when they
	// released the revision B7. 6 is now rev B7. We still have
	// to see what they do when they release B8. At the moment
	// there is no B8 out yet
	if (rev > 5)
		rev++;
	return (rev);
}

// A number of utility functions to enable/disable broadcast and multicast bits
void ENC28J60_EnableBroadcast(void) {
	erxfcon |= ERXFCON_BCEN;
	ENC28J60_Write(ERXFCON, erxfcon);
}

void ENC28J60_DisableBroadcast(void) {
	erxfcon &= (0xff ^ ERXFCON_BCEN);
	ENC28J60_Write(ERXFCON, erxfcon);
}

void ENC28J60_EnableMulticast(void) {
	erxfcon |= ERXFCON_MCEN;
	ENC28J60_Write(ERXFCON, erxfcon);
}

void ENC28J60_DisableMulticast(void) {
	erxfcon &= (0xff ^ ERXFCON_MCEN);
	ENC28J60_Write(ERXFCON, erxfcon);
}

// link status
uint8_t ENC28J60_Linkup(void) {
	// bit 10 (= bit 3 in upper reg)
	return (ENC28J60_PhyReadH(PHSTAT2) && 4);
}

void ENC28J60_PacketSend(uint16_t len, uint8_t* packet) {
	// Check no transmit in progress
	while (ENC28J60_ReadOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_TXRTS) {
		// Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
		if ((ENC28J60_Read(EIR) & EIR_TXERIF)) {
			ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
			ENC28J60_WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
		}
	}

	// Set the write pointer to start of transmit buffer area
	ENC28J60_WriteWord(EWRPTL, TXSTART_INIT);
	// Set the TXND pointer to correspond to the packet size given
	ENC28J60_WriteWord(ETXNDL, (TXSTART_INIT + len));
	// write per-packet control byte (0x00 means use macon3 settings)
	ENC28J60_WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
	// copy the packet into the transmit buffer
	ENC28J60_WriteBuffer(len, packet);
	// send the contents of the transmit buffer onto the network
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
	// Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
}

// just probe if there might be a packet
//uint8_t ENC28J60_HasRxPkt(void)
//{
//       return ENC28J60_ReadByte(EPKTCNT) > 0;
//}

// Gets a packet from the network receive buffer, if one is available.
// The packet will by headed by an ethernet header.
//      maxlen  The maximum acceptable length of a retrieved packet.
//      packet  Pointer where packet data should be stored.
// Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
uint16_t ENC28J60_PacketReceive(uint16_t maxlen, uint8_t* packet) {
	uint16_t rxstat;
	uint16_t len;
	// check if a packet has been received and buffered
	//if( !(ENC28J60_Read(EIR) & EIR_PKTIF) ){
	// The above does not work. See Rev. B4 Silicon Errata point 6.
	if (ENC28J60_Read(EPKTCNT) == 0) {
		return (0);
	}

	// Set the read pointer to the start of the received packet
	ENC28J60_WriteWord(ERDPTL, gNextPacketPtr);
	//ENC28J60_Write(ERDPTL, (gNextPacketPtr &0xFF));
	//ENC28J60_Write(ERDPTH, (gNextPacketPtr)>>8);
	// read the next packet pointer
	gNextPacketPtr = ENC28J60_ReadBufferWord();
	//gNextPacketPtr  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0);
	//gNextPacketPtr |= ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	// read the packet length (see datasheet page 43)
	len = ENC28J60_ReadBufferWord() - 4;
	//len = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0);
	//len |= ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	//len-=4; //remove the CRC count
	// read the receive status (see datasheet page 43)
	rxstat = ENC28J60_ReadBufferWord();
	//rxstat  = ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0);
	//rxstat |= ((uint16_t)ENC28J60_ReadOp(ENC28J60_READ_BUF_MEM, 0))<<8;
	// limit retrieve length
	if (len > maxlen - 1) {
		len = maxlen - 1;
	}
	// check CRC and symbol errors (see datasheet page 44, table 7-3):
	// The ERXFCON.CRCEN is set by default. Normally we should not
	// need to check this.
	if ((rxstat & 0x80) == 0) {
		// invalid
		len = 0;
	} else {
		// copy the packet from the receive buffer
		ENC28J60_ReadBuffer(len, packet);
	}
	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
	ENC28J60_WriteWord(ERXRDPTL, gNextPacketPtr);
	//ENC28J60_Write(ERXRDPTL, (gNextPacketPtr &0xFF));
	//ENC28J60_Write(ERXRDPTH, (gNextPacketPtr)>>8);
	// However, compensate for the errata point 13, rev B4: enver write an even address!
	if ((gNextPacketPtr - 1 < RXSTART_INIT)
			|| (gNextPacketPtr - 1 > RXSTOP_INIT)) {
		ENC28J60_WriteWord(ERXRDPTL, RXSTOP_INIT);
		//ENC28J60_Write(ERXRDPTL, (RXSTOP_INIT)&0xFF);
		//ENC28J60_Write(ERXRDPTH, (RXSTOP_INIT)>>8);
	} else {
		ENC28J60_WriteWord(ERXRDPTL, (gNextPacketPtr - 1));
		//ENC28J60_Write(ERXRDPTL, (gNextPacketPtr-1)&0xFF);
		//ENC28J60_Write(ERXRDPTH, (gNextPacketPtr-1)>>8);
	}
	// decrement the packet counter indicate we are done with this packet
	ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	return (len);

	/*
	 uint16_t rxstat;
	 uint16_t len;
	 // check if a packet has been received and buffered
	 //if( !(ENC28J60_Read(EIR) & EIR_PKTIF) ){
	 // The above does not work. See Rev. B4 Silicon Errata point 6.
	 if( ENC28J60_Read(EPKTCNT) ==0 ){
	 return(0);
	 }

	 // Set the read pointer to the start of the received packet
	 ENC28J60_WriteWord(ERDPTL, gNextPacketPtr);
	 // read the next packet pointer
	 gNextPacketPtr  = ENC28J60_ReadBufferWord();
	 // read the packet length (see datasheet page 43)
	 len  = ENC28J60_ReadBufferWord() - 4;
	 // read the receive status (see datasheet page 43)
	 rxstat  = ENC28J60_ReadBufferWord();
	 // limit retrieve length
	 if (len>maxlen-1){
	 len=maxlen-1;
	 }
	 // check CRC and symbol errors (see datasheet page 44, table 7-3):
	 // The ERXFCON.CRCEN is set by default. Normally we should not
	 // need to check this.
	 if ((rxstat & 0x80)==0){
	 // invalid
	 len=0;
	 }else{
	 // copy the packet from the receive buffer
	 ENC28J60_ReadBuffer(len, packet);
	 }
	 // Move the RX read pointer to the start of the next received packet
	 // This frees the memory we just read out
	 //	ENC28J60_WriteWord(ERXRDPTL, gNextPacketPtr );
	 // However, compensate for the errata point 13, rev B4: enver write an even address!
	 if ((gNextPacketPtr - 1 < RXSTART_INIT)
	 || (gNextPacketPtr -1 > RXSTOP_INIT)) {
	 ENC28J60_WriteWord(ERXRDPTL, RXSTOP_INIT);
	 } else {
	 ENC28J60_WriteWord(ERXRDPTL, (gNextPacketPtr-1));
	 }
	 // decrement the packet counter indicate we are done with this packet
	 ENC28J60_WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	 return(len);
	 */
}

