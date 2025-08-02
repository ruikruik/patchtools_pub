#TODO: Enable -Wconversion one day...

SRCS_C =\
	patchfile.c \
	crypto.c \
	fprom.c \
	cpukeys.c \
	dump_patch.c \
	file_io.c \
	filefmt.c
CFLAGS +=-g -DUSE_C_BLOCKFUNC -O2 -Wall -Wextra

patchtools: $(SRCS_C)

clean:
	rm -f patchtools patchtools.exe
