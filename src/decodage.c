#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "qtc.h"

#define MAX_HEADER_SIZE 256


QuadTreeNode* decodeQuadTree(const char *filename,int *depth, int *g2) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Fail to open the file");
        exit(EXIT_FAILURE);
    }
    char line[256];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    if (strncmp(line, "Q1", 2) != 0) {
        fprintf(stderr, "Not a qtc file\n");
        fclose(fp);
       exit(EXIT_FAILURE);
    }
    
    long pos;
    while (1) {
        pos = ftell(fp);
        if (!fgets(line, sizeof(line), fp)) {
            break;
        }
        if (line[0] == '#') {
            if (strstr(line, " Grille required") != NULL) {
                *g2 = 1; 
            }
        }else{
            fseek(fp, pos, SEEK_SET);
            break;
            }    
        }
    long bitStart = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long bitEnd = ftell(fp);
    long bitSize = bitEnd - bitStart;
    fseek(fp, bitStart, SEEK_SET);
    BitReader *br = (BitReader*)malloc(sizeof(BitReader));
    br->data = (unsigned char*)malloc(bitSize);
    fread(br->data, 1, bitSize, fp);
    fclose(fp);
    br->bitpos = 0;
    br->bit_size = (size_t)bitSize * 8;
    *depth = readBits(br, 8);
    int max_nodes = (pow(4,*depth+1)-1)/3;
    QuadTreeNode *tree = (QuadTreeNode *)calloc(max_nodes, sizeof(QuadTreeNode));
    int *mean = (int *)calloc(max_nodes,sizeof(int));
    for (int i = 0 ; i < max_nodes;i++){
        mean[i] = -1;
    }
    for (int i = 0; i < max_nodes; i++) {
        if (4*i + 1 >= max_nodes) {
            if (mean[i] != -1){
                continue;
            }
            if ((i % 4) == 0 && i != 0){
                tree[i].error = 0;
                tree[i].uniform = 1;
                tree[i].mean = (4 * tree[(i-4)/4].mean + tree[(i-4)/4].error)  - (tree[i-3].mean + tree[i-2].mean + tree[i-1].mean);
                continue;
        }
            unsigned char m = readBits(br, 8); 
            tree[i].mean = m;
            tree[i].error = 0; 
            tree[i].uniform = 1;
            continue;
        }
        if (mean[i] != -1){
            for (int j = 1 ; j <= 4 ;j++){
                tree[4*i + j].mean = tree[i].mean;
                tree[4*i + j].error = 0;
                tree[4*i + j].uniform = 1;
                mean[4*i + j] = tree[i].mean;
            }
            continue;
        }
        if ((i % 4) == 0 && i != 0){
            unsigned char e = readBits(br,2);
            tree[i].error = e;
             if (e == 0) {
                unsigned char u = readBits(br, 1);
                tree[i].uniform = u;
             } else {
                tree[i].uniform = 0;
            }
            tree[i].mean = (4 * tree[(i-4)/4].mean + tree[(i-4)/4].error)  - (tree[i-3].mean + tree[i-2].mean + tree[i-1].mean);        
        }
        else {
            unsigned char m = readBits(br, 8);
            tree[i].mean = m;
            unsigned char e = readBits(br, 2);
            tree[i].error = e;
            if (e == 0) {
                unsigned char u = readBits(br, 1);
                tree[i].uniform = u;
            } else {
                tree[i].uniform = 0;
            }
        }
        if(tree[i].uniform == 1){
            for (int j = 1 ; j <= 4 ;j++){
                tree[4*i + j].mean = tree[i].mean;
                tree[4*i + j].error = 0;
                tree[4*i + j].uniform = 1;
                mean[4*i + j] = tree[i].mean;
            }
        }
    }
    freeBitReader(br);
    free(mean);
    return tree;
}

/**
 * @brief Fills a block of an image with pixel values based on the QuadTree.
 * 
 * Recursively fills blocks of an image with pixel values derived from the QuadTree,
 * ensuring proper representation of uniform regions and subdivisions.
 * 
 * @param image Pointer to the image buffer.
 * @param width The width of the image.
 * @param startX The starting x-coordinate of the block.
 * @param startY The starting y-coordinate of the block.
 * @param size The size of the block.
 * @param nodeIndex The index of the current node in the QuadTree.
 * @param tree Pointer to the QuadTree structure.
 * @param maxNodes The maximum number of nodes in the QuadTree.
 * @param currDepth The current depth of the QuadTree.
 * @param maxDepth The maximum depth of the QuadTree.
 */
