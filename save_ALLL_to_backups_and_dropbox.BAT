set path_to_backup=C:
set DIR_to_backup=RTT_MLSSA
set DIR_to_save_in=c:\RTT_MLSSA
set TEMP_DIR=C:\temptemptemp_temptemptemp
DEL /F /Q RTT_MLSSA_*.*

RMDIR /S /Q %TEMP_DIR%
mkdir %TEMP_DIR%
robocopy /E %path_to_backup%\%DIR_TO_BACKUP% %TEMP_DIR%\%DIR_TO_BACKUP%
cd %TEMP_DIR%
REM RMDIR /S /Q %TEMP_DIR%\%DIR_TO_BACKUP%\bitmaps
"C:\Program Files\7-Zip\7z.exe" a "%DIR_to_backup%_%date:~-4,4%_%date:~-10,2%_%date:~-7,2%_%time:~0,2%h%time:~3,2%m_fh.7z" %DIR_TO_BACKUP%
RMDIR /S /Q %TEMP_DIR%\%DIR_TO_BACKUP%
cd ..
copy %TEMP_DIR%\* %DIR_to_save_in%
copy %TEMP_DIR%\* C:\Users\Administrator\Dropbox\.
mkdir D:\backups\%DIR_to_backup%
copy %TEMP_DIR%\* D:\backups\%DIR_to_backup%\.
mkdir E:\backups\%DIR_to_backup%
move %TEMP_DIR%\* E:\backups\%DIR_to_backup%\.
RMDIR /S /Q %TEMP_DIR%
timeout 50
 
