#include <stdio.h>
#include <string.h>
#include "patchfile.h"

void dump_patch_header( const patch_hdr_t *hdr ) {
	printf("Header version:  %08X\n", hdr->header_ver);
	printf("Update revision: %08X\n", hdr->update_rev);
	printf("Date:            %08X\n", hdr->date_bcd);
	printf("Processor sign.: %08X\n", hdr->proc_sig);
	printf("Checksum:        %08X\n", hdr->checksum);
	printf("Loader revision: %08X\n", hdr->loader_ver);
	printf("Processor flags: %08X\n", hdr->proc_flags);
	printf("Data size:       %08X\n", hdr->data_size);
	printf("Total size:      %08X\n", hdr->total_size);
}

void dump_patch_body( const patch_body_t *body, epatch_layout_t *l ) {
	uint32_t grp_or[MSRAM_GROUP_SIZE_MAX];
	int i,j;
	if (l->key_seed_offs == 1) {
		printf("Unknown:  %08X\n", body->unknown);
		printf("Version:  %08X\n", body->update_rev2);
	}
	printf("MSRAM: \n");
	memset( grp_or, 0, sizeof grp_or );

	for ( i = 0; i < l->msram_dword_count; i++ ) {
		if ((i % l->msram_group_size) == 0) {
			printf("\t%04X", l->msram_base + i);
		}
		printf(" %08X", body->msram[i]);
		grp_or[i % l->msram_group_size] |= body->msram[i];
		if (((i % l->msram_group_size) == (l->msram_group_size - 1)) ||
		    (i == (l->msram_dword_count - 1)))  {
			printf("\n");
		}
	}

	printf("\n\tOR :");
	for ( j = 0; j < l->msram_group_size; j++ ) {
		printf(" %08X", grp_or[j]);
	}

	printf("\nControl register ops: \n");
	for ( i = 0; i < l->cr_ops_count; i++ ) {
		printf("\tAddr: %08X  Mask: %08X  Value: %08X\n",
			body->cr_ops[i].address,
			body->cr_ops[i].mask,
			body->cr_ops[i].value);
	}
}
