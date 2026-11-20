@echo off
REM =====================================================================
REM deploy_to_assessment3.bat - One-Click Deployment Script
REM Copies all Assessment 3 project files to C:\Users\SACHIN\Pictures\ASSESSMENT\ASSESSMENT3
REM Student: S. Sachin | Register Number: 192525045 | CSA04 CO3
REM =====================================================================

set SOURCE=C:\Users\SACHIN\CSA04-192525045-Assignment3
set DEST=C:\Users\SACHIN\Pictures\ASSESSMENT\ASSESSMENT3

echo =====================================================================
echo  DEPLOYING ASSESSMENT 3 FILES TO PICTURES FOLDER
echo  Source: %SOURCE%
echo  Destination: %DEST%
echo =====================================================================

if not exist "%DEST%" mkdir "%DEST%"

xcopy "%SOURCE%\*" "%DEST%\" /E /I /H /Y

echo.
echo =====================================================================
echo  DEPLOYMENT COMPLETED SUCCESSFULLY!
echo =====================================================================
pause
