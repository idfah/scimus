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
 *  Virtual Science Museum in OpenGL & Glut
 */

// standard c headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

// OpenGL and GLUT headers
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glut.h>
#endif

// png loader library
#include "pngLoader.h"

// 3d navigation
#include "navigator.h"

// double helix
#include "doubleHelix.h"

// custom primative shapes
#include "primatives.h"

// frame cap
// removed for c compat, uncomment in animate as well
// #include "saveFrame.h"

// prototypes and macros
#include "scimus.h"

// debug level
short debug = DEBUG;

// textures and counts
glpngtexture *pix[MAX_NUM_PIX];
int           numPix;

bool showTextures = true;

// full screen mode status
bool gameMode = false;
int gameWindowID;

// status of screen cap
bool capture = false;

// amount window is open
bool glassIsOpening = false;
GLdouble glassOpen  = 0;

GLUquadric *quadric;

// animation variables
bool animation = false; // are we currently animating
bool frozen    = false; // is animation frozen

// sculpture1
GLdouble earthTheta = 0.0;
GLdouble earthDist  = 400.0;
GLdouble moonTheta  = 0.0;
GLdouble moonDist   = 75.0;
GLdouble mercuryTheta = 0.0;
GLdouble mercuryDist  = 300.0;

// sculpture2
GLdouble diskRot[4] = {0.0, 90.0, 0.0, 120.0};

// sculpture3
bool showHelix = true;

// sculpture4
GLdouble pistHeight = 0.0;
GLdouble crankTheta = 0.0;
GLdouble const crankRadius  = 210.0;
GLdouble const rodLength    = 300.0;
bool showBurn = false;

// main control loop
int main(int nargs, char *args[])
{
    // hard-coded texture file names
    char *p[2] = {
        "images/skyline3.png",
        "images/ceiling_texture.png",
    };

    // used for glu predefined shapes
    quadric = gluNewQuadric();
    gluQuadricOrientation(quadric, GLU_OUTSIDE);
    gluQuadricNormals(quadric, GLU_SMOOTH);

    // initialize double helix
    initDoubleHelix();

    // load pictures/textures from file
    loadTextures(2, p);

    // initialize the display window
    navInit(nargs, args);

    // initialize our pictures/textures 
    initTextures();

    // register glut call-backs 
    initCallBacks();

    // initialize scene lighting 
    initLighting();

    // pass control to glut 
    glutMainLoop();

    // all went well 
    return 0;
}

// load textures from file 
void loadTextures(int n, char *picNames[])
{
    int i;  // general use counter 

    // set our global number of textures 
    numPix = n;

    // check our array limit 
    if (numPix > MAX_NUM_PIX) {
        fprintf(stderr, "Fatal Error:  Attempted to Initialize %d textures.  Limit is %d.\n", numPix, MAX_NUM_PIX);
        exit(MAX_TEX_ERROR);
    }

    // initialize the array 
    for (i = 0; i < MAX_NUM_PIX; ++i)
        pix[i] = NULL;

    // for each texture 
    for (i = 0; i < numPix; ++i) {
        // use pngLoader to generate from png file 
        pix[i] = genPNGTexture(picNames[i]);

        // file dimentions must be a power of 2 or we're done 
        if ((!isPower2((pix[i])->width)) && (!isPower2((pix[i])->height))) {
            fprintf(stderr, "Fatal Error:  Invalid image size:  %dX%d.  "
            "Must be power of 2.", (pix[i]->width), (pix[i]->height));
            exit(IMAGE_SIZE_ERROR);
        }
    }
}

// generate OpenGL textures from the loaded images 
void initTextures()
{
    int i;
    GLuint ids[MAX_NUM_PIX];  // array holding our texture id's 

    // initialize our id's all to 0 
    for (i = 0; i < MAX_NUM_PIX; ++i)
        ids[i] = 0;

    // generate numPix identifiers and store them in ids 
    glGenTextures(numPix, ids);

    for (i = 0; i < numPix; ++i) {
        // give each texture the id assigned by glGenTextures 
        pix[i]->id = ids[i];

        // set texture properties and pixels 
        glBindTexture(GL_TEXTURE_2D, pix[i]->id);
        if (i == 0) {
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.1);
        }
        else {
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, pix[i]->internalFormat,
                pix[i]->width, pix[i]->height, 0, pix[i]->format,
                GL_UNSIGNED_BYTE, pix[i]->texels);
    }
}

// initialize glut call-backs 
void initCallBacks()
{
    // initialize display function 
    navDrawFunc(draw);

    // initialize keyboard functions 
    navKeyboardFunc(keyDown);
    navKeyboardUpFunc(keyUp);

    navClipFunc(enforceWallClipping);
}

