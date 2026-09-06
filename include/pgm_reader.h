#ifndef PGM_READER_H
#define PGM_READER_H

/**
 * @brief Structure representing a PGM (Portable Gray Map) image.
 */
typedef struct {
    int width;              /**< The width of the image in pixels. */
    int height;             /**< The height of the image in pixels. */
    int max_gray;           /**< The maximum gray level value (e.g., 255 for 8-bit images). */
    unsigned char *data;    /**< Pointer to the pixel data of the image. */
} PGMImage;

/**
 * @brief Reads a PGM image from a file.
 * 
 * This function reads a PGM file and creates a `PGMImage` structure 
 * containing the image's metadata (width, height, max gray value) and pixel data.
 * 
 * @param filename The name of the PGM file to be read.
 * @return A pointer to the newly created `PGMImage` structure containing the image data.
 *         Returns `NULL` if the file cannot be read or if an error occurs.
 */
PGMImage *readPGM(const char *filename);

/**
 * @brief Frees the memory associated with a `PGMImage` structure.
 * 
 * This function releases the memory allocated for the pixel data and 
 * the `PGMImage` structure itself.
 * 
 * @param img Pointer to the `PGMImage` structure to be freed.
 */
void freePGM(PGMImage *img);

#endif
