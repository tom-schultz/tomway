set timestamp=%date:~4,2%%date:~7,2%%date:~10,4%_%time:~0,2%%time:~3,2%%time:~6,2%
start /B /D ..\x64\Perf ..\x64\Perf\tomway.exe
timeout /t 2 /nobreak
tracy-capture.exe -a 127.0.0.1 -s 10 -o tomway-%timestamp%.tracy
taskkill /im tomway.exe
tracy-csvexport.exe tomway-%timestamp%.tracy > tomway-%timestamp%.csv