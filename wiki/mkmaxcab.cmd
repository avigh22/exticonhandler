@Echo Off
Title CAB文件夹最大压缩工具—无忧启动
Set Prestr=MAKECAB /D CompressionType=LZX /D CompressionMemory=21 /D MaxDiskSize=CDROM /D Cabinet=On /D Compress=On /D FolderSizeThreshold=5000000
Set dirfull=%~1
Set dirpath=%~dp1
Set dirname=%~nx1
Set Cmdstr=%Prestr% /D DiskDirectoryTemplate="%dirpath%." /D CabinetNameTemplate="%dirname%.cab"
Set Tempfile=%TEMP%\mkcabtmp.tmp
If Exist "%Tempfile%" Del "%Tempfile%" >NUL
SETLOCAL ENABLEDELAYEDEXPANSION
For /f "delims=" %%i In ('dir "%dirfull%" /s /b /a-d ^| find /v ".svn" ^| find /v "Thumbs.db" ') Do (
Set subname=%%i
Set "subname=!subname:%dirfull%\=!"
Echo "%%i" "%dirname%\!subname!">>%Tempfile%
)
ENDLOCAL
%Cmdstr% /F %Tempfile%
del setup.* /f
Set Tempfile=
Set Cmdstr=
Set dirname=
set dirpath=
Set dirfull=
Set Prestr=
