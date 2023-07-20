#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#include "string_helper.h"

unsigned char DEBUG = 0;

int main() {
    struct BlockChain chain;
    BlockChain(&chain, 0, 2.5);

    BlockChain_PushExchange(&chain, "Endereco1", "Endereco2", 5);
    BlockChain_PushExchange(&chain, "Endereco2", "Endereco3", 2);

    BlockChain_Mine(&chain, "EnderecoMinerador");

    printf("A BlockChain e valida? %s\n", (BlockChain_Validate(&chain) ? "Sim" : "Nao"));
    
    for (unsigned i = 0; i < chain.block_count; ++i) {
        printf("Bloco %u hash : %s\n", chain.block[i].index, chain.block[i].hash);
    }

    BlockChain_free(&chain);
}