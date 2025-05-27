#ifndef __crypto_h__
#define __crypto_h__

void crypto_init( uint32_t tmp4, uint32_t r34 );
uint32_t crypto_getstate( void );
uint32_t crypto_decrypt( uint32_t ciphertext );
uint32_t crypto_encrypt( uint32_t plaintext );

#endif
