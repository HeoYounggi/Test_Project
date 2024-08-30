#ifndef _XBITSTREAM_H_
#define _XBITSTREAM_H_
//==============================================================================

class AFX_EXT_CLASS CxBitStream
{
public:
	CxBitStream(unsigned char* pbBuffer, int nSizeBits);
	virtual ~CxBitStream();

	unsigned int GetBits(int nBits);
	unsigned int ShowBits(int nBits);
	void SkipBits(int nBits);

	unsigned int GetBitCount() { return m_nIdx; };
	unsigned int GetBitRemainCount() { return m_nSizeBits - m_nIdx; };

protected:
	unsigned char* m_pbBuffer;
	unsigned char* m_pbBufferEnd;

	int m_nIdx;
	int m_nSizeBits;
};
//==============================================================================

#endif // _XBITSTREAM_H_
