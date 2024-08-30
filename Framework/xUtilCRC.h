#ifndef _XUTILCRC_H_
#define _XUTILCRC_H_
//==============================================================================

#ifdef __cplusplus
extern "C" {
#endif
//==============================================================================

AFX_EXT_API unsigned short AxCRC16(unsigned char* pData, unsigned short unLength);
AFX_EXT_API unsigned short AxCRC16CCITT(unsigned char* pData, unsigned short unLength);

// IP/UDP Checksum
AFX_EXT_API unsigned short AxIPCheckSum(unsigned char* pData, unsigned short unLength);
AFX_EXT_API unsigned short AxUDPCheckSum(unsigned char* pSrcAddr, unsigned char* pDestAddr, unsigned char* pData, unsigned short unUDPLength);

// Base64
AFX_EXT_API int AxBase64Encode(unsigned char *pSrc, int nSrcLength, unsigned char* pDst);
AFX_EXT_API int AxBase64Decode(unsigned char *pSrc, int nSrcLength, unsigned char* pDst);

//==============================================================================
#ifdef __cplusplus
};
#endif
//==============================================================================
#endif //_XUTILCRC_H_

