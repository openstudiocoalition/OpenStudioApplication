@ECHO OFF
REM OpenStudioApp leaves orphan openstudio.exe in Debug mode, this kills them
taskkill /F /IM openstudio.exe /T
