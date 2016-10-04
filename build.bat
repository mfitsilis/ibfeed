rem http://stackoverflow.com/questions/16821784/input-line-is-too-long-error-in-bat-file#19929778
Pushd "%~dp0"
if not defined DevEnvDir (
	CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
)
popd
nmake -f Makefile.win

copy /Y ibfeed.dll c:\q\w32
