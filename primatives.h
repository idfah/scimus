/*****************************************************************************\
* Copyright (c) 2007, Elliott Forney, http://www.elliottforney.com            *
* All rights reserved.                                                        *
*                                                                             *
* Redistribution and use in source and binary forms, with or without          *
* modification, are permitted provided that the following conditions are met: *
*                                                                             *
* 1. Redistributions of source code must retain the above copyright notice,   *
*    this list of conditions and the following disclaimer.                    *
*                                                                             *
* 2. Redistributions in binary form must reproduce the above copyright        *
*    notice, this list of conditions and the following disclaimer in the      *
*    documentation and/or other materials provided with the distribution.     *
*                                                                             *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  *
* POSSIBILITY OF SUCH DAMAGE.                                                 *
\*****************************************************************************/

/*
 *  Draw primatives shapes in OpenGL
 */

#ifndef PRIMATIVES_H
    #define PRIMATIVES_H

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

    // number of tessilations
    #define PRIMATIVE_RES 8

    // draw a frustum with base w1, top width w2, and height h
    void drawFrustum(GLdouble w1, GLdouble w2, GLdouble h);

    // draw a frustum with base w1, top width w2, and height h
    void drawTrap(GLdouble w1, GLdouble w2, GLdouble h);

    #ifdef __cplusplus
        }
    #endif

#endif
