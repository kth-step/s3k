#pragma once
#include <stddef.h>
#include <stdint.h>

// Generate round keys
void aes128_keyexpansion(const uint32_t key[4], uint32_t rk[44]);

// Encryption provided round keys
void aes128_enc(const uint32_t rk[44], uint8_t buf[16]);

// Decryption provided round keys
void aes128_dec(const uint32_t rk[44], uint8_t buf[16]);

// CBC-MAC with AES.
void aes128_cbc_mac(const uint32_t rk[44], uint8_t *buf, uint8_t mac[16],
		    int len);

// Cipher block chaining encryption
void aes128_cbc_enc(const uint32_t rk[44], uint8_t iv[16], uint8_t *buf,
		    size_t len);

// Cipher block chaining decryption
void aes128_cbc_dec(const uint32_t rk[44], uint8_t iv[16], uint8_t *buf,
		    size_t len);
