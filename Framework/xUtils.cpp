#include "pch.h"
//==============================================================================

#include "xUtils.h"
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================

CString GetExeName()
{
	TCHAR tszTemp[1024] = { 0, };
	DWORD dwLen = ::GetModuleFileName(NULL, tszTemp, sizeof(tszTemp));
	return CString(tszTemp, dwLen);
}
//==============================================================================

bool FileExists(CString strFileName)
{
	DWORD dwAttrib = ::GetFileAttributes(strFileName);
	return (dwAttrib != (DWORD)-1) &&
		((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0);
}
//==============================================================================

bool DirectoryExists(CString strDirName)
{
	DWORD dwAttrib = ::GetFileAttributes(strDirName);
	return (dwAttrib != (DWORD)-1) &&
		((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}
//==============================================================================

CString ExtractFileName(CString strFullPathName)
{
	int	nPos = strFullPathName.ReverseFind('\\');
	if (nPos == -1)
		return strFullPathName;

	return strFullPathName.Right(strFullPathName.GetLength() - nPos - 1);
}
//==============================================================================

CString ExtractFilePath(CString strFullPathName)
{
	int	nPos = strFullPathName.ReverseFind('\\');
	if (nPos == -1)
		return CString("");

	return strFullPathName.Left(nPos + 1);
}
//==============================================================================

CString ExtractFileDir(CString strFullPathName)
{
	int	nPos = strFullPathName.ReverseFind('\\');
	if (nPos == -1)
		return CString("");

	return strFullPathName.Left(nPos);
}
//==============================================================================

CString ExtractFileExt(CString strFileName)
{
	int	nPos = strFileName.ReverseFind('.');
	if (nPos == -1)
		return CString("");

	return strFileName.Right(strFileName.GetLength() - nPos - 1);
}
//==============================================================================

CString ChangeFileExt(CString strFileName, CString strExt)
{
	int	nPos = strFileName.ReverseFind('.');
	if (nPos == -1)
		return strFileName;

	return strFileName.Left(nPos + 1) + strExt;
}
//==============================================================================

void ForceDirectories(CString strDirName)
{
	if (DirectoryExists(strDirName))
		return;

	int	nPos = strDirName.ReverseFind('\\');
	if (nPos == -1)
		return;

	ForceDirectories(strDirName.Left(nPos));

	::CreateDirectory(strDirName, NULL);
}
//==============================================================================

CString Int2Str(int nVal)
{
	CString strResult;
	strResult.Format(TEXT("%d"), nVal);
	return strResult;
}
//==============================================================================

CString Int642Str(__int64 nVal)
{
	CString strResult;
	strResult.Format(TEXT("%0I64d"), nVal);
	return strResult;
}
//==============================================================================

int Str2Int(CString strValue)
{
	int	nlResult = 0;

	// Bin/hex decimal string
	if (strValue.GetLength() >= 2 && strValue[0] == TEXT('0'))
	{
		// Hex string
		if (strValue[1] == TEXT('x') || strValue[1] == TEXT('X'))
		{
			_stscanf_s(strValue, TEXT("%X"), &nlResult);
			return nlResult;
		}
		// Binary string
		if (strValue[1] == TEXT('b') || strValue[1] == TEXT('B'))
		{
			for (int i = 2; i < strValue.GetLength(); i++)
			{
				nlResult = (nlResult << 1) | (strValue[i] & 0x01); //(strValue[i] - '0');
			}

			return nlResult;
		}
	}

	// int string
	_stscanf_s(strValue, TEXT("%d"), &nlResult);

	return nlResult;
}
//==============================================================================

long long Str2Int64(CString strValue)
{
	long long nlResult = 0;

	// Bin/hex decimal string
	if (strValue.GetLength() >= 2 && strValue[0] == TEXT('0'))
	{
		// Hex string
		if (strValue[1] == TEXT('x') || strValue[1] == TEXT('X'))
		{
			_stscanf_s(strValue, TEXT("%I64X"), &nlResult);
			return nlResult;
		}
		// Binary string
		if (strValue[1] == TEXT('b') || strValue[1] == TEXT('B'))
		{
			for (int i = 2; i < strValue.GetLength(); i++)
			{
				nlResult = (nlResult << 1) | (strValue[i] & 0x01); //(strValue[i] - '0');
			}

			return nlResult;
		}
	}

	// int string
	_stscanf_s(strValue, TEXT("%I64d"), &nlResult);

	return nlResult;
}
//==============================================================================

float Str2Float(CString strValue)
{
	float fResult;

	_stscanf_s(strValue, TEXT("%f"), &fResult);

	return fResult;
}
//==============================================================================

double Str2Double(CString strValue)
{
	double dbResult;

	_stscanf_s(strValue, TEXT("%lf"), &dbResult);

	return dbResult;
}
//==============================================================================

CString Int2Hex(__int64 nVal)
{
	CString	strResult;
	strResult.Format(TEXT("%0X"), nVal);
	return strResult;
}
//==============================================================================

CString Int642Hex(long long nVal)
{
	CString	strResult;
	strResult.Format(TEXT("%0I64X"), nVal);
	return strResult;
}
//==============================================================================

// hex string --> int, strVal은 '0x' 가 없는 Hex String
int Hex2Int(CString strVal)
{
	int nlResult;

	strVal = TEXT("0x") + strVal;

	_stscanf_s(strVal, TEXT("%X"), &nlResult);

	return nlResult;
}
//==============================================================================

long long Hex2Int64(CString strVal)
{
	long long nlResult;

	strVal = TEXT("0x") + strVal;

	_stscanf_s(strVal, TEXT("%I64X"), &nlResult);

	return nlResult;
}
//==============================================================================

CString GetNumberString(int nValue, int nNumFraction)
{
	TCHAR szNumberIn[64];
	TCHAR szNumberOut[64];

	NUMBERFMT fmt;
	fmt.NumDigits = nNumFraction;
	fmt.LeadingZero = 1;
	fmt.Grouping = 3;
	fmt.lpDecimalSep = TEXT(".");
	fmt.lpThousandSep = TEXT(",");
	fmt.NegativeOrder = 1;

	_stprintf_s(szNumberIn, sizeof(szNumberIn) / sizeof(TCHAR), TEXT("%d"), nValue);
	::GetNumberFormat(NULL, NULL, szNumberIn, &fmt, szNumberOut, sizeof(szNumberOut) / sizeof(TCHAR));

	return szNumberOut;
}
//==============================================================================

CString GetNumberString64(long long nValue, int nNumFraction)
{
	TCHAR szNumberIn[64];
	TCHAR szNumberOut[64];

	NUMBERFMT fmt;
	fmt.NumDigits = nNumFraction;
	fmt.LeadingZero = 1;
	fmt.Grouping = 3;
	fmt.lpDecimalSep = TEXT(".");
	fmt.lpThousandSep = TEXT(",");
	fmt.NegativeOrder = 1;

	_stprintf_s(szNumberIn, sizeof(szNumberIn) / sizeof(TCHAR), TEXT("%lld"), nValue);
	::GetNumberFormat(NULL, NULL, szNumberIn, &fmt, szNumberOut, sizeof(szNumberOut) / sizeof(TCHAR));

	return szNumberOut;
}
//==============================================================================

int Log2_32(unsigned int nValue)
{
	const int tab32[32] = {
		0,  9,  1, 10, 13, 21,  2, 29,
		11, 14, 16, 18, 22, 25,  3, 30,
		8, 12, 20, 28, 15, 17, 24,  7,
		19, 27, 23,  6, 26,  5,  4, 31 };

	nValue |= nValue >> 1;
	nValue |= nValue >> 2;
	nValue |= nValue >> 4;
	nValue |= nValue >> 8;
	nValue |= nValue >> 16;

	return tab32[(nValue * 0x07C4ACDD) >> 27];
}
//==============================================================================

int Log2_64(unsigned long long nValue)
{
	const int tab64[64] = {
		63,  0, 58,  1, 59, 47, 53,  2,
		60, 39, 48, 27, 54, 33, 42,  3,
		61, 51, 37, 40, 49, 18, 28, 20,
		55, 30, 34, 11, 43, 14, 22,  4,
		62, 57, 46, 52, 38, 26, 32, 41,
		50, 36, 17, 19, 29, 10, 13, 21,
		56, 45, 25, 31, 35, 16,  9, 12,
		44, 24, 15,  8, 23,  7,  6,  5 };

	nValue |= nValue >> 1;
	nValue |= nValue >> 2;
	nValue |= nValue >> 4;
	nValue |= nValue >> 8;
	nValue |= nValue >> 16;
	nValue |= nValue >> 32;

	return tab64[(((nValue - (nValue >> 1)) * 0x07EDD5E59A4E28C2)) >> 58];
}
//==============================================================================
