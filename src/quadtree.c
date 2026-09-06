#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    unsigned char mean;      
    unsigned char error;     
    unsigned char uniform;  
} QuadTreeNode;

void buildQuadTree(QuadTreeNode *tree, unsigned char *data, int x, int y, int size, int depth, int node_index, int img_width) {
    if (depth == 0 || size == 1) {
        unsigned char value = data[y * img_width + x]; 
        tree[node_index].mean = value;   
        tree[node_index].error = 0;     
        tree[node_index].uniform = 1;    
        return; 
    }
    int half = size / 2;
    int offsets[4][2] = {{0, 0}, {half, 0}, {half, half}, {0, half}};
    unsigned char child_means[4];
    unsigned char child_uniforms[4];
    for (int k = 0; k < 4; k++) {
        int startX = x + offsets[k][0];
        int startY = y + offsets[k][1];
        int child_index = 4 * node_index + k + 1; 
        buildQuadTree(tree, data, startX, startY, half, depth - 1, child_index, img_width);
        child_means[k] = tree[child_index].mean;
        child_uniforms[k] = tree[child_index].uniform;
    }
    int sum = child_means[0] + child_means[1] + child_means[2] + child_means[3];
    unsigned char mean = sum / 4;
    unsigned char error = sum % 4;
    tree[node_index].mean = mean;
    tree[node_index].error = error;
    tree[node_index].uniform = (error == 0) && child_uniforms[0] && child_uniforms[1] && child_uniforms[2] && child_uniforms[3]&&(child_means[0] == child_means[1] && child_means[1] == child_means[2]&&child_means[2]==child_means[3]); // 如果误差为0，则为均匀
}


void freeQuadTree(QuadTreeNode *tree) {
    free(tree); 
}
