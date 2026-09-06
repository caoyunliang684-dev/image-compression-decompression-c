#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int width;
    int height;
    int max_gray;
    unsigned char *data;
} PGMImage;

PGMImage *readPGM(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Fail to open the file");
        return NULL;
    }
    char buffer[16];
    fgets(buffer, sizeof(buffer), file);
    if (strncmp(buffer, "P5", 2) != 0) {
        fprintf(stderr, "Unsupported file \n");
        fclose(file);
        return NULL;
    }

    while (fgetc(file) == '#') {
        while (fgetc(file) != '\n');
    }
    fseek(file, -1, SEEK_CUR);

    PGMImage *img = (PGMImage *)malloc(sizeof(PGMImage));
    fscanf(file, "%d %d %d", &img->width, &img->height, &img->max_gray);
    fgetc(file);
    img->data = (unsigned char *)malloc(img->width * img->height);
    fread(img->data, img->width * img->height, 1, file);
    fclose(file);
    return img;
}

void freePGM(PGMImage *img) {
    free(img->data);
    free(img);
}