// initialize scene lighting 
void initLighting()
{
    // overall ambient lighting 
    GLfloat const ambient[4]  = {0.04, 0.04, 0.04, 1.0};

    // ambient lighting from each light source 
    GLfloat const colorA0[4]  = {0.20, 0.20, 0.01, 1.0};
    GLfloat const colorA1[4]  = {0.25, 0.08, 0.01, 1.0};
    GLfloat const colorA2[4]  = {0.90, 0.90, 0.90, 1.0};
    GLfloat const colorA3[4]  = {0.90, 0.90, 0.90, 1.0};
    GLfloat const colorA4[4]  = {0.20, 0.15, 0.15, 1.0};
    GLfloat const colorA5[4]  = {0.20, 0.20, 0.20, 1.0};
    GLfloat const colorA6[4]  = {0.20, 0.20, 0.20, 1.0};
    GLfloat const colorA7[4]  = {0.20, 0.20, 0.20, 1.0};

    // diffuse lighting from each light source 
    GLfloat const colorD0[4]  = {0.9, 0.9, 0.0, 1.0};
    GLfloat const colorD1[4]  = {0.9, 0.2, 0.0, 1.0};
    GLfloat const colorD2[4]  = {0.9, 0.9, 0.9, 1.0};
    GLfloat const colorD3[4]  = {0.9, 0.9, 0.9, 1.0};
    GLfloat const colorD4[4]  = {0.6, 0.6, 0.6, 1.0};
    GLfloat const colorD5[4]  = {0.6, 0.6, 0.6, 1.0};
    GLfloat const colorD6[4]  = {0.6, 0.6, 0.6, 1.0};
    GLfloat const colorD7[4]  = {0.6, 0.6, 0.6, 1.0};

    // specular lighting from each light source 
    GLfloat const colorS0[4]  = {0.9, 0.9, 0.0, 1.0};
    GLfloat const colorS1[4]  = {0.9, 0.2, 0.0, 1.0};
    GLfloat const colorS2[4]  = {0.0, 0.0, 0.0, 1.0};
    GLfloat const colorS3[4]  = {0.0, 0.0, 0.0, 1.0};
    GLfloat const colorS4[4]  = {0.6, 0.6, 0.6, 1.0};
    GLfloat const colorS5[4]  = {0.6, 0.6, 0.6, 1.0};
    GLfloat const colorS6[4]  = {0.6, 0.6, 0.6, 1.0};
    GLfloat const colorS7[4]  = {0.6, 0.6, 0.6, 1.0};

    // setup lighting scheme 
    glEnable(GL_LIGHTING);
    // glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // enable all 8 light sources
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);
    glEnable(GL_LIGHT4);
    glEnable(GL_LIGHT5);
    glEnable(GL_LIGHT6);
    glEnable(GL_LIGHT7);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    // initialize light colors and attenuation
    glLightf(GL_LIGHT0,  GL_CONSTANT_ATTENUATION,  0.500000);
    glLightf(GL_LIGHT0,  GL_LINEAR_ATTENUATION,    0.000000);
    glLightf(GL_LIGHT0,  GL_QUADRATIC_ATTENUATION, 0.000000);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  colorA0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  colorD0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, colorS0);

    glLightf(GL_LIGHT1,  GL_CONSTANT_ATTENUATION,  0.001000);
    glLightf(GL_LIGHT1,  GL_LINEAR_ATTENUATION,    0.000100);
    glLightf(GL_LIGHT1,  GL_QUADRATIC_ATTENUATION, 0.0000004);
    glLightfv(GL_LIGHT1, GL_AMBIENT,  colorA1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  colorD1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, colorS1);

    glLightf(GL_LIGHT2,  GL_CONSTANT_ATTENUATION,  2.200000);
    glLightf(GL_LIGHT2,  GL_LINEAR_ATTENUATION,    0.000100);
    glLightf(GL_LIGHT2,  GL_QUADRATIC_ATTENUATION, 0.000000);
    glLightfv(GL_LIGHT2, GL_AMBIENT,  colorA2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  colorD2);
    glLightfv(GL_LIGHT2, GL_SPECULAR, colorS2);

    glLightf(GL_LIGHT3,  GL_CONSTANT_ATTENUATION,  2.200000);
    glLightf(GL_LIGHT3,  GL_LINEAR_ATTENUATION,    0.000100);
    glLightf(GL_LIGHT3,  GL_QUADRATIC_ATTENUATION, 0.000000);
    glLightfv(GL_LIGHT3, GL_AMBIENT,  colorA3);
    glLightfv(GL_LIGHT3, GL_DIFFUSE,  colorD3);
    glLightfv(GL_LIGHT3, GL_SPECULAR, colorS3);

    glLightf(GL_LIGHT4,  GL_CONSTANT_ATTENUATION,  0.001000);
    glLightf(GL_LIGHT4,  GL_LINEAR_ATTENUATION,    0.000100);
    glLightf(GL_LIGHT4,  GL_QUADRATIC_ATTENUATION, 0.0000005);
    glLightfv(GL_LIGHT4, GL_AMBIENT,  colorA4);
    glLightfv(GL_LIGHT4, GL_DIFFUSE,  colorD4);
    glLightfv(GL_LIGHT4, GL_SPECULAR, colorS4);

    glLightf(GL_LIGHT5,  GL_CONSTANT_ATTENUATION,  0.001000);
    glLightf(GL_LIGHT5,  GL_LINEAR_ATTENUATION,    0.000100);
    glLightf(GL_LIGHT5,  GL_QUADRATIC_ATTENUATION, 0.0000005);
    glLightfv(GL_LIGHT5, GL_AMBIENT,  colorA5);
    glLightfv(GL_LIGHT5, GL_DIFFUSE,  colorD5);
    glLightfv(GL_LIGHT5, GL_SPECULAR, colorS5);

    glLightf(GL_LIGHT6,  GL_CONSTANT_ATTENUATION,  0.001000);
    glLightf(GL_LIGHT6,  GL_LINEAR_ATTENUATION,    0.000100);
    glLightf(GL_LIGHT6,  GL_QUADRATIC_ATTENUATION, 0.0000005);
    glLightfv(GL_LIGHT6, GL_AMBIENT,  colorA6);
    glLightfv(GL_LIGHT6, GL_DIFFUSE,  colorD6);
    glLightfv(GL_LIGHT6, GL_SPECULAR, colorS6);

    glLightf(GL_LIGHT7,  GL_CONSTANT_ATTENUATION,  0.001000);
    glLightf(GL_LIGHT7,  GL_LINEAR_ATTENUATION,    0.000100);
    glLightf(GL_LIGHT7,  GL_QUADRATIC_ATTENUATION, 0.0000005);
    glLightfv(GL_LIGHT7, GL_AMBIENT,  colorA7);
    glLightfv(GL_LIGHT7, GL_DIFFUSE,  colorD7);
    glLightfv(GL_LIGHT7, GL_SPECULAR, colorS7);
}

// test if x is a power of 2
// borrowed from Dr. Ross Beveridge
int isPower2(int x)
{
    return( (x > 0) && ((x & (x - 1)) == 0) );
}

// draw to the display
void draw()
{
    // place lighting in the scene
    placeLights();

    // draw the floor
    drawFloor();

    // draw the ceiling
    drawCeiling();

    // draw the walls
    drawWalls();

    // draw the outside world
    drawOutside();

    drawSculpture1();
    drawSculpture2();
    drawSculpture3();
    drawSculpture4();
    drawSculpture5();

    // draw the window
    drawGlass();

    if (!animation && !frozen)
        animate(1);
    /*
       glMatrixMode(GL_MODELVIEW);
       glPushMatrix();
           glRotated(-90.0, 1.0, 0.0, 0.0);
           gluCylinder(quadric, 512.0, 512.0, 1024.0, 80, 80);
       glPopMatrix();
       */
}

