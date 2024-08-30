call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"

pushd "%~dp0"

rem --- delete output files
IF NOT EXIST .\x64 (
    mkdir .\x64
)ELSE (
    del /Q .\x64\*
)

IF NOT EXIST .\x86 (
    mkdir .\x86
) ELSE (
    del /Q .\x86\*
)

rem --- Build Part
devenv ..\..\ACapPlayer.sln /rebuild "Release|x64"
devenv ..\..\ACapPlayer.sln /rebuild "Release|x86"

rem --- x64 Copy
xcopy /F /Y ..\..\OutputRel64\ACapPlayer.exe .\x64\
xcopy /F /Y ..\..\OutputRel64\Framework.dll .\x64\
xcopy /F /Y ..\..\OutputRel64\Packet.dll .\x64\
xcopy /F /Y ..\..\OutputRel64\wpcap.dll .\x64\
xcopy /F /Y ..\..\OutputRel64\ToolkitPro2210vc150x64U.dll .\x64\


rem --- x86 Copy
xcopy /F /Y ..\..\OutputRel32\ACapPlayer.exe .\x86\
xcopy /F /Y ..\..\OutputRel32\Framework.dll .\x86\
xcopy /F /Y ..\..\OutputRel32\Packet.dll .\x86\
xcopy /F /Y ..\..\OutputRel32\wpcap.dll .\x86\
xcopy /F /Y ..\..\OutputRel32\ToolkitPro2210vc150U.dll .\x86\


pause