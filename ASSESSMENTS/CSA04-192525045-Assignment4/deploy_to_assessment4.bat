@echo off
echo ==============================================================================
echo  CSA04 Operating Systems: Assessment 4 Deployment Utility
echo  Student: S. Sachin (192525045)
echo ==============================================================================

set SOURCE_DIR=C:\Users\SACHIN\CSA04-192525045-Assignment4
set TARGET_DIR=C:\Users\SACHIN\Pictures\ASSESSMENT\ASSESSMENT4

echo Source Directory: %SOURCE_DIR%
echo Target Directory: %TARGET_DIR%
echo.

if not exist "%SOURCE_DIR%" (
    echo Error: Source directory does not exist!
    pause
    exit /b 1
)

echo Creating target directory structure...
if not exist "%TARGET_DIR%" mkdir "%TARGET_DIR%"

echo Mirroring project files using Robocopy...
robocopy "%SOURCE_DIR%" "%TARGET_DIR%" /E /R:1 /W:1 /XD .git

echo.
echo ==============================================================================
echo  Deployment to %TARGET_DIR% completed successfully!
echo ==============================================================================
pause