// perform timed scene animation
void animate(int i)
{
    /*
       if (capture)
       saveFrame(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
       */

    if (!frozen) {
        animation = true;
        updateSculpture1();
        updateSculpture2();
        updateSculpture4();
        openGlass();
        glutPostRedisplay();
        glutTimerFunc(ANI_RATE, animate, 1);
    }
    else
        animation = false;
}

// place lights in the scene
void placeLights()
{
    // outside light locations
    GLfloat const lightLoc0[4] = {0.0, 0.0, (ROOM_LENGTH/-2.0)+0.0, 1.0};
    GLfloat const lightLoc1[4] = {0.0, 1024.0, (ROOM_LENGTH/-2.0)-1024.0, 1.0};

    GLfloat const lightLoc2[4] = {OUTSIDE_WIDTH/-2.0+1024.0,  (2.0*FLOOR_LEVEL)+OUTSIDE_HEIGHT-1024.0, (ROOM_LENGTH/-2.0)-OUTSIDE_LENGTH+1024.0, 1.0};
    GLfloat const lightLoc3[4] = {OUTSIDE_WIDTH/ 2.0-1024.0,  (2.0*FLOOR_LEVEL)+OUTSIDE_HEIGHT-1024.0, (ROOM_LENGTH/-2.0)-OUTSIDE_LENGTH+1024.0, 1.0};

    /*
       glMatrixMode(GL_MODELVIEW);
       glPushMatrix();
           glTranslated(OUTSIDE_WIDTH/-2.0+1024.0,  (2.0*FLOOR_LEVEL)+OUTSIDE_HEIGHT-1024.0, (ROOM_LENGTH/-2.0)-OUTSIDE_LENGTH+1024.0);
           gluSphere(quadric, 256.0, 60, 40);
       glPopMatrix();

       glPushMatrix();
           glTranslated(OUTSIDE_WIDTH/ 2.0-1024.0,  (2.0*FLOOR_LEVEL)+OUTSIDE_HEIGHT-1024.0, (ROOM_LENGTH/-2.0)-OUTSIDE_LENGTH+1024.0);
           gluSphere(quadric, 256.0, 60, 40);
       glPopMatrix();
       */    

    // GLfloat const spotdir0[3] = {0.0, -cos(M_PI/4.0), cos(M_PI/4.0)};
    GLfloat const spotdir0[3] = {0.0, -1.0, 2.5};

    // inside light locations
    GLfloat const lightLoc4[4] = {(ROOM_WIDTH/ 2.0)-512, 512, (ROOM_LENGTH/2.0)-(    ROOM_LENGTH/3.0), 1.0};
    GLfloat const lightLoc5[4] = {(ROOM_WIDTH/-2.0)+512, 512, (ROOM_LENGTH/2.0)-(    ROOM_LENGTH/3.0), 1.0};
    GLfloat const lightLoc6[4] = {(ROOM_WIDTH/ 2.0)-512, 512, (ROOM_LENGTH/2.0)-(2.0*ROOM_LENGTH/3.0), 1.0};
    GLfloat const lightLoc7[4] = {(ROOM_WIDTH/-2.0)+512, 512, (ROOM_LENGTH/2.0)-(2.0*ROOM_LENGTH/3.0), 1.0};

    // light 0 is has spotlight properties, it shines through the window
    glLightfv(GL_LIGHT0, GL_POSITION, lightLoc0);
    glLightf(GL_LIGHT0,  GL_SPOT_CUTOFF,           65.0);
    glLightf(GL_LIGHT0,  GL_SPOT_EXPONENT,         35.0);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION,        spotdir0);

    // lights 1-3 are outside
    glLightfv(GL_LIGHT1, GL_POSITION, lightLoc1);
    glLightfv(GL_LIGHT2, GL_POSITION, lightLoc2);
    glLightfv(GL_LIGHT3, GL_POSITION, lightLoc3);

    // light 4-7 are inside
    glLightfv(GL_LIGHT4, GL_POSITION, lightLoc4);
    glLightfv(GL_LIGHT5, GL_POSITION, lightLoc5);
    glLightfv(GL_LIGHT6, GL_POSITION, lightLoc6);
    glLightfv(GL_LIGHT7, GL_POSITION, lightLoc7);
}

// draw a tiled floor in the scene
void drawFloor()
{
    int i, j, k, l;

    char label[10] = "";

    GLfloat const colorA1[4] = {0.4, 0.4, 0.4, 1.0};
    GLfloat const colorD1[4] = {0.7, 0.7, 0.7, 1.0};
    GLfloat const colorS1[4] = {0.9, 0.9, 0.9, 1.0};

    GLfloat const colorA2[4] = {0.1, 0.4, 0.4, 1.0};
    GLfloat const colorD2[4] = {0.1, 0.7, 0.7, 1.0};
    GLfloat const colorS2[4] = {0.1, 0.9, 0.9, 1.0};

    // save our current modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    // turn the world upside down
    glRotated(180.0, 1.0, 0.0, 0.0);
    // translate to far corner of the room at floor level
    glTranslated(ROOM_WIDTH/-2.0, -1.0*FLOOR_LEVEL, ROOM_LENGTH/-2.0);

    // draw the tiles
    for (i = 0; i < ROOM_WIDTH/512; ++i) {
        for (j = 0; j < ROOM_LENGTH/512; ++j) {
            if (debug > 0) {
                sprintf(label, "(%d,%d)", i, j);
                drawText(i*512, 0, j*512, label);
            }

            if ((i+j)%2 == 0) {
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA1);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD1);
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS1);
                glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);
            }
            else {
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA2);
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD2);
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS2);
                glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);
            }

            // draw tiles on x-z plane
            for (k = i*512/TILE_RES; k < (i+1)*512/TILE_RES; ++k)
                for (l = j*512/TILE_RES; l < (j+1)*512/TILE_RES; ++l) {
                    glBegin(GL_QUADS);
                        glNormal3f(0.0, -1.0, 0.0);
                        glVertex3i( k   *TILE_RES, 0,  l   *TILE_RES);
                        glNormal3f(0.0, -1.0, 0.0);
                        glVertex3i((k+1)*TILE_RES, 0,  l   *TILE_RES);
                        glNormal3f(0.0, -1.0, 0.0);
                        glVertex3i((k+1)*TILE_RES, 0, (l+1)*TILE_RES);
                        glNormal3f(0.0, -1.0, 0.0);
                        glVertex3i( k   *TILE_RES, 0, (l+1)*TILE_RES);
                    glEnd();
                }
        }
    }
    glPopMatrix();
}

