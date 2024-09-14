#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1) // Ensures structure is tightly packed
typedef struct {
    unsigned short type;
    unsigned int size;
    unsigned short reserved1, reserved2;
    unsigned int offset;
} BMPHeader;

typedef struct {
    unsigned int size;
    int width, height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int compression;
    unsigned int imageSize;
    int xResolution, yResolution;
    unsigned int colorsUsed;
    unsigned int importantColors;
} DIBHeader;

#pragma pack(pop)

int isBMPHeader(unsigned char *chunkData) {
    BMPHeader *bmpHeader = (BMPHeader *)chunkData;
    return (bmpHeader->type == 0x4D42); // Check if 'BM'
}

void rotateImage(unsigned char *input, unsigned char *output, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            output[x * height + (height - y - 1)] = input[y * width + x]; // 90-degree rotation
        }
    }
}

int main() {
    FILE *file = fopen("COE499.bmp", "rb");
    if (!file) {
        printf("Error: Cannot open file\n");
        return 1;
    }

    BMPHeader bmpHeader;
    fread(&bmpHeader, sizeof(BMPHeader), 1, file);

    DIBHeader dibHeader;
    fread(&dibHeader, sizeof(DIBHeader), 1, file);

    int imageSize = dibHeader.width * dibHeader.height * (dibHeader.bitsPerPixel / 8);

    unsigned char *imageData = (unsigned char *)malloc(imageSize);
    fseek(file, bmpHeader.offset, SEEK_SET);
    fread(imageData, imageSize, 1, file);
    fclose(file);

    unsigned char *rotatedImageData = (unsigned char *)malloc(imageSize);
    rotateImage(imageData, rotatedImageData, dibHeader.width, dibHeader.height);

    // Store rotated image into buffer
    FILE *outputFile = fopen("rotated.bmp", "wb");
    fwrite(&bmpHeader, sizeof(BMPHeader), 1, outputFile);
    fwrite(&dibHeader, sizeof(DIBHeader), 1, outputFile);
    fwrite(rotatedImageData, imageSize, 1, outputFile);
    fclose(outputFile);

    
    return 0;
}
