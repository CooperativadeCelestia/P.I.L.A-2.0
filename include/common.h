#ifndef COMMON_HEADER
#define COMMON_HEADER

#pragma once

struct Exchange /*Transaction*/ {
    int amount;
    char *sender;
    char *receiver;
};

struct Block {
    unsigned index, nonce, exchange_count;

    unsigned long long time;

    unsigned char *previous_hash;
    unsigned char *hash;
    char *miner;
    
    struct Exchange *exchange;
};

struct BlockChain {
    unsigned block_count, pendent_count, difficulty;

    unsigned block_size, pendent_size;

    double reward;

    struct Block *block;
    struct Exchange *pendent;
};

void BlockChain(struct BlockChain *chain, unsigned difficulty, double reward);
void BlockChain_PushExchange(struct BlockChain *chain, char *sender, char *receiver, int amount);
void BlockChain_Mine(struct BlockChain *chain, char *miner);
int BlockChain_Validate(struct BlockChain *chain);
void BlockChain_free(struct BlockChain *chain);

#endif
