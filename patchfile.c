#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "rotate.h"
#include "crypto.h"
#include "patchtools.h"
#include "patchfile.h"

#define ENCRYPT_MISSING_FPROM   (1)
#define ENCRYPT_OK              (0)
#define IV_KEY_INDEX_MASK       (0x9C)
#define INTEGRITY_INDEX_MASK    (0xFF)
#define CPUID_STEPPING_MASK     (0xF)

epatch_layout_t *get_epatch_layout(uint32_t proc_sig)
{
	static epatch_layout_t l;
	assert(proc_sig > 0x600);
	l.filesize = 2048;
	l.key_seed_offs = 0;
	l.msram_offs = 2;
	if (proc_sig < 0x630) {
		l.msram_dword_count = 0x4a * 2;
	} else {
		l.msram_dword_count = 0x54 * 2;
	}
	l.msram_integrity_offs = l.msram_offs + l.msram_dword_count;
	l.cr_ops_offs = l.msram_integrity_offs + 2;
	l.cr_ops_count = PATCH_CR_OP_COUNT_MAX;
	assert(l.filesize <= MAX_UF_SIZE);
	assert(l.msram_dword_count <= MSRAM_DWORD_COUNT_MAX);
	assert(l.cr_ops_count <= PATCH_CR_OP_COUNT_MAX);
	return &l;
}

/**
 * Decrypts and validates an integrity check word based on the current
 * encryption state, and exits with an error if it was unsuccessful.
 *
 * This function does a FPROM lookup and as such, might fail if the FPROM table
 * is not complete.
 *
 * @param ct_integ   The encrypted ICV to validate
 */
void decrypt_verify_integrity( uint32_t ct_integ ) {
	uint32_t integrity_idx, pt_integ, exp_integ;

	/* The ICV is derived from the crypto state before it is encrypted, so
	 * compute it first. The current state of the ciphermode is masked and
	 * indexed into the FPROM to get the check value */
	integrity_idx = crypto_getstate() & INTEGRITY_INDEX_MASK;

	/* Decrypt the ICV from the input */
	pt_integ = crypto_decrypt( ct_integ );

	/* Check that the FPROM entry used to derive the ICV is mapped in the
	 * program's table */
	if ( !fprom_exists( integrity_idx ) ) {
		/* Assuming correct decryption, this tells us a new FPROM table
		 * entry */
		fprintf( stderr,
		"Integrity check uses unknown FPROM[0x%02X] = 0x%08X\n",
		integrity_idx,
		pt_integ );
		return;
	}

	/* Compute the expected ICV */
	exp_integ = fprom_get( integrity_idx );

	/* Compare it against the stored, decrypted ICV */
	if ( pt_integ != exp_integ ) {
		/* Assuming correct decryption, this means our table was wrong*/
		fprintf( stderr,
		"Integrity check failed, got 0x%08X expected 0x%08X\n",
		pt_integ,
		exp_integ );
		exit( EXIT_FAILURE );
	}

}

/**
 * Generates an integrity check word based on the current encryption state, and
 * encrypts it.
 *
 * This function does a FPROM lookup and as such, might fail if the FPROM table
 * is not complete.
 *
 * @param status   Output parameter, ENCRYPT_OK when successful
 * @error          ENCRYPT_MISSING_FPROM : A location in the FPROM was ref'd
 * @return         The encrypted integrity check word
 */
uint32_t encrypt_generate_integrity( int *status ) {
	uint32_t integrity_idx, pt_integ;

	/* The ICV is derived from the crypto state before it is encrypted, so
	 * compute it first. The current state of the ciphermode is masked and
	 * indexed into the FPROM to get the check value */
	integrity_idx = crypto_getstate() & INTEGRITY_INDEX_MASK;

	/* Ensure that the FPROM table in the program contains this index */
	if ( !fprom_exists( integrity_idx ) ) {
		*status = ENCRYPT_MISSING_FPROM;
		return 0xFFFFFFFF;
	}

	/* Generate and encrypt the ICV */
	pt_integ = fprom_get( integrity_idx );

	*status = ENCRYPT_OK;
	return crypto_encrypt( pt_integ );

}

/**
 * Key derivation routine
 * @param iv       Output parameter for the derived Initialization Vector
 * @param key      Output parameter for the derived key.
 * @param proc_sig The CPUID/processor signature to derive the key for.
 * @param seed     The key seed used to derive an unique IV.
 * @return         ENCRYPT_OK when successful
 * @error          ENCRYPT_MISSING_FPROM : A location in the FPROM was ref'd
 *                 that was not correctly set in the
 */
int derive_key(
	uint32_t *iv,
	uint32_t *key,
	uint32_t proc_sig,
	uint32_t plat_id,
	uint32_t seed ) {

	uint32_t _iv, key_idx;

	/* The CPU base key is rotated by the stepping */
	_iv  = rotl32(
	             cpukeys_get_base( proc_sig, plat_id ),
	             proc_sig & CPUID_STEPPING_MASK );

	/* and has 6 plus the key seed added to it to form the IV */
	_iv += 6 + seed;

	*iv = _iv;

	/* The low 32 bits of the floating point constant ROM at the index
	 * given by masking the IV is used as the key/polynomial for the LFSR */

	if (proc_sig < 0x619) {
		key_idx = _iv & 0xff;
	} else {
		key_idx = _iv & IV_KEY_INDEX_MASK;
	}

	/* Ensure that the FPROM table in the program contains this index */
	if ( !fprom_exists( key_idx ) ) {
		return ENCRYPT_MISSING_FPROM;
	}

	*key = fprom_get( key_idx );

	return ENCRYPT_OK;
}

