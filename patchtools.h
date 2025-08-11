#ifndef __patchtools_h__
#define __patchtools_h__
#include "patchfile.h"

int fprom_exists( uint32_t addr );

uint32_t fprom_get( uint32_t addr );

uint32_t cpukeys_get_base( uint32_t proc_sig, uint32_t plat_id );

epatch_layout_t *get_epatch_layout(uint32_t proc_sig);

void encrypt_patch_body(
	uint32_t *out,
	const patch_body_t *in,
	uint32_t proc_sig,
	 uint32_t plat_id,
	uint32_t seed );

void decrypt_patch_body(
	patch_body_t *out,
	const uint32_t *in,
	uint32_t proc_sig,
	 uint32_t plat_id );

void dump_patch_header( const patch_hdr_t *hdr );

void dump_patch_body( const patch_body_t *body, epatch_layout_t *l );
                                                                     
void read_file(const char *path, void *data, size_t size);

void write_file(const char *path, const void *data, size_t size);

void write_patch_config(
	const patch_hdr_t *hdr,
	const patch_body_t *body,
	const char *filename,
	const char *msram_fn,
	uint32_t key_seed,
	epatch_layout_t *l);

void write_msram_file(
	const patch_body_t *body,
	const char *filename,
	int group_count );

void read_patch_config(
	patch_hdr_t *hdr,
	patch_body_t *body,
	const char *filename,
	char **msram_fnp,
	uint32_t *key_seed );

void read_msram_file( patch_body_t *body, const char *filename );

#endif
