@echo off
REM Quick start guide for the C Fullstack Menu Client

echo.
echo ╔════════════════════════════════════════════════════════╗
echo ║   C Fullstack Server - Menu Client Quick Start         ║
echo ╚════════════════════════════════════════════════════════╝
echo.
echo STEP 1: Building the project...
echo.
cd /d "%~dp0"
cmake --build build

echo.
echo STEP 2: Starting the server...
echo.
echo Server will run on port 8090
echo Press Ctrl+C in the server window to stop it
echo.
timeout /t 2

start "C Fullstack Server" .\build\c_fullstack_server.exe --db-config .\server\db.conf --port 8090

echo.
echo Waiting for server to start...
timeout /t 3

echo.
echo STEP 3: Starting the menu client...
echo.
echo Demo Credentials:
echo   Email: admin@example.com
echo   Password: password
echo.
echo   OR
echo.
echo   Email: player@example.com
echo   Password: password
echo.
.\build\menu_client.exe

pause
