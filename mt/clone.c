// clone.c 
// Cryptopals Set 3, Challenge 23: Clone an MT19337 RNG from its output.
//
// WEAKNESS of this current implementation:
// assumes that the first value we tap from the PRNG instance 
// is also the first value generated by the instance since either 
// its inception or its most recent twist() operation
// in a more realistic scenario, such as in a remote environment, 
// this assumption would be invalid
// to make the attack more robust, we would need to tap the real 
// PRNG instance for more values, 1247 (624*2-1) in the worst case
// from there, we could attempt all of the internal state 'ranges'
// within this vector of collected values (not that hard) to determine 
// where in the sequence we actually entered the conversation, and 
// complete the clone operation
//
// QUESTION: is this attack still possible if another party is simultaneously 
// tapping the PRNG for values? 


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "mt.h"

/* ----------------------------------------------------------------------------
	Local Constants 
*/

// constant values for MT19337
#define MT_COEFF_W 32
#define MT_COEFF_N 624
#define MT_COEFF_M 397
#define MT_COEFF_R 31

#define MT_COEFF_A 0x9908B0DF

#define MT_COEFF_U 11
#define MT_COEFF_D 0xFFFFFFFF

#define MT_COEFF_S 7
#define MT_COEFF_B 0x9D2C5680

#define MT_COEFF_T 15
#define MT_COEFF_C 0xEFC60000

#define MT_COEFF_F 1812433253

#define MT_COEFF_L 18


/* ----------------------------------------------------------------------------
	Global Variables 
*/

static const uint32_t SEED = 1;

/* ----------------------------------------------------------------------------
	Local Prototypes 
*/

static uint32_t untemper(uint32_t val); 
static uint32_t untemper_r(uint32_t r, int shift, uint32_t mask);
static uint32_t untemper_l(uint32_t r, int shift, uint32_t mask);

/* ----------------------------------------------------------------------------
	Main
*/

int main(int argc, char *argv[]) {
	// create the original PRNG instance
	mt_t real;
	mt_seed(&real, SEED); 

	// tap instance for 624 values, reconstruct internal state 
	uint32_t state[624]; 
	for (int i = 0; i < 624; i++) 
		state[i] = untemper(mt_rand(&real));

	// create a new PRNG instance by splicing in reconstructed state
	mt_t clone;
	mt_splice(&clone, state); 

	// determine if clone was successful:

	// tap real PRNG instance for 5 values
	printf("real:  ");
	for (int i = 0; i < 5; i++)
		printf("%08x ", mt_rand(&real));
	putchar('\n');

	// tap cloned PRNG instance for 5 values
	printf("clone: ");
	for (int i = 0; i < 5; i++)
		printf("%08x ", mt_rand(&clone));
	putchar('\n');

	return 0;
}


/* ----------------------------------------------------------------------------
	Local Functions 
*/

// untemper the value returned by MT to recover original state value
static uint32_t untemper(uint32_t r) {          // inverting:
	r = untemper_r(r, MT_COEFF_L, 0xFFFFFFFF);  // y = y ^ (y >> MT_COEFF_L);
	r = untemper_l(r, MT_COEFF_T, MT_COEFF_C);  // y = y ^ ((y << MT_COEFF_T) & MT_COEFF_C);
	r = untemper_l(r, MT_COEFF_S, MT_COEFF_B);  // y = y ^ ((y << MT_COEFF_S) & MT_COEFF_B);
	r = untemper_r(r, MT_COEFF_U, MT_COEFF_D);  // y = y ^ ((y >> MT_COEFF_U) & MT_COEFF_D);
	return r; 
}

// invert 'right' tempering operation 
static uint32_t untemper_r(uint32_t r, int shift, uint32_t mask) {
	int i = 1;
	uint32_t submask = (0xFFFFFFFF << (32 - shift));

	// iterate until all 32 bits of value recovered 
	for (int recovered = shift; recovered < 32; recovered += shift) 
		r ^= ((r >> shift) & mask) & (submask >> shift*i++);
	
	return r; 
}

// invert 'left' tempering operation 
static uint32_t untemper_l(uint32_t r, int shift, uint32_t mask) {
	int i = 1;
	uint32_t submask = ~(0xFFFFFFFF << shift);

	// iterate until all 32 bits of value recovered 
	for (int recovered = shift; recovered < 32; recovered += shift) 
		r ^= ((r << shift) & mask) & (submask << shift*i++);
	
	return r; 
}