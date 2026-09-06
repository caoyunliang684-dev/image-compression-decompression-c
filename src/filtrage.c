#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "qtc.h"

/**
 * @brief Gets the index of a child node in a QuadTree.
 * 
 * Computes the index of a specified child of a given node in the QuadTree.
 * If the child index exceeds the maximum number of nodes, returns -1.
 * 
 * @param i The index of the parent node.
 * @param childNum The child number (0 to 3).
 * @param maxNodes The maximum number of nodes in the QuadTree.
 * @return The index of the child node, or -1 if the child index is out of bounds.
 */
static int getChildIndex(int i, int childNum, int maxNodes) {
    int c = 4 * i + (childNum + 1); 
    if (c >= maxNodes) return -1; 
    return c;
}

/**
 * @brief Computes the variance of a single node in the QuadTree.
 * 
 * Recursively calculates the variance for a given node based on its children's 
 * variances and mean values. Stores the variance in the provided variance array.
 * 
 * @param tree Pointer to the QuadTree.
 * @param variance Array to store the variance of each node.
 * @param nodeIndex The index of the current node.
 * @param maxNodes The maximum number of nodes in the QuadTree.
 * @param depth The remaining depth to process.
 * @return The computed variance of the node.
 */
double computeNodeVariance(QuadTreeNode *tree, double *variance,
                           int nodeIndex, int maxNodes, int depth)
{
    if (nodeIndex >= maxNodes) return 0.0;
    if (tree[nodeIndex].uniform == 1 || depth == 0) {
        variance[nodeIndex] = 0.0;
        return 0.0;
    }

    double childVar[4];
    unsigned char childMean[4];
    for (int c = 0; c < 4; c++) {
        int cIdx = getChildIndex(nodeIndex, c, maxNodes);
        if (cIdx == -1) {
            childVar[c] = 0.0;
            childMean[c] = tree[nodeIndex].mean;
        } else {
            double cv = computeNodeVariance(tree, variance, cIdx, maxNodes, depth - 1);
            childVar[c] = cv;
            childMean[c] = tree[cIdx].mean;
        }
    }

    double sumSq = 0.0;
    double m_n = (double)tree[nodeIndex].mean;
    for (int c = 0; c < 4; c++) {
        double m_c = (double)childMean[c];
        sumSq += (childVar[c] * childVar[c]) + (m_n - m_c) * (m_n - m_c);
    }

    double varN = sqrt(sumSq / 4.0);
    variance[nodeIndex] = varN;
    return varN;
}


double* computeAllVariance(QuadTreeNode *tree, int maxNodes, int depth) {
    double *variance = (double*)calloc(maxNodes, sizeof(double));
    computeNodeVariance(tree, variance, 0, maxNodes, depth);
    return variance;
}



void findMaxVarAndMedVar(double *variance, int n, double *pMax, double *pMed) {
    double mx = variance[0];
    for (int i = 1; i < n; i++) {
        if (variance[i] > mx) {
            mx = variance[i];
        }
    }
    *pMax = mx;

    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += variance[i];
    }
    *pMed = sum / n;
}


int filtrage(QuadTreeNode *tree, double *variance,
             int nodeIndex, int maxNodes,
             double sigma, double alpha,
             int currentDepth, int maxDepth)
{
    if (nodeIndex < 0 || nodeIndex >= maxNodes) {
        return 1;
    }

    if (tree[nodeIndex].uniform == 1) {
        return 1;
    }

    if (currentDepth == maxDepth) {
        return 1;
    }

    int sumChildren = 0;
    for (int c = 0; c < 4; c++) {
        int childIdx = getChildIndex(nodeIndex, c, maxNodes);
        sumChildren += filtrage(tree, variance, childIdx, maxNodes,
                                sigma * alpha, alpha,
                                currentDepth + 1, maxDepth);
    }

    if (sumChildren < 4) {
        return 0;
    }

    double var = variance[nodeIndex];
    if (var > sigma) {
        return 0;
    }

    tree[nodeIndex].uniform = 1; 
    tree[nodeIndex].error = 0;
    return 1;
}
