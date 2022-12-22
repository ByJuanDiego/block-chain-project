#pragma once
#ifndef HASHFUNCTIONS_H
#define HASHFUNCTIONS_H
#include <iostream>
#include <cstring>
#include <string>

class hashFunctions
{
protected:
    typedef unsigned char register_8;
    typedef unsigned int register_32;
    typedef unsigned long long register_64;

    const static register_32 hash_keys[];
    static const unsigned int BLOCK_SIZE_of_256 = (512/8);
public:
    void stateregister();
    void adjust_digest(const unsigned char *, unsigned int);
    void digest_final(unsigned char *);
    static const unsigned int PADD_SIZE = ( 256 / 8);

protected:
    void compress(const unsigned char *, unsigned int);
    unsigned int s_r_totlen;
    unsigned int s_r_len;
    unsigned char s_r_block[2*BLOCK_SIZE_of_256];
    register_32 s_r[8];
};

std::string sha256(const std::string&);

#define R_SHFT(x, n)    (x >> n) //Right shift functions
#define R_ROTATE(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n))) //Right rotate functions
#define L_ROTATE(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n))) //Left rotate functions
#define CHOICE_OF(x, y, z)  ((x & y) ^ (~x & z)) //functions to find choice of
#define MAJORITY_OF(x, y, z) ((x & y) ^ (x & z) ^ (y & z)) //functions to find majority of
#define SHAF_1(x) (R_ROTATE(x,  2) ^ R_ROTATE(x, 13) ^ R_ROTATE(x, 22)) //sigma rotation functions
#define SHAF_2(x) (R_ROTATE(x,  6) ^ R_ROTATE(x, 11) ^ R_ROTATE(x, 25)) //sigma rotation functions
#define SHAF_3(x) (R_ROTATE(x,  7) ^ R_ROTATE(x, 18) ^ R_SHFT(x,  3)) //sigma0 rotation
#define SHAF_4(x) (R_ROTATE(x, 17) ^ R_ROTATE(x, 19) ^ R_SHFT(x, 10)) //sigma1 rotation
#define SHAF_UNPACK32(x, str)                       \
{                                                   \
    *((str) + 3) = (register_8) ((x)      );        \
    *((str) + 2) = (register_8) ((x) >>  8);        \
    *((str) + 1) = (register_8) ((x) >> 16);        \
    *((str) + 0) = (register_8) ((x) >> 24);        \
}
#define SHAF_PACK32(str, x)                         \
{                                                   \
    *(x) =   ((register_32) *((str) + 3)      )     \
           | ((register_32) *((str) + 2) <<  8)     \
           | ((register_32) *((str) + 1) << 16)     \
           | ((register_32) *((str) + 0) << 24);    \
}

const unsigned int hashFunctions::hash_keys[64] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

void hashFunctions::compress(const unsigned char *message, unsigned int block_nb)
{
    register_32 w[64];
    register_32 buffer[8];
    register_32 t1, t2;
    const unsigned char *sub_block;
    int m;
    int n;
    for (m = 0; m < (int) block_nb; m++)
    {
        sub_block = message + (m << 6);
        for (n = 0; n < 16; n++) { SHAF_PACK32(&sub_block[n << 2], &w[n]); }
        for (n = 16; n < 64; n++) { w[n] =  SHAF_4(w[n -  2]) + w[n -  7] + SHAF_3(w[n - 15]) + w[n - 16]; }
        for (n = 0; n < 8; n++) { buffer[n] = s_r[n]; }
        for (n = 0; n < 64; n++)
        {
            t1 = buffer[7] + SHAF_2(buffer[4]) + CHOICE_OF(buffer[4], buffer[5], buffer[6]) + hash_keys[n] + w[n];
            t2 = SHAF_1(buffer[0]) + MAJORITY_OF(buffer[0], buffer[1], buffer[2]);
            buffer[7] = buffer[6];
            buffer[6] = buffer[5];
            buffer[5] = buffer[4];
            buffer[4] = buffer[3] + t1;
            buffer[3] = buffer[2];
            buffer[2] = buffer[1];
            buffer[1] = buffer[0];
            buffer[0] = t1 + t2;
        }
        for (n = 0; n < 8; n++) { s_r[n] += buffer[n]; }
    }
}

void hashFunctions::stateregister()
{
    s_r[0] = 0x6a09e667;
    s_r[1] = 0xbb67ae85;
    s_r[2] = 0x3c6ef372;
    s_r[3] = 0xa54ff53a;
    s_r[4] = 0x510e527f;
    s_r[5] = 0x9b05688c;
    s_r[6] = 0x1f83d9ab;
    s_r[7] = 0x5be0cd19;
    s_r_len = 0;
    s_r_totlen = 0;
}

void hashFunctions::adjust_digest(const unsigned char *text, unsigned int text_len)
{
    unsigned int block_nb;
    unsigned int new_len, rem_len, tmp_len;
    const unsigned char *shifted_message;
    tmp_len = BLOCK_SIZE_of_256 - s_r_len;
    rem_len = text_len < tmp_len ? text_len : tmp_len;
    memcpy(&s_r_block[s_r_len], text, rem_len);
    if (s_r_len + text_len < BLOCK_SIZE_of_256)
    {
        s_r_len += text_len;
        return;
    }
    new_len = text_len - rem_len;
    block_nb = new_len / BLOCK_SIZE_of_256;
    shifted_message = text + rem_len;
    compress(s_r_block, 1);
    compress(shifted_message, block_nb);
    rem_len = new_len % BLOCK_SIZE_of_256;
    memcpy(s_r_block, &shifted_message[block_nb << 6], rem_len);
    s_r_len = rem_len;
    s_r_totlen += (block_nb + 1) << 6;
}

void hashFunctions::digest_final(unsigned char *digest)
{
    unsigned int block_nb;
    unsigned int pm_len;
    unsigned int len_b;
    int i;
    block_nb = (1 + ((BLOCK_SIZE_of_256 - 9) < (s_r_len % BLOCK_SIZE_of_256)));
    len_b = (s_r_totlen + s_r_len) << 3;
    pm_len = block_nb << 6;
    memset(s_r_block + s_r_len, 0, pm_len - s_r_len);
    s_r_block[s_r_len] = 0x80;
    SHAF_UNPACK32(len_b, s_r_block + pm_len - 4);
    compress(s_r_block, block_nb);
    for (i = 0 ; i < 8; i++) { SHAF_UNPACK32(s_r[i], &digest[i << 2]); }
}

std::string sha256(const std::string& input)
{
    unsigned char digest[hashFunctions::PADD_SIZE];
    memset(digest, 0, hashFunctions::PADD_SIZE);

    hashFunctions obj = hashFunctions();
    obj.stateregister();
    obj.adjust_digest((unsigned char*)input.c_str(), input.length());
    obj.digest_final(digest);

    char buf[2*hashFunctions::PADD_SIZE+1];
    buf[2*hashFunctions::PADD_SIZE] = 0;
    for (int i = 0; i < hashFunctions::PADD_SIZE; i++) { sprintf(buf+i*2, "%02x", digest[i]); }
    return std::string(buf);
}

#endif