// (C) 2016 University of Bristol. See LICENSE.txt

#ifndef _SHA1
#define _SHA1

/*
 * SHA1 routine optimized to do word accesses rather than byte accesses,
 * and to avoid unnecessary copies into the context array.
 *
 * This was initially based on the Mozilla SHA1 implementation, although
 * none of the original Mozilla code remains.
 */

#define HASH_SIZE 20

typedef struct {
	unsigned long long size;
	unsigned int H[5];
	unsigned int W[16];
} blk_SHA_CTX;

void blk_SHA1_Init(blk_SHA_CTX *ctx);
void blk_SHA1_Update(blk_SHA_CTX *ctx, const void *dataIn, unsigned long len);
void blk_SHA1_Final(unsigned char hashout[20], blk_SHA_CTX *ctx);

#define git_SHA_CTX	blk_SHA_CTX
#define git_SHA1_Init	blk_SHA1_Init
#define git_SHA1_Update	blk_SHA1_Update
#define git_SHA1_Final	blk_SHA1_Final

#endif
