// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cs50.h>

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

    int i_lim = abs(bi.biHeight);
    int j_lim = bi.biWidth;

    // determine padding for scanlines
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;//

    //Edit bi and bf
    bi.biWidth *= n;
    bi.biHeight *= n;

    //New padding
    int padding_new = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding_new) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // iterate over infile's scanlines
    // for (int i = 0, i < i_lim; i++)
    // {
    //     //if (i == 2) break;
    //     // iterate over pixels in scanline
    //     for (int j = 0; j < j_lim; j++)
    //     {
    //         // temporary storage
    //         RGBTRIPLE triple;

    //         // read RGB triple from infile
    //         fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

    //         //Operation
    //         // write RGB triple to outfile
    //         for (int m = 1; m <= n; m++)
    //         {
    //             fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
    //             printf("\nfor i = %d, j = %d, m = %d, writing: %x %x %x, after wrt cursor = %ld\n", i, j, m,
    //             triple.rgbtBlue, triple.rgbtGreen, triple.rgbtRed, ftell(outptr));
    //         }
    //     }

    //     // skip over padding, if any
    //     fseek(inptr, padding, SEEK_CUR);

    //     // then add it back (to demonstrate how)
    //     for (int k = 0; k < padding_new; k++)
    //     {
    //         fputc(0x00, outptr);
    //     }

    // }

    ///Test iteration

    //RGBTRIPLE arr[100];
    for (int i = 0; i < i_lim; i++)
    {
        string arr[100];
        int count = 0;
        //iteration into the width
        for (int j = 0; j < j_lim; j++)
        {
            //RGBTRIPLE triple;
            //string triple = NULL;
            char triple[20];
            //Reading a pixel in triple
            //fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            fread(triple, sizeof(char), 3, inptr);
            //Storing the px in array n times
            // for (int k = 1; k <= n; k++)
            // {
            //     //arr[count] = triple;
            //     strcpy(arr[count], triple);
            //     count++;
            // }
            printf("%s\n", triple);
            printf("len = %lu\n", strlen(triple));
        }
        break;

    // skip over padding, if any
    //     fseek(inptr, padding, SEEK_CUR);

    //     //Insert padding if needed
    //     for (int m = 1; m <= padding_new; m++)
    //     {
    //         //arr[count++] = 0x00;
    //         // arr[count].rgbtBlue = 0;
    //         // arr[count].rgbtGreen = 0;
    //         // arr[count].rgbtRed = 0;
    //         arr[count] = "0x00";
    //         count++;
    //     }


    //     //Write a row n times
    //     //int elements = sizeof(arr)/sizeof(BYTE);
    //     for (int p = 1; p <= n; p++)
    //     {
    //         // for (int q = 0; q < elements; q++)
    //         // {
    //         //     // fwrite(arr[q], sizeof(RGBTRIPLE), 1, outptr);
    //         //     fputc(arr[q], outptr);
    //         // }
    //         fwrite(arr, sizeof(char), 3, outptr);
    //     }

    }

    ///Test iteration ends here

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
