make clean
make > make.out 2>&1


# if make fails, this will indicate errors, and the executable will be missing or from a past time

tail make.out
cp dist/Release/GNU-Linux-x86/lztd .
ls -l lztd

