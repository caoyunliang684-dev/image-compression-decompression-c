#ifndef DECODAGE_H
#define DECODAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quadtree.h"

/**
 * @brief Decodes a QuadTree from a file.
 * 
 * This function reads an encoded QuadTree from a file and reconstructs
 * the QuadTree structure. It also retrieves additional metadata, such as
 * the tree depth and a custom parameter `g2`.
 * 
 * @param filename The name of the file containing the encoded QuadTree data.
 * @param depth Pointer to an integer where the depth of the decoded QuadTree will be stored.
 * @param g2 Pointer to an integer where an additional parameter will be stored (e.g., grid size or configuration).
 * @return A pointer to the root of the reconstructed QuadTree.
 */
QuadTreeNode* decodeQuadTree(const char *filename, int *depth, int *g2);

/**
 * @brief Converts a QuadTree to a PGM (Portable Gray Map) image file.
 * 
 * This function traverses a QuadTree structure and generates a PGM file
 * representing the image encoded in the tree.
 * 
 * @param pgmFilename The name of the output PGM file.
 * @param tree Pointer to the root of the QuadTree.
 * @param max_nodes The maximum number of nodes allowed in the QuadTree.
 * @param depth The depth of the QuadTree.
 */
void quadtreeToPGM(const char *pgmFilename, QuadTreeNode *tree, int max_nodes, int depth);

/**
 * @brief Converts a filtered QuadTree to a PGM file.
 * 
 * This function processes a filtered QuadTree structure and generates
 * a PGM file that represents the resulting image after filtering.
 * 
 * @param pgmFilename The name of the output PGM file.
 * @param tree Pointer to the root of the filtered QuadTree.
 * @param max_nodes The maximum number of nodes allowed in the QuadTree.
 * @param depth The depth of the QuadTree.
 */
void quadtreeToPGMfiltrage(const char *pgmFilename, QuadTreeNode *tree, int max_nodes, int depth);

#endif
