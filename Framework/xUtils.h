#ifndef _XUTIL_H_
#define _XUTIL_H_
//==============================================================================

// SAFE String to Number
#define SAFE_TTOI(x) _ttoi(x == NULL ? TEXT("0") : x)
#define SAFE_ATOI(x) atoi(x == NULL ? "0" : x)

#define SAFE_TTOI64(x) _ttoi64(x == NULL ? TEXT("0") : x)
#define SAFE_ATOI64(x) _atoi64(x == NULL ? "0" : x)

#define SAFE_TTOF(x) _ttof(x == NULL ? TEXT("0") : x)
#define SAFE_ATOF(x) atof(x == NULL ? "0" : x)
//==============================================================================

//==============================================================================
// file/directory handling
AFX_EXT_API extern CString GetExeName();
AFX_EXT_API extern bool FileExists(CString strFileName);
AFX_EXT_API extern bool DirectoryExists(CString strDirName);
AFX_EXT_API extern CString ExtractFileName(CString strFullPathName);
AFX_EXT_API extern CString ExtractFilePath(CString strFullPathName);
AFX_EXT_API extern CString ExtractFileDir(CString strFullPathName);
AFX_EXT_API extern CString ExtractFileExt(CString strFileName);
AFX_EXT_API extern CString ChangeFileExt(CString strFileName, CString strExt);
AFX_EXT_API extern void ForceDirectories(CString strDirName);
//==============================================================================

// Number & String
AFX_EXT_API extern CString Int2Str(int nVal);
AFX_EXT_API extern CString Int642Str(__int64 nVal);
AFX_EXT_API extern int Str2Int(CString strValue);
AFX_EXT_API extern long long Str2Int64(CString strValue);
AFX_EXT_API extern float Str2Float(CString strValue);
AFX_EXT_API extern double Str2Double(CString strValue);
AFX_EXT_API extern CString Int2Hex(__int64 nVal);
AFX_EXT_API extern CString Int642Hex(long long nVal);
AFX_EXT_API extern int Hex2Int(CString strVal);
AFX_EXT_API extern long long Hex2Int64(CString strVal);
//==============================================================================

// String Format
AFX_EXT_API extern CString GetNumberString(int nValue, int nNumFraction);
AFX_EXT_API extern CString GetNumberString64(long long	nValue, int nNumFraction);

// Log 2
AFX_EXT_API extern int Log2_32(unsigned int nValue);
AFX_EXT_API extern int Log2_64(unsigned long long nValue);

#endif //_XUTIL_H_