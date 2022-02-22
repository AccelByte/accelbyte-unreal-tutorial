@echo off

::REMEMBER TO SET ENGINE PATH

SET ENGINEPATH="<YourEnginePath>\Engine\Binaries\Win64\UE4Editor.exe"
SET UPROJECTPATH="<YourProjectPath>\TutorialProject\TutorialProject.uproject"

ECHO Lauching Client 1...
start "Client 1" %ENGINEPATH% %UPROJECTPATH% MainMenu -game -log -localds -ResX=1280 -ResY=720 -windowed -r.SetRes 1280x720w

ECHO Lauching Client 2...
start "Client 2" %ENGINEPATH% %UPROJECTPATH% MainMenu -game -log -localds -ResX=1280 -ResY=720 -windowed -r.SetRes 1280x720w

::Remove Comment Character below to test steam client - Make sure you are already login with steam platform application
::ECHO Lauching Client 3...
::start "Client 3" %ENGINEPATH% %UPROJECTPATH% MainMenu -game -steam -log -localds -ResX=1280 -ResY=720 -windowed -r.SetRes 1280x720w

ECHO Lauching the dedicated game server...
start "Local DS Test" %ENGINEPATH% %UPROJECTPATH% Game -server -log -localds

exit /B 0