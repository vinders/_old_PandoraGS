@echo off

:: Identify version file
set filename=build_version.txt
if not exist %filename% cd ..
if not exist %filename% (echo "%filename% not found in current directory nor in parent directory!" && exit /B 1)

:: Update version file
for /f "tokens=1* delims=:" %%a in ('findstr /n "^" "%filename%"') do if %%a leq 1 echo %%b>>tmp_version.txt
call git rev-list --count HEAD >> tmp_version.txt

del %filename%
ren tmp_version.txt %filename%

exit /B %errorlevel%
