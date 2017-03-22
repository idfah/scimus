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

#ifndef PNGLOADER_H
    #define PNGLOADER_H

    // make c++ friendly
    #ifdef __cplusplus
        extern "C" {
    #endif

    // OpenGL and GLUT headers
    #ifdef __APPLE__
        #include <GLUT/glut.h>
    #else
        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/glut.h>
    #endif

    // libpng header
    #include <png.h>


    struct _glpngtexture {
        GLsizei  width;
        GLsizei  height;
        GLenum   format;
        GLint    internalFormat;
        GLuint   id;
        GLubyte *texels;
    };
    typedef struct _glpngtexture glpngtexture;


    glpngtexture *genPNGTexture(char *filename);
    void GetPNGtextureInfo (int color_type, glpngtexture *currentTexture);


    #ifdef __cplusplus
        }
    #endif

#endif