// draw a textured ceiling in the scene
void drawCeiling()
{
    int i, j;

    GLfloat const colorA[4] = {0.6, 0.6, 0.6, 1.0};
    GLfloat const colorD[4] = {0.9, 0.9, 0.9, 1.0};
    GLfloat const colorS[4] = {0.0, 0.0, 0.0, 1.0};

    // save our current modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // assign material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    // translate to far corner of the room at ceiling level
    glTranslated(ROOM_WIDTH/-2.0, ROOM_HEIGHT+FLOOR_LEVEL, ROOM_LENGTH/-2.0);

    // draw the ceiling
    for (i = 0; i < ROOM_WIDTH/512; ++i) {
        for (j = 0; j < ROOM_LENGTH/512; ++j) {
            if (showTextures)
                glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, pix[numPix-1]->id);

            glBegin(GL_QUADS);
                glTexCoord2i(0, 0);
                glNormal3f(0.0, -1.0, 0.0);
                glVertex3i( i   *512, 0,  j   *512);
                glTexCoord2i(1, 0);
                glNormal3f(0.0, -1.0, 0.0);
                glVertex3i((i+1)*512, 0,  j   *512);
                glTexCoord2i(1, 1);
                glNormal3f(0.0, -1.0, 0.0);
                glVertex3i((i+1)*512, 0, (j+1)*512);
                glTexCoord2i(0, 1);
                glNormal3f(0.0, -1.0, 0.0);
                glVertex3i( i   *512, 0, (j+1)*512);
            glEnd();

            if (showTextures)
                glDisable(GL_TEXTURE_2D);
        }
    }

    glPopMatrix();
}

// draw walls in the scene
void drawWalls()
{
    int i, j;

    // material properties
    GLfloat const colorA[4] = {0.3, 0.3, 0.3, 1.0};
    GLfloat const colorD[4] = {0.2, 0.2, 0.2, 1.0};
    GLfloat const colorS[4] = {0.5, 0.5, 0.5, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    // draw right wall
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    // rotate to face wall
    glRotated(-90.0, 0.0, 1.0, 0.0);
    // move to lower left corner
    glTranslated(ROOM_LENGTH/-2.0, FLOOR_LEVEL, ROOM_WIDTH/-2.0);
    // draw wall panels
    for (i = 0; i < ROOM_LENGTH/TILE_RES; ++i) {
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= ROOM_HEIGHT/TILE_RES; ++j) {
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i( i   *TILE_RES, j*TILE_RES, 0);
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i((i+1)*TILE_RES, j*TILE_RES, 0);
        }
        glEnd();
    }
    glPopMatrix();

    // draw left wall
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // rotate to face wall
    glRotated(90.0, 0.0, 1.0, 0.0);

    // move to lower left corner
    glTranslated(ROOM_LENGTH/-2.0, FLOOR_LEVEL, ROOM_WIDTH/-2.0);

    // draw wall panels
    for (i = 0; i < ROOM_LENGTH/TILE_RES; ++i) {
        glBegin(GL_QUAD_STRIP);
        for(j = 0; j <= ROOM_HEIGHT/TILE_RES; ++j) {
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i( i   *TILE_RES, j*TILE_RES, 0);
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i((i+1)*TILE_RES, j*TILE_RES, 0);
        }
        glEnd();
    }
    glPopMatrix();

    // draw near wall
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // rotate to face wall
    glRotated(180.0, 0.0, 1.0, 0.0);

    // move to lower left corner
    glTranslated(ROOM_WIDTH/-2.0, FLOOR_LEVEL, ROOM_LENGTH/-2.0);

    // draw wall panels
    for (i = 0; i < ROOM_WIDTH/TILE_RES; ++i) {
        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= ROOM_HEIGHT/TILE_RES; ++j) {
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i( i   *TILE_RES, j*TILE_RES, 0);
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i((i+1)*TILE_RES, j*TILE_RES, 0);
        }
        glEnd();
    }
    glPopMatrix();

    // draw far wall
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // rotate to face wall
    // glRotated(0.0, 0.0, 1.0, 0.0);

    // move to lower left corner
    glTranslated(ROOM_WIDTH/-2.0, FLOOR_LEVEL, ROOM_LENGTH/-2.0);

    // draw wall panels left of window
    for (i = 0; i < ((ROOM_WIDTH/TILE_RES/2)-(GLASS_WIDTH/TILE_RES/2)); ++i) {
        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= ROOM_HEIGHT/TILE_RES; ++j) {
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i( i   *TILE_RES, j*TILE_RES, 0);
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i((i+1)*TILE_RES, j*TILE_RES, 0);
        }
        glEnd();
    }

    // draw wall panels above window
    for (i = ((ROOM_WIDTH/TILE_RES/2)-(GLASS_WIDTH/TILE_RES/2));
            i < ((ROOM_WIDTH/TILE_RES/2)+(GLASS_WIDTH/TILE_RES/2)); ++i) {
        glBegin(GL_QUAD_STRIP);
        for (j = (GLASS_ELEV+GLASS_HEIGHT)/TILE_RES; j <= ROOM_HEIGHT/TILE_RES; ++j) {
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i( i   *TILE_RES, j*TILE_RES, 0);
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i((i+1)*TILE_RES, j*TILE_RES, 0);
        }
        glEnd();
    }

    // draw wall panels right of window
    for (i = ((ROOM_WIDTH/TILE_RES/2)+(GLASS_WIDTH/TILE_RES/2));
            i < ROOM_WIDTH/TILE_RES; ++i) {
        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= ROOM_HEIGHT/TILE_RES; ++j) {
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i( i   *TILE_RES, j*TILE_RES, 0);
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i((i+1)*TILE_RES, j*TILE_RES, 0);
        }
        glEnd();
    }

    // draw wall panels below window
    for (i = ((ROOM_WIDTH/TILE_RES/2)-(GLASS_WIDTH/TILE_RES/2));
            i < ((ROOM_WIDTH/TILE_RES/2)+(GLASS_WIDTH/TILE_RES/2)); ++i) {
        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= GLASS_ELEV/TILE_RES; ++j) {
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i( i   *TILE_RES, j*TILE_RES, 0);
            glNormal3f(0.0, 0.0, 1.0);
            glVertex3i((i+1)*TILE_RES, j*TILE_RES, 0);
        }
        glEnd();
    }

    glPopMatrix();
}

