#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "patchfile.h"

void write_patch_config( 
	const patch_hdr_t *hdr, 
	const patch_body_t *body, 
	const char *filename,
	const char *msram_fn,
	uint32_t key_seed,
	epatch_layout_t *l) {
	FILE *file;
	int i;

	file = fopen(filename, "w");
	if ( !file ) {
		perror( "Could not open patch config output file" );
		exit( EXIT_FAILURE );
	}

	fprintf( file, "header_ver  0x%08X\n", hdr->header_ver );
	fprintf( file, "update_rev  0x%08X\n", hdr->update_rev );
	fprintf( file, "date_bcd    0x%08X\n", hdr->date_bcd );
	fprintf( file, "proc_sig    0x%08X\n", hdr->proc_sig );
	fprintf( file, "checksum    0x%08X\n", hdr->checksum );
	fprintf( file, "loader_rev  0x%08X\n", hdr->loader_ver );
	fprintf( file, "proc_flags  0x%08X\n", hdr->proc_flags );
	fprintf( file, "data_size   0x%08X\n", hdr->data_size );
	fprintf( file, "total_size  0x%08X\n", hdr->total_size );
	fprintf( file, "key_seed    0x%08X\n", key_seed );
	if (l->key_seed_offs == 1) {
	fprintf( file, "unknown     0x%08X\n", body->unknown );
	fprintf( file, "update_rev2 0x%08X\n", body->update_rev2 );
	}
	fprintf( file, "msram_file %s\n"    , msram_fn );

	for ( i = 0; i < l->cr_ops_count; i++ ) {
		fprintf( file,
		        "write_creg 0x%03X 0x%08X 0x%08X\n",
		        body->cr_ops[i].address,
		        body->cr_ops[i].mask,
		        body->cr_ops[i].value);	
	}

	fclose( file );
}

char line_buf[4096];

void read_patch_config(
	patch_hdr_t *hdr,
	patch_body_t *body,
	const char *filename,
	char **msram_fnp,
	uint32_t *key_seed ) {
	
	int i;
	char *par_n, *par_v, *par_v2, *par_v3;
	char *msram_fn;
	uint32_t addr, mask, data;
	FILE *file;
	msram_fn = NULL;

	file = fopen(filename, "r");
	if ( !file ) {
		perror( "Could not open patch config input file" );
		exit( EXIT_FAILURE );
	}
	
	i = 0;

	while ( fgets( line_buf, sizeof line_buf, file ) ) {
		par_n = strtok(line_buf, " ");
		if ( !par_n )
			continue;
		par_v = strtok(NULL, " \n");
		if ( !par_v ) {
			fprintf( stderr, 
				"Config key without value: \"%s\"\n", 
				par_n );
			exit( EXIT_FAILURE );
		}

		if ( strcmp( par_n, "header_ver" ) == 0 ) {
			hdr->header_ver = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "update_rev" ) == 0 ) {
			hdr->update_rev = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "date_bcd" ) == 0 ) {
			hdr->date_bcd = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "proc_sig" ) == 0 ) {
			hdr->proc_sig = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "checksum" ) == 0 ) {
			hdr->checksum = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "loader_rev" ) == 0 ) {
			hdr->loader_ver = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "proc_flags" ) == 0 ) {
			hdr->proc_flags = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "data_size" ) == 0 ) {
			hdr->data_size  = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "total_size" ) == 0 ) {
			hdr->total_size = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "key_seed" ) == 0 ) {
			*key_seed = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "unknown" ) == 0 ) {
			body->unknown = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "update_rev2" ) == 0 ) {
			body->update_rev2 = strtol( par_v, NULL, 0 );
		} else if ( strcmp( par_n, "msram_file" ) == 0 ) {
			msram_fn = strdup( par_v );
		} else if ( strcmp( par_n, "write_creg" ) == 0 ) {
			par_v2 = strtok(NULL, " \n");
			par_v3 = strtok(NULL, " \n");
			if ( !(par_v2 || par_v3) ){
				fprintf( stderr, "Incomplete write_creg\n" );
				exit( EXIT_FAILURE );
			}
			addr = strtol( par_v,  NULL, 0 );
			mask = strtol( par_v2, NULL, 0 );
			data = strtol( par_v3, NULL, 0 );
			if ( addr & ~0x3FF ) {
				fprintf( stderr, 
					"Invalid creg address: 0x%03X\n", 
					addr );
				exit( EXIT_FAILURE );
			}
			if ( i >= PATCH_CR_OP_COUNT_MAX ) {
				fprintf( stderr, 
					"Too many write_creg statements\n");
				exit( EXIT_FAILURE );
			}
			body->cr_ops[i].address = addr;
		        body->cr_ops[i].mask = mask;
		        body->cr_ops[i].value = data;
			i++;
		} else {
			fprintf( stderr, "Unknown config key \"%s\"\n", par_n );
			exit( EXIT_FAILURE );
		}
	}
	
	fclose( file );

	*msram_fnp = msram_fn;

}

void write_msram_file(
	const patch_body_t *body,
	const char *filename,
	epatch_layout_t *l )
{
	FILE *file;
	int i, addr;

	file = fopen(filename, "w");
	if ( !file ) {
		perror( "Could not open MSRAM output file" );
		exit( EXIT_FAILURE );
	}

	addr = l->msram_base;
	for ( i = 0; i < l->msram_dword_count; i++ ) {
		if ((i % l->msram_group_size) == 0) {
			fprintf(file, "%04X:", addr);
			addr += 8;
		}
		fprintf(file," %08X", body->msram[i]);
		if (((i % l->msram_group_size) == (l->msram_group_size - 1)) ||
		    (i == (l->msram_dword_count - 1)))  {
			fprintf(file,"\n");
		}
	}

	fclose( file );

}

void read_msram_file(
	patch_body_t *body,
	const char *filename,
	epatch_layout_t *l  )
{
	char *ts;
	FILE *file;
	int addr, raddr, group_size;
	int g;


	file = fopen(filename, "r");
	if ( !file ) {
		perror( "Could not open MSRAM input file" );
		exit( EXIT_FAILURE );
	}

	while ( fgets( line_buf, sizeof line_buf, file ) ) {
		ts = strtok(line_buf, ": ");
		if ( !ts )
			continue;
		addr = strtol( ts, NULL, 16 );
		if ( addr % 8 ) {
			fprintf( stderr, "Misaligned address in input :%08X\n",
				 addr );
			exit( EXIT_FAILURE );
		}
		if ( addr < l->msram_base ) {
			fprintf( stderr, 
				"Address not in MSRAM range :%08X\n",
				 addr );
			exit( EXIT_FAILURE );
		}
		/*
		 * Microcode addresses in PPro match as if there were 8 DWords per triplet
		 * Adjust the index on PPro
		 */
		raddr = (addr - l->msram_base) / 8;
		raddr *= l->msram_group_size;

		if (raddr >= l->msram_dword_count) {
			fprintf( stderr, 
				"Address not in MSRAM range :%08X\n", addr );
			exit( EXIT_FAILURE );
		}

		group_size = l->msram_group_size;

		if ((raddr + group_size) > l->msram_dword_count) {
			group_size = l->msram_dword_count - raddr;
		}

		for ( g = 0; g < group_size; g++ ) {
			ts = strtok(NULL, " ");
			if ( !ts ) {
				fprintf( stderr, 
					"Incomplete data for address %04X", 
					raddr );
				exit( EXIT_FAILURE );
			}
			assert((raddr + g) < l->msram_dword_count);
			body->msram[raddr + g] = strtol( ts, NULL, 16 );
		}
	
	}

	fclose( file );

}

