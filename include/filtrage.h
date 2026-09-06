#ifndef FILTRAGE_H
#define FILTRAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "quadtree.h"

/**
 * @brief Computes the variance for all nodes in a QuadTree.
 * 
 * This function calculates the variance of pixel intensities for every node 
 * in the QuadTree, storing the results in an array.
 * 
 * @param tree Pointer to the root of the QuadTree.
 * @param maxNodes The maximum number of nodes in the QuadTree.
 * @param depth The depth of the QuadTree.
 * @return A pointer to an array of variances for all nodes in the QuadTree.
 *         The caller is responsible for freeing the allocated memory.
 */
double* computeAllVariance(QuadTreeNode *tree, int maxNodes, int depth);

/**
 * @brief Finds the maximum and median variance from an array of variances.
 * 
 * This function computes the maximum and median values from the provided 
 * variance array, useful for adaptive filtering processes.
 * 
 * @param variance Pointer to an array of variance values.
 * @param n The number of variance values in the array.
 * @param pMax Pointer to a double where the maximum variance will be stored.
 * @param pMed Pointer to a double where the median variance will be stored.
 */
void findMaxVarAndMedVar(double *variance, int n, double *pMax, double *pMed);

/**
 * @brief Filters a QuadTree based on variance thresholds.
 * 
 * This function applies a filtering process to a QuadTree by evaluating the 
 * variance of nodes. Nodes with a variance below a computed threshold are 
 * marked as uniform.
 * 
 * @param tree Pointer to the root of the QuadTree.
 * @param variance Pointer to the array of variances for the QuadTree nodes.
 * @param nodeIndex Index of the current node being processed.
 * @param maxNodes The maximum number of nodes in the QuadTree.
 * @param sigma The base threshold for variance filtering.
 * @param alpha Multiplier used to adjust the variance threshold adaptively.
 * @param currentDepth The current depth of the node in the QuadTree.
 * @param maxDepth The maximum depth of the QuadTree.
 * @return Returns 1 if the node is marked as uniform, otherwise returns 0.
 */
int filtrage(QuadTreeNode *tree, double *variance,
             int nodeIndex, int maxNodes,
             double sigma, double alpha,
             int currentDepth, int maxDepth);

#endif