// draw a glass window in the scene
void drawGlass()
{
    // save our current modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // translate to lower left corner of window
    glTranslated(-GLASS_WIDTH/2.0, FLOOR_LEVEL+GLASS_ELEV, ROOM_LENGTH/-2.0);

    // draw the frame
    GLfloat const colorA[4] = {0.2, 0.2, 0.2, 1.0};
    GLfloat const colorD[4] = {0.5, 0.5, 0.5, 1.0};
    GLfloat const colorS[4] = {0.8, 0.8, 0.8, 1.0};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    glBegin(GL_QUAD_STRIP);
        glNormal3f(1.0, 1.0, 0.0);
        glVertex3i(0, 0, -50);
        glVertex3i(0, 0, 0);

        glNormal3f(-1.0, 1.0, 0.0);
        glVertex3i(GLASS_WIDTH, 0, -50);
        glVertex3i(GLASS_WIDTH, 0, 0);

        glNormal3f(-1.0, -1.0, 0.0);
        glVertex3i(GLASS_WIDTH, GLASS_HEIGHT, -50);
        glVertex3i(GLASS_WIDTH, GLASS_HEIGHT, 0);

        glNormal3f(1.0, -1.0, 0.0);
        glVertex3i(0, GLASS_HEIGHT, -50);
        glVertex3i(0, GLASS_HEIGHT, 0);

        glNormal3f(1.0, 1.0, 0.0);
        glVertex3i(0, 0, -50);
        glVertex3i(0, 0, 0);
    glEnd();

    // draw the window
    glDisable(GL_CULL_FACE);

    GLfloat const wcolorA[4] = {0.1, 0.1, 0.7, 0.25};
    GLfloat const wcolorD[4] = {0.1, 0.1, 0.7, 0.25};
    GLfloat const wcolorS[4] = {0.1, 0.1, 0.7, 0.25};

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   wcolorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   wcolorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  wcolorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    glBegin(GL_QUADS);
        glNormal3f(0.0, 0.0, 1.0);
        glVertex3d(0.0,                   0.0,          -50.0);
        glVertex3d(GLASS_WIDTH+glassOpen, 0.0,          -50.0);
        glVertex3d(GLASS_WIDTH+glassOpen, GLASS_HEIGHT, -50.0);
        glVertex3d(0.0,                   GLASS_HEIGHT, -50.0);
    glEnd();
    glEnable(GL_CULL_FACE);

    glPopMatrix();
}

// update window animation
void openGlass()
{
    if (glassIsOpening) {
        if (glassOpen > -GLASS_WIDTH)
            glassOpen -= 50.0*ANI_RATE/200.0;
        else
            glassOpen = -GLASS_WIDTH;
    }
    else {
        if (glassOpen < 0)
            glassOpen += 50.0*ANI_RATE/200.0;
        else
            glassOpen = 0;
    }
}

void drawSculpture1()
{
    int i;

    GLfloat const coneColorA[4] = {0.33, 0.33, 0.33, 1.0};
    GLfloat const coneColorD[4] = {0.78, 0.78, 0.78, 1.0};
    GLfloat const coneColorS[4] = {0.90, 0.90, 0.90, 1.0};

    /*GLfloat const coneColorA[4] = {0.4, 0.4, 0.4, 0.30};
    GLfloat const coneColorD[4] = {0.4, 0.4, 0.4, 0.30};
    GLfloat const coneColorS[4] = {1.0, 1.0, 1.0, 0.30};*/

    GLfloat const sunColorA[4] = {0.6, 0.4, 0.1, 1.0};
    GLfloat const sunColorD[4] = {0.8, 0.6, 0.1, 1.0};
    GLfloat const sunColorS[4] = {1.0, 0.8, 0.1, 1.0};

    GLfloat const earthColorA[4] = {0.1, 0.1, 0.4, 1.0};
    GLfloat const earthColorD[4] = {0.1, 0.1, 0.6, 1.0};
    GLfloat const earthColorS[4] = {0.1, 0.1, 0.8, 1.0};

    GLfloat const moonColorA[4] = {0.3, 0.3, 0.3, 1.0};
    GLfloat const moonColorD[4] = {0.8, 0.8, 0.8, 1.0};
    GLfloat const moonColorS[4] = {0.0, 0.0, 0.0, 1.0};

    GLfloat const mercuryColorA[4] = {0.3, 0.3, 0.3, 1.0};
    GLfloat const mercuryColorD[4] = {0.8, 0.8, 0.8, 1.0};
    GLfloat const mercuryColorS[4] = {0.0, 0.0, 0.0, 1.0};

    // save modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    // the system is heliocentric
    glTranslated((ROOM_WIDTH/2.0)-768.0, 0.0, (ROOM_LENGTH/2.0)-(2.0*ROOM_LENGTH/5.0));

    // assign material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   coneColorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   coneColorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  coneColorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 27.8f);

    // draw the stand
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    for (i = 0; i <= TILE_RES; ++i) {
        glNormal3f(sin(i*2.0*M_PI/TILE_RES), 0.0, cos(i*2.0*M_PI/TILE_RES));
        glVertex3d(100.0*sin(i*2.0*M_PI/TILE_RES), FLOOR_LEVEL, 100.0*cos(i*2.0*M_PI/TILE_RES));
    }
    glEnd();

    // assign material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   sunColorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   sunColorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  sunColorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    // draw the sun
    gluSphere(quadric, 128.0, 60, 40);

    // tilted for viewing pleasure
    glRotated(5.0, 0.0, 0.0, 1.0);

    glPushMatrix();

    // move to earth's center
    glTranslated(earthDist*sin(earthTheta), 0.0, earthDist*-cos(earthTheta));

    // assign material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   earthColorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   earthColorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  earthColorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    // draw the earth
    gluSphere(quadric, 32.0, 35, 25);

    // move to moon's center
    glTranslated(moonDist*sin(moonTheta), 0.0, moonDist*-cos(moonTheta));

    // assign material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   moonColorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   moonColorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  moonColorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 1.0f);

    // draw the moon
    gluSphere(quadric, 10.0, 20, 15);

    glPopMatrix();

    // draw mecury
    glPushMatrix();

    // move to mercury's center
    glTranslated(mercuryDist*sin(mercuryTheta), 0.0, mercuryDist*-cos(mercuryTheta));

    // assign material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mercuryColorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mercuryColorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mercuryColorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 1.0f);

    // draw mercury
    gluSphere(quadric, 20.0, 20, 15);

    glPopMatrix();

    // restore modelview
    glPopMatrix();
}

