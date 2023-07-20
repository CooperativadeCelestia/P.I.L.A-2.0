#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sha256.h"

static void compact(unsigned char data[64], unsigned states[8]);

void sha256(char *seed, unsigned char *hash) {
  unsigned char data[64];
  unsigned datalength = 0;
  unsigned long long bitlength = 0;

  unsigned *states = malloc(8 * sizeof(unsigned));
  memcpy(states, (unsigned[]){0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19}, 8 * sizeof(unsigned));

  unsigned length = strlen(seed);

  unsigned i = 0;

  for(; i < length; ++i) {
    data[datalength] = seed[i];

    if(++datalength == 64) {
      compact(data, states);
      bitlength += 512;
      datalength = 0;
    }
  }

  i = datalength;

  data[i++] = 0x80;
  if(datalength < 56) while(i < 56) data[i++] = 0x0;
  else {
    while (i < 64) data[i++] = 0x0;
    compact(data, states);
    memset(data, 0, 56);
  }

  bitlength += datalength * 8;
  data[63] = bitlength;
  data[62] = bitlength >> 8;
  data[61] = bitlength >> 16;
  data[60] = bitlength >> 24;
  data[59] = bitlength >> 32;
  data[58] = bitlength >> 40;
  data[57] = bitlength >> 48;
  data[56] = bitlength >> 56;
  compact(data, states);

  for(i = 0; i < 4; ++i) {
    hash[i] = (states[0] >> (24 - i * 8)) & 0xff;
    hash[i + 4] = (states[1] >> (24 - i * 8)) & 0xff;
    hash[i + 8] = (states[2] >> (24 - i * 8)) & 0xff;
    hash[i + 12] = (states[3] >> (24 - i * 8)) & 0xff;
    hash[i + 16] = (states[4] >> (24 - i * 8)) & 0xff;
    hash[i + 20] = (states[5] >> (24 - i * 8)) & 0xff;
    hash[i + 24] = (states[6] >> (24 - i * 8)) & 0xff;
    hash[i + 28] = (states[7] >> (24 - i * 8)) & 0xff;
  }

  free(states);
}

#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (32 - (b))))

#define CH(a,b,c) (((a) & (b)) ^ (~(a) & (c)))
#define MAJ(a,b,c) (((a) & (b)) ^ ((a) & (c)) ^ ((b) & (c)))
#define EP0(a) (ROTRIGHT((a),2) ^ ROTRIGHT((a),13) ^ ROTRIGHT((a),22))
#define EP1(a) (ROTRIGHT((a),6) ^ ROTRIGHT((a),11) ^ ROTRIGHT((a),25))
#define SIG0(a) (ROTRIGHT((a),7) ^ ROTRIGHT((a),18) ^ ((a) >> 3))
#define SIG1(a) (ROTRIGHT((a), 17) ^ ROTRIGHT(a, 19) ^ ((a) >> 10))


static unsigned k[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static void compact(unsigned char data[64], unsigned *states) {
  unsigned i = 0;

  unsigned tmp[64];

  for(unsigned j = 0; i < 16; ++i, j += 4) tmp[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | data[j + 3];
  for(; i < 64; ++i) tmp[i] = SIG1(tmp[i - 2]) + tmp[i - 7] + SIG0(tmp[i - 15]) + tmp[i - 16];

  unsigned *vstates = malloc(8 * sizeof(unsigned));
  memcpy(vstates, states, 8 * sizeof(unsigned));

  i = 0;
  for(unsigned t1; i < 64; ++i) {
    t1 = vstates[7] + EP1(vstates[4]) + CH(vstates[4], vstates[5], vstates[6]) + k[i] + tmp[i];
    vstates[7] = vstates[6];
    vstates[6] = vstates[5];
    vstates[5] = vstates[4];
    vstates[4] = vstates[3] + t1;
    vstates[3] = vstates[2];
    vstates[2] = vstates[1];
    vstates[1] = vstates[0];
    vstates[0] = t1 + EP0(vstates[0]) + MAJ(vstates[0], vstates[1], vstates[2]);
  }

  for(i = 0; i < 8; i++) states[i] += vstates[i];

  free(vstates);
}