#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "qtc.h"

/**
 * @brief Checks if a given string represents a valid double value.
 * 
 * This function uses `strtod` to determine if the input string can
 * be converted to a double.
 * 
 * @param str The string to check.
 * @return Returns 1 if the string is a valid double, otherwise 0.
 */
int isDouble(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0; 
    }
    char *endptr;
    strtod(str, &endptr); 
    return (*endptr == '\0');
}

/**
 * @brief Prints the usage instructions for the program.
 * 
 * Displays the available options and example usage.
 * 
 * @param prog The name of the program (from `argv[0]`).
 */
static void printUsage(const char *prog) {
    fprintf(stderr, "Usage: %s [option] -c|-u -i <input> -o <output> -g -a <alpha>\n", prog);
    fprintf(stderr, "  -c              encode, input PGM, output QTC\n");
    fprintf(stderr, "  -u              decode, input QTC, output PGM\n");
    fprintf(stderr, "  -i <file>       input filename (.pgm or .qtc)\n");
    fprintf(stderr, "  -o <file>       output filename (default out.qtc or out.pgm)\n");
    fprintf(stderr, "  -a <alpha>      filtrage \n");
    fprintf(stderr, "  -g              grille \n");
    fprintf(stderr, "  -h              help\n");
    fprintf(stderr, "\nExample:\n");
    fprintf(stderr, "  %s -c -i ./PGM/input.pgm -o ./QTC/output.qtc -a 1.5\n", prog);
    fprintf(stderr, "  %s -u -i ./QTC/input.qtc -o ./PGM/output.pgm\n", prog);
}

/**
 * @brief Main function to encode or decode an image using the QuadTree-based compression.
 * 
 * The program processes command-line arguments to determine the mode (encode or decode),
 * input and output filenames, and optional parameters like filtering alpha and grid display.
 * 
 * Supported options:
 * - `-c`: Encode a PGM image into QTC format.
 * - `-u`: Decode a QTC file into PGM format.
 * - `-i <file>`: Specify the input file.
 * - `-o <file>`: Specify the output file (default: `out.qtc` or `out.pgm`).
 * - `-a <alpha>`: Enable filtering with the given alpha value.
 * - `-g`: Enable grid display during decoding or encoding.
 * - `-h`: Display help and usage information.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return Returns EXIT_SUCCESS on successful completion, otherwise EXIT_FAILURE.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    int encodeMode = 0;        /**< Encoding mode: 1 for encode, 2 for decode. */
    const char *inputFile = NULL;
    const char *outputFile = NULL;
    int g = 0;                 /**< Enable grid display. */
    double alpha = 0.0;        /**< Filtering alpha value. */

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-c")) {
            encodeMode = 1;  
        } else if (!strcmp(argv[i], "-u")) {
            encodeMode = 2;  
        } else if (!strcmp(argv[i], "-i") && i+1 < argc) {
            inputFile = argv[++i];
        } else if (!strcmp(argv[i], "-o") && i+1 < argc) {
            outputFile = argv[++i];
        } else if (!strcmp(argv[i], "-a") && i+1 < argc && isDouble(argv[i+1])) {
            alpha = atof(argv[++i]);
        } else if (!strcmp(argv[i], "-h")) {
            printUsage(argv[0]);
            return 0; 
        } else if (!strcmp(argv[i], "-g")) {
            g = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            printUsage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (encodeMode == 0) {
        fprintf(stderr, "error: need -c or -u\n");
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }
    if (!inputFile) {
        fprintf(stderr, "error: need -i <inputFile>\n");
        return EXIT_FAILURE;
    }
    if (!outputFile) {
        outputFile = (encodeMode == 1) ? "./QTC/out.qtc" : "./PGM/out.pgm";
    } 

    if (encodeMode == 1) {
        // Encode mode
        PGMImage *img = readPGM(inputFile);
        if (!img) {
            fprintf(stderr, "Fail to read PGM: %s\n", inputFile);
            return EXIT_FAILURE;
        }

        if (img->width != img->height || (img->width & (img->width - 1)) != 0) {
            fprintf(stderr, "The image must be 2^n x 2^n\n");
            freePGM(img);
            return EXIT_FAILURE;
        }

        int depth = (int)log2(img->width);
        int max_nodes = (int)((pow(4, depth + 1) - 1) / 3);
        QuadTreeNode *tree = (QuadTreeNode *)calloc(max_nodes, sizeof(QuadTreeNode));

        if (!tree) {
            fprintf(stderr, "Memory allocation failed\n");
            freePGM(img);
            return EXIT_FAILURE;
        }

        buildQuadTree(tree, img->data, 0, 0, img->width, depth, 0, img->width);

        if (alpha) {
            double *variance = computeAllVariance(tree, max_nodes, depth);
            double maxvar, medvar;
            findMaxVarAndMedVar(variance, max_nodes, &maxvar, &medvar);
            double sigma = medvar / maxvar;
            filtrage(tree, variance, 0, max_nodes, sigma, alpha, 0, depth);
            free(variance);
        }

        encodeQuadTree(outputFile, tree, max_nodes, img->width, depth, g);
        printf("Finished encoding => %s\n", outputFile);

        freeQuadTree(tree);
        freePGM(img);
    } else {
        // Decode mode
        int depth = 0;
        int g2 = 0;
        QuadTreeNode *tree = decodeQuadTree(inputFile, &depth, &g2);
        if (!tree) {
            fprintf(stderr, "Fail to decode: %s\n", inputFile);
            return EXIT_FAILURE;
        }

        int max_nodes = (int)((pow(4, depth + 1) - 1) / 3);
        if (g || g2) {
            quadtreeToPGMfiltrage(outputFile, tree, max_nodes, depth);
        } else {
            quadtreeToPGM(outputFile, tree, max_nodes, depth);
        }

        printf("Finished decoding => %s\n", outputFile);
        freeQuadTree(tree);
    }

    return EXIT_SUCCESS;
}
