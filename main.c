#include "main.h"

int main(int argc, char *argv[])
{
	/*
		Usage:
			./main keygen seed
			./main encrypt key.txt plaintext
			./main decrypt key.txt ciphertext
			./main --help
	*/
	char *mode;
	switch(argc)
	{
		case 2:
			if(strstr(argv[1], "help") != NULL)
			{
				printHelpMessage(argv[0]);
				exit(0);
			}
			else
			{
				fprintf(stderr, "Error: Invalid arguments given to %s\n", argv[0]);
				printUsage(argv[0]);
				exit(1);
			}
			break;
		case 3:
			mode = argv[1];
			if(strstr(mode, "keygen") != NULL)
			{
				keygen(atoi(argv[2]));
			}
			else
			{
				fprintf(stderr, "Error: Invalid arguments given to %s\n", argv[0]);
				printUsage(argv[0]);
				exit(1);	
			}
			break;
		case 4:
			mode = argv[1];
			if(strstr(mode, "encrypt") != NULL)
			{
				// attempt to open relevant files
				char *keyPath = argv[2];
				char *plainPath = argv[3];

				FILE *keyFile = fopen(keyPath, "r");
				FILE *plainFile = fopen(plainPath, "r");

				if(keyFile == NULL || plainPath == NULL)
				{
					fprintf(stderr, "Error: unable to open one or more of the specified files. Exiting...\n");
					exit(1);
				}

				encrypt(plainFile, keyFile);
			}
			else if(strstr(mode, "decrypt") != NULL)
			{
				// attempt to open relevant files
				char *keyPath = argv[2];
				char *cipherPath = argv[3];

				FILE *keyFile = fopen(keyPath, "r");
				FILE *cipherFile = fopen(cipherPath, "r");

				if(keyFile == NULL || cipherPath == NULL)
				{
					fprintf(stderr, "Error: unable to open one or more of the specified files. Exiting...\n");
					exit(1);
				}

				decrypt(cipherFile, keyFile);
			}
			else
			{
				fprintf(stderr, "Error: Invalid arguments given to %s\n", argv[0]);
				printUsage(argv[0]);
				exit(1);	
			}
			break;
		default:
			fprintf(stderr, "Error: Invalid arguments given to %s\n", argv[0]);
			printUsage(argv[0]);
			exit(1);
			break;
	}
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

//	Witness test to be used in the above Miller-Rabin alg.
//	Used to check if the number n is definitely composite, or needs further checking.
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
	algorithm to find prime with primitive root 2:
		select random k-1 bit prime q such that q mod 12 = 5
		compute p=2q-1 and test for primality
		repeat until p is prime
*/

// Generates a 32-bit long prime p with primitive root 2.
// This number will be the number p in both keys.
// Each key will have a g of 2, as guaranteed by this prime-finding process.
Num findPrime(int seed)
{
	// Seed the RNG with either the current time or the given number
	if(seed > 0)
		srand(seed);
	else
		srand((unsigned int)time(NULL));

	Num p;
	PreKey q;
	bool primeFound = false;

	// while we haven't found a prime...
	while(!primeFound)
	{
		while(!millerRabin((q = randomNumber()).concat, 40));	// randomly generate a PreKey q and check for primality
		// q is now a prime number of length 31 bits
		// if q % 12 == 5 and p = 2q + 1 is prime, we're done
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

	number.discrete.high = 16384;							// ensure that 31st bit is high

	number.discrete.high ^= ((uint16_t)rand()) % 16384;		// random 15 bit #
	number.discrete.low = ((uint16_t)rand()) % 65536;		// random 16 bit #

	// if number is even, add one to it
	if((number.discrete.low & 0x01) == 0)
		number.discrete.low ^= 0x01;

	return number;
}

// Generates public and private keys based upon the seed given.
void keygen(int seed)
{
	Num p = findPrime(seed);			// generate a prime modulus p such that p has a primitive root of 2
	Num g = 2;
	Num d = randBetween(1, p);			// d = random number on [1, p) (for private key)
	Num e2 = fastModExp(g, d, p);		// e2 = g^d mod p (for public key)

	Key publicKey, privateKey;
	publicKey.p = privateKey.p = p;		// private and public share a p and a g
	publicKey.g = privateKey.g = g;
	publicKey.d = e2;					// the third field in each key is called d for simplicity
	privateKey.d = d;					// but bear in mind they are not the same thing

	printf("Public Key Info: p: %" PRIu64 " g: %" PRIu64 " e2: %" PRIu64 "\n", publicKey.p, publicKey.g, publicKey.d);
	printf("Private Key Info: p: %" PRIu64 " g: %" PRIu64 " d: %" PRIu64 "\n", privateKey.p, privateKey.g, privateKey.d);

	// write out key info to file

	FILE *publicFile = fopen("pubkey.txt", "w");
	FILE *privateFile = fopen("prikey.txt", "w");

	fprintf(publicFile, "%" PRIu64 " %" PRIu64 " %" PRIu64 "\n", publicKey.p, publicKey.g, publicKey.d);
	fprintf(privateFile, "%" PRIu64 " %" PRIu64 " %" PRIu64 "\n", privateKey.p, privateKey.g, privateKey.d);

	fclose(publicFile);
	fclose(privateFile);
}

// Generates a random number on the interval [low, high)
Num randBetween(Num low, Num high)
{
	// returns some number on [low, high)
	return low + ((Num)rand() % (high - low));
}

// EN/DECRYPTION STUFF

// Encrypts the file inFile using the key info found in keyFile.
void encrypt(FILE *inFile, FILE *keyFile)
{
	Block blockBuf;				// Stores plaintext blocks
	Num p, g, d;				// Key info
	int numKeyPartsRead = 0;	// Scanf sanity check variable
	
	// read in key info
	numKeyPartsRead += fscanf(keyFile, "%" SCNu64 "", &p);
	numKeyPartsRead += fscanf(keyFile, "%" SCNu64 "", &g);
	numKeyPartsRead += fscanf(keyFile, "%" SCNu64 "", &d);

	// if we didn't get all the info we needed, quit out
	if(numKeyPartsRead < 3)
	{
		fprintf(stderr, "Error: Unable to fully read key. (Needed 3 parts, got %d)\nExiting...\n", numKeyPartsRead);
		exit(1);
	}

	// open up the output file
	FILE *outFile = fopen("ciphertext.txt", "w");

	// if we couldn't open the output file, quit out
	if(outFile == NULL)
	{
		fprintf(stderr, "Error: unable to open ciphertext.txt for writing.\nExiting...\n");
		exit(1);
	}

	// read in plaintext blocks until we hit EOF
	while(fgets(blockBuf.asChars, 4, inFile) != NULL)
	{
		// each plaintext block is essentially a 31 bit number
		// we have the block as chars and as an int
		Num k = randBetween(0, p);
		Num c1 = fastModExp(g, k, p);
		Num c2 = (fastModExp(d, k, p) * blockBuf.asInt) % p;

		printf("Generated ciphertext block (%" PRIu64 ", %" PRIu64 ")\n", c1, c2);
		fprintf(outFile, "%" PRIu64 " %" PRIu64 "\n", c1, c2);
	}

	fclose(outFile);
	printf("Encryption complete.\nCiphertext written to ciphertext.txt in current directory.\n");
}

// Decrypts the data in inFile using the key data found in keyFile.
void decrypt(FILE *inFile, FILE *keyFile)
{
	char lineBuf[120];			// Buffer for ciphertext lines
	Block blockBuf;				// Buffer for plaintext blocks
	Num c1, c2;					// Buffers for ciphertext values
	Num p, g, d;				// Key info
	int numKeyPartsRead = 0;	// Scanf sanity check variable
	
	// read in key info
	numKeyPartsRead += fscanf(keyFile, "%" SCNu64 "", &p);
	numKeyPartsRead += fscanf(keyFile, "%" SCNu64 "", &g);
	numKeyPartsRead += fscanf(keyFile, "%" SCNu64 "", &d);

	// if we didn't get all the info we needed, quit out
	if(numKeyPartsRead < 3)
	{
		fprintf(stderr, "Error: Unable to fully read key. (Needed 3 parts, got %d)\nExiting...\n", numKeyPartsRead);
		exit(1);
	}

	// open up the output file
	FILE *outFile = fopen("plaintext.txt", "w");

	// if we couldn't open the output file, quit out
	if(outFile == NULL)
	{
		fprintf(stderr, "Error: unable to open plaintext.txt for writing.\nExiting...\n");
		exit(1);
	}

	// read in ciphertext blocks until we hit EOF
	while(fgets(lineBuf, 120, inFile) != NULL)
	{
		// grab both parts of the ciphertext block
		int numGrabbed = sscanf(lineBuf, "%" SCNu64 " %" SCNu64 "", &c1, &c2);

		// if we didn't grab both pieces, quit out
		if(numGrabbed != 2)
		{
			fprintf(stderr, "Error: Ciphertext block \"%s\" contains invalid amount of data (Expected 2, got %d)\nExiting...\n", lineBuf, numGrabbed);
			exit(1);
		}

		// we have c1 and c2 now in memory
		blockBuf.asInt = (fastModExp(c1, p-1-d, p) * c2) % p;

		// translation of plaintext from int -> char* taken care of for us by the Block union
		printf("Generated plaintext block <%s>\n", blockBuf.asChars);
		fprintf(outFile, "%s", blockBuf.asChars);
	}

	printf("Decryption complete.\nPlaintext written out to plaintext.txt in current directory.\n");
}

// Prints out usage message.
void printUsage(char *name)
{
	printf("Usage: \n\t%s keygen <seed>\n\t%s encrypt <key_file> <plaintext_file>\n\t%s decrypt <key_file> <ciphertext_file>\n\t%s --help\n", 
			name, name, name, name);	
}

// Prints out usage message and a glossary for the symbols contained therein.
void printHelpMessage(char *name)
{
	printUsage(name);
	printf("\n");
	printf("\tkeygen\t\t\tGenerate two keys (public and private) based on the given seed.\n\t\t\t\tKeys will be written to pubkey.txt and prikey.txt respectively.\n");
	printf("\tencrypt\t\t\tEncrypt <plaintext_file> using the key contained in <key_file>.\n\t\t\t\tCiphertext will be written to ciphertext.txt.\n");
	printf("\tdecrypt\t\t\tDecrypt <ciphertext_file> using the key contained in <key_file>.\n\t\t\t\tPlaintext will be written to plaintext.txt.\n");
	printf("\t<seed>\t\t\tThe number you want to seed the random number generator with.\n\t\t\t\tUse -1 if you want to seed with current time.\n");
	printf("\t<key_file>\t\tFile containing key info generated by running \"%s keygen <seed>\".\n\t\t\t\tIn most contexts, they will be called either pubkey.txt or prikey.txt.\n", name);
	printf("\t<plaintext_file>\tThe file you want to encrypt.\n");
	printf("\t<ciphertext_file>\tThe file you want to decrypt.\n>");
}