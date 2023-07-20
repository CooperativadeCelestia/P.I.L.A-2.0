#ifndef WALLET_HEADER
#define WALLET_HEADER

#pragma once

struct Wallet {
    char* pkey;
    char *skey;
};

void Wallet(struct Wallet *wallet);
void Wallet_free(struct Wallet *wallet);

#endif