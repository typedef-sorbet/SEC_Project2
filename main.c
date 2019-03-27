#include "main.h"

int main()
{
	srand((unsigned int)time(NULL));
	printf("%" PRIu64 "\n", findPrime(71771));
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

	srand(n-2*s);
	srand(rand());
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
Num findPrime(Num seed)
{
	Num p;
	PreKey q;
	bool primeFound = false;
	while(!primeFound)
	{
		while(!millerRabin((q = randomNumber()).concat, 40));
		// q is now a prime number of length 32 bits
		printf("Found %" PRIu64 " to be prime\n", q.concat);
		if(q.concat % 12 == 5)
		{
			printf("Passes mod 12 test\n");
			p = 2 * q.concat + 1;
			if(millerRabin(p, 40))
			{
				printf("Found prime %" PRIu64 " with primitive root 2\n", p);
				primeFound = true;
				return p;
			}
		}
	}

	// this is just in case, the program should never get here
	return p;
}

PreKey randomNumber()
{
	PreKey number;
	number.discrete.byte1 = 128;	// ensure 32nd bit is set

	number.discrete.byte1 += rand() % 128;
	number.discrete.byte2 = rand() % 256;
	number.discrete.byte3 = rand() % 256;
	number.discrete.byte4 = rand() % 256;

	return number;
}