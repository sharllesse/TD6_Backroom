SET "UE_EXE=C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor.exe"

SET "PROJECT=C:\Repos\Unreal Projects\TD6_Backroom\TD6_Backroom.uproject"

start "" "%UE_EXE%" "%PROJECT%" -game -windowed -ResX=1200 -ResY=900 -AUTH_TYPE=Developer -AUTH_LOGIN=127.0.0.1:8082 -AUTH_PASSWORD=Charles_Testing -log