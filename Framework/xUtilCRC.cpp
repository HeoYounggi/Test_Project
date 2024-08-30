#include "pch.h"
//==============================================================================

#include "xUtilCRC.h"
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================

/* CRC-CCITT lookup table		
//	  Polynominal = 0x1021		
//	  X^16 + X^15 + X^2 + 1		*/
const unsigned short CRC16_TABLE[] =	
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, 
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49, 
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78, 
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067, 
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};
//==============================================================================

static char cBase64Pad = '=';
static const char szBase64Char[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//==============================================================================

unsigned short AxCRC16(unsigned char* pData, unsigned short unLength)
{
	unsigned short unCRC = 0x0000;

	if(unLength < 2)
	{
		return 0;
	}

	while(unLength--)
	{
		unCRC = (unCRC << 8) ^ CRC16_TABLE[((unCRC >> 8) ^ *pData++)];
	}

	return unCRC;
}
//==============================================================================


unsigned short AxCRC16CCITT(unsigned char* pData, unsigned short unLength)
{
	unsigned short unCRC = 0xFFFF;

	if(unLength < 2)
	{
		return 0;
	}

	while(unLength--)
	{
		unCRC = (unCRC << 8) ^ CRC16_TABLE[((unCRC >> 8) ^ *pData++)];
	}

	return ~unCRC;
}
//==============================================================================

unsigned short AxIPCheckSum(unsigned char* pData, unsigned short unLength)
{
	unsigned short unWord16;
	unsigned int unSum = 0;
	unsigned short i;

	// make 16 bit words out of every two adjacent 8 bit words in the packet
	// and add them up
	for (i = 0; i < unLength; i = i + 2)
	{
		unWord16 =((pData[i] << 8) & 0xFF00)+(pData[i+1] & 0xFF);
		unSum = unSum + unWord16;	
	}

	// take only 16 bits out of the 32 bit sum and add up the carries
	while(unSum >> 16)
	{
		unSum = (unSum & 0xFFFF) + (unSum >> 16);
	}
	// one's complement the result
	unSum = ~unSum;

	return (unsigned short)unSum;
}
//==============================================================================

unsigned short AxUDPCheckSum(unsigned char* pSrcAddr, unsigned char* pDestAddr, unsigned char* pData, unsigned short unUDPLength)
{
	unsigned short unProtocol = 17;
	unsigned short unPadd = 0;
	unsigned short unWord16;
	unsigned int unSum;	

	unsigned short i;

	// Find out if the length of data is even or odd number. If odd,
	// add a padding byte = 0 at the end of packet
	if (unUDPLength % 2 == 1)
	{
		unPadd = 1;
		pData[unUDPLength] = 0;
	}

	//initialize sum to zero
	unSum = 0;

	// make 16 bit words out of every two adjacent 8 bit words and 
	// calculate the sum of all 16 bit words
	for(i = 0; i < unUDPLength + unPadd; i = i + 2)
	{
		unWord16 =((pData[i] << 8) & 0xFF00) + (pData[i + 1] & 0xFF);
		unSum = unSum + (unsigned int)unWord16;
	}

	// add the UDP pseudo header which contains the IP source and destination addresses
	for (i = 0; i < 4; i = i + 2)
	{
		unWord16 =((pSrcAddr[i] << 8) & 0xFF00) + (pSrcAddr[i + 1] & 0xFF);
		unSum = unSum + unWord16;	
	}

	for (i = 0; i < 4; i = i + 2)
	{
		unWord16 = ((pDestAddr[i] << 8) & 0xFF00) + (pDestAddr[i + 1] & 0xFF);
		unSum = unSum + unWord16; 	
	}
	// the protocol number and the length of the UDP packet
	unSum = unSum + unProtocol + unUDPLength;

	// keep only the last 16 bits of the 32 bit calculated sum and add the carries
	while(unSum >> 16)
	{
		unSum = (unSum & 0xFFFF)+(unSum >> 16);
	}
	// Take the one's complement of sum
	unSum = ~unSum;

	return (unsigned short)unSum;
}
//==============================================================================

int AxBase64Encode(unsigned char *pSrc, int nSrcLength, unsigned char* pDst)
{
	unsigned char* pCurrent = pSrc;
	int i = 0;
	//unsigned char *result = (unsigned char *)malloc(((length + 3 - length % 3) * 4 / 3 + 1) * sizeof(char));

	while (nSrcLength > 2)
	{
		/* keep going until we have less than 24 bits */
		pDst[i++] = szBase64Char[pCurrent[0] >> 2];
		pDst[i++] = szBase64Char[((pCurrent[0] & 0x03) << 4) + (pCurrent[1] >> 4)];
		pDst[i++] = szBase64Char[((pCurrent[1] & 0x0f) << 2) + (pCurrent[2] >> 6)];
		pDst[i++] = szBase64Char[pCurrent[2] & 0x3f];

		pCurrent += 3;
		nSrcLength -= 3; /* we just handle 3 octets of data */
	}

	/* now deal with the tail end of things */
	if (nSrcLength != 0)
	{
		pDst[i++] = szBase64Char[pCurrent[0] >> 2];
		if (nSrcLength > 1)
		{
			pDst[i++] = szBase64Char[((pCurrent[0] & 0x03) << 4) + (pCurrent[1] >> 4)];
			pDst[i++] = szBase64Char[(pCurrent[1] & 0x0f) << 2];
			pDst[i++] = cBase64Pad;
		}
		else
		{
			pDst[i++] = szBase64Char[(pCurrent[0] & 0x03) << 4];
			pDst[i++] = cBase64Pad;
			pDst[i++] = cBase64Pad;
		}
	}

	pDst[i] = '\0';

	return i;
}
//==============================================================================

int AxBase64Decode(unsigned char *pSrc, int nSrcLength, unsigned char* pDst)
{
	unsigned char* pCurrent = pSrc;
	int ch, i = 0, j = 0, k;
	/* this sucks for threaded environments */
	static short reverse_table[256];
	static int table_built;

	if (++table_built == 1)
	{
		char *chp;
		for(ch = 0; ch < 256; ch++)
		{
			chp = (char*)strchr(szBase64Char, ch);
			if(chp)
			{
				reverse_table[ch] = (short)(chp - szBase64Char);
			}
			else
			{
				reverse_table[ch] = -1;
			}
		}
	}

	/* run through the whole string, converting as we go */
	while ((ch = *pCurrent++) != '\0')
	{
		if (ch == cBase64Pad) break;

		/* When Base64 gets POSTed, all pluses are interpreted as spaces.
		This line changes them back.  It's not exactly the Base64 spec,
		but it is completely compatible with it (the spec says that
		spaces are invalid).  This will also save many people considerable
		headache.  - Turadg Aleahmad <turadg@wise.berkeley.edu>
		*/

		if (ch == ' ') ch = '+';

		ch = reverse_table[ch];
		if (ch < 0) continue;

		switch(i % 4)
		{
		case 0:
			pDst[j] = ch << 2;
			break;
		case 1:
			pDst[j++] |= ch >> 4;
			pDst[j] = (ch & 0x0f) << 4;
			break;
		case 2:
			  pDst[j++] |= ch >>2;
			pDst[j] = (ch & 0x03) << 6;
			break;
		case 3:
			pDst[j++] |= ch;
			break;
		}
		i++;
	}

	k = j;
	/* mop things up if we ended on a boundary */
	if (ch == cBase64Pad)
	{
		switch(i % 4)
		{
		case 0:
		case 1:
			return 0;
		case 2:
			k++;
		case 3:
			pDst[k++] = 0;
		}
	}

	pDst[k] = '\0';

	return j;
}
//==============================================================================
