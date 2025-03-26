
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include "ksubc.h"

#define ROTORSZ 256
#define MASK 0377

static char base64table[] = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int encb64(
char *dst,	/* must have enough size about 4/3 times */
const char *src,
int n )		/* strlen(src) */
{
    int32_t d,m;
    int l = 0;
    for (int i=0; i<n; i+=3){
	m = (i+3 <= n) ? 3 : n-i;
	d = 0;
	for (int j=0; j<3; j++){
	    d <<= 8;
	    if (j < m){
		d += src[i+j] & 0xff;
	    }
	}
	for (int j=0; j<4; j++){
	    dst[3-j] = (j+m > 2) ? base64table[d & 0x3f] : '=';
	    d >>= 6;
	}
	dst += 4;
	l += 4;
/*
	if (l % 73 == 72){
	    *(dst++) = '\n';
	    l++;
	}
*/
    }
    *dst = '\0';
    return l;
}

int decb64(
char *dst,	/* must have enough size about 4/3 times */
const char *src,
int n ){	/* strlen(src) */
    int l = 0;
    int si = 0;
    int32_t d = 0;
    int abt = 0;
    char *p;
    for (int i=0; (i<n)&&(abt==0); i++){
	if (src[i] == '='){
	    if (si == 2){
		dst[l++] = (d >> 4) & 0xff;
	    } else if (si == 3){
		d >>= 2;
		dst[l+1] = d & 0xff;
		dst[l] = (d >> 8) & 0xff;
		l += 2;
	    }
	    abt = 1;
	} else {
	    p = strchr(base64table, src[i]);
	    if (p != NULL){
		d <<= 6;
		d += (p-base64table) & 0x3f;
		si++;
		if (si == 4){
		    for (int j=2; j>=0; j--){
			dst[l+j] = d & 0xff;
			d >>= 8;
		    }
		    l += 3;
		    d = 0;
		    si = 0;
		}
	    }
	}
    }
    return l;
}

extern int32_t crypt_des(const char *pw, const char *salt, char *buf);

void enisetup( struct ENIGBASE *eb, char *pw ){
    eb->buf[0] = '\0';
    if (pw != NULL){
	char salt[3],dbuf[256];
	strncpy(salt, pw, sizeof(salt)-1);
	salt[2] = '\0';
	if (crypt_des(pw,salt,dbuf) == 0){
	    memcpy(eb->buf, dbuf, sizeof(eb->buf));
	}
    }
    if (eb->buf[0] == '\0'){
	fprintf(stderr, "crypt(3) failure\n");
	exit(1);
    }
    int32_t seed = 123;
    for (int32_t i=0; i<13; i++){
	seed = seed*eb->buf[i] + i;
    }
    for(int32_t i=0; i<ROTORSZ; i++){
	eb->t1[i] = i;
	eb->t2[i] = 0;
	eb->t3[i] = 0;
	eb->deck[i] = i;
    }
    int32_t ic, k, temp;
    uint32_t rnd;
    for(int32_t i=0; i<ROTORSZ; i++){
	seed = 5*seed + eb->buf[i%13];
	rnd = seed % 65521;
	k = ROTORSZ-1 - i;
	ic = (rnd&MASK)%(k+1);
	rnd >>= 8;
	temp = eb->t1[k];
	eb->t1[k] = eb->t1[ic];
	eb->t1[ic] = temp;
	if(eb->t3[k]!=0) continue;
	ic = (rnd&MASK) % k;
	while(eb->t3[ic]!=0) ic = (ic+1) % k;
	eb->t3[k] = ic;
	eb->t3[ic] = k;
    }
    for(int32_t i=0;i<ROTORSZ;i++){
	eb->t2[eb->t1[i]&MASK] = i;
    }
}

uint8_t eniconv( struct ENIGBASE *eb, int32_t n, uint8_t *s, uint8_t *d )
{
    int n1,n2,nr1,nr2;
    n1 = n2 = nr2 = 0;
    int32_t j;
    for (int32_t i=0; i<n; i++){
	j = s[i];
	nr1 = n1;
	d[i] = eb->t2[(eb->t3[(eb->t1[(j+nr1)&MASK]+nr2)&MASK]-nr2)&MASK]-nr1;
	n1++;
	if (n1 == ROTORSZ){
	    n1 = 0;
	    n2++;
	    if (n2 == ROTORSZ){
		n2 = 0;
	    }
	    nr2 = n2;
	}
    }
    return 0;
}

int getRndStr( unsigned char *s, int n ){
    int m = 0;
    memset(s, 0, n);
    int uRnd = open("/dev/urandom", O_RDONLY);
    if (uRnd < 0){
	fprintf(stderr, "Open /dev/urandom failed.\n");
    } else {
	m = read(uRnd, s, n);
	m -= n;
	close(uRnd);
    }
    return m;
}

int aes_enc( char *src, int slen, char *pwd, char *dst ){
    int sz = 0;
    if ((src != NULL) && (strlen(src) < 256)){
	unsigned char keyiv[64];
	unsigned char *key = keyiv;
	unsigned char *iv = &keyiv[32];
	int iter = 10000;
	const EVP_MD *dgst = EVP_sha256();
	unsigned char salt[16];
	strcpy((char *)salt, "Salted__");
	unsigned char *u = &salt[8];
	getRndStr(u, 8);
	for (int j=0; j<8; j++){
	    u[j] = 'a' + (u[j] % 26);
	}
	memcpy(dst, salt, 16);
	sz = 16;
	unsigned char *d = (unsigned char *)&dst[sz];
	int pwdlen = strlen(pwd);
	int hok = PKCS5_PBKDF2_HMAC(pwd, pwdlen, u, 8, iter, 
		dgst, 48, keyiv);
	if (hok == 0){
	    fprintf(stderr, "PKCS5_PBKDF2_HMAC for key failed.\n");
	    return -1;
	}
	EVP_CIPHER_CTX* en = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(en);
	EVP_EncryptInit_ex(en, EVP_aes_256_cbc(), NULL, key, iv);
	int len = 0;
	EVP_EncryptUpdate(en, d, &len, (unsigned char *)src, slen);
	int ciphertext_len = len;
	EVP_EncryptFinal_ex(en, &d[len], &len);
	ciphertext_len += len;
	sz += ciphertext_len;
	EVP_CIPHER_CTX_free(en);
    }
    return sz;
}

int aes_dec( char *src, int slen, char *pwd, char *dst ){
    int sz = 0;
    if ((src != NULL) && (slen < 256)){
	unsigned char keyiv[64];
	unsigned char *key = keyiv;
	unsigned char *iv = &keyiv[32];
	int iter = 10000;
	const EVP_MD *dgst = EVP_sha256();
	unsigned char salt[16];
	memcpy(salt, src, 16);
	unsigned char *u = &salt[8];
	sz = 0;
	unsigned char *d = (unsigned char *)dst;
	int pwdlen = strlen(pwd);
	int hok = PKCS5_PBKDF2_HMAC(pwd, pwdlen, u, 8, iter, 
		dgst, 48, keyiv);
	if (hok == 0){
	    fprintf(stderr, "PKCS5_PBKDF2_HMAC for key failed.\n");
	    return -1;
	}
	EVP_CIPHER_CTX* de = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(de);
	EVP_DecryptInit_ex(de, EVP_aes_256_cbc(), NULL, key, iv);
	int len = 0;
	EVP_DecryptUpdate(de, d, &len, (unsigned char *)&src[16], slen-16);
	int plaintext_len = len;
	EVP_DecryptFinal_ex(de, (unsigned char *)&d[len], &len);
	plaintext_len += len;
	sz += plaintext_len;
	EVP_CIPHER_CTX_free(de);
    }
    return sz;
}