// update sculpture1 animation
void updateSculpture1()
{
    GLdouble const earth_semi_latus_rectum  = 350.0;
    GLdouble const earth_eccentricity       = 0.75;

    GLdouble const mercury_semi_latus_rectum = 250.0;
    GLdouble const mercury_eccentricity      = 0.58;

    earthTheta += (75000.0/(earthDist*earthDist) - (M_PI/220.0)) * ANI_RATE/200.0;
    earthTheta  = fmod(earthTheta, 2.0*M_PI);
    earthDist   = earth_semi_latus_rectum/(1+earth_eccentricity*cos(earthTheta));

    moonTheta += (M_PI/6.0) * ANI_RATE/200.0;
    moonTheta  = fmod(moonTheta, 2.0*M_PI);

    mercuryTheta += (60000.0/(mercuryDist*mercuryDist) - (M_PI/220.0)) * ANI_RATE/200.0;
    mercuryTheta  = fmod(mercuryTheta, 2.0*M_PI);
    mercuryDist   = mercury_semi_latus_rectum/(1+mercury_eccentricity*cos(mercuryTheta));
}

void drawSculpture2()
{
    GLfloat const colorA1[4] = {0.1, 0.4, 0.1, 1.0};
    GLfloat const colorD1[4] = {0.1, 0.6, 0.1, 1.0};
    GLfloat const colorS1[4] = {0.1, 0.8, 0.1, 1.0};

    GLfloat const colorA2[4] = {0.4, 0.1, 0.1, 1.0};
    GLfloat const colorD2[4] = {0.6, 0.1, 0.1, 1.0};
    GLfloat const colorS2[4] = {0.8, 0.1, 0.1, 1.0};

    GLfloat const colorA3[4] = {0.1, 0.1, 0.4, 1.0};
    GLfloat const colorD3[4] = {0.1, 0.1, 0.6, 1.0};
    GLfloat const colorS3[4] = {0.1, 0.1, 0.8, 1.0};

    GLfloat const colorA4[4] = {0.1, 0.4, 0.4, 1.0};
    GLfloat const colorD4[4] = {0.1, 0.6, 0.6, 1.0};
    GLfloat const colorS4[4] = {0.1, 0.8, 0.8, 1.0};

    GLfloat const colorA5[4] = {0.4, 0.4, 0.4, 1.0};
    GLfloat const colorD5[4] = {0.6, 0.6, 0.6, 1.0};
    GLfloat const colorS5[4] = {0.8, 0.8, 0.8, 1.0};

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
        glTranslated((ROOM_WIDTH/-2.0)+512, 0.0, (ROOM_LENGTH/2.0)-(2.0*ROOM_LENGTH/8.0));
        glRotated(90.0, 0.0, 1.0, 0.0);

        glPushMatrix();
            glRotated(diskRot[0], 1.0, 0.0, 0.0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA1);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD1);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS1);
            glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

            // gluDisk(quadric, 200.0, 220.0, 40, 60);
            glDisable(GL_CULL_FACE);
            glutSolidTorus(10.0, 210.0, 20, 50);
            glEnable(GL_CULL_FACE);

            glRotated(diskRot[1], 0.0, 1.0, 0.0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA2);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD2);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS2);
            glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

            // gluDisk(quadric, 180.0, 200.0, 40, 60);
            glDisable(GL_CULL_FACE);
            glutSolidTorus(10.0, 190.0, 20, 50);
            glEnable(GL_CULL_FACE);

            glRotated(diskRot[2], 1.0, 0.0, 0.0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA3);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD3);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS3);
            glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

            // gluDisk(quadric, 160.0, 180.0, 40, 60);
            glDisable(GL_CULL_FACE);
            glutSolidTorus(10.0, 170.0, 20, 50);
            glEnable(GL_CULL_FACE);

            glRotated(diskRot[3], 0.0, 1.0, 0.0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA4);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD4);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS4);
            glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

            // gluDisk(quadric, 140.0, 160.0, 40, 60);
            glDisable(GL_CULL_FACE);
            glutSolidTorus(10.0, 150.0, 20, 50);
            glEnable(GL_CULL_FACE);
        glPopMatrix();

        glPushMatrix();
            glTranslated(-230.0, 0.0, 0.0);
            glRotated(90.0, 1.0, 0.0, 0.0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA5);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD5);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS5);
            glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

            gluCylinder(quadric, 10.0, 10.0, -1.0*FLOOR_LEVEL, 20, 80);
            gluSphere(quadric, 10.0, 10, 15);
        glPopMatrix();

        glPushMatrix();
            glTranslated(230.0, 0.0, 0.0);
            glRotated(90.0, 1.0, 0.0, 0.0);

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA5);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD5);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS5);
            glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

            gluCylinder(quadric, 10.0, 10.0, -1.0*FLOOR_LEVEL, 20, 80);
            gluSphere(quadric, 10.0, 10, 15);
        glPopMatrix();

    glPopMatrix();
}

// update sculpture2 animation
void updateSculpture2()
{
    int i;

    diskRot[0] += 05.0 * (ANI_RATE/200.0);
    diskRot[1] += 15.0 * (ANI_RATE/200.0);
    diskRot[2] += 25.0 * (ANI_RATE/200.0);
    diskRot[3] += 35.0 * (ANI_RATE/200.0);

    for (i = 0; i < 4; ++i)
        diskRot[i] = fmod(diskRot[i], 360.0);
}

void drawSculpture3()
{
    GLfloat const colorA[4] = {0.33, 0.22, 0.03, 1.0};
    GLfloat const colorD[4] = {0.78, 0.57, 0.11, 1.0};
    GLfloat const colorS[4] = {0.99, 0.91, 0.81, 1.0};

    // assign material properties
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    // place the sculpture
    glTranslated((ROOM_WIDTH/-2.0)+512, 0.0, (ROOM_LENGTH/2.0)-(4.0*ROOM_LENGTH/8.0));

    // draw the stand
    glPushMatrix();
    glTranslated(0.0, FLOOR_LEVEL, 0.0);
    drawFrustum(512.0, 128.0, 512.0);
    glPopMatrix();

    glRotated(90.0, 0.0, 1.0, 0.0);
    // glFrontFace(GL_CW);
    glDisable(GL_CULL_FACE);
    glutSolidTeapot(128.0);
    glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CCW);

    glPopMatrix();
}

void drawSculpture4()
{
    GLfloat const metalColorA[4] = {0.4, 0.4, 0.4, 1.0};
    GLfloat const metalColorD[4] = {0.6, 0.6, 0.6, 1.0};
    GLfloat const metalColorS[4] = {0.6, 0.6, 0.6, 1.0};

    GLfloat const blockColorA[4] = {0.4, 0.4, 0.4, 0.30};
    GLfloat const blockColorD[4] = {0.4, 0.4, 0.4, 0.30};
    GLfloat const blockColorS[4] = {1.0, 1.0, 1.0, 0.30};

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    // place the sculpture
    glTranslated((ROOM_WIDTH/2.0)-512, 200.0, (ROOM_LENGTH/2.0)-(3.0*ROOM_LENGTH/5.0));

    // show the explosion
    if (0) {
        glPushMatrix();

        GLfloat const rColorA[4] = {0.4, 0.0, 0.0, 1.0};
        GLfloat const rColorD[4] = {0.8, 0.0, 0.0, 1.0};
        GLfloat const rColorS[4] = {0.9, 0.0, 0.0, 1.0};

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   rColorA);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   rColorD);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  rColorS);
        glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

        glTranslated(0.0, -rodLength-420.0, 0.0);

        if (fabs(crankTheta) < (40.0*M_PI/180.0))
            glScaled(1.0, 200.0/(pistHeight)+0.1, 1.0);
        else 
            glScaled(1.0, 0.9-200.0/(pistHeight)+0.1, 1.0);

        printf("%f\n", (200.0/(pistHeight)));

        gluSphere(quadric, 256.0, 20, 30);

        glPopMatrix();
    }

    // make it a metallic grey
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   metalColorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   metalColorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  metalColorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    // draw piston assembly
    glPushMatrix();

    // attach to wall
    glPushMatrix();
    glTranslated(150.0, 0.0, 0.0);
    glRotated(90.0, 0.0, 1.0, 0.0);

    gluSphere(quadric, 50.0, 20, 30);
    gluCylinder(quadric, 50.0, 50.0, 362.0, 20, 30);

    glPopMatrix();

    // draw crank
    glPushMatrix();
    glTranslated(150.0, 0.0, 0.0);
    glRotated(-crankTheta*180.0/M_PI+90.0, 1.0, 0.0, 0.0);

    gluCylinder(quadric, 50.0, 50.0, crankRadius, 20, 30);
    glTranslated(0.0, 0.0, crankRadius);
    gluSphere(quadric, 50.0, 20, 30);

    glPopMatrix();

    // give motion
    glTranslated(0.0, -pistHeight, 0.0);
    glRotated(90.0, 1.0, 0.0, 0.0);

    // draw piston
    gluCylinder(quadric, 256.0, 256.0, 128.0, 20, 30);

    // draw piston top
    glRotated(180.0, 1.0, 0.0, 0.0);
    gluDisk(quadric, 0.0, 256.0, 20, 30);

    // draw the push rod
    glPushMatrix();
    glRotated((asin(crankRadius*sin(crankTheta)/rodLength))*180.0/M_PI, 1.0, 0.0, 0.0);
    gluSphere(quadric, 50.0, 20, 30);
    gluCylinder(quadric, 50.0, 50.0, rodLength, 20, 30);

    // attach to crank
    glTranslated(0.0, 0.0, rodLength);
    glRotated(90.0, 0.0, 1.0, 0.0);
    gluSphere(quadric, 50.0, 20, 30);
    gluCylinder(quadric, 50.0, 50.0, 150.0, 20, 30);

    glPopMatrix();

    // draw piston bottom
    glTranslated(0.0, 0.0, -128.0);
    glRotated(-180.0, 1.0, 0.0, 0.0);
    gluDisk(quadric, 0.0, 256.0, 20, 30);

    glPopMatrix();

    // draw the block
    glPushMatrix();

    glTranslated(0.0, FLOOR_LEVEL-200, 0.0);
    glRotated(-90.0, 1.0, 0.0, 0.0);

    // make it clear
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   blockColorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   blockColorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  blockColorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    glDisable(GL_CULL_FACE);
    gluCylinder(quadric, 260.0, 260.0, 670.0, 60, 80);
    glEnable(GL_CULL_FACE);

    glPopMatrix();

    glPopMatrix();
}

void updateSculpture4()
{
    crankTheta += (35.0*(M_PI/180.0)) * (ANI_RATE/200.0);
    crankTheta = fmod(crankTheta, 2.0*M_PI);

    //if ((crankTheta < (80.0*(M_PI/180.0))) && (crankTheta > (-80.0*(M_PI/180.0))))
    if (fabs(crankTheta) < (90.0*(M_PI/180.0)))
        showBurn = true;
    else
        showBurn = false;

    pistHeight = crankRadius*cos(crankTheta) + sqrt((rodLength*rodLength) - (crankRadius*sin(crankTheta))*(crankRadius*sin(crankTheta)));
}

// draw sculpture5
void drawSculpture5()
{
    if (showHelix) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        glTranslated((ROOM_WIDTH/-2.0)+512, 0.0, (ROOM_LENGTH/2.0)-(6.0*ROOM_LENGTH/8.0));
        glRotated(-95.0, 1.0, 0.0, 0.0);
        glScaled(35.0, 35.0, 35.0);

        // draw double helix
        drawDoubleHelix();

        glPopMatrix();
    }
}

// draw everything outside the room
void drawOutside()
{
    // save our current modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    // move origin outside
    glTranslated(OUTSIDE_WIDTH/-2.0, 2.0*FLOOR_LEVEL, ROOM_LENGTH/-2.0);

    // draw the grass
    GLfloat const colorA[4] = {0.0, 1.0, 0.0, 1.0};
    GLfloat const colorD[4] = {0.0, 1.0, 0.0, 1.0};
    GLfloat const colorS[4] = {0.0, 0.0, 0.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   colorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   colorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  colorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 100.0f);

    glBegin(GL_POLYGON);
        glVertex3i(0,             0,  0         );
        glVertex3i(OUTSIDE_WIDTH, 0,  0         );
        glVertex3i(OUTSIDE_WIDTH, 0, -OUTSIDE_LENGTH);
        glVertex3i(0,             0, -OUTSIDE_LENGTH);
    glEnd();

    // draw the skyline
    if (showTextures)
        glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, pix[numPix-2]->id);

    GLfloat const scolorA[4] = {1.0, 1.0, 1.0, 1.0};
    GLfloat const scolorD[4] = {1.0, 1.0, 1.0, 1.0};
    GLfloat const scolorS[4] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   scolorA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   scolorD);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  scolorS);
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);

    glBegin(GL_QUADS);
        glTexCoord2i(0, 0);
        glVertex3i(0,             0,              -OUTSIDE_LENGTH);
        glTexCoord2i(1, 0);
        glVertex3i(OUTSIDE_WIDTH, 0,              -OUTSIDE_LENGTH);
        glTexCoord2i(1, 1);
        glVertex3i(OUTSIDE_WIDTH, OUTSIDE_HEIGHT, -OUTSIDE_LENGTH);
        glTexCoord2i(0, 1);
        glVertex3i(0,             OUTSIDE_HEIGHT, -OUTSIDE_LENGTH);
    glEnd();

    if (showTextures)
        glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}


// draw text at x, y, z
// doesn't work well in 3d
// need to rotate text to face camera
void drawText(int x, int y, int z, char *text)
{
    int  i;
    char c;

    glColor4d(0.0, 0.0, 0.0, 1.0);
    c = text[0];
    for (i = 1; c != '\0'; ++i) {
        glRasterPos3i(x, y, z);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
        x += glutBitmapWidth(GLUT_BITMAP_TIMES_ROMAN_24, c);
        c = text[i];
    }
}

// respond to key press
void keyDown(unsigned char key, int x, int y)
{
    int i;

    if (isdigit(key)) {
        char keyStr[2];
        int  keyDigit = -1;

        GLenum lights[8] =
        {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
            GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};

        keyStr[0] = key;
        keyStr[1] = '\0';
        keyDigit  = atoi(keyStr);
        for (i = 1; i <= 8; ++i) {
            if (keyDigit == i) {
                if (glIsEnabled(lights[i-1]))
                    glDisable(lights[i-1]);
                else
                    glEnable(lights[i-1]);

                glutPostRedisplay();
            }
        }
    }

    if (key == 'a') {
        frozen = !frozen;
        glutPostRedisplay();
    }

    if (key == 'f') {
        frozen = true;
        if (gameMode == false) {
            // check if environment supports full screen mode
            if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) {
                gameWindowID = glutGetWindow();
                // tell glut to go full screen
                glutEnterGameMode();
                // have to re-initialize glut context
                // since this is treated as a new window
                navInitDisplay();
                navInitCallBacks();
                initCallBacks();
                initLighting();
                initTextures();

                gameMode = true;
            }
            else
                fprintf(stderr, "Full screen mode is not available.\n");

        }
        else {
            glutLeaveGameMode();
            glutSetWindow(gameWindowID);
            navInitDisplay();
            navInitCallBacks();
            initCallBacks();
            initLighting();
            initTextures();

            gameMode = false;
        }
        frozen = false;
    }

    if (key == 'h') {
        showHelix = !showHelix;
        glutPostRedisplay();
    }

    if (key == 'k')
        capture = !capture;

    if (key == 'q')
        cleanUpAndQuit();

    if (key == 't') {
        showTextures = !showTextures;    
        glutPostRedisplay();
    }

    if (key == 'w')
        glassIsOpening = !glassIsOpening;
}

// respond to key release
void keyUp(unsigned char key, int x, int y)
{
    // do nothing
}

// don't allow the camera to go outside the walls
void enforceWallClipping(GLdouble *x, GLdouble *y, GLdouble *z)
{
    if (*x > ((ROOM_WIDTH/2.0)-512.0-WALL_CLIP_H))
        *x = ((ROOM_WIDTH/2.0)-512.0-WALL_CLIP_H);
    if (*x < ((ROOM_WIDTH/-2.0)+512.0+WALL_CLIP_H))
        *x = ((ROOM_WIDTH/-2.0)+512.0+WALL_CLIP_H);
    if (*z > ((ROOM_LENGTH/2.0)-512.0-WALL_CLIP_H))
        *z = ((ROOM_LENGTH/2.0)-512.0-WALL_CLIP_H);
    if (*z < ((ROOM_LENGTH/-2.0)+512.0+WALL_CLIP_H))
        *z = ((ROOM_LENGTH/-2.0)+512.0+WALL_CLIP_H);

    if (*y > (ROOM_HEIGHT+FLOOR_LEVEL-WALL_CLIP_V))
        *y = (ROOM_HEIGHT+FLOOR_LEVEL-WALL_CLIP_V);
    if (*y < (FLOOR_LEVEL+WALL_CLIP_V))
        *y = FLOOR_LEVEL+WALL_CLIP_V;
}

// clean up and exit
void cleanUpAndQuit()
{
    int i;

    // free memory alloocated for textures
    for (i = 0; i < numPix; ++i)
        free(pix[i]);

    exit(ALL_IS_WELL);
}
