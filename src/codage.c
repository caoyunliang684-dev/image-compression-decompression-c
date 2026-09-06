#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "qtc.h"

/**
 * @brief Separates the mean, error, and uniform values from a QuadTree into individual arrays.
 * 
 * @param mean Pointer to the array where the mean values will be stored.
 * @param error Pointer to the array where the error values will be stored.
 * @param uniform Pointer to the array where the uniform flags will be stored.
 * @param tree Pointer to the QuadTree structure.
 * @param max_nodes The maximum number of nodes in the QuadTree.
 */
void separate(int *mean, unsigned char *error, unsigned char *uniform, QuadTreeNode *tree, int max_nodes) {
    for (int i = 0; i < max_nodes; i++) {
        mean[i] = tree[i].mean;
        error[i] = tree[i].error;
        uniform[i] = tree[i].uniform;
    }
}

/**
 * @brief Adjusts the mean values to mark specific nodes for special handling.
 * 
 * Sets mean values to -1 for every fourth node, excluding the root node.
 * 
 * @param mean Pointer to the array of mean values.
 * @param max_nodes The maximum number of nodes in the QuadTree.
 */
void adjustmean(int *mean, int max_nodes) {
    for (int i = 0; i < max_nodes; i++) {
        if ((i % 4) == 0 && i != 0) {
            mean[i] = -1;
        }
    }
}

/**
 * @brief Adjusts the mean values for uniform nodes to mark their children.
 * 
 * For each uniform node, sets the mean values of its child nodes to -2.
 * 
 * @param mean Pointer to the array of mean values.
 * @param uniform Pointer to the array of uniform flags.
 * @param max_nodes The maximum number of nodes in the QuadTree.
 */
void adjustuniform(int *mean, unsigned char *uniform, int max_nodes) {
    for (int i = 0; i < max_nodes; i++) {
        if (uniform[i] == 1 && (4 * i + 1) < max_nodes) {
            mean[4 * i + 1] = -2;
            mean[4 * i + 2] = -2;
            mean[4 * i + 3] = -2;
            mean[4 * i + 4] = -2;
        }
    }
}

/**
 * @brief Computes the compression rate.
 * 
 * Calculates the compression rate based on the number of bytes used and the image width.
 * 
 * @param bytes The number of bytes used in the compressed data.
 * @param width The width of the image.
 * @return The compression rate as a double.
 */
double compressrate(int bytes, int width) {
    if (bytes < 120) {
        return (double)120 / (8 * width * width);
    }
    return (double)bytes / (8 * width * width);
}

/**
 * @brief Compresses the QuadTree data into a bit buffer.
 * 
 * Writes the mean, error, and uniform data into a bit buffer for encoding.
 * 
 * @param mean Pointer to the array of mean values.
 * @param error Pointer to the array of error values.
 * @param uniform Pointer to the array of uniform flags.
 * @param max_nodes The maximum number of nodes in the QuadTree.
 * @param bb Pointer to the BitReader structure for storing compressed data.
 * @return The number of bits written during compression.
 */
int compress(int *mean, unsigned char *error, unsigned char *uniform, int max_nodes, BitReader *bb) {
    int count = 0;
    for (int i = 0; i < max_nodes; i++) {
        if (4 * i + 1 >= max_nodes && mean[i] >= 0) {
            writeBits(bb, mean[i] & 0xFF, 8);
            count += 8;
            continue;
        }
        if (mean[i] >= 0) {
            writeBits(bb, mean[i] & 0xFF, 8);
            count += 8;
            writeBits(bb, error[i], 2);
            count += 2;
            if (error[i] == 0) {
                writeBits(bb, uniform[i], 1);
                count += 1;
            }
        }
        if (mean[i] == -1 && 4 * i + 1 < max_nodes) {
            writeBits(bb, error[i], 2);
            count += 2;
            if (error[i] == 0) {
                writeBits(bb, uniform[i], 1);
                count += 1;
            }
        }
    }
    return count;
}

void encodeQuadTree(const char *filename, QuadTreeNode *tree, int max_nodes, int width, int depth, int g) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Fail to open the file");
        exit(EXIT_FAILURE);
    }
    time_t t;
    struct tm *tm_info;
    time(&t);
    tm_info = localtime(&t);
    int year = tm_info->tm_year + 1900;
    int month = tm_info->tm_mon + 1;
    int day = tm_info->tm_mday;

    int *mean = (int *)malloc(sizeof(int) * max_nodes);
    unsigned char *error = (unsigned char *)malloc(sizeof(unsigned char) * max_nodes);
    unsigned char *uniform = (unsigned char *)malloc(sizeof(unsigned char) * max_nodes);

    separate(mean, error, uniform, tree, max_nodes);
    adjustmean(mean, max_nodes);
    adjustuniform(mean, uniform, max_nodes);

    BitReader *bb = createBitBuffer(1024);
    writeBits(bb, depth & 0xFF, 8);
    int count = compress(mean, error, uniform, max_nodes, bb);
    double rate = compressrate(count, width);

    fprintf(file, "Q1\n");
    if (g) {
        fprintf(file, "# Grille required\n");
    }
    fprintf(file, "# Created by CAO : %02d-%02d-%d\n", day, month, year);
    fprintf(file, "# Compression rate : %f %%\n", rate * 100);

    size_t bytes_used = bitBufferSizeInBytes(bb);
    fwrite(bb->data, 1, bytes_used, file);

    fclose(file);
    free(mean);
    free(error);
    free(uniform);
}
