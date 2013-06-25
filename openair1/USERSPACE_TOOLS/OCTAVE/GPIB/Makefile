# Maxime Guillaud - created Wed May 10 17:47:36 CEST 2006
# see http://etow.com/UsingGPIBOctaveLinux.htm

all:	gpibcmd querygpib


gpibcmd:	gpibcmd.c
	gcc gpibcmd.c -I/usr/include/gpib/ -o gpibcmd -lgpib


querygpib:	querygpib.c
	gcc querygpib.c -I/usr/include/gpib/ -o querygpib -lgpib

