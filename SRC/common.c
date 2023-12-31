#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "common.h"

#include "string_helper.h"
#include "sha256.h"

#define PENDENT_COUNT_CONSTANT 10
#define BLOCK_COUNT_CONSTANT 10

extern unsigned char DEBUG;

static struct BlockChain *bind;

void BlockChain(struct BlockChain *chain, unsigned diff, double reward) {
    chain->block = malloc((chain->block_size = BLOCK_COUNT_CONSTANT) * sizeof(struct Block)); // aloca a lista de blocos na memoria com um tamanho predefinido(deixa o codigo mais rapido mas consome mais memoria)
    chain->block_count = 1;
    chain->pendent = malloc((chain->pendent_size = PENDENT_COUNT_CONSTANT) * sizeof(struct Exchange)); // aloca a lista de transaçoes pendentes
    chain->pendent_count = 0;
    chain->difficulty = diff;
    chain->reward = reward;

    memset(chain->block, 0, sizeof(struct Block)); // deixa todos as variaveis do primeiro bloco(genesis block) como 0 ou ""

    if(DEBUG) printf("BlockChain Created: block_count = %u, pendent_count = %u, difficulty = %u, reward = %lf\n", chain->block_count, chain->pendent_count, chain->difficulty, chain->reward);
}

void BlockChain_PushExchange(struct BlockChain *chain, char *sender, char *receiver, int amount) {
    // aumenta o tamanho da lista de transaçoes caso a quantidade de transaçoes seja maior que o tamanho da lista
    if(chain->pendent_count == chain->pendent_size) {
        chain->pendent = realloc(chain->pendent, (chain->pendent_size += PENDENT_COUNT_CONSTANT) * sizeof(struct Exchange));

        if(DEBUG) printf("Pendent Transactions List Reallocated\n");
    }

    struct Exchange *pe = chain->pendent + chain->pendent_count;

    unsigned l = strlen(sender);
    pe->sender = malloc(l + 1);
    memcpy(pe->sender, sender, l + 1);
    pe->receiver = malloc((l = strlen(receiver)) + 1);
    memcpy(pe->receiver, receiver, l + 1);
    pe->amount = amount;

    chain->pendent_count++;

    if(DEBUG) printf("Pendent Transaction Created: sender = %s, receiver = %s, amount = %d\n", sender, receiver, amount);
}

void BlockChain_Mine(struct BlockChain *chain, char *miner) {
    if(DEBUG) printf("\nMining:\n");

    unsigned char *previous_hash = chain->block[chain->block_count - 1].hash;

    unsigned char *hash = malloc(64 + 1); // + 1 é necessario pra por o null byte(byte que marca o fim da string)
    hash[64] = 0;

    char *diffstr = malloc(chain->difficulty + 1);
    memset(diffstr, '0', chain->difficulty); // poe todos os bytes da string como '0'
    diffstr[chain->difficulty] = 0; // poe o null byte

    for(unsigned nonce = 0; 1; ++nonce) {
        unsigned long long t = time(0x0);

        char *tmp = 0x0; // Serve apenas para liberar memoria alocada
        char *blockstr = str_append("timestamp: ", 2, tmp = str_decimal(t), ", transactions: [");
        free(tmp);

        char *tmp2; // Serve apenas para liberar memoria alocada

        for(unsigned n = 0; n < chain->pendent_count; ++n) {
            tmp2 = blockstr;
            blockstr = str_append(blockstr, 7, "{sender: ", chain->pendent[n].sender, ", receiver: ", chain->pendent[n].receiver, ", amount: ", tmp = str_decimal(chain->pendent[n].amount), "}, ");

            free(tmp2);
        }
        tmp2 = blockstr;
        blockstr = str_append(blockstr, 4, "], previous_hash: ", (!previous_hash ? "(null)" : (char *) previous_hash), ", nonce: ", tmp = str_decimal(nonce));
        free(tmp);
        free(tmp2);

        str_shuffle(blockstr, t);
        
        sha256(blockstr, hash);
        
        free(blockstr);

        if(str_nequal((char *) hash, diffstr, chain->difficulty)) {
        //o if acima é a mesma coisa de:
        // if(strncmp(hash, diffstr, chain->difficulty) == 0)
        // pesquise sobre o strncmp para entender seu uso

            free(diffstr);

            // aumenta o tamanho da lista de blocos caso a quantidade de blocos criados seja maior que o tamanho da lista
            if(chain->block_count == chain->block_size) {
                chain->block = realloc(chain->block, (chain->block_size += BLOCK_COUNT_CONSTANT) * sizeof(struct Block));

                if(DEBUG) printf("Block List Reallocated\n");
            }

            struct Block *block = chain->block + chain->block_count;
            block->index = chain->block_count++;
            block->time = t;
            block->exchange_count = chain->pendent_count;
            block->exchange = realloc(chain->pendent, chain->pendent_count * sizeof(struct Exchange));
            chain->pendent = malloc((chain->pendent_size = PENDENT_COUNT_CONSTANT) * sizeof(struct Exchange)); // zera a lista
            chain->pendent_count = 0;
            block->hash = hash;
            block->previous_hash = chain->block[chain->block_count - 2].hash;
            unsigned l = strlen(miner);
            block->miner = malloc(l + 1);
            memcpy(block->miner, miner, l + 1);
            block->nonce = nonce;

            if(DEBUG) printf("Block Mined: index = %u, time = %llu, exchange_count = %u, nonce = %u, miner = %s, hash = \n%s\n\nprevious_hash = \n%s\n\n", block->index, block->time, block->exchange_count, block->nonce, block->miner, block->hash, block->previous_hash);

            BlockChain_PushExchange(chain, "Sistema", miner, chain->reward);

            return;
        }
    }
}

