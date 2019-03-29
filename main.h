#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

// 	STRUCTS

struct composition{		// Structure for representing a 32-bit uunsigned int as two 16-bit unsigned ints. Makes random generation easier/faster.
	uint16_t low;			// Least significant bits
	uint16_t high;			// Most significant bits
};

// TYPEDEFS + STRUCTS/UNIONS

typedef uint64_t Num;				// Simple name mask for a 64-bit unsigned int.
typedef union prime{				// Union for modeling a 32-bit prime as either two seperate 16-bit ints, or one 64-bit number.
	struct composition discrete;		// Two seperate ints representation
	Num concat;							// One large int representation
}PreKey;

typedef struct key{		// Simple stucture for containing key data.
	Num p;					// Prime modulus p			
	Num g;					// Generator g (usually 2)
	Num d;					// If private key, secret d, if public key, value e2
}Key;

typedef union block{	// Union for modelling plaintext blocks as either a string of 4 characters, or one 32-bit int.
	char asChars[4];		// String representation
	uint32_t asInt;			// Integer representation
}Block;


// DEFINES

#define COMPOSITE false		// Name masks for return values in Miller-Rabin
#define PRIME true

// FUNCTION DECLARATIONS

Num fastModExp(Num, Num, Num);
bool millerRabin(Num, Num);
bool witness(Num, Num);
bool isBitSet(Num, int);
int numLen(Num);
Num findPrime(int);
PreKey randomNumber();
void keygen(int);
Num randBetween(Num, Num);
void encrypt(FILE *, FILE *);
void decrypt(FILE *, FILE*);
void printHelpMessage(char *);
void printUsage(char *);

#endif