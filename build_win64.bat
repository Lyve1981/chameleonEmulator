set outdir=temp\cmake_win64\
cmake . -B %outdir% -G "Visual Studio 15 2017 Win64"
IF %ERRORLEVEL% NEQ 0 (
	popd 
	exit /B 2
)
pushd %outdir%
cmake --build . --config Debug
IF %ERRORLEVEL% NEQ 0 (
	popd 
	exit /B 2
)
popd
