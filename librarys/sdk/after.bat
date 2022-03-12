set file=%0
for /f %%a in ("%file%") do (
set fpath=%%~dpa
)

copy %1 %fpath%

copy %2\*.h %fpath%inc\cortex-m4\