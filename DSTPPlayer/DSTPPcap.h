#ifndef _DSTPPCAP_H_
#define _DSTPPCAP_H_
//==============================================================================

#include "pch.h"
//==============================================================================

typedef struct pcap_hdr_s		// [24 Byte]
{
	uint32_t magic_number;		// magic number [4 Byte]
	uint16_t version_major;		// major version number [2 Byte]
	uint16_t version_minor;		// minor version number [2 Byte]
	int32_t thiszone;			// GMT to local correction [4 Byte]
	uint32_t sigfigs;			// accuracy of timestamps [4 Byte]
	uint32_t snaplen;			// max length of captured packets, in octets [4 Byte]
	uint32_t network;			// data link type [4 Byte]
}pcap_hdr_t, *Ppcap_hdr_t;
//============================================================================== 

typedef struct pcaprec_hdr_s	// [16 Byte]
{
	uint32_t ts_sec;			// timestamp seconds [4 Byte]
	uint32_t ts_usec;			// timestamp microseconds [4 Byte]
	uint32_t incl_len;			// number of octets of packet saved in file (caplen) [4 Byte]
	uint32_t orig_len;			// actual length of packet (len) [4 Byte]
}pcaprec_hdr_t, *Ppcaprec_hdr_t;
//============================================================================== 
#endif // _DSTPPCAP_H_
