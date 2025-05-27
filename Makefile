SRCS_C =\
	patchfile.c \
	crypto.c \
	fprom.c \
	cpukeys.c \
	dump_patch.c \
	file_io.c \
	filefmt.c
CFLAGS +=-g -DUSE_C_BLOCKFUNC

patchtools: $(SRCS_C)

clean:
	rm -f patchtools
