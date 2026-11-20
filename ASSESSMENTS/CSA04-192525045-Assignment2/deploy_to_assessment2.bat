@echo off
echo ======================================================================
echo Deploying Complete Assignment 2 Files to Pictures\ASSESSMENT\ASSESSMENT2
echo ======================================================================
set SRC=C:\Users\SACHIN\CSA04-192525045-Assignment2
set DST=C:\Users\SACHIN\Pictures\ASSESSMENT\ASSESSMENT2

echo Source: %SRC%
echo Destination: %DST%
echo.

xcopy /E /I /Y "%SRC%\*" "%DST%\"

echo.
echo Deployment finished successfully!
pause
