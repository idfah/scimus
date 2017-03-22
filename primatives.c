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

// OpenGL and GLUT headers
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glut.h>
#endif

// standard c includes
#include <stdio.h>
#include <math.h>

// prototypes and definitions
#include "primatives.h"

// draw a frustum with base w1, top width w2, and height h
void drawFrustum(GLdouble w1, GLdouble w2, GLdouble h)
{
    int i;

    GLdouble a      = (w1 - w2) / 2.0;
    GLdouble theta  = atan2(h,a);
    GLdouble phi    = (M_PI_2) - theta;
    GLdouble w4     = sqrt(a*a+h*h);

    // draw frustum
    glMatrixMode(GL_MODELVIEW);
    for (i = 0; i < 4; ++i) {
        glPushMatrix();
            glRotated(i*90.0, 0.0, 1.0, 0.0);
            glTranslated(0.0, 0.0, w1/2.0);
            glRotated(-phi*(180.0/M_PI), 1.0, 0.0, 0.0);
            drawTrap(w1, w2, w4);
        glPopMatrix();
    }

    // draw cap
    glBegin(GL_QUADS);
        glNormal3f(0.0, 1.0, 0.0);
        glVertex3d(-w2/2.0, h,  w2/2.0);
        glVertex3d( w2/2.0, h,  w2/2.0);
        glVertex3d( w2/2.0, h, -w2/2.0);
        glVertex3d(-w2/2.0, h, -w2/2.0);
    glEnd();
}

// draw a frustum with base w1, top width w2, and height h
void drawTrap(GLdouble w1, GLdouble w2, GLdouble h)
{
    GLdouble a = (w1 - w2) / 2.0;

    glBegin(GL_QUADS);
        glNormal3f(0.0, 0.0, 1.0);
        glVertex3d( -w1/2.0,    0.0, 0.0);
        glVertex3d(  w1/2.0,    0.0, 0.0);
        glVertex3d( (w1/2.0)-a, h,   0.0);
        glVertex3d((-w1/2.0)+a, h,   0.0);
    glEnd();
}
