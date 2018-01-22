@echo off
pushd "%~dp0\AnswerHelperServer"
call npm install mime
call npm install segment
call npm install cheerio
move ..\baidu-aip-sdk node_modules\
pushd node_modules\baidu-aip-sdk
call npm install || goto :EOF
popd
echo ok
echo.
pause 1>nul 2>nul