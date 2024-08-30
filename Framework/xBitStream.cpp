#include "pch.h"
//==============================================================================

#include "xBitStream.h"
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================

typedef unsigned int uint32_t;
//==============================================================================

static inline unsigned int bswap_32(unsigned int x)
{
	x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0x00FF00FF);
	x = (x >> 16) | (x << 16);
	return x;
}

#define AV_RN(s, p) (*((const uint##s##_t*)(p)))
#define AV_RB(s, p)		bswap_##s(AV_RN(s, p))
#define AV_RB32(p)		AV_RB(32, p)

#define NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))
#define SKIP_COUNTER(name, num)\
	name##_index += (num);\

#define OPEN_READER(name) \
	int name##_index= m_nIdx;\
	int name##_cache= 0;\

#define UPDATE_CACHE(name) \
	name##_cache= AV_RB32( m_pbBuffer + (name##_index >> 3) ) << (name##_index&0x07);\

#define SHOW_UBITS(name, num)\
	NEG_USR32(name##_cache, num)

#define LAST_SKIP_BITS(name, num) SKIP_COUNTER(name, num)

#define CLOSE_READER(name)\
	m_nIdx= name##_index;\
//==============================================================================

CxBitStream::CxBitStream(unsigned char* pbBuffer, int nSizeBits)
{
	int nBufferSize = (nSizeBits + 7) >> 3;
	if (nBufferSize < 0 || nSizeBits < 0)
	{
		nBufferSize = nSizeBits = 0;
		pbBuffer = NULL;
	}

	m_pbBuffer = pbBuffer;
	m_nSizeBits = nSizeBits;
	m_pbBufferEnd = pbBuffer + nBufferSize;
	m_nIdx = 0;
}
//==============================================================================

CxBitStream::~CxBitStream()
{

}
//==============================================================================

/**
* reads 1-17 bits.
* Note, the alt bitstream reader can read up to 25 bits, but the libmpeg2 reader can't
*/
unsigned int CxBitStream::GetBits(int nBits)
{
	register int tmp;
	OPEN_READER(re)
	UPDATE_CACHE(re)
	tmp = SHOW_UBITS(re, nBits);
	LAST_SKIP_BITS(re, nBits)
	CLOSE_READER(re)
	return tmp;
}
//==============================================================================

/**
* shows 1-17 bits.
* Note, the alt bitstream reader can read up to 25 bits, but the libmpeg2 reader can't
*/
unsigned int CxBitStream::ShowBits(int nBits)
{
	register int tmp;
	OPEN_READER(re)
	UPDATE_CACHE(re)
	tmp = SHOW_UBITS(re, nBits);
	//CLOSE_READER(re)
	return tmp;
}
//==============================================================================

void CxBitStream::SkipBits(int nBits)
{
	OPEN_READER(re)
	UPDATE_CACHE(re)
	LAST_SKIP_BITS(re, nBits)
	CLOSE_READER(re)
}
//==============================================================================
