#include "wallet.h"

unsigned char DEBUG = 1;

int main() {
    struct Wallet wallet;
    Wallet(&wallet);
    Wallet_free(&wallet);
}