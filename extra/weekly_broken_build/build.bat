rmdir /S /Q files

del cr-wbb*.exe

mkdir files

"c:\WINDOWS\microsoft.net\Framework\v4.0.30319\MSBuild.exe" "..\msvc2010\Chaotic Rage.vcxproj"

copy ..\msvc2010\debug\chaoticrage.exe files\chaoticrage.exe

mkdir files\data
mkdir files\data\cr
copy ..\..\data\cr\*.conf files\data\cr

"C:\Program Files\NSIS\makensis.exe" weekly.nsi

pscp cr-wbb-*.exe josh@chaoticrage.com:/srv/http/chaoticrage/public_html/weekly_broken_build
