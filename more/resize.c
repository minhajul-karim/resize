#include <math.h>
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

    float f;
    char ch;
    if (sscanf(argv[1], "%f %c", &f, &ch) != 1)
    {
        fprintf(stderr, "Enter only floats\n");
        return -1;
    }
    else if (f <= 0 || f > 100)
    {
        fprintf(stderr, "The floating point number, f should be in range of (0 < f <= 100)\n");
        return -2;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

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

    // preserving the current width & height
    int width_prev = bi.biWidth;
    int height_prev = abs(bi.biHeight);
    int n = 0;

    // changing the width and height according to f
    if (f < 1)
    {
        bi.biWidth *= f;
        bi.biHeight *= f;
    }

    else
    {
        n = f;
        bi.biWidth *= n;
        bi.biHeight *= n;
    }

    // determine the new padding for scanlines
    int padding_new = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // recalculation of biSizeImage & bfSize
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding_new) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    int count = 0;

    // for f < 1
    if (f < 1)
    {
        //Setting the cols and rows in array
        int arr[width_prev];
        for (int m = 0; m < width_prev; m++)
        {
            float val = round((float)m / f);
            int val_int = val;
            arr[m] = val_int;
        }

        int row_counter = 0;
        int biHeight = abs(bi.biHeight);
        int biWidth = bi.biWidth;

        // iterate over infile's scanlines
        for (int i = 0; i < height_prev; i++)
        {
            // iterate over columns in scanline
            int col_counter = 0;
            for (int j = 0; j < width_prev; j++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile
                if (row_counter < biHeight && col_counter < biWidth && i == arr[row_counter] && j == arr[col_counter])
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    col_counter++;
                }
            }

            if (i == arr[row_counter])
            {
                row_counter++;

                // then add it back (to demonstrate how)
                for (int k = 0; k < padding_new; k++)
                {
                    fputc(0x00, outptr);
                }
            }

            // skip over padding of input file, if any
            fseek(inptr, padding, SEEK_CUR);
        }
    }

    // f >= 1
    else
    {
        // iterate over infile's scanlines
        for (int i = 0; i < height_prev; i++)
        {
            // repeat rows vertically
            for (int j = 1; j <= n; j++)
            {
                count = 0;
                // repeat rows horizontally
                for (int k = 0; k < width_prev; k++)
                {
                    RGBTRIPLE triple;

                    // read RGB triple from infile
                    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                    count++;
                    // repeat pixels horizontally
                    for (int l = 1; l <= n; l++)
                    {
                        fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    }
                }

                // add padding if applicable
                for (int m = 0; m < padding_new; m++)
                {
                    fputc(0x00, outptr);
                }
                // setting the cursor to the initial position
                fseek(inptr, -count * sizeof(RGBTRIPLE), SEEK_CUR);
            }
            // resetting the cursor to the last read position
            fseek(inptr, count * sizeof(RGBTRIPLE), SEEK_CUR);

            // skip padding if applicable
            fseek(inptr, padding, SEEK_CUR);
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
