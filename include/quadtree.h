#ifndef QUADTREE_H
#define QUADTREE_H

/**
 * @brief Structure representing a node in a QuadTree.
 * 
 * Each node in the QuadTree stores information about a block of an image,
 * including its mean intensity, error value, and whether the block is uniform.
 */
typedef struct QuadTreeNode {
    unsigned char mean;     /**< The mean intensity value of the block. */
    unsigned char error;    /**< The error value for the block (used for lossless compression). */
    unsigned char uniform;  /**< A flag indicating whether the block is uniform (1 for uniform, 0 otherwise). */
} QuadTreeNode;

/**
 * @brief Builds a QuadTree from image data.
 * 
 * This function constructs a QuadTree recursively from the given image data. 
 * It calculates the mean intensity and evaluates whether the block is uniform 
 * or requires further subdivision.
 * 
 * @param tree Pointer to the QuadTree structure to build.
 * @param data Pointer to the image data.
 * @param x The x-coordinate of the top-left corner of the current block.
 * @param y The y-coordinate of the top-left corner of the current block.
 * @param size The size of the current block (width and height in pixels).
 * @param depth The current depth level of the QuadTree.
 * @param node_index The index of the current node in the QuadTree.
 * @param width The width of the image.
 */
void buildQuadTree(QuadTreeNode *tree, unsigned char *data, int x, int y, int size, int depth, int node_index, int width);

/**
 * @brief Frees the memory used by a QuadTree.
 * 
 * This function releases the memory allocated for a QuadTree and its nodes.
 * 
 * @param node Pointer to the root node of the QuadTree to be freed.
 */
void freeQuadTree(QuadTreeNode *node);

#endif
