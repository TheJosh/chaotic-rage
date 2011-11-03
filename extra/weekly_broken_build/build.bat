rmdir /S /Q files

del cr-wbb*.exe

mkdir files

"c:\WINDOWS\microsoft.net\Framework\v4.0.30319\MSBuild.exe" "..\msvc2010\Chaotic Rage.vcxproj"

copy ..\msvc2010\debug\chaoticrage.exe files\chaoticrage.exe

mkdir files\data
mkdir files\data\cr
mkdir files\data\cr\animmodels
mkdir files\data\cr\gametypes
mkdir files\data\cr\sounds
mkdir files\maps
mkdir files\maps\test
mkdir files\maps\blank

copy ..\..\data\cr\*.conf files\data\cr
copy ..\..\data\cr\animmodels\*.obj files\data\cr\animmodels
copy ..\..\data\cr\gametypes\*.lua files\data\cr\gametypes
copy ..\..\data\cr\sounds\*.wav files\data\cr\sounds
copy ..\..\maps\test\*.conf files\maps\test
copy ..\..\maps\blank\*.conf files\maps\blank

"C:\Program Files\NSIS\makensis.exe" weekly.nsi

pscp cr-wbb-*.exe josh@chaoticrage.com:/srv/http/chaoticrage/public_html/weekly_broken_build
