/**********************************************************************\
* Copyright (c) 2005-2006 David HENRY                                  *
*                                                                      *
* Permission is hereby granted, free of charge, to any person          *
* obtaining a copy of this software and associated documentation       *
* files (the "Software"), to deal in the Software without              *
* restriction, including without limitation the rights to use,         *
* copy, modify, merge, publish, distribute, sublicense, and/or         *
* sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions: *
*                                                                      *
* The above copyright notice and this permission notice shall be       *
* included in all copies or substantial portions of the Software.      *
*                                                                      *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      *
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   *
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                *
* NONINFRINGEMENT.                                                     *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR     *
* ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF       *
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION   *
* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.      *
\**********************************************************************/

/*
 *  This library is based upon the png texture loader written
 *      by David Henry
 *  Modified by Elliott Forney 10.15.2007
 */

#include "pngLoader.h"

#include <stdio.h>
#include <stdlib.h>

glpngtexture *genPNGTexture(char *filename)
{
    png_byte magic[8];
    png_structp png_ptr;
    png_infop info_ptr;
    png_uint_32 width, height;
    int bit_depth, color_type;
    FILE *fp = NULL;
    png_bytep *row_pointers = NULL;
    glpngtexture *currentTexture;
    int i;

    currentTexture = malloc(sizeof(glpngtexture));

    // open image file
    fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "error: couldn't open \"%s\"!\n", filename);
        exit(1);
    }

    // read magic number
    fread(magic, 1, sizeof(magic), fp);

    // check for valid magic number
    if (!png_check_sig(magic, sizeof(magic))) {
        fprintf(stderr, "error: \"%s\" is not a valid PNG image!\n", filename);
        fclose(fp);
        exit(1);
    }

    // create a png read struct
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        exit(1);
    }

    // create a png info struct
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        exit(1);
    }

    // initialize the setjmp for returning
    // properly after a libpng error occured
    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        if (row_pointers)
            free(row_pointers);
        if (currentTexture->texels)
            free(currentTexture->texels);
        exit(1);
    }

    // setup libpng for using standard C fread() function
    // with our FILE pointer
    png_init_io(png_ptr, fp);

    // tell libpng that we have already read the magic number
    png_set_sig_bytes(png_ptr, sizeof(magic));

    // read png info
    png_read_info(png_ptr, info_ptr);

    // get some usefull information from header
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);

    // convert index color images to RGB images
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png_ptr);

    // convert 1-2-4 bits grayscale images to 8 bits
    // grayscale.
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png_ptr);

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png_ptr);

    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    else if (bit_depth < 8)
        png_set_packing(png_ptr);

    // update info structure to apply transformations
    png_read_update_info(png_ptr, info_ptr);

    // retrieve updated information
    png_get_IHDR(png_ptr, info_ptr,
                  (png_uint_32*)(&width),
                  (png_uint_32*)(&height),
                  &bit_depth, &color_type,
                  NULL, NULL, NULL);

    // get image format and components per pixel
    GetPNGtextureInfo(color_type, currentTexture);

    // we can now allocate memory for storing pixel data
    (currentTexture->texels) = (GLubyte*)malloc(sizeof(GLubyte) * (width) *
            height * (currentTexture->internalFormat));

    // setup a pointer array.  Each one points at the begening of a row.
    row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * (height));

    for (i = 0; i < height; ++i)
        row_pointers[i] = (png_bytep)((currentTexture->texels)  +
                ((height - (i + 1)) * width * (currentTexture->internalFormat)));

    // read pixel data using row pointers
    png_read_image(png_ptr, row_pointers);

    // finish decompression and release memory
    png_read_end(png_ptr, NULL);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    // we don't need row pointers anymore
    free(row_pointers);

    fclose(fp);

    // cast to OpenGL data types
    currentTexture->width = (GLsizei)width;
    currentTexture->height = (GLsizei)height;

    return currentTexture;
}

void GetPNGtextureInfo(int color_type, glpngtexture *currentTexture)
{
    switch (color_type) {
        case PNG_COLOR_TYPE_GRAY:
            (currentTexture->format) = GL_LUMINANCE;
            (currentTexture->internalFormat) = 1;
            // printf("Loaded a PNG_COLOR_TYPE_GRAY image\n");
            break;

        case PNG_COLOR_TYPE_GRAY_ALPHA:
            (currentTexture->format) = GL_LUMINANCE_ALPHA;
            (currentTexture->internalFormat) = 2;
            // printf("Loaded a PNG_COLOR_TYPE_GRAY_ALPHA image\n");
            break;

        case PNG_COLOR_TYPE_RGB:
            (currentTexture->format) = GL_RGB;
            (currentTexture->internalFormat) = 3;
            // printf("Loaded a PNG_COLOR_TYPE_RGB image\n");
            break;

        case PNG_COLOR_TYPE_RGB_ALPHA:
            (currentTexture->format) = GL_RGBA;
            (currentTexture->internalFormat) = 4;
            // printf("Loaded a PNG_COLOR_TYPE_RGB_ALPHA image\n");
            break;

        default:
            // Badness
            break;
    }
}
