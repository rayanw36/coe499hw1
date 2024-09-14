#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1) // Ensures structure is tightly packed
typedef struct {
    unsigned short type; // Magic identifier: 0x4D42 ('BM')
    unsigned int size;   // File size in bytes
    unsigned short reserved1, reserved2;
    unsigned int offset; // Offset to image data
} BMPHeader;

typedef struct {
    unsigned int size;       // Header size in bytes
    int width, height;       // Width and height of image
    unsigned short planes;   // Number of color planes
    unsigned short bitsPerPixel; // Bits per pixel
    unsigned int compression; // Compression type
    unsigned int imageSize;   // Image size in bytes
    int xResolution, yResolution;
    unsigned int colorsUsed;
    unsigned int importantColors;
} DIBHeader;

#pragma pack(pop)

// Function to initialize a valid BMP and DIB header for a 640x480 24-bit BMP image
void createBMPHeader(BMPHeader *bmpHeader, DIBHeader *dibHeader, int width, int height) {
    bmpHeader->type = 0x4D42;  // 'BM'
    bmpHeader->size = sizeof(BMPHeader) + sizeof(DIBHeader) + (width * height * 3);  // File size
    bmpHeader->reserved1 = 0;
    bmpHeader->reserved2 = 0;
    bmpHeader->offset = sizeof(BMPHeader) + sizeof(DIBHeader);  // Start of pixel data

    dibHeader->size = sizeof(DIBHeader);
    dibHeader->width = width;
    dibHeader->height = height;
    dibHeader->planes = 1;
    dibHeader->bitsPerPixel = 24;  // 24-bit image
    dibHeader->compression = 0;    // No compression
    dibHeader->imageSize = width * height * 3;  // Image size
    dibHeader->xResolution = 2835; // 72 DPI (in pixels per meter)
    dibHeader->yResolution = 2835; // 72 DPI (in pixels per meter)
    dibHeader->colorsUsed = 0;
    dibHeader->importantColors = 0;
}

int main() {
    FILE *dumpFile = fopen("caine.mem", "rb");
    if (!dumpFile) {
        printf("Error: Cannot open memory dump file\n");
        return 1;
    }

    // Get the size of the memory dump
    fseek(dumpFile, 0, SEEK_END);
    long dumpSize = ftell(dumpFile);
    rewind(dumpFile);

    // Define BMP image size (640x480, 24-bit)
    int width = 640;
    int height = 480;
    int chunkSize = width * height * 3; // Assuming 24-bit BMP image

    // Allocate memory for the image chunk
    unsigned char *chunkData = (unsigned char *)malloc(chunkSize);

    // Initialize BMP and DIB headers
    BMPHeader bmpHeader;
    DIBHeader dibHeader;
    createBMPHeader(&bmpHeader, &dibHeader, width, height);

    int fileCounter = 0;  // To keep track of output file names
    long currentPosition = 0;

    // Loop through the entire memory dump in chunks
    while (currentPosition + chunkSize <= dumpSize) {
        // Read a chunk from memory dump
        fread(chunkData, 1, chunkSize, dumpFile);

        // Construct a unique file name for the extracted image
        char outputFileName[50];
        sprintf(outputFileName, "extracted_image_%03d.bmp", fileCounter);

        // Write the BMP header, DIB header, and chunk as BMP image
        FILE *outputFile = fopen(outputFileName, "wb");
        if (!outputFile) {
            printf("Error: Cannot create output file %s\n", outputFileName);
            free(chunkData);
            fclose(dumpFile);
            return 1;
        }

        // Write BMP and DIB headers
        fwrite(&bmpHeader, sizeof(BMPHeader), 1, outputFile);
        fwrite(&dibHeader, sizeof(DIBHeader), 1, outputFile);

        // Write image data (the chunk from memory dump)
        fwrite(chunkData, 1, chunkSize, outputFile);

        fclose(outputFile);
        printf("Extracted image saved as %s\n", outputFileName);

        // Move to the next chunk
        currentPosition += chunkSize;
        fseek(dumpFile, currentPosition, SEEK_SET);

        fileCounter++;
    }

    fclose(dumpFile);
    free(chunkData);

    printf("Finished scanning memory dump.\n");
    return 0;
}

