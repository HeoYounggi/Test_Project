/*
 * xIPHeader.h
 *
 *  Created on: 2019. 6. 11.
 *      Author: thkim
 */

#ifndef XIPHEADER_H_
#define XIPHEADER_H_

#include <vector>
using namespace std;

class AFX_EXT_CLASS CxIPHeader {
public:
	class IP_Header
	{
	public:
		unsigned int m_unVersion;
		unsigned int m_unIHL;
		unsigned int m_unTypeOfService;
		unsigned int m_unTotalLength;
		unsigned int m_unIdentification;
		unsigned int m_unFlags;
		unsigned int m_unFragmentOffset;
		unsigned int m_unTimeToLive;
		unsigned int m_unProtocol;
		unsigned int m_unHeaderCheckSum;
		unsigned int m_unSourceAddress;
		unsigned int m_unDestinationAddress;

		IP_Header();
		virtual ~IP_Header();

		int Push(unsigned char* buffer, int size);
		int Pull(unsigned char* buffer, int size);
	};
	IP_Header	m_cIP_Header;

	class UDP_Header
	{
	public:
		unsigned int m_unSourcePort;
		unsigned int m_unDestinationPort;
		unsigned int m_unLength;
		unsigned int m_unCheckSum;

		UDP_Header();
		virtual ~UDP_Header();

		int Push(unsigned char* buffer, int size);
		int Pull(unsigned char* buffer, int size);
	};
	UDP_Header	m_cUDP_Header;

	CxIPHeader();
	virtual ~CxIPHeader();

	void SetPayloadSize(int size);
	unsigned int IPCheckSum(unsigned char* buffer, unsigned short size);

	int Push(unsigned char* buffer, int size);
	int Pull(unsigned char* buffer, int size);
	int GetSize();
};
typedef vector<CxIPHeader*> Vector_CxIPHeader;

#endif /* XIPHEADER_H_ */
