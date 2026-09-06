#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


typedef struct {
    unsigned char *data;  
    size_t bit_size;     
    size_t bitpos;     
} BitReader;


BitReader* createBitBuffer(size_t initial_bytes) {
    BitReader* bb = (BitReader*)malloc(sizeof(BitReader));
    bb->data = (unsigned char*)calloc(initial_bytes, 1);
    bb->bit_size = initial_bytes;
    bb->bitpos = 0;
    return bb;
}

/**
 * @brief Ensures the bit buffer has enough capacity for additional bits.
 * 
 * Expands the buffer if the current capacity is insufficient to accommodate 
 * the required number of additional bits.
 * 
 * @param bb Pointer to the `BitReader` structure.
 * @param extra_bits The number of additional bits to ensure capacity for.
 */
static void ensureCapacity(BitReader* bb, size_t extra_bits) {
    size_t needed_bits = bb->bitpos + extra_bits;
    size_t needed_bytes = (needed_bits + 7) / 8;
    if (needed_bytes > bb->bit_size) {
        size_t new_capacity = bb->bit_size * 2;
        if (new_capacity < needed_bytes) {
            new_capacity = needed_bytes;
        }
        bb->data = (unsigned char*)realloc(bb->data, new_capacity);
        memset(bb->data + bb->bit_size, 0, new_capacity - bb->bit_size);
        bb->bit_size = new_capacity;
    }
}

void writeBits(BitReader* bb, unsigned int val, int nbits) {
    if (nbits <= 0) return;
    ensureCapacity(bb, nbits);
    for (int i = nbits - 1; i >= 0; i--) {
        unsigned int bit = (val >> i) & 1;
        size_t bytePos = bb->bitpos >> 3;    
        int bitInByte = 7 - (bb->bitpos & 7);
        if (bit) {
            bb->data[bytePos] |= (1 << bitInByte);
        }
        bb->bitpos++;
    }
}

size_t bitBufferSizeInBytes(const BitReader* bb) {
    return (bb->bitpos + 7) / 8; 
}


BitReader* readFileToBitReader(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Fail to open the file");
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp); 
    fseek(fp, 0, SEEK_SET);

    unsigned char *buf = (unsigned char*)malloc(fsize);
    if (!buf) {
        fclose(fp);
        return NULL;
    }
    fread(buf, 1, fsize, fp);
    fclose(fp);

    BitReader *br = (BitReader*)malloc(sizeof(BitReader));
    br->data = buf;
    br->bit_size = (size_t)fsize * 8;
    br->bitpos = 0;
    return br;
}


void freeBitReader(BitReader *br) {
    if (!br) return;
    free(br->data);
    free(br);
}

unsigned char readBits(BitReader *br, int nbits) {
    if (nbits <= 0) return 0;

    unsigned int value = 0;
    for (int i = 0; i < nbits; i++) {
        if (br->bitpos >= br->bit_size) {
            break;
        }
        size_t byteIndex = br->bitpos >> 3;       
        int bitInByte = 7 - (br->bitpos & 7);     
        unsigned char bit = (br->data[byteIndex] >> bitInByte) & 1;
        value = (value << 1) | bit;
        br->bitpos++;
    }
    return value;
}
