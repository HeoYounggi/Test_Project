#ifndef _VERSION_H_
#define _VERSION_H_

#include "Revision.h"

#define MAJOR_VER 0
#define MINOR_VER 1

#ifdef _DEBUG
#define MICRO_VER 0
#else
#if GIT_BRANCH_NAME == master
#define MICRO_VER 1
#elif GIT_BRANCH_NAME == hotfix
#define MICRO_VER 2
#else
#define MICRO_VER 0
#endif
#endif

// New Style
#define STR_VER(a, b, c, d, e) TEXT(#a) TEXT(".") TEXT(#b) TEXT(".") TEXT(#c) TEXT(".") TEXT(#d) TEXT("-") TEXT(#e) TEXT("\0")
#define STRING_VER(a, b, c, d, e) STR_VER(a, b, c, d, e)

#define STRING_VERSION STRING_VER(MAJOR_VER, MINOR_VER, MICRO_VER, GIT_COMMIT_COUNT, GIT_REVISION_SHA1)

// Old Style
#define STR_VER4(a, b, c, d ) TEXT(#a) TEXT(".") TEXT(#b) TEXT(".") TEXT(#c) TEXT(".") TEXT(#d) TEXT("\0")
#define STRING_VER4(a, b, c, d) STR_VER4(a, b, c, d)

#define STRING_VERSION4 STRING_VER4(MAJOR_VER, MINOR_VER, MICRO_VER, GIT_COMMIT_COUNT)

#endif //_VERSION_H_
