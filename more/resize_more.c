// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char *num = argv[1];
    char *infile = argv[2];
    char *outfile = argv[3];
    float f = atof(num);

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    //***********************************************************************************************//

    printf("bf.bfType = %x, bf.bfOffBits = %d, bi.biSize = %d, bi.biBitCount = %d, bi.biCompression = %d\n",
    bf.bfType, bf.bfOffBits, bi.biSize, bi.biBitCount, bi.biCompression);

    printf("bi.biWidth = %d, bi.biHeight = %d, bi.biSizeImage = %d, bf.bfSize = %d, bf.bfOffBits = %d\n",
    bi.biWidth, bi.biHeight, bi.biSizeImage, bf.bfSize, bf.bfOffBits);

    //*************************************************************************************************//

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    printf("Padding = %d\n", padding);

    //Preserving the current width & height
    int width_prev = bi.biWidth;
    int height_prev = bi.biHeight;

    // changing the width and height according to f
    bi.biWidth *= f;
    bi.biHeight *= f;

    // determine the new padding for scanlines
    int padding_new = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    printf("Padding new = %d\n", padding_new);

    // recalculation of biSizeImage & bfSize
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding_new) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

    //***********************************************************************************************//

    printf("bf.bfType = %x, bf.bfOffBits = %d, bi.biSize = %d, bi.biBitCount = %d, bi.biCompression = %d\n",
    bf.bfType, bf.bfOffBits, bi.biSize, bi.biBitCount, bi.biCompression);

    printf("bi.biWidth = %d, bi.biHeight = %d, bi.biSizeImage = %d, bf.bfSize = %d, bf.bfOffBits = %d\n",
    bi.biWidth, bi.biHeight, bi.biSizeImage, bf.bfSize, bf.bfOffBits);

    //*************************************************************************************************//

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    for (int i = 0; i < bi.biHeight; i++)
    {
        // selecting the rows
        float row = i / f;
        int row_int = row;

        // taking the cursor to the beginning of the selected row
        long beginning = row_int * width_prev * sizeof(RGBTRIPLE);
        fseek(inptr, beginning, SEEK_SET);

        // iterate over columns in scanline
        for (int j = 0; j < bi.biWidth; j++)
        {
            // selecting the rows
            float col = j / f;
            int col_int = col;

            // take the current position
            int current_pos = ftell(inptr);

            // difference between the next pixel and the previous pixel
            long offset = ((row_int * width_prev * sizeof(RGBTRIPLE)) + (col_int * sizeof(RGBTRIPLE))) - current_pos;

            // take cursor to the specific byte
            fseek(inptr, offset, SEEK_CUR);

            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // last read position of i'th row after each row iteration
        int last_byte_read_of_row = ftell(inptr);

        // difference between last_byte_read_of_row & the last byte of i'th row
        long difference = ((width_prev * sizeof(RGBTRIPLE)) + (i * width_prev * sizeof(RGBTRIPLE))) - last_byte_read_of_row;

        // taking the cursor to the end of i'th row
        fseek(inptr, difference, SEEK_CUR);

        // skip over padding of input file, if any
        fseek(inptr, padding, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding_new; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
