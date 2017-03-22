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
 *  OpenGL/glut 3D scene navigation
 */

#ifndef NAVIGATOR_H 
    #define NAVIGATOR_H

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

    // default debug level
    #define NAV_DEBUG 0

    // glut window defaults
    #define DEFAULT_WIN_WIDTH   1000
    #define DEFAULT_WIN_HEIGHT  800
    #define GAME_MODE_STRING   "1920x1200:24"

    // mouse modes
    #define MOVING        1
    #define TURNING       2
    #define ZOOMING       3
    #define IDLE          4

    // keyboard modes
    #define MOVE_FORWARD  1
    #define MOVE_BACKWARD 2
    #define MOVE_LEFT     3
    #define MOVE_RIGHT    4
    #define TURN_LEFT     5
    #define TURN_RIGHT    6
    #define TURN_UP       7
    #define TURN_DOWN     8
    #define ZOOM_IN       9
    #define ZOOM_OUT      10
    #define DUCK          11
    #define JUMP          12
    #define OPEN          13

    #define KEY_MOTION_DELAY  40
    #define DEFAULT_TURN_UNIT 2.5
    #define DEFAULT_MOVE_UNIT 120.0

    #define DEFAULT_JUMP_UNIT 170.0
    #define JUMP_DELTA        27.0

    // default camera orientation
    #define DEFAULT_CAMERA_X 600.0
    #define DEFAULT_CAMERA_Y 0.0
    #define DEFAULT_CAMERA_Z 5200.0

    #define DEFAULT_ROTATION_H 0.0
    #define DEFAULT_ROTATION_V 0.0

    // default zoom
    #define DEFAULT_ZOOM_LEVEL 256.0

    // move camera around scene 0
    // move scene around camera 1
    // 1 has more features
    #define CAMERA_UPDATE_MODE 1

    // maximum vertical rotation
    #define MAX_VERT_ROT  15.0

    // use multisample anti-aliasing
    #define MULTISAMPLE_AA true

    void navInit(int nargs, char *args[]);               // initialize navigator
    void navInitWindow(int nargs, char *args[]);         // initialize our window
    void navInitDisplay();                               // initialize the OpenGL display
    void navInitCallBacks();                             // register glut call-backs
    void navDisplay();                                   // draw to the display
    void navUpdateCamera();                              // update our view of the world
    void navDrawFunc(void (*func)(void));                // register external display function
    void navDefaultDrawFunc();                           // default display function
    void navDrawOrigin();                                // draw the world origin
    void navWindowResize(int w, int h);                  // respond to window resize
    void navTurnHorizontal(GLdouble d);                  // turn d degrees horizontally
    void navTurnVertical(GLdouble d);                    // turn d degrees vertically
    void navMoveForward(GLdouble d);                     // move d units forward
    void navMoveSideways(GLdouble d);                    // move d units left
    void navClipFunc(void (*func)(GLdouble *x,           // register a clipping/collision function
                            GLdouble *y, GLdouble *z));
    void navDefaultClipFunc(GLdouble *x,                 // default clipping function
                            GLdouble *y, GLdouble *z);
    void navZoom(GLdouble amount);                       // zoom camera in or out

    void navKeyboardFunc(void (*func)(unsigned char key, int x, int y));
    void navDefaultKeyFunc(unsigned char key, int x, int y);
    void navKeyboard(unsigned char key, int x, int y);   // respond to key press

    void navKeyboardUpFunc(void (*func)(unsigned char key, int x, int y));
    void navDefaultKeyUpFunc(unsigned char key, int x, int y);
    void navKeyboardUp(unsigned char key, int x, int y); // respond to key release

    void navKeyboardArrow(int key, int x, int y);        // respond to arrow key press
    void navKeyboardArrowUp(int key, int x, int y);      // respond to arrow key release
    void navSmoothMotion(int m);                         // animate keyboard motion
    void navMouse(int button, int state, int x, int y);  // respond to mouse clicks
    void navActiveMouse(int x, int y);                   // respond to mouse motion

    #ifdef __cplusplus
        }
    #endif

#endif
