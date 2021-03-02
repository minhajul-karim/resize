#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    long count = 0;
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: copy infile outfile\n");
        return 1;
    }

    // remember filenames
    char *num = argv[1];
    int n = atoi(num);
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

    //Test the initial attributes
    // printf("bf.bfType = %x, bf.bfOffBits = %d, bi.biSize = %d, bi.biBitCount = %d, bi.biCompression = %d\n",
    // bf.bfType, bf.bfOffBits, bi.biSize, bi.biBitCount, bi.biCompression);

    // printf("bi.biWidth = %d, bi.biHeight = %d, bi.biSizeImage = %d, bf.bfSize = %d, bf.bfOffBits = %d\n",
    // bi.biWidth, bi.biHeight, bi.biSizeImage, bf.bfSize, bf.bfOffBits);


    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    int biHeight_prev = abs(bi.biHeight);
    int biWidth_prev = bi.biWidth;


    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //Edit bi and bf
    bi.biWidth *= n;
    bi.biHeight *= n;

    //New padding
    int padding_new = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding_new) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

    //Test to see if the modification has any effect in attributes
    // printf("bf.bfType = %x, bf.bfOffBits = %d, bi.biSize = %d, bi.biBitCount = %d, bi.biCompression = %d\n",
    // bf.bfType, bf.bfOffBits, bi.biSize, bi.biBitCount, bi.biCompression);

    // printf("bi.biWidth = %d, bi.biHeight = %d, bi.biSizeImage = %d, bf.bfSize = %d, bf.bfOffBits = %d\n",
    // bi.biWidth, bi.biHeight, bi.biSizeImage, bf.bfSize, bf.bfOffBits);


    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);


    //Declaring the rgbt array to store rows
    //RGBTRIPLE *arr = malloc((biWidth_prev * n) * sizeof(RGBTRIPLE));

    // iterate over infile's scanlines
    for (int i = 0; i < biHeight_prev; i++)
    {
        //repeat rows vertically
        for (int j = 1; j <= n; j++)
        {
            count = 0;
            for (int k = 0; k < biWidth_prev; k++)
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

            ///Then add padding if applicable
            for (int m = 0; m < padding_new; m++)
            {
                fputc(0x00, outptr);
            }
            //Setting the cursor to the initial position
            fseek(inptr, -count * sizeof(RGBTRIPLE), SEEK_CUR);
        }
        //Resetting the cursor to the last read position
        fseek(inptr, count * sizeof(RGBTRIPLE), SEEK_CUR);

        ///skip padding if applicable
        fseek(inptr, padding, SEEK_CUR);
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
