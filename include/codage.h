#ifndef CODAGE_H
#define CODAGE_H

#include <stdio.h>
#include <stdlib.h>
#include "quadtree.h"

/**
 * @brief Encodes a QuadTree into a file.
 * 
 * This function encodes the structure of a QuadTree into a specified file.
 * The encoding includes information about the tree's nodes, width, depth, 
 * and additional parameters for compression or format customization.
 * 
 * @param filename The name of the output file where the encoded tree will be stored.
 * @param tree A pointer to the root of the QuadTree structure to be encoded.
 * @param max_nodes The maximum number of nodes allowed in the QuadTree.
 * @param width The width of the image or structure being encoded.
 * @param depth The depth of the QuadTree.
 * @param g A parameter for additional encoding customization (e.g., segmentation grid).
 */
void encodeQuadTree(const char *filename, QuadTreeNode *tree, int max_nodes, int width, int depth, int g);

#endif
