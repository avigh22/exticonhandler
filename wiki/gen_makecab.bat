reg  add HKCR\Directory\shell\makecab /f
reg  add HKCR\Directory\shell\makecab /ve /d ���cab /f
reg  add HKCR\Directory\shell\makecab\command /f
reg  add HKCR\Directory\shell\makecab\command /ve /d "%~dp0mkmaxcab.cmd \"%%1\"" /f


exit