int BlockChain_Validate(struct BlockChain *chain) {
    if(DEBUG) printf("Validating Blocks:\n");

    char *diffstr = malloc(chain->difficulty + 1);
    memset(diffstr, '0', chain->difficulty);
    diffstr[chain->difficulty] = 0;

    unsigned char *hash = malloc(64 + 1);
    hash[64] = 0;

    for(unsigned i = 1; i < chain->block_count; ++i) {
        char *tmp = 0x0; // Serve apenas para liberar memoria alocada
        char *blockstr = str_append("timestamp: ", 2, tmp = str_decimal(chain->block[i].time), ", transactions: [");
        free(tmp);

        char *tmp2; // Serve apenas para liberar memoria alocada

        for(unsigned n = 0; n < chain->block[i].exchange_count; ++n) {
            tmp2 = blockstr;
            blockstr = str_append(blockstr, 7, "{sender: ", chain->block[i].exchange[n].sender, ", receiver: ", chain->block[i].exchange[n].receiver, ", amount: ", tmp = str_decimal(chain->block[i].exchange[n].amount), "}, ");

            free(tmp2);
        }
        tmp2 = blockstr;
        blockstr = str_append(blockstr, 4, "], previous_hash: ", (!chain->block[i - 1].hash ? "(null)" : (char *) chain->block[i - 1].hash), ", nonce: ", tmp = str_decimal(chain->block[i].nonce));
        free(tmp);
        free(tmp2);
        
        str_shuffle(blockstr, chain->block[i].time);

        sha256(blockstr, hash);

        free(blockstr);

        int a = 0, b = 0;
        if((a = !str_equal((char *) hash, (char *) chain->block[i].hash)) || (b = !str_nequal((char *) hash, diffstr, chain->difficulty))) {
            
            free(diffstr);

            if(DEBUG) printf("Block %u is not valid\n", i);

            return 0;
        }

        if(DEBUG) printf("Block %u is valid\n", i);
    }

    free(hash);
    free(diffstr);

    if(DEBUG) printf("BlockChain is valid\n");

    return 1;
}

void BlockChain_free(struct BlockChain *chain) {
    unsigned i = 1;
    
    for(; i < chain->block_count; ++i) {
        free(chain->block[i].hash);
        free(chain->block[i].miner);

        for(unsigned n = 0; n < chain->block[i].exchange_count; ++n) {
            free(chain->block[i].exchange[n].sender);
            free(chain->block[i].exchange[n].receiver);
        }

        free(chain->block[i].exchange);
    }

    free(chain->block);

    for(i = 0; i < chain->pendent_count; ++i) {
        free(chain->pendent[i].sender);
        free(chain->pendent[i].receiver);
    }

    free(chain->pendent);

    if(DEBUG) printf("BlockChain Freed\n");
}