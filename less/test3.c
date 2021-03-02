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
    printf("old pad = %d\n", padding);

    //Edit bi and bf
    bi.biWidth *= n;
    //bi.biHeight *= n;

    //New padding after changing  biwidht
    int padding_new = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding_new) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);


    //New padding
    printf("new pad = %d\n", padding_new);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);


    //Declaring the rgbt array to store rows
    RGBTRIPLE *arr = malloc((bi.biWidth + padding_new) * sizeof(RGBTRIPLE));

    // iterate over infile's scanlines
    for (int i = 0; i < biHeight_prev; i++)
    {
        //if (i == 2) break;
        // iterate over pixels in scanline
        int counter = 0;
        for (int j = 0; j < biWidth_prev; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            //Store each pixel in array for n times
            for (int k = 0; k < n; k++)
            {
                arr[counter] = triple;
                counter++;
            }
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        //Now print each row n times after addition of padding
        // for (int m = 1; m <= n; m++)
        // {
            fwrite(arr, sizeof(arr), 1, outptr);
            //Now add padding after it if required
            for (int p = 1; p <= padding_new; p++)
            {
                fputc(0x00, outptr);
            }
        //}
    }

    free(arr);

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
