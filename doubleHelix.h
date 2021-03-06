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
 *  Draws a double helix converted from vrml (using awk).
 *
 *  vrml borrowed from:
 *    Paul May, School of Chemistry, University of Bristol
 *    <http://www.chm.bris.ac.uk/motm/dna/dnav.htm>
 */

#ifndef DOUBLEHELIX_H
    #define DOUBLEHELIX_H

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

    // number of tesilations
    #define MOLI_RES 8
    #define BOND_RES 8

    // initialize draw routines
    void initDoubleHelix();

    // generate random material properties
    void genRandColor();

    // draw a sphere
    void drawMolicule(GLdouble tx, GLdouble ty, GLdouble tz, GLdouble rad);

    // draw a cylinder
    void drawBond(GLdouble tx, GLdouble ty, GLdouble tz,
                  GLdouble rr, GLdouble rx, GLdouble ry, GLdouble rz,
                  GLdouble rad, GLdouble h);

    // draw the double helix
    void drawDoubleHelix();

    #ifdef __cplusplus
        }
    #endif

#endif
