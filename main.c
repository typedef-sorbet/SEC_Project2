#include "main.h"

int main()
{
	// srand((unsigned int)time(NULL));
	keygen(-1);
}

//	Written using the pseudocode from the Algorithms class book.
Num fastModExp(Num a, Num b, Num n)
{
	Num c = 0;
	Num d = 1;
	for(int i = numLen(b)-1; i >= 0; i--)
	{
		c *= 2;
		d = (d*d) % n;
		if(isBitSet(b, i))
		{
			c++;
			d = (d*a) % n;
		}
	}
	
	return d;
}

//	Given some number n = <bm, bm-1, ..., b2, b1, b0>,
//	this function will return true if bi is equal to 1.
// 	This function assumes that i is within [0, m].
bool isBitSet(Num n, int i)
{
	return ((n >> i) & 0x01) == 1;
}

//	This function returns the number of significant bits
//	in the binary representation of n.
//	If n somehow is represented by more bits than n can hold as
// 	a uint64_t (which will never happen), this function returns -1.
int numLen(Num n)
{
	for(int i = 0; i < 8*sizeof(Num); i++)
	{
		if((n >> i) == ((Num)0))
			return i;
	}
	return -1;
}

//	Implements the Miller-Rabin primality testing algorithm with s random witness tests.
//	Returns COMPOSITE (false) if the number is definitely not prime
//	via the witness test, and PRIME (true) if the number is probably prime,
// 	that is, none of the witness tests proved that it is not prime.
bool millerRabin(Num n, Num s)
{
	if(n == 2)
		return PRIME;
	else if(n%2 == 0)
		return COMPOSITE;

	for(Num i = 0; i < s; i++)
	{
		Num a = ((Num)rand() % (n-3)) + 2;
		if(witness(a,n))
			return COMPOSITE;
	}
	return PRIME;
}

//	Witness test to be used in the above Miller-Rabin alg.s.
//	Used to check if the number n is definitely composite, or needs
//	further checking.
//	Returns true if n is definitely composite, false if it might be prime.
bool witness(Num a, Num n)
{
	// n assumed to be odd, because why do a primality check on an even number?
	// let t and u be such that t >= 1, u is odd, and n-1 = u*2^t
	// that is, find t and u such that n-1 = u plus t extra trailing zeros
	Num u = (n - 1) >> 1, t = (Num)1;
	while(u % 2 == 0)
	{
		u = u >> 1;
		t++;
	}

	Num current, prev;
	current = fastModExp(a,u,n);
	for(Num i = 0; i < t; i++)
	{
		prev = current;
		current = (prev * prev) % n;
		if(current == 1 && prev != 1 && prev != n-1)
			return true;
	}
	if(current != 1)
		return true;
	else
		return false;
}

// KEYGEN STUFF

/*
	working backwards from generator to prime:
	
	starting with g = 2, find a prime that works
	alg:
		select random k-1 bit prime q such that q mod 12 = 5
		compute p=2q-1 and test for primality
		repeat until p is prime
*/

// Generates a 33-bit long prime p with primitive root 2.
// This number will be the number p in both keys.
// Each key will have a g of 2, as guaranteed by this prime-finding process.
Num findPrime(int seed)
{
	if(seed > 0)
		srand(seed);
	else
		srand((unsigned int)time(NULL));

	Num p;
	PreKey q;
	bool primeFound = false;
	while(!primeFound)
	{
		while(!millerRabin((q = randomNumber()).concat, 40));
		// q is now a prime number of length 32 bits
		if(q.concat % 12 == 5)
		{
			p = 2 * q.concat + 1;
			if(millerRabin(p, 40))
			{
				primeFound = true;
				return p;
			}
		}
	}

	// this is just in case, the program should never get here
	return p;
}

// Randomly generates an odd PreKey of length 31 bits.
PreKey randomNumber()
{
	PreKey number;

	number.discrete.high = 16384;

	number.discrete.high ^= ((uint16_t)rand()) % 16384;		// 15 bits
	number.discrete.low = ((uint16_t)rand()) % 65536;		// 16 bits

	if((number.discrete.low & 0x01) == 0)
		number.discrete.low ^= 0x01;

	return number;
}

void keygen(int seed)
{
	Num p = findPrime(seed);
	Num g = 2;
	Num d = randBetween(1, p);
	Num e2 = fastModExp(g, d, p);

	Key publicKey, privateKey;
	publicKey.p = privateKey.p = p;
	publicKey.g = privateKey.g = g;
	publicKey.d = e2;
	privateKey.d = d;

	printf("Public Key Info: p: %" PRIu64 " g: %" PRIu64 " e2: %" PRIu64 "\n", publicKey.p, publicKey.g, publicKey.d);
	printf("Private Key Info: p: %" PRIu64 " g: %" PRIu64 " d: %" PRIu64 "\n", privateKey.p, privateKey.g, privateKey.d);

	
}

Num randBetween(Num low, Num high)
{
	// returns some number is [low, high)
	return low + ((Num)rand() % (high - low));
}

