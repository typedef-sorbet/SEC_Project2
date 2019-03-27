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

// 	STRUCTS
struct composition{
	// <byte1, byte2, byte3, byte4>
	unsigned char byte1;
	unsigned char byte2;
	unsigned char byte3;
	unsigned char byte4;
};

// TYPEDEFS
typedef uint64_t Num;
typedef union prime{
	struct composition discrete;
	Num concat;
}PreKey;

// DEFINES
#define COMPOSITE false
#define PRIME true

// FUNCTION DECLARATIONS
Num fastModExp(Num, Num, Num);
bool millerRabin(Num, Num);
bool witness(Num, Num);
bool isBitSet(Num, int);
int numLen(Num);
Num findPrime(Num);
PreKey randomNumber();

#endif