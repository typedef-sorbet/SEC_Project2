Author:
	Spencer Warneke
Assignment:
	Computer Security, Project 2: Public/Private Crypto
Date:
	April 1, 2019
Project Overview:
	Project 2 is a small investigation into the mechanics of public/private crypto systems.
	This project covers public and private key generation using large primes and modular
	exponentiation, as well as encryption and decryption using moduler exponentiation.
	Files can be encrypted and decrypted using keys generated from the keygen algorithm,
	and depending on which key you encrypt with, you can either encrypt for confidentiality,
	or encrypt for digital signing and origin authentication.
Included Files:
	spencer.warneke.zip
	├── main.c
	├── main.h
	├── Makefile
	└── README.txt
To Compile:
	If you have make installed, simply do

		$ make

	If you don't, or want to do it yourself for some reason, do

		$ gcc -o main main.c -Wall -pedantic

To Run:
	This program has 4 main functions:

	./main keygen <seed>
			Runs the public and private key generation procedure using <seed> as the seed for the random number generator. 
			If <seed> is less than 0, the RNG will be seeded with the current system time. 
			Keys will be output to pubkey.txt and prikey.txt for the public key and the private key respectively.
	./main encrypt <key_file> <plaintext_file>
			Encrypts <plaintext_file> using the key data found in <key_file>.
			Outputs the resulting ciphertext to ciphertext.txt.
	./main decrypt <key_file> <ciphertext_file>
			Decrypts <ciphertext_file> using the key data found in <key_file>.
			Outputs the resulting plaintext to plaintext.txt.
			Note that <key_file> should be the opposite key to the one used for encryption.
	./main --help
			Prints out a usage message, as well as descriptions for the symbols the usage message contains.
			Similar to this message, but less verbose.