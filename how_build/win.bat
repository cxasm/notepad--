cd build/bin

windeployqt notepad--.exe
@REM copy .\src\qscint\libqscint.a .\%1\plugin
@REM copy .\src\plugin\^*\^*.dll .\%1\plugin
