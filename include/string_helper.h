#ifndef STRING_HELPER_HEADER
#define STRING_HELPER_HEADER

#pragma once
// Ao chamar essa funçao, o valor que o ponteiro "length" aponta precisa ser a quantidade de strings a serem adicionadas
// No final dessa funçao, o valor que o ponteiro "length" aponta vai ser o tamanho da string final(excluindo o byte nulo no final)
char *str_append(char *original, unsigned argcount, ...);
char *str_decimal(unsigned value);
void str_shuffle(char *original, unsigned long long seed);
int str_equal(char *a, char *b);
int str_nequal(char *a, char *b, int n);

#endif