void fillBlock(unsigned char *image, int width,
               int startX, int startY, int size,
               int nodeIndex,
               QuadTreeNode *tree, int maxNodes,
               int currDepth, int maxDepth)
{
    if (nodeIndex >= maxNodes) {
        return;
    }
    if (tree[nodeIndex].uniform == 1 || (currDepth == maxDepth)) {
        unsigned char val = tree[nodeIndex].mean;
        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                int px = startX + c;
                int py = startY + r;
                image[py * width + px] = val;
            }
        }
        return;
    }
    int c1 = 4 * nodeIndex + 1; 
    int c2 = 4 * nodeIndex + 2; 
    int c3 = 4 * nodeIndex + 4; 
    int c4 = 4 * nodeIndex + 3; 
    int half = size / 2;
    fillBlock(image, width, startX, startY, half,
              c1, tree, maxNodes, currDepth+1, maxDepth);

    fillBlock(image, width, startX + half, startY, half,
              c2, tree, maxNodes, currDepth+1, maxDepth);

    fillBlock(image, width, startX, startY + half, half,
              c3, tree, maxNodes, currDepth+1, maxDepth);

    fillBlock(image, width, startX + half, startY + half, half,
              c4, tree, maxNodes, currDepth+1, maxDepth);
}

/**
 * @brief Draws block outlines on an image based on the QuadTree structure.
 * 
 * This function recursively draws outlines of QuadTree blocks on an image,
 * highlighting the segmentation of the QuadTree.
 * 
 * @param image Pointer to the image buffer.
 * @param width The width of the image.
 * @param startX The starting x-coordinate of the block.
 * @param startY The starting y-coordinate of the block.
 * @param size The size of the block.
 * @param nodeIndex The index of the current node in the QuadTree.
 * @param tree Pointer to the QuadTree structure.
 * @param maxNodes The maximum number of nodes in the QuadTree.
 * @param currDepth The current depth of the QuadTree.
 * @param maxDepth The maximum depth of the QuadTree.
 */
void drawBlock(unsigned char *image, int width,
               int startX, int startY, int size,
               int nodeIndex,
               QuadTreeNode *tree, int maxNodes,
               int currDepth, int maxDepth)
{
    if (nodeIndex >= maxNodes) {
        return;
    }

    if (tree[nodeIndex].uniform == 1 || (currDepth == maxDepth)) {
        if (size <= 1) {
            return;
        }
        for (int x = startX; x < startX + size; x++) {
            image[startY * width + x] = 200;
        }
        for (int x = startX; x < startX + size; x++) {
            image[(startY + size - 1) * width + x] = 200;
        }
        for (int y = startY; y < startY + size; y++) {
            image[y * width + startX] =200;
        }
        for (int y = startY; y < startY + size; y++) {
            image[y * width + (startX + size - 1)] = 200;
        }

        return; 
    }
    int c1 = 4 * nodeIndex + 1; 
    int c2 = 4 * nodeIndex + 2; 
    int c3 = 4 * nodeIndex + 4; 
    int c4 = 4 * nodeIndex + 3;
    int half = size / 2;
    drawBlock(image, width, startX, startY, half,
              c1, tree, maxNodes, currDepth+1, maxDepth);

    drawBlock(image, width, startX + half, startY, half,
              c2, tree, maxNodes, currDepth+1, maxDepth);

    drawBlock(image, width, startX, startY + half, half,
              c3, tree, maxNodes, currDepth+1, maxDepth);

    drawBlock(image, width, startX + half, startY + half, half,
              c4, tree, maxNodes, currDepth+1, maxDepth);
}

void quadtreeToPGM(const char *pgmFilename,QuadTreeNode *tree,int max_nodes,int depth)
{
    int width = 1 << depth;
    unsigned char *image = (unsigned char*)malloc(width * width);
    fillBlock(image, width, 0, 0, width, 0, tree, max_nodes,0, depth );
    FILE *fp = fopen(pgmFilename, "wb");
    if (!fp) {
        perror("Fail to create PGM");
        free(image);
        return;
    }
    fprintf(fp, "P5\n");
    fprintf(fp, "# Created by CAO\n");
    fprintf(fp, "%d %d\n255\n", width, width);
    fwrite(image, 1, width * width, fp);
    fclose(fp);
    free(image);
}


void quadtreeToPGMfiltrage(const char *pgmFilename,QuadTreeNode *tree,int max_nodes,int depth)
{
    int width = 1 << depth;
    unsigned char *image = (unsigned char*)malloc(width * width);
    drawBlock(image, width, 0, 0, width, 0,tree, max_nodes,0, depth );
    FILE *fp = fopen(pgmFilename, "wb");
    if (!fp) {
        perror("Fail to create PGM");
        free(image);
        return;
    }
    fprintf(fp, "P5\n");
    fprintf(fp, "# Created by CAO\n");
    fprintf(fp, "%d %d\n255\n", width, width);
    fwrite(image, 1, width * width, fp);
    fclose(fp);
    free(image);
}