
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "tea_round_attack.h"


void encrypt_TEA_1round(TEA_U32_t v[2], const TEA_U32_t k[4]) {
    TEA_U32_t v0=v[0], v1=v[1], sum=0, i;           /* set up */
    TEA_U32_t delta=0x9E3779B9;                     /* a key schedule constant */
    TEA_U32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i<1; i++) {                           /* basic cycle start */
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    }                                               /* end cycle */
    v[0]=v0; v[1]=v1;
}

int main(int argc, char *argv[])
{
    struct tea_round_sample_s samples[16];
    struct tea_key_s key_candidates[8];
    unsigned int i, key_cnt;
    TEA_U32_t data[16][2];
    TEA_U32_t keys[4];
    time_t t;

    srand((unsigned) time(&t));

    printf("Generating random data and keys...\n");

    /* Generate 16 random data blocks. */
    for (i = 0; i < 16; i++)
    {
        data[i][0] = rand();
        data[i][1] = rand();

        samples[i].v0_plain = data[i][0];
        samples[i].v1_plain = data[i][1];
    }

    /* Generate random keys. */
    for (i = 0; i < 4; i++)
    {
        keys[i] = rand();
    }

    printf("Generating done.\nOriginal Keys: K0=0x%08x K1=0x%08x K2=0x%08x K3=0x%08x\n",
        keys[0], keys[1], keys[2], keys[3]);

    /* Encrypt the data. */
    for (i = 0; i < 16; i++)
    {
        encrypt_TEA_1round(data[i], keys);
    }    

    /* Get encrypted samples. */
    for (i = 0; i < 16; i++)
    {
        samples[i].v0_cipher = data[i][0];
        samples[i].v1_cipher = data[i][1];
    }

    /* Run TEA round attack. */
    printf("Bruteforcing now... This may take a minute, please be patient.\n");
    key_cnt = tea_round_attack(samples, 16, key_candidates, 8, TEA_DEFAULT_DELTA);

    /* Print key candidates. */
    printf("Found %d key pairs.\n", key_cnt);
    for (i = 0; i < key_cnt; i++)
    {
        printf("[%d] K0=0x%08x K1=0x%08x K2=0x%08x K3=0x%08x\n", 
            i, key_candidates[i].k0, key_candidates[i].k1,
            key_candidates[i].k2, key_candidates[i].k3);
    }
}