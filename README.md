Program for encrypting microcode patches. Intially written by Peter Bosch
<public@pbx.sh> and and extended by Rudolf Marek <r.marek@assembler.cz>.

# Supported Hardware

This program can decrypt and encrypt microcode patches for most of the
Pentium Pro, Pentium II, Pentium III, Pentium M processors and some Core
Solo, Core Duo and Core 2 Solo and Core 2 Duo (Yonah, Merom, Penryn).

# Disclaimer
Patchfiles produced by this program might crash or damage the system they are
loaded onto and the authors takes no responsibility for any damages resulting
from use of the software.

Only public resources and publically available hardware were used by the
authors to produce this program.

# Key material
The program needs a 32 bit base key to work, the file cpukeys.c lists the
various unique keys used by certain CPU models.

The encryption support for Yonah, Merom and Penryn is WIP.

# MSRAM contents
The MSRAM contents are scrambled, and to edit them you need to descramble them.
An example implementation of this can be found at
https://github.com/peterbjornx/p6tools

# Usage
	patchtools [-dec] [-p <patch.dat>] [-i <config.txt>]


		-h                Print this message and exit

		-e                Extract a patch to a configuration and
		                  MSRAM hexdump file

		-c                Create a patch from a configuration and
		                  MSRAM hexdump file

		-d                Dump the patch contents and keys to the
		                  console after encrypting or decrypting.

		-p <patch.dat>    Specifies the path of the patchfile to
		                  create or decrypt. When encrypting this
		                  option is not required as the program
		                  will use the path of the configuration
		                  file to generate the output path.

		-i <config.txt>   Specifies the path of the config file
		                  to use or extract. When extracting this
		                  option is not required as the program
		                  will use the path of the patch file to
		                  generate the output path.


# More information
More information about the patch format can be found at
 https://twitter.com/peterbjornx/status/1321653489899081728

There are 2 known patch format families where the block size
of each element can further differ.

Lets begin with Pentium Pro, Pentium II, Pentium III and Pentium M.

The distributed patch size is 2048 bytes. The format described below is
encrypted with exception of seed and traps. It starts at file offset
0x30, right after documented header.

| Size | Description |
| ---- | ----------- |
|  4   | Seed Value |
|  4   | A trap, not read by the CPU, can be anything, must be skipped while decrypting |
|  N   | Start of MSRAM data patch, N = 592 bytes on Pentium Pro, else N = 672 bytes |
|  4   | Integrity value for MSRAM data |
|  4   | A trap, not read by the CPU, can be anything, must be skipped while decrypting |
|  256 | Start of CRBUS data |
| rest | The rest of the file is filled with sometimes same random data pattern |

The MSRAM data are stored in scrambled form. Each 16 bytes can be then unsrambled into 3 uops.

The CRBUS data consist of 16 entries, each of 16 byte.

| Size | Description      |
| ---- | ---------------- |
|  4   | CRBUS address    |
|  4   | data AND mask    |
|  4   | data OR mask     |
|  4   | integrity value  |

Entry with CRBUS address 0x1ff are ignored as it is likely a sink register.

The newer processors Yonah, Merom and Penryn use something else. The
distributed patch size is 4096 or 5120 bytes. The format described below
is encrypted with exception of seed and traps. It starts at file offset
0x30, right after documented header.

| Size | Description |
| ---- | ----------- |
|  4   | A trap, not read by the CPU, can be anything, must be skipped while decrypting |
|  4   | Seed Value |
|  4   | Currently unknown, likely some kind of checksum over data |
|  4   | Microcode revision, the bits 7:0 store microcode revision |
|  N   | Start of MSRAM data patch, different processors use different value of N, seen 1024, 2048 and 3072 bytes |
|  4   | Integrity value for MSRAM data |
|  4   | A trap, not read by the CPU, can be anything, must be skipped while decrypting |
|  M   | Start of CRBUS data, same format as above, different processors use different value of M, seen 928, 1024 |
| rest | The rest of the file is filled with sometimes same random data pattern |

Use the Source Luke. Documentation updates are welcome.
