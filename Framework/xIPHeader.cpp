/*
 * xIPHeader.cpp
 *
 *  Created on: 2019. 6. 11.
 *      Author: thkim
 */

#include "pch.h"

#include "xIPHeader.h"

CxIPHeader::IP_Header::IP_Header()
{
	m_unVersion = 4;
	m_unIHL = 5;
	m_unTypeOfService = 0;
	m_unTotalLength = 0;
	m_unIdentification = 0;
	m_unFlags = 2;
	m_unFragmentOffset = 0;
	m_unTimeToLive = 1;
	m_unProtocol = 0x11;
	m_unHeaderCheckSum = 0;
	m_unSourceAddress = 0;
	m_unDestinationAddress = 0;
}

CxIPHeader::IP_Header::~IP_Header()
{

}

int CxIPHeader::IP_Header::Push(unsigned char* buffer, int size)
{
	unsigned char* buf = buffer;

	m_unVersion = (*buf & 0xF0) >> 4;
	m_unIHL = (*buf & 0x0F);
	buf++;

	m_unTypeOfService = (*buf & 0xFF);
	buf++;

	m_unTotalLength = ((*buf & 0xFF) << 8) | (*(buf+1) & 0xFF);
	buf += 2;

	m_unIdentification = ((*buf & 0xFF) << 8) | (*(buf+1) & 0xFF);
	buf += 2;

	m_unFlags = (*buf & 0xE0) >> 5;
	m_unFragmentOffset = ((*buf & 0x1F) << 8) | (*(buf+1) & 0xFF);
	buf += 2;

	m_unTimeToLive = (*buf & 0xFF);
	buf++;

	m_unProtocol = (*buf & 0xFF);
	buf++;

	m_unHeaderCheckSum = ((*buf & 0xFF) << 8) | (*(buf+1) & 0xFF);
	buf += 2;

	m_unSourceAddress = ((*buf & 0xFF) << 24) | ((*(buf+1) & 0xFF) << 16) | ((*(buf+2) & 0xFF) << 8) | (*(buf+3) & 0xFF);
	buf += 4;

	m_unDestinationAddress = ((*buf & 0xFF) << 24) | ((*(buf+1) & 0xFF) << 16) | ((*(buf+2) & 0xFF) << 8) | (*(buf+3) & 0xFF);
	buf += 4;

	return m_unIHL * 4;
}

int CxIPHeader::IP_Header::Pull(unsigned char* buffer, int size)
{
	unsigned char* buf = buffer;

	*buf++ = ((m_unVersion << 4) & 0xF0) | (m_unIHL & 0x0F);
	*buf++ = m_unTypeOfService & 0xFF;
	*buf++ = (m_unTotalLength >> 8) & 0xFF;
	*buf++ = m_unTotalLength & 0xFF;

	*buf++ = (m_unIdentification >> 8) & 0xFF;
	*buf++ = m_unIdentification & 0xFF;
	*buf++ = ((m_unFlags << 5) & 0xE0) | ((m_unFragmentOffset >> 8) & 0x1F);
	*buf++ = m_unFragmentOffset & 0xFF;

	*buf++ = m_unTimeToLive & 0xFF;
	*buf++ = m_unProtocol & 0xFF;
	*buf++ = (m_unHeaderCheckSum >> 8) & 0xFF;
	*buf++ = m_unHeaderCheckSum & 0xFF;

	*buf++ = (m_unSourceAddress >> 24) & 0xFF;
	*buf++ = (m_unSourceAddress >> 16) & 0xFF;
	*buf++ = (m_unSourceAddress >> 8) & 0xFF;
	*buf++ = m_unSourceAddress & 0xFF;

	*buf++ = (m_unDestinationAddress >> 24) & 0xFF;
	*buf++ = (m_unDestinationAddress >> 16) & 0xFF;
	*buf++ = (m_unDestinationAddress >> 8) & 0xFF;
	*buf++ = m_unDestinationAddress & 0xFF;

	return m_unIHL * 4;
}


CxIPHeader::UDP_Header::UDP_Header()
{
	m_unSourcePort = 0;
	m_unDestinationPort = 0;
	m_unLength = 0;
	m_unCheckSum = 0;
}

CxIPHeader::UDP_Header::~UDP_Header()
{

}

