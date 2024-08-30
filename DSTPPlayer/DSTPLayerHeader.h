#ifndef _DSTPLAYERHEADER_H_
#define _DSTPLAYERHEADER_H_
//==============================================================================

#include "pch.h"
//==============================================================================

typedef struct EthernetHeader	// [14 Byte]
{
	uint8_t ether_dst_host[6];	// Destination address [6 Byte]
	uint8_t ether_src_host[6];	// Sources address [6 Byte]
	uint16_t ether_type;		// Protocol [2 Byte]
}EthernetHeader, *PEthernetHeader;
//==============================================================================

typedef struct IPHeader			// [27 Byte]
{
	uint8_t ip_len : 4;			// IP Header Length [4 Byte]
	uint8_t ip_version : 4;		// IP Version [4 Byte]
	uint8_t tos;				// Type of service [1 Byte]
	uint16_t total_len;			// Total Length [2 Byte]
	uint16_t id;				// Identification [2 Byte]
	uint8_t frag_offset : 5;	// Fragment offset [2 Byte]
	uint8_t ttl;				// Time to live [1 Byte]
	uint8_t protocol;			// Protocol [1 Byte]
	uint16_t check;				// Checksum [2 Byte]
	uint8_t src_addr[4];		// Source address [4 Byte]
	uint8_t dst_addr[4];		// Destination address [4 Byte]
}IPHeader, *PIPHeader;
//==============================================================================

typedef struct UDPHeader		// [8 Byte]
{
	uint16_t src_port;			// Source port number [2 Byte]
	uint16_t dst_port;			// Destination port number [2 Byte]
	uint16_t length;			// Length of UDP header and data [2 Byte]
	uint16_t checksum;			// Checksum for error-checking (optional) [2 Byte]
}UDPHeader, *PUDPHeader;
//==============================================================================

typedef struct TCPHeader		// [23 Byte]
{
	uint16_t src_port;			// Source port number [2 Byte]
	uint16_t dst_port;			// Destination port number [2 Byte]
	uint32_t seq_number;		// Sequence number [4 Byte]
	uint32_t ack_number;		// Acknowledgment number [4 Byte]
	uint8_t  data_offset;		// Data offset (size of TCP header in 32-bit words) [1 Byte]
	uint8_t  reserved;			// Reserved for future use [3 Byte]
	uint8_t  flags;				// Flags (control bits) [1 Byte]
	uint16_t window;			// Window size [2 Byte] 
	uint16_t checksum;			// Checksum for error-checking [2 Byte]
	uint16_t urgent_pointer;	// Urgent pointer [2 Byte]
}TCPHeader, *PTCPHeader;
//==============================================================================
#endif // _DSTPLAYERHEADER_H_

