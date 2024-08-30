#include "pch.h"
//==============================================================================

#include "xUtilTAI.h"
//==============================================================================

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================

/* The leap second table is obtained from
* https://www.ietf.org/timezones/data/leap-seconds.list
* and generated from the file with the function
* generate_leap_second_tbl() below.
*/
static const size_t nleap_seconds = 28;
struct leap_second_t
{
	time_t posix, tai, delta;
};

struct leap_second_t latest_leap = {1483228800, 1483228837, 37};
//struct leap_second_t validity_limit = {1498608000, 1498608037, 37}; // Anders

// This Table expires on:  28 June 2017
static const struct leap_second_t leap_seconds[] = {
	{  63072000,   63072010,  10}, // 2272060800    10  # 1 Jan 1972
	{  78796800,   78796811,  11}, // 2287785600    11  # 1 Jul 1972
	{  94694400,   94694412,  12}, // 2303683200    12  # 1 Jan 1973
	{ 126230400,  126230413,  13}, // 2335219200    13  # 1 Jan 1974
	{ 157766400,  157766414,  14}, // 2366755200    14  # 1 Jan 1975
	{ 189302400,  189302415,  15}, // 2398291200    15  # 1 Jan 1976
	{ 220924800,  220924816,  16}, // 2429913600    16  # 1 Jan 1977
	{ 252460800,  252460817,  17}, // 2461449600    17  # 1 Jan 1978
	{ 283996800,  283996818,  18}, // 2492985600    18  # 1 Jan 1979
	{ 315532800,  315532819,  19}, // 2524521600    19  # 1 Jan 1980
	{ 362793600,  362793620,  20}, // 2571782400    20  # 1 Jul 1981
	{ 394329600,  394329621,  21}, // 2603318400    21  # 1 Jul 1982
	{ 425865600,  425865622,  22}, // 2634854400    22  # 1 Jul 1983
	{ 489024000,  489024023,  23}, // 2698012800    23  # 1 Jul 1985
	{ 567993600,  567993624,  24}, // 2776982400    24  # 1 Jan 1988
	{ 631152000,  631152025,  25}, // 2840140800    25  # 1 Jan 1990
	{ 662688000,  662688026,  26}, // 2871676800    26  # 1 Jan 1991
	{ 709948800,  709948827,  27}, // 2918937600    27  # 1 Jul 1992
	{ 741484800,  741484828,  28}, // 2950473600    28  # 1 Jul 1993
	{ 773020800,  773020829,  29}, // 2982009600    29  # 1 Jul 1994
	{ 820454400,  820454430,  30}, // 3029443200    30  # 1 Jan 1996
	{ 867715200,  867715231,  31}, // 3076704000    31  # 1 Jul 1997
	{ 915148800,  915148832,  32}, // 3124137600    32  # 1 Jan 1999
	{1136073600, 1136073633,  33}, // 3345062400    33  # 1 Jan 2006
	{1230768000, 1230768034,  34}, // 3439756800    34  # 1 Jan 2009
	{1341100800, 1341100835,  35}, // 3550089600    35  # 1 Jul 2012
	{1435708800, 1435708836,  36}, // 3644697600    36  # 1 Jul 2015
	{1483228800, 1483228837,  37}, // 3644697600    37  # 1 Jan 2017
};
//==============================================================================

time_t UTC2TAI(time_t tUTC)
{
	if (tUTC >= latest_leap.posix)
	{
		//return tUTC < validity_limit.posix ? tUTC + latest_leap.delta : 0;
		return tUTC + latest_leap.delta; // Anders. 테이블 업데이트 되지 않았을 경우에도 마지막값 처리.
	}

	size_t n = (sizeof(leap_seconds) / sizeof(struct leap_second_t)) - 1, i = 0;

	while (n > 1)
	{
		size_t s = n / 2, j = i + s;
		if (tUTC < leap_seconds[j].posix)
		{
			n = s;
		}
		else
		{
			i = j;
			n -= s;
		}
	}

	return tUTC + leap_seconds[i].delta;
}
//==============================================================================

time_t TAI2UTC(time_t tTAI)
{
	if (tTAI > latest_leap.tai)
	{
		//return tTAI < validity_limit.tai ? tTAI - latest_leap.delta : 0;
		return tTAI - latest_leap.delta; // Anders. 테이블 업데이트 되지 않았을 경우에도 마지막값 처리.
	}

	size_t n = (sizeof(leap_seconds) / sizeof(struct leap_second_t)) - 1, i = 0;
	
	while (n > 1)
	{
		size_t s = n / 2, j = i + s;
		if (tTAI < leap_seconds[j].tai)
		{
			n = s;
		}
		else
		{
			i = j;
			n -= s;
		}
	}

	return tTAI - leap_seconds[i].delta;
}
//==============================================================================