int CxIPHeader::UDP_Header::Push(unsigned char* buffer, int size)
{
	unsigned char* buf = buffer;

	m_unSourcePort = ((*buf & 0xFF) << 8) | (*(buf+1) & 0xFF);
	buf += 2;

	m_unDestinationPort = ((*buf & 0xFF) << 8) | (*(buf+1) & 0xFF);
	buf += 2;

	m_unLength = ((*buf & 0xFF) << 8) | (*(buf+1) & 0xFF);
	buf += 2;

	m_unCheckSum = ((*buf & 0xFF) << 8) | (*(buf+1) & 0xFF);
	buf += 2;

	return (int)(buf - buffer);
}

int CxIPHeader::UDP_Header::Pull(unsigned char* buffer, int size)
{
	unsigned char* buf = buffer;

	*buf++ = (m_unSourcePort >> 8) & 0xFF;
	*buf++ = m_unSourcePort & 0xFF;

	*buf++ = (m_unDestinationPort >> 8) & 0xFF;
	*buf++ = m_unDestinationPort & 0xFF;

	*buf++ = (m_unLength >> 8) & 0xFF;
	*buf++ = m_unLength & 0xFF;

	*buf++ = (m_unCheckSum >> 8) & 0xFF;
	*buf++ = m_unCheckSum & 0xFF;

	return (int)(buf - buffer);
}

CxIPHeader::CxIPHeader()
{
}

CxIPHeader::~CxIPHeader()
{
}

unsigned int CxIPHeader::IPCheckSum(unsigned char* buffer, unsigned short size)
{
	unsigned short unWord16;
	unsigned int unSum = 0;
	unsigned short i;

	// make 16 bit words out of every two adjacent 8 bit words in the packet
	// and add them up
	for (i = 0; i < size; i = i + 2)
	{
		unWord16 =((buffer[i] << 8) & 0xFF00)+(buffer[i+1] & 0xFF);
		unSum = unSum + unWord16;
	}

	// take only 16 bits out of the 32 bit sum and add up the carries
	while(unSum >> 16)
	{
		unSum = (unSum & 0xFFFF) + (unSum >> 16);
	}
	// one's complement the result
	unSum = ~unSum;

	return unSum & 0xFFFF;
}

void CxIPHeader::SetPayloadSize(int size)
{
	m_cUDP_Header.m_unLength = size + 8;
	m_cIP_Header.m_unTotalLength = m_cUDP_Header.m_unLength + m_cIP_Header.m_unIHL * 4;
}

int CxIPHeader::Push(unsigned char* buffer, int size)
{
	unsigned char* buf = buffer;

	buf += m_cIP_Header.Push(buf, size - (int)(buf - buffer));
	if(m_cIP_Header.m_unProtocol == 0x11)
	{
		if(m_cIP_Header.m_unFlags == 0)
		{
			if(m_cIP_Header.m_unFragmentOffset == 0)
			{
				buf += m_cUDP_Header.Push(buf, size - (int)(buf - buffer));
			}
		}
		else if(m_cIP_Header.m_unFlags == 1)
		{
			if(m_cIP_Header.m_unFragmentOffset == 0)
			{
				buf += m_cUDP_Header.Push(buf, size - (int)(buf - buffer));
			}
		}
		else
		{
			buf += m_cUDP_Header.Push(buf, size - (int)(buf - buffer));
		}
	}

	return (int)(buf - buffer);
}

int CxIPHeader::Pull(unsigned char* buffer, int size)
{
	unsigned char* buf = buffer;

	m_cIP_Header.m_unHeaderCheckSum = 0;

	buf += m_cIP_Header.Pull(buf, size - (int)(buf - buffer));

	if(m_cIP_Header.m_unProtocol == 0x11)
	{
		buf += m_cUDP_Header.Pull(buf, size - (int)(buf - buffer));
	}

	m_cIP_Header.m_unHeaderCheckSum = IPCheckSum(buffer, 20);
	m_cIP_Header.Pull(buffer, 20);

	return (int)(buf - buffer);
}

int CxIPHeader::GetSize()
{
	if(m_cIP_Header.m_unProtocol == 0x11)
	{
		return m_cIP_Header.m_unIHL * 4 + 8;
	}

	return m_cIP_Header.m_unIHL * 4;
}
