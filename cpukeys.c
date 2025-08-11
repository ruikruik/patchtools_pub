#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


/*
 * Only works for:
 *
 * cpu00630_plat00_ver00000013_1996-08-27_PRD_F316FC3B.bin
 *
 * Can be partially decrypted, fails integrity checks
 * possibly looks like the structure is different from
 * everything else.
 *
 */
#define CPU_KEY_KLAMATH_MYSTERY 0x159c0bac

#define CPU_KEY_PPRO_A0 0x715f1f2f
#define CPU_KEY_PPRO_B0 0x28d4fc58
#define CPU_KEY_PPRO_B1 0x61dab85e
#define CPU_KEY_KLAMATH_A 0x30910229
#define CPU_KEY_KLAMATH_B 0x3adb7701
#define CPU_KEY_DESCHUTES_A 0x3b021ce0
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
#define CPU_KEY_TIMNA_20 0x4b1615c5
#define CPU_KEY_TIMNA_10 0x41af33f6
#define CPU_KEY_BANIAS_A 0x1c514c40
#define CPU_KEY_CASCADES_A 0x6b8a374e
#define CPU_KEY_CASCADES_B 0x44d5346c
#define CPU_KEY_CASCADES_B0 0x6cfe00ab
#define CPU_KEY_MENDOCINO_A 0x4ef83ad6
#define CPU_KEY_DOTHAN_A0 0x63ff4b0d
#define CPU_KEY_DOTHAN_B1 0x61e342a6
#define CPU_KEY_DOTHAN_C0 0x51fc0681
#define CPU_KEY_TOLAPAI 0x1a915957
#define CPU_KEY_YONAH_A0 0x4fec1b47
#define CPU_KEY_YONAH_B0 0x048029bd
#define CPU_KEY_YONAH_C0 0x5b3f2dd8
#define CPU_KEY_YONAH_E0 0x4ec123aa
#define CPU_KEY_MEROM_A0 0x1abecade
#define CPU_KEY_MEROM_L2 0x287469cd
/* Similar to Conroe ?! */
#define CPU_KEY_MEROM_Bx 0x4f72bfa3
#define CPU_KEY_MEROM_Mx 0x956b4321
/* Why it is nearly same? */
#define CPU_KEY_CONROE_A0 0x4f72c0a3
#define CPU_KEY_CONROE_A1 0x4f72c023
/* Why it is nearly same? */
#define CPU_KEY_PENRYN_A0 0x78680efb
#define CPU_KEY_PENRYN_A1 0x78680e7b

uint32_t cpukeys_get_base( uint32_t cpu_sig, uint32_t plat_id ) {

	switch ( cpu_sig & 0xFFF3FFF ) {
		/* Probably different ucode patch format */
		case 0x611:
			return CPU_KEY_PPRO_B0;
		case 0x612:
		case 0x616:
		case 0x617:
			return CPU_KEY_PPRO_A0;
		case 0x619:
			return CPU_KEY_PPRO_B1;
/*		case 0x630: see above */
		case 0x632: /* Klamath */
                      return CPU_KEY_KLAMATH_A;
		case 0x633: /* Klamath */
		case 0x634: /* Klamath */
			return CPU_KEY_KLAMATH_B;
		case 0x650: /* Deschutes */
		case 0x651: /* Deschutes */
			return CPU_KEY_DESCHUTES_A;
		case 0x652: /* Deschutes */
		case 0x653: /* Deschutes */
		case 0x1632: /* Deschutes */
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
		case 0x690: /* Unknown */
		case 0x691: /* Timna */
		case 0x692: /* Timna */
		case 0x696: /* Unknown */
                        if (plat_id == 0x20) {
				return CPU_KEY_TIMNA_20;
			}
			return CPU_KEY_TIMNA_10;
		case 0x694:  /* Banias */
		case 0x695:  /* Banias */
			return CPU_KEY_BANIAS_A;
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
		case 0x6e0:
		case 0x6e1:  /* Yonah A0 ? */
			return CPU_KEY_YONAH_A0;
		case 0x6e4:  /* Yonah B0 */
			return CPU_KEY_YONAH_B0;
		case 0x6e8:  /* Yonah C0 */
			return CPU_KEY_YONAH_C0;
		case 0x6ec:  /* Yonah E0 */
			return CPU_KEY_YONAH_E0;
		/*   0x6ed: Yonah M0 has ext header */
		case 0x6f0:
		case 0x6f1:  /* Merom A0 ? */
			return CPU_KEY_MEROM_A0;
		case 0x6f2: /* Merom L2 */
			return CPU_KEY_MEROM_L2;
		case 0x6f4: /* Merom B0 */
		case 0x6f5: /* Merom B1 */
		case 0x6f6: /* Merom B2 */
		case 0x6f7: /* Merom B3 */
			return CPU_KEY_MEROM_Bx;
		/*   0x6f9: Merom Tigerton E0 has ext header */
		case 0x6fa: /* Merom E1 */
		/*   0x6fb: Merom Tigerton has ext header */
		case 0x6fd: /* Merom M0 / M1 */
			return CPU_KEY_MEROM_Mx;
		case 0x10650:
			return CPU_KEY_TOLAPAI;
		case 0x10660: /* Conroe A0 ? */
			return CPU_KEY_CONROE_A0;
		case 0x10661: /* Conroe A1 */
			return CPU_KEY_CONROE_A1;
		case 0x106D0: /* Penryn A0 ? */
			return CPU_KEY_PENRYN_A0;
		case 0x106D1:  /* Penryn A1 */
			return CPU_KEY_PENRYN_A1;
		default:
			fprintf( stderr, "Unknown cpu key for CPUID: %03X\n",
			         cpu_sig );
			exit( EXIT_FAILURE );
	}
}
