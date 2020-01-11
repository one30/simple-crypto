// test_cipher.c
// Test program for chacha20 stream cipher.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "chacha20.h"

/* ----------------------------------------------------------------------------
	Local Prototypes
*/

static void dump_bytes_iter(uint8_t *bytes, int n, int width); 
static void dump_bytes(uint8_t *bytes, int n, int offset);

/* ----------------------------------------------------------------------------
	Main
*/

int main(void) {
	uint8_t key[] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
		0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
	};

	uint8_t nonce[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00
	};

	uint32_t ctr = 1;

	uint8_t pt[] = {
		0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c,  
		0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73,  
		0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63,  
		0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f,  
		0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20,  
		0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73,  
		0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69,  
		0x74, 0x2e
	};

	uint8_t ct[114];  // ciphertext buffer
	uint8_t nt[114];  // newtext buffer

	puts("[DEBUG] input key:");
	dump_bytes(key, 32, 0);
	puts("[DEBUG] input nonce:");
	dump_bytes(nonce, 12, 0);
	printf("[DEBUG] input block counter: %08x\n", ctr);

	// do encryption / decryption operations
	chacha20_cipher(ct, pt, 114, key, nonce, ctr);
	chacha20_cipher(nt, ct, 114, key, nonce, ctr);

	// how did we do?
	puts("[DEBUG] plaintext:");
	dump_bytes_iter(pt, 114, 16);
	puts("[DEBUG] ciphertext:");
	dump_bytes_iter(ct, 114, 16);
	puts("[DEBUG] newtext:");
	dump_bytes_iter(nt, 114, 16);

	return 0;
}

/* ----------------------------------------------------------------------------
	Local Functions
*/

// iteratively dump bytes at width 
static void dump_bytes_iter(uint8_t *bytes, int n, int width) {
	for (int i = 0; i < n; i += width) {
		for (int j = 0; (j < width) && (i + j < n); j++)
			printf("%02x ", bytes[i+j]);
		putchar('\n');
	}
}

// dump bytes in pretty-printed format
static void dump_bytes(uint8_t *bytes, int n, int offset) {
	for (int i = offset; i < offset + n; i++)
		printf("%02x ", bytes[i]);
	putchar('\n');
}
