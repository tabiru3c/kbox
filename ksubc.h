/*
*F: eni.h
* Coded at: 2023.7.16 by T. Abi
*/
#include <openssl/evp.h>
#include <openssl/aes.h>

#define ROTORSZ 256

struct ENIGBASE {
char t1[ROTORSZ];
char t2[ROTORSZ];
char t3[ROTORSZ];
char deck[ROTORSZ];
char buf[13];
};

struct AESBASE {
EVP_CIPHER_CTX* en;
EVP_CIPHER_CTX* de;
char key_data[32];
int key_data_len;

unsigned char salt[8];
int iter;
unsigned char *key[32];
unsigned char *iv[16];
};

