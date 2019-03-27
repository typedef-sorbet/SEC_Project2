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
	uint16_t low;
	uint16_t high;
};

// TYPEDEFS
typedef uint64_t Num;
typedef union prime{
	struct composition discrete;
	Num concat;
}PreKey;

typedef struct key{
	Num p;
	Num g;
	Num d;
}Key;


// DEFINES
#define COMPOSITE false
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

#endif