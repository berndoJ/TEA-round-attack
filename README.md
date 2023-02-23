TEA Round Attack
================

TEA (Tiny Encryption Algorithm) is a 64-bit, usually 32 or 64 round, ECB-style
block cipher.

This is an attack that bruteforces the encryption keys in under a minute with
a given set of single-round samples. So, if it possible to extract the before
and after values for `v0` and `v1` for a single encryption round, TEA is broken.

## Why does this work?

Here's the reference implementation of TEA:
```c
void encrypt (uint32_t v[2], const uint32_t k[4]) {
    uint32_t v0=v[0], v1=v[1], sum=0, i;           /* set up */
    uint32_t delta=0x9E3779B9;                     /* a key schedule constant */
    uint32_t k0=k[0], k1=k[1], k2=k[2], k3=k[3];   /* cache key */
    for (i=0; i<32; i++) {                         /* basic cycle start */
        sum += delta;
        v0 += ((v1<<4) + k0) ^ (v1 + sum) ^ ((v1>>5) + k1);
        v1 += ((v0<<4) + k2) ^ (v0 + sum) ^ ((v0>>5) + k3);
    }                                              /* end cycle */
    v[0]=v0; v[1]=v1;
}
```

As one can see, one single round is basically just a shift and XOR of the
input data. We have two equations and four unknowns (the 32-bit keys). So we
can just bruteforce `k0` and `k2`, obtain the other two keys through 
rearranging the equation and thus get the whole 128-bit key.

## PoC

For testing and as a proof of concept, [example.c](./example.c) can be used.
Just compile it with `gcc -o example example.c tea_round_attack.c` and execute,
it will generate some random 16 blocks of data and keys and bruteforces them.

To use in another application, just the `tea_round_attack.c` and `tea_round_attack.h`
files are needed.