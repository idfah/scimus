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

#ifndef SCULPT_H 
    #define SCULPT_H

    // OpenGL and GLUT headers
    #ifdef __APPLE__
        #include <GLUT/glut.h>
    #else
        #include <GL/gl.h>
        #include <GL/glu.h>
        #include <GL/glut.h>
    #endif

    // png loader library
    // #include "pngLoader.h"

    // default debug level
    #define DEBUG 0

    // maximum number of pics
    #define MAX_NUM_PIX 20

    // wall clipping distances
    #define WALL_CLIP_H   140
    #define WALL_CLIP_V   420

    // gallery dimensions
    #define ROOM_WIDTH    512*8
    #define ROOM_LENGTH   512*23
    #define FLOOR_LEVEL  -650
    #define ROOM_HEIGHT   1536

    // glass window dimensions
    #define GLASS_WIDTH   1024
    #define GLASS_HEIGHT  640
    #define GLASS_ELEV    384

    // outside dimensions
    #define OUTSIDE_WIDTH   256*32
    #define OUTSIDE_LENGTH  256*5
    #define OUTSIDE_HEIGHT  256*16

    // width of smallest tile
    #define TILE_RES  16

    // animation rate in ms/refresh
    #define ANI_RATE  100

    // exit stati
    #define ALL_IS_WELL       0
    #define MAX_TEX_ERROR     1
    #define IMAGE_SIZE_ERROR  2
    #define OUT_OF_MEM_ERROR  3

    /* wall paintings */
    typedef struct {
        /* center of painting */
        GLdouble xcenter, ycenter, zcenter;
        /* horizontal rotation */
        GLdouble hrot;
        /* pointer png texture */
        glpngtexture *pic;
    } painting;

    void  loadTextures(int n, char *picNames[]);    // load images from file
    void  initTextures();                           // create OpenGL textures from loaded images
    void  initLighting();                           // initialize scene lighting
    void  initPaintings();                          // initialize painting locations
    void  initCallBacks();                          // initialize glut call-back functions
    void  draw();                                   // draw to the display
    void  animate(int i);                           // perform timed animation
    void  placeLights();                            // place lights in the scene
    void  drawFloor();                              // draw a tiled floor
    void  drawCeiling();                            // draw the room ceiling
    void  drawWalls();                              // draw the room walls
    void  drawGlass();                              // draw the window
    void  openGlass();                              // open the window
    void  drawOutside();                            // draw the skyline
    void  drawText(int x, int y, int z, char *t);   // draw 2d text
    void  drawSculpture1();                         // draw the sculptures
    void  drawSculpture2();
    void  drawSculpture3();
    void  drawSculpture4();
    void  drawSculpture5();
    void  drawPaintings();
    void  updateSculpture1();                       // update sculpture animation
    void  updateSculpture2();
    void  updateSculpture4();
    void  keyDown(unsigned char key, int x, int y); // respond to key press
    void  keyUp(unsigned char key, int x, int y);   // respond to key release
    void  enforceWallClipping(GLdouble *x,          // wall clipping call-back
                      GLdouble *y, GLdouble *z);
    void  cleanUpAndQuit();                         // clean up and exit
    int   isPower2(int x);                          // test if x is a power of 2

#endif
