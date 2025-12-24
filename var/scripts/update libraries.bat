REM --------------------------------------------------------------------------------------------------------------------------
REM Alle header bestanden copieren van AppRemoteCfg naar binding software...

REM set SRC_PROJECT_DIR="D:\developer\sw\desktop\dt\Engineering tool - AppRemoteCfg\AppRemoteCfg_v2\"


set SRC_PROJECT_DIR="..\..\src\DEPENDS\AppRemoteCfg_v2\"


xcopy %SRC_PROJECT_DIR%"src\dtapprcfgmfclib\apprcfg\*.h" "..\..\src\DTBindingSoftware\libraries\dtapprcfgmfclib\apprcfg\" /sy 
xcopy %SRC_PROJECT_DIR%"src\dtapprcfgmfclib\appremotecfg\*.h" "..\..\src\DTBindingSoftware\libraries\dtapprcfgmfclib\appremotecfg\" /sy 
xcopy %SRC_PROJECT_DIR%"src\dtapprcfgmfclib\httprcfg\*.h" "..\..\src\DTBindingSoftware\libraries\dtapprcfgmfclib\httprcfg\" /sy 
xcopy %SRC_PROJECT_DIR%"src\dtapprcfgmfclib\win32\*.h" "..\..\src\DTBindingSoftware\libraries\dtapprcfgmfclib\win32\" /sy 


REM --------------------------------------------------------------------------------------------------------------------------
REM Alle lib & pdb bestanden updaten: Niet deze van libCurl & OpenSSL

xcopy %SRC_PROJECT_DIR%"vs_2008\AppRemoteCfg_vs2008\Debug\*.lib" "..\..\src\DTBindingSoftware\Debug_lib\vs2008\" /sy
xcopy %SRC_PROJECT_DIR%"vs_2008\AppRemoteCfg_vs2008\Debug\*.pdb" "..\..\src\DTBindingSoftware\Debug\lib\vs2008\" /sy
xcopy %SRC_PROJECT_DIR%"vs_2008\AppRemoteCfg_vs2008\Release\*.lib" "..\..\src\DTBindingSoftware\Release_lib\vs2008\" /sy

xcopy %SRC_PROJECT_DIR%"vs_2015\AppRemoteCfg_vs2015\Debug\*.lib" "..\..\src\DTBindingSoftware\Debug_lib\vs2015\" /sy
xcopy %SRC_PROJECT_DIR%"vs_2015\AppRemoteCfg_vs2015\Debug\*.pdb" "..\..\src\DTBindingSoftware\Debug_lib\vs2015\" /sy
xcopy %SRC_PROJECT_DIR%"vs_2015\AppRemoteCfg_vs2015\Release\*.lib" "..\..\src\DTBindingSoftware\Release_lib\vs2015\" /sy

REM --------------------------------------------------------------------------------------------------------------------------
pause