/**
 * Decrypts an encrypted microcode patch using a given IV and key
 * @param out      The buffer to write the decrypted patch body to.
 * @param in       The encrypted patch body to decrypt.
 * @param iv       The initialization vector to use.
 * @param key      The key to use.
 */
static void _decrypt_patch(
	patch_body_t *out,
	const uint32_t *in,
	uint32_t iv,
	uint32_t key,
	uint32_t proc_sig) {

	uint32_t integrity_idx;
	epatch_layout_t *l = get_epatch_layout(proc_sig);
	int i;

	/* Zero out the output buffer to prevent leaking memory contents */
	memset( out, 0, sizeof(patch_body_t) );

	/* Load the IV and key into the cipher implementation */
	crypto_init( key, iv );

	/* Decrypt the patch MSRAM contents */
	for ( i = 0; i < l->msram_dword_count; i++ ) {
		out->msram[i] = crypto_decrypt( in[i + l->msram_offs]);
	}

	/* Validate the patch MSRAM contents */
	decrypt_verify_integrity( in[l->msram_integrity_offs]);

	/* Decrypt the patch control register operations */
	for ( i = 0; i < l->cr_ops_count; i++ ) {
		/* Decrypt operation fields */
		out->cr_ops[i].address =
			crypto_decrypt(in[l->cr_ops_offs + 4 * i]);
		out->cr_ops[i].mask =
			crypto_decrypt(in[l->cr_ops_offs + 4 * i + 1]);
		out->cr_ops[i].value =
			crypto_decrypt(in[l->cr_ops_offs + 4 * i + 2]);

		/* Validate operation */
		decrypt_verify_integrity( in[l->cr_ops_offs + 4 * i + 3]);
	}

}

/**
 * Encrypts a patch body using a given processor signature and key seed.
 * Due to a possibly incomplete FPROM table not all seeds may be usable. If
 * the chosen seeds results in an unknown FPROM entry being used, this function
 * will report an error.
 *
 * @param out      The buffer to write the encrypted patch body to
 * @param in       The plaintext patch body
 * @param proc_sig The CPUID/processor signature to encrypt for
 * @param seed     The seed to be tried
 * @return         ENCRYPT_OK when successful
 * @error          ENCRYPT_MISSING_FPROM : A location in the FPROM was ref'd
 *                 that was not correctly set in the
 */
static int _encrypt_patch(
	uint32_t *out,
	const patch_body_t *in,
	uint32_t proc_sig,
	uint32_t plat_id,
	uint32_t seed ) {

	uint32_t integrity_idx, iv, key_idx, key;
	int i, status;
	epatch_layout_t *l = get_epatch_layout(proc_sig);

	out[l->key_seed_offs] = seed;

	/* Derive the IV and key */
	status = derive_key( &iv, &key, proc_sig, plat_id, seed);
	if ( status != ENCRYPT_OK )
		return status;

	/* Load the IV and key into the cipher implementation */
	crypto_init( key, iv );

	/* Encrypt the MSRAM contents */
	for ( i = 0; i < l->msram_dword_count; i++ ) {
		out[i + l->msram_offs] = crypto_encrypt( in->msram[i] );
	}

	/* Try to calculate ICV for the MSRAM */
	out[l->msram_integrity_offs] = encrypt_generate_integrity( &status );
	if ( status != ENCRYPT_OK )
		return status;

	/* Encrypt the control register operations */
	for ( i = 0; i < l->cr_ops_count; i++ ) {
		/* Encrypt operation fields */
		out[l->cr_ops_offs + 4 * i] =
			crypto_encrypt( in->cr_ops[i].address );
		out[l->cr_ops_offs + 4 * i + 1] =
			crypto_encrypt( in->cr_ops[i].mask );
		out[l->cr_ops_offs + 4 * i + 2] =
			crypto_encrypt( in->cr_ops[i].value );

		/* Try to generate control register op ICV */
		out[l->cr_ops_offs + 4 * i + 3] =
			encrypt_generate_integrity( &status );
		if ( status != ENCRYPT_OK )
			return status;
	}

	return ENCRYPT_OK;
}

/**
 * Encrypts a patch body using a given processor signature and key seed.
 * Due to a possibly incomplete FPROM table not all seeds may be usable, to
 * be able to encrypt arbitrary plaintext the program will search until it finds
 * a seed that does not result in any unknown FPROM locations being used for
 * integrity check or key generation.
 * @param out      The buffer to write the encrypted patch body to
 * @param in       The plaintext patch body
 * @param proc_sig The CPUID/processor signature to encrypt for
 * @param seed     The initial key seed to be tried
 */
void encrypt_patch_body(
	uint32_t *out,
	const patch_body_t *in,
	uint32_t proc_sig,
	uint32_t plat_id,
	uint32_t seed )
{
	while( _encrypt_patch( out, in, proc_sig, plat_id, seed ) != ENCRYPT_OK ) {
		seed++;
	}
}

/**
 * Decrypts an encrypted microcode patch using a given proc. sig and key seed.
 * @param out      The buffer to write the decrypted patch body to.
 * @param in       The encrypted patch body to decrypt
 * @param proc_sig The CPUID/processor signature to decrypt for
 */
void decrypt_patch_body(
	patch_body_t *out,
	const uint32_t *in,
	uint32_t proc_sig,
	uint32_t plat_id) {

	uint32_t iv, key_idx, key;
	epatch_layout_t *l = get_epatch_layout(proc_sig);

	/* Derive the IV and key */
	if ( derive_key( &iv, &key, proc_sig, plat_id,
			in[l->key_seed_offs] ) != ENCRYPT_OK ) {
		fprintf( stderr,
			"Patch file uses unknown FPROM[0x%02X] as key.",
			key_idx );
		return;
	}

	/* Actually decrypt the patch */
	_decrypt_patch( out, in, iv, key, proc_sig);

}
