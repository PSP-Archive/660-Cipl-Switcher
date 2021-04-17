PATH = c:/pspsdk/bin
make
psp-build-exports -s exports.exp
move kpspident.S ../kpspident.S
rm *.o *.elf *.sfo
pause