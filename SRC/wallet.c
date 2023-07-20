#include <stdio.h>
#include <stdlib.h>

#include "wallet.h"
#include "string_helper.h"

extern unsigned char DEBUG;

/*static inline unsigned long long modularExp(unsigned long long base, unsigned long long exponent, unsigned long long modulus);
static inline int isprime(unsigned long long number, unsigned precision);
static inline unsigned long long genprime(unsigned digits, unsigned precision);
static inline unsigned long long getcoprime(unsigned long long number);
static inline unsigned long long mdc(unsigned long long a, unsigned long long b);*/

void Wallet(struct Wallet *wallet) {
    /*unsigned long long p = genprime(10, 10);
    unsigned long long q = genprime(10, 10);

    while(q == p) q = genprime(10, 10);

    unsigned long long totient = (p - 1) * (q - 1);
    unsigned long long e = getcoprime(totient);*/

    wallet->pkey = "public key"; //pkey length == 10
    wallet->skey = "private key"; //skey length = 11

#ifdef _WIN32
    FILE *fd;
    fopen_s(&fd, "wallet.txt", "r");

    if(!fd) { if(DEBUG) printf("No wallet file\n"); goto _savewallet; }

    fseek(fd, 0, SEEK_END);
    unsigned size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // 10 <pkey length> + 11 <skey length> == 21
    if(size < 21) { if(DEBUG) printf("Invalid wallet\n"); goto _savewallet; }

    char *buf = malloc(size + 1);
    fread_s(buf, size + 1, 1, size, fd);
#else
    FILE *fd = fopen("wallet.txt", "r");
    
    if(!fd) { if(DEBUG) printf("No wallet file\n"); goto _savewallet; }

    fseek(fd, 0, SEEK_END);
    unsigned size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // 10 <pkey length> + 11 <skey length> == 21
    if(size < 21) { if(DEBUG) printf("Invalid wallet\n"); goto _savewallet; }

    char *buf = malloc(size + 1);
    fread(buf, 1, size, fd);
#endif

    buf[size] = 0;

    fclose(fd);

    if(!str_nequal(wallet->pkey, buf, 10 /*pkey length*/)) { if(DEBUG) printf("Wrong Public Key\n"); goto _L1; }
    if(!str_nequal(wallet->skey, buf + 10, 11 /*skey length*/)) { if(DEBUG) printf("Wrong Private key\n"); goto _L1; }

    if(DEBUG) printf("Wallet Content: %s\n", buf + 21);

    free(buf);

_savewallet:
    if(!fd) {
#ifdef _WIN32
        fopen_s(&fd, "wallet.txt", "a");
#else
        fd = fopen("wallet.txt", "a");
#endif

        if(!fd) { if(DEBUG) printf("Couldn't create a wallet file\n"); wallet->pkey = wallet->skey = 0x0; return; }

        fwrite(wallet->pkey, 1, 10, fd);
        fwrite(wallet->skey, 1, 11, fd);
        fclose(fd);

        if(DEBUG) printf("Wallet Created\n");
    }

    return;

_L1:
    wallet->pkey = 0x0;
    wallet->skey = 0x0;

    free(buf);
}

void Wallet_free(struct Wallet *wallet) {
    //if(wallet->pkey) free(wallet->pkey);
    //if(wallet->skey) free(wallet->skey);
}

/*static inline unsigned long long genprime(unsigned digits, unsigned precision) {
    unsigned long long min = 1, max = 1;

    for (unsigned i = 0; i < digits - 1; i++) {
        min *= 10;
        max = max * 10 + 9;
    }

    srand(time(0x0));

    while (1) {
        unsigned long long number = min + (rand() % (max - min + 1));
        if (number % 6 != 1 && number % 6 != 5)
            continue;

        if (isprime(number, precision))
            return number;
    }
}

static inline int isprime(unsigned long long number, unsigned precision) { // teste de primalidade de Miller-Rabin
    if(number == 2 || number == 3) return 1;
    if(number <= 1 || !(number % 2) || !(number % 3)) return 0;

    unsigned long long d = number - 1;
    unsigned long long r = 0;
    for(; !(d % 2); ++r) d /= 2;

    unsigned j;

    for(unsigned i = 0; i < precision; ++i) {
        unsigned long long a = 2 + rand() % (number - 4);
        unsigned long long x = modularExp(a, d, number);

        if(x == 1 || x == number - 1) return 1;

        for(j = 0; j < r - 1; ++j) {
            x = modularExp(x, 2, number);

            if(x == 1) return 0;
            if(x == number - 1) return 1;
        }
    }

    return 0;
}

static inline unsigned long long modularExp(unsigned long long base, unsigned long long exponent, unsigned long long modulus) {
    unsigned long long result = 1;
    base = base % modulus;

    for(; exponent > 0; base = base * base % modulus, exponent /= 2) {
        if (exponent % 2)
            result = (result * base) % modulus;
    }

    return result;
}

static inline unsigned long long getcoprime(unsigned long long number) {
    number += number % 2;

    unsigned long long i, value;

    unsigned char flag;

    while(1) {
        flag = 1;
        value = 2 + rand() % (number - 1);

        if(!(value % 2) || mdc(number, value) != 1) continue;

        for (i = 3; i * i <= value; i += 2) {
            if (!(value % i)) {
                flag = 0;
                break;
            }
        }

        if(flag) return value;
    }
}

static inline unsigned long long mdc(unsigned long long a, unsigned long long b) {
    unsigned tmp;

    while(b) {
        tmp = b;
        b = a % b;
        a = tmp;
    }

    return a;
}*/