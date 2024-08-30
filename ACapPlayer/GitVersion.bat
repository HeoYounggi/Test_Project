@echo off

set CURPATH=%cd%
set GIT="git.exe"
set OUTPUT=Revision.h
set CUR_BRANCH_NAME=develope
set CUR_REVISION_SHA1=0
set CUR_COMMIT_COUNT=0

echo %GIT%

for /f "tokens=*" %%i in ('%GIT% branch --show-current') do set CUR_BRANCH_NAME=%%i
for /f "tokens=*" %%i in ('%GIT% rev-parse --short^=10 HEAD') do set CUR_REVISION_SHA1=%%i
for /f "tokens=*" %%i in ('%GIT% rev-list --all --count') do set CUR_COMMIT_COUNT=%%i

echo %CUR_BRANCH_NAME%
echo %CUR_REVISION_SHA1%
echo %CUR_COMMIT_COUNT%

echo #define GIT_BRANCH_NAME %CUR_BRANCH_NAME% > %OUTPUT%
echo #define GIT_REVISION_SHA1 %CUR_REVISION_SHA1% >> %OUTPUT%
echo #define GIT_COMMIT_COUNT %CUR_COMMIT_COUNT% >> %OUTPUT%
