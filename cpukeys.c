#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Only works for:
 *
 * cpu00632_plat00_ver00000020_1996-09-03_PRD_EBC16165.bin
 */
#define CPU_KEY_KLAMATH_A 0x30910229

/*
 * Only works for:
 *
 * cpu00690_plat01_ver00000004_2000-02-06_PRD_BE8FFBD9.bin
 * cpu00691_plat01_ver00000002_2000-02-07_PRD_E6DA1028.bin
 * cpu00692_plat10_ver00000003_2000-03-22_PRD_B675019E.bin
 * cpu00696_plat10_ver00000001_2000-07-07_PRD_99C6BF9B.bin
 */
#define CPU_KEY_PARTLY_WORKS 0x41af33f6

#define CPU_KEY_KLAMATH_B 0x3adb7701
#define CPU_KEY_DESCHUTES_A 0x3b021ce0
/* Works for overdrive cpu01632_plat00_ver00000002_1998-06-10_PRD_6AAE5598.bin as well */
#define CPU_KEY_DESCHUTES_B 0x17ae63a2
#define CPU_KEY_MOBILE_A 0x33c949f6
#define CPU_KEY_MOBILE_B 0x2eba562a
#define CPU_KEY_KATMAI_A 0x2d753ea0
#define CPU_KEY_KATMAI_B 0x72b97882
#define CPU_KEY_KATMAI_C 0x7bd10552
#define CPU_KEY_COPPERMINE_A 0x3942095e
#define CPU_KEY_COPPERMINE_B 0x28121d58
#define CPU_KEY_COPPERMINE_C 0x2cb473c4
#define CPU_KEY_COPPERMINE_B0 0x2e664bbb
#define CPU_KEY_BANIAS_A 0x1c514c40
#define CPU_KEY_CASCADES_A 0x6b8a374e
#define CPU_KEY_CASCADES_B 0x44d5346c
#define CPU_KEY_CASCADES_B0 0x6cfe00ab
#define CPU_KEY_MENDOCINO_A 0x4ef83ad6
#define CPU_KEY_DOTHAN_A0 0x63ff4b0d
#define CPU_KEY_DOTHAN_B1 0x61e342a6
#define CPU_KEY_DOTHAN_C0 0x51fc0681

uint32_t cpukeys_get_base( uint32_t cpu_sig ) {

	switch ( cpu_sig & 0xFFF ) {
		/* Probably different ucode patch format */
//		case 0x611:
//		case 0x612:
//		case 0x616:
//		case 0x617:
//		case 0x619:
		/* Obtaining keys for KLAMATH failed, likely microcode update has different format or FPROM is different */
//		case 0x630: /* Klamath */
//		case 0x632: /* Klamath */
//                      return CPU_KEY_KLAMATH_A;
		case 0x633: /* Klamath */
		case 0x634: /* Klamath */
			return CPU_KEY_KLAMATH_B;
		case 0x650: /* Deschutes */
		case 0x651: /* Deschutes */
			return CPU_KEY_DESCHUTES_A;
		case 0x652: /* Deschutes */
		case 0x653: /* Deschutes */
                        /* plus cpu01632 */
			return CPU_KEY_DESCHUTES_B;
		case 0x660: /* Dixon */
		case 0x66A: /* Dixon */
		case 0x66D: /* Dixon */
			return CPU_KEY_MOBILE_A;
		case 0x665:
			return CPU_KEY_MOBILE_B;
		case 0x670:
		case 0x671: /* katmai, tanner */
			return CPU_KEY_KATMAI_A;
		case 0x672: /* katmai, tanner */
			return CPU_KEY_KATMAI_B;
		case 0x673: /* katmai, tanner */
			return CPU_KEY_KATMAI_C;
		case 0x680: /* coppermine A2 */
		case 0x681: /* coppermine C0 */
			return CPU_KEY_COPPERMINE_A;
		case 0x683: /* coppermine B0 */
			 return CPU_KEY_COPPERMINE_B0;
		case 0x686:  /* coppermine C0 */
			return CPU_KEY_COPPERMINE_B;
		case 0x68a: /* coppermine D0 */
			return CPU_KEY_COPPERMINE_C;
//		case 0x690: /* unknown name */
//		case 0x691: /* Timna */
//		case 0x692: /* Timna */
//			return CPU_KEY_PARTLY_WORKS;
		case 0x694:  /* Banias */
		case 0x695:  /* Banias */
			return CPU_KEY_BANIAS_A;
//		case 0x696:
//			return CPU_KEY_PARTLY_WORKS;
		case 0x6a0: /* Cascades PIII xeon A0 */
		case 0x6a1: /* Cascades PIII xeon A1 */
			return CPU_KEY_CASCADES_A;
		case 0x6a4:  /* Cascades PIII xeon B0 */
                        return CPU_KEY_CASCADES_B0;
		case 0x6b0:
		case 0x6b1:
			return CPU_KEY_CASCADES_B;
		case 0x6b4: /* CPUID 0665H Medocino with PPGA/370 package */
			return CPU_KEY_MENDOCINO_A;
		case 0x6d0:  /* Dothan Processor A0 */
		case 0x6d1:  /* Dothan Processor A1 */
		case 0x6d2:  /* Dothan Processor A? */
	                return CPU_KEY_DOTHAN_A0;
		case 0x6d6:  /* Dothan Processor B1 */
			return CPU_KEY_DOTHAN_B1;
		case 0x6d8:  /* Dothan Processor C0 */
			return CPU_KEY_DOTHAN_C0;
		default:
			fprintf( stderr, "Unknown cpu key for CPUID: %03X\n",
			         cpu_sig & 0xFFF );
			exit( EXIT_FAILURE );
	}
}
