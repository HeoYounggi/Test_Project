#ifndef _XRSCODER_H_
#define _XRSCODER_H_
//==============================================================================

#ifdef __cplusplus
extern "C" {
#endif

	AFX_EXT_API void* AxCreateRS(int symsize, int gfpoly, int fcr, int prim, int nroots, int pad);
	AFX_EXT_API void AxReleaseRS(void* pRS);

	AFX_EXT_API int AxDecodeRS(void* pRS, unsigned char* pData, int* eras_pos, int no_eras);
	AFX_EXT_API void AxEncodeRS(void *pRS, unsigned char* data, unsigned char* bb);

#ifdef __cplusplus
};
#endif
//==============================================================================

#endif // _XRSCODER_H_
