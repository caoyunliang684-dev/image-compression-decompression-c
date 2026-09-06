#ifndef BITREADER_H
#define BITREADER_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief A structure for managing bit-level reading and writing operations.
 */
typedef struct {
    unsigned char *data; /**< Pointer to the buffer storing bit data. */
    size_t bit_size;     /**< Total number of bits in the buffer. */
    size_t bitpos;       /**< Current bit position for reading or writing. */
} BitReader;

/**
 * @brief Creates a new bit buffer.
 * 
 * @param initial_bytes Initial size of the buffer in bytes.
 * @return A pointer to the newly created BitReader.
 */
BitReader* createBitBuffer(size_t initial_bytes);

/**
 * @brief Writes a specified number of bits to the bit buffer.
 * 
 * @param bb Pointer to the BitReader.
 * @param val The value to write (unsigned integer).
 * @param nbits Number of bits to write.
 */
void writeBits(BitReader* bb, unsigned int val, int nbits);

/**
 * @brief Gets the size of the bit buffer in bytes.
 * 
 * @param bb Pointer to the BitReader.
 * @return The size of the buffer in bytes.
 */
size_t bitBufferSizeInBytes(const BitReader* bb);

/**
 * @brief Reads data from a file and creates a BitReader.
 * 
 * @param filename Name of the file to read.
 * @return A pointer to a BitReader containing the file data.
 */
BitReader* readFileToBitReader(const char *filename);

/**
 * @brief Frees the memory allocated for a BitReader.
 * 
 * @param br Pointer to the BitReader to be freed.
 */
void freeBitReader(BitReader *br);

/**
 * @brief Reads a specified number of bits from the bit buffer.
 * 
 * @param br Pointer to the BitReader.
 * @param nbits Number of bits to read.
 * @return The value of the bits read as an unsigned integer.
 */
unsigned int readBits(BitReader *br, int nbits);

#endif
