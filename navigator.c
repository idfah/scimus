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

// standard c headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// OpenGL and GLUT headers
#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glut.h>
#endif

// type defs and prototypes
#include "navigator.h"

// debug level
short navDebug = NAV_DEBUG;

// window attributes
int windowID;
int winWidth  = DEFAULT_WIN_WIDTH;
int winHeight = DEFAULT_WIN_HEIGHT;

// current zoom
GLdouble zoomLevel = DEFAULT_ZOOM_LEVEL;

// camera variables
GLdouble cameraLocX = DEFAULT_CAMERA_X;
GLdouble cameraLocY = DEFAULT_CAMERA_Y;
GLdouble cameraLocZ = DEFAULT_CAMERA_Z;

GLdouble rotationH = DEFAULT_ROTATION_H;
GLdouble rotationV = DEFAULT_ROTATION_V;

// clipping status
bool wallClipping = true;

// show the origin
bool showO = false;

// mouse variables
bool warpFlag = false;
int mouseMode = IDLE;

// keyboard variables
bool smoothMotionLeft  = false;
bool smoothMotionRight = false;
bool smoothMotionUp    = false;
bool smoothMotionDown  = false;
bool smoothMotionZoom  = false;
bool smoothMotionDuck  = false;
bool ducking           = false;
bool smoothMotionJump  = false;
bool jumping           = false;
bool smoothMotionOpen  = false;
GLdouble turnUnit      = DEFAULT_TURN_UNIT;
GLdouble moveUnit      = DEFAULT_MOVE_UNIT;
GLdouble jumpUnit      = DEFAULT_JUMP_UNIT;

// call-back functions
void (*navDraw)(void) = navDefaultDrawFunc;
void (*navClip)(GLdouble *x, GLdouble *y, GLdouble *z) = navDefaultClipFunc;
void (*navKey)(unsigned char key, int x, int y)   = navDefaultKeyFunc;
void (*navKeyUp)(unsigned char key, int x, int y) = navDefaultKeyUpFunc;

// initialize navigator
void navInit(int nargs, char *args[])
{
    // initialize glut & pass any args
    glutInit(&nargs, args);

    navInitWindow(nargs, args);
    navInitDisplay();
    navInitCallBacks();
}

// initialize our window
void navInitWindow(int nargs, char *args[])
{
    // set to double buffer & RGB
    if (MULTISAMPLE_AA)
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    else
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // create the window
    glutInitWindowSize(winWidth, winHeight);
    glutInitWindowPosition(50, 50);
    windowID = glutCreateWindow("Virtual Science Museum");

    //glutGameModeString(GAME_MODE_STRING);
}

// initialize our OpenGL display
void navInitDisplay()
{
    winWidth = glutGet(GLUT_WINDOW_WIDTH);
    winHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // initialize the perspective projection matrix
    navWindowResize(winWidth, winHeight);

    // initialize the modelview matrix
    navUpdateCamera();

    // set depth test for 3d drawing
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    if (MULTISAMPLE_AA)
        glEnable(GL_MULTISAMPLE_ARB);

    // set default background color
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // configure alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // set up default polygon mode
    glPolygonMode(GL_FRONT, GL_FILL);

    // only show front faces
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
}

// initialize mouse and keyboard
void navInitCallBacks()
{
    // set the display call-back
    glutDisplayFunc(navDisplay);

    // set the window resize call-back
    glutReshapeFunc(navWindowResize);

    // use crosshair for cursor
    //glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);
    glutSetCursor(GLUT_CURSOR_NONE);

    // set the mouse call-back
    glutMouseFunc(navMouse);

    // set the mouse motion call-back
    glutMotionFunc(navActiveMouse);

    // ignore keyboard auto-repeate
    glutIgnoreKeyRepeat(true);

    // set the keyboard call-backs
    glutKeyboardFunc(navKeyboard);
    glutKeyboardUpFunc(navKeyboardUp);
    glutSpecialFunc(navKeyboardArrow);
    glutSpecialUpFunc(navKeyboardArrowUp);
}

// register external display call-back
void navDrawFunc(void (*func)(void))
{
    navDraw = func;
}

void navDefaultDrawFunc()
{
    // insert super-kewl draw function here
}

// draw to the display
void navDisplay()
{
    // clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    navUpdateCamera();

    if (showO)
        navDrawOrigin();

    navDraw();

    // swap doubble buffers
    glutSwapBuffers();
}

// update our view of the world
void navUpdateCamera()
{
    // move camera around the scene
    // note, vertical rotation not implimented
    if (!CAMERA_UPDATE_MODE) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cameraLocX, cameraLocY, cameraLocZ,
                cameraLocX-sin(rotationH*(M_PI/180.0)), cameraLocY, cameraLocZ-cos(rotationH*(M_PI/180.0)),
                0.0, 1.0, 0.0);
    }

    // move scene around the camera
    else if (CAMERA_UPDATE_MODE) {
        // load identity for modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        // apply our vertical rotation
        glRotated(-rotationV, 1.0, 0.0, 0.0);
        // apply our horizontal rotation
        glRotated(-rotationH, 0.0, 1.0, 0.0);
        // move to our current location
        glTranslated(-cameraLocX, -cameraLocY, -cameraLocZ);
    }

    if (navDebug > 0) {
        printf("cameraLocX: %f\n", cameraLocX);
        printf("cameraLocY: %f\n", cameraLocY);
        printf("cameraLocZ: %f\n", cameraLocZ);
        printf("rotationH:  %f\n", rotationH);
        printf("rotationV:  %f\n", rotationV);
    }
}

// draw the world origin in the scene
void navDrawOrigin()
{
    // save current modelview
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // draw x-axis
    glColor4d(1.0, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(256.0, 0.0, 0.0);
    glEnd();

    // draw y-axis
    glColor4d(0.0, 1.0, 0.0, 1.0);
    glBegin(GL_LINES);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(0.0, 256.0, 0.0);
    glEnd();

    // draw z-axis
    glColor4d(0.0, 0.0, 1.0, 1.0);
    glBegin(GL_LINES);
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3d(0.0, 0.0, 256.0);
    glEnd();

    // restore modelview
    glPopMatrix();
}

// respond to window resize
// reloads the perspective projection matrix
void navWindowResize(int w, int h)
{
    double winRatio = (double)w / (double)h;

    winWidth  = w;
    winHeight = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-zoomLevel*winRatio, zoomLevel*winRatio, -zoomLevel, zoomLevel, 512.0, 24000.0);

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void navClipFunc(void (*func)(GLdouble *x, GLdouble *y, GLdouble *z))
{
    navClip = func;
}

// default clipping function
// checks for floor clipping only
void navDefaultClipFunc(GLdouble *x, GLdouble *y, GLdouble *z)
{
    if (*y < ((-650)+420))
        *y = (-650)+(420);
}

// turn d degrees left
void navTurnHorizontal(GLdouble d)
{
    rotationH = fmod((rotationH + d), 360.0);
}

// turn d degrees up
void navTurnVertical(GLdouble d)
{
    if ((rotationV+d) >= MAX_VERT_ROT)
        rotationV = MAX_VERT_ROT;
    else if ((rotationV+d) <= -MAX_VERT_ROT)
        rotationV = -MAX_VERT_ROT;
    else
        rotationV = fmod((rotationV + d), 360.0);
}

// move d units forward
void navMoveForward(GLdouble d)
{
    cameraLocX += -d*(GLdouble)sin(rotationH * (M_PI/180.0));
    cameraLocZ += -d*(GLdouble)cos(rotationH * (M_PI/180.0));

    if (wallClipping == true)
        navClip(&cameraLocX, &cameraLocY, &cameraLocZ);
}

// move d units sideways
void navMoveSideways(GLdouble d)
{
    cameraLocX += -d*(GLdouble)cos(rotationH * (M_PI/180.0));
    cameraLocZ +=  d*(GLdouble)sin(rotationH * (M_PI/180.0));

    if (wallClipping == true)
        navClip(&cameraLocX, &cameraLocY, &cameraLocZ);
}

// move d units up
void navMoveUp(GLdouble d)
{
    cameraLocY += d;

    if (wallClipping == true)
        navClip(&cameraLocX, &cameraLocY, &cameraLocZ);
}

// zoom camera in or out
void navZoom(GLdouble amount)
{
    double winRatio;

    if ((zoomLevel - amount) <= 0)
        zoomLevel = 0.1;
    else if ((zoomLevel - amount) >= DEFAULT_ZOOM_LEVEL)
        zoomLevel = DEFAULT_ZOOM_LEVEL;
    else
        zoomLevel -= amount;

    // initialize the perspective projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    winRatio = (double)winWidth / (double)winHeight;
    glFrustum(-zoomLevel*winRatio, zoomLevel*winRatio, -zoomLevel, zoomLevel, 512.0, 24000.0);
}

// register and external keyboard function
void navKeyboardFunc(void (*func)(unsigned char key, int x, int y))
{
    navKey = func;
}

// default keyboard call-back
void navDefaultKeyFunc(unsigned char key, int x, int y)
{
    if (key == 'q')
        exit(EXIT_SUCCESS);
}

// respond to key press
void navKeyboard(unsigned char key, int x, int y)
{
    if ((key == '+') || (key == '=')) {
        smoothMotionZoom = true;
        navSmoothMotion(ZOOM_IN);
    }
    if ((key == '-') || (key == '_')) {
        smoothMotionZoom = true;
        navSmoothMotion(ZOOM_OUT);
    }
    if (key == 'c') {
        wallClipping = !wallClipping;
    }
    if ((key == 'j') || (key == 'J')) {
        if (!jumping) {
            jumping = true;
            smoothMotionJump = true;
            navSmoothMotion(JUMP);
        }
    }
    if ((key == 'd') || (key == 'D')) {
        smoothMotionDuck = true;
        navSmoothMotion(DUCK);
    }
    if (key == 'o') {
        showO = !showO;
        glutPostRedisplay();
    }
    if (key == '0') {
        /*
           cameraLocX = DEFAULT_CAMERA_X;
           cameraLocY = DEFAULT_CAMERA_Y;
           cameraLocZ = DEFAULT_CAMERA_Z;

           rotationH = DEFAULT_ROTATION_H;
           */
        rotationV = DEFAULT_ROTATION_V;

        zoomLevel = DEFAULT_ZOOM_LEVEL;
        navZoom(0);

        glutPostRedisplay();
    }

    navKey(key, x, y);
}

// register and external key-release function
void navKeyboardUpFunc(void (*func)(unsigned char key, int x, int y))
{
    navKeyUp = func;
}

// default key release function
void navDefaultKeyUpFunc(unsigned char key, int x, int y)
{
    // no nothing
}

// respond to key release
void navKeyboardUp(unsigned char key, int x, int y)
{
    if ((key == '+') || (key == '='))
        smoothMotionZoom = false;

    if ((key == '-') || (key == '_'))
        smoothMotionZoom = false;

    if ((key == 'u') || (key == 'U'))
        smoothMotionJump = false;

    if ((key == 'd') || (key == 'D'))
        smoothMotionDuck = false;

    navKeyUp(key, x, y);
}

// respond to arrow press
void navKeyboardArrow(int key, int x, int y)
{
    int mod = glutGetModifiers();

    turnUnit = DEFAULT_TURN_UNIT;
    moveUnit = DEFAULT_MOVE_UNIT;

    if ((mod & GLUT_ACTIVE_SHIFT) == (GLUT_ACTIVE_SHIFT)) {
        turnUnit *= 2.0;
        moveUnit *= 2.0;
    }

    if ((key == GLUT_KEY_LEFT) && (smoothMotionLeft == false)) {
        smoothMotionLeft = true;

        if ((mod & GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT)
            navSmoothMotion(MOVE_LEFT);
        else
            navSmoothMotion(TURN_LEFT);
    }

    if ((key == GLUT_KEY_RIGHT) && (smoothMotionRight == false)) {
        smoothMotionRight = true;

        if ((mod & GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT)
            navSmoothMotion(MOVE_RIGHT);
        else
            navSmoothMotion(TURN_RIGHT);
    }

    if ((key  == GLUT_KEY_UP) && (smoothMotionUp == false)) {
        smoothMotionUp = true;

        if ((mod & GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT)
            navSmoothMotion(TURN_UP);
        else
            navSmoothMotion(MOVE_FORWARD);
    }

    if ((key  == GLUT_KEY_DOWN) && (smoothMotionDown == false)) {
        smoothMotionDown = true;

        if ((mod & GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT)
            navSmoothMotion(TURN_DOWN);
        else
            navSmoothMotion(MOVE_BACKWARD);
    }
}

// respond to arrow release
void navKeyboardArrowUp(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
        smoothMotionLeft = false;

    if (key == GLUT_KEY_RIGHT)
        smoothMotionRight = false;

    if (key == GLUT_KEY_UP)
        smoothMotionUp = false;

    if (key == GLUT_KEY_DOWN)
        smoothMotionDown = false;
}

// animate keyboard motion
void navSmoothMotion(int m)
{
    if (m == MOVE_FORWARD) {
        navMoveForward(moveUnit);
        if (smoothMotionUp)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
    }

    if (m == MOVE_BACKWARD) {
        navMoveForward(-moveUnit);
        if (smoothMotionDown)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
    }

    if (m == MOVE_LEFT) {
        navMoveSideways(moveUnit);
        if (smoothMotionLeft)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
    }

    if (m == MOVE_RIGHT) {
        navMoveSideways(-moveUnit);
        if (smoothMotionRight)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
    }

    if (m == TURN_LEFT) {
        navTurnHorizontal(turnUnit);
        if (smoothMotionLeft)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
    }

    if (m == TURN_RIGHT) {
        navTurnHorizontal(-turnUnit);
        if (smoothMotionRight)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
    }

    if (m == TURN_UP) {
        navTurnVertical(turnUnit);
        if (smoothMotionUp)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
    }

    if (m == TURN_DOWN) {
        navTurnVertical(-turnUnit);
        if (smoothMotionDown)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
    }

    if (m == ZOOM_IN) {
        navZoom(5.0);
        if (smoothMotionZoom)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
    }

    if (m == ZOOM_OUT) {
        navZoom(-5.0);
        if (smoothMotionZoom)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
    }

    if (m == DUCK) {
        if (smoothMotionDuck)
            navMoveUp(-70.0);
        else
            navMoveUp(70.0);

        if (smoothMotionDuck || (cameraLocY < 0.0))
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
        else
            cameraLocY = 0.0;
    }

    if (m == JUMP) {
        jumpUnit -= JUMP_DELTA;
        navMoveUp(jumpUnit);

        if (cameraLocY > 0.0)
            glutTimerFunc(KEY_MOTION_DELAY, navSmoothMotion, m);
        else {
            cameraLocY = 0.0;
            jumpUnit = DEFAULT_JUMP_UNIT;
            /*if (smoothMotionJump)
              glutTimerFunc(400, smoothMotion, m);
              else*/
            jumping = false;
        }
    }

    glutPostRedisplay();
}

// respond to mouse clicks
void navMouse(int button, int state, int x, int y)
{
    if (state == GLUT_UP) {
        mouseMode = IDLE;
        //glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    }
    else if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON)
            mouseMode = MOVING;

        else if (button == GLUT_MIDDLE_BUTTON)
            mouseMode = ZOOMING;

        else if (button == GLUT_RIGHT_BUTTON)
            mouseMode = TURNING;

        warpFlag = true;
        glutWarpPointer(winWidth/2, winHeight/2);

        //glutSetCursor(GLUT_CURSOR_NONE);
    }
}

// respond to mouse motion
void navActiveMouse(int x, int y)
{
    // glutWarpPointer posts a mouse-motion event
    // so we need to skip every warp
    if (warpFlag) {
        warpFlag = false;
        return;
    }

    int mouseLocX = winWidth/2;
    int mouseLocY = winHeight/2;

    if (mouseMode != IDLE) {
        if (mouseMode == TURNING) {
            navTurnHorizontal(0.2*(mouseLocX-x));
            navTurnVertical(0.2*(mouseLocY-y));
        }
        else if (mouseMode == ZOOMING) {
            navZoom(mouseLocY-y);
        }
        else if (mouseMode == MOVING) {
            navMoveSideways(4.5*(mouseLocX-x));
            navMoveForward(6.0*(mouseLocY-y));
        }

        glutPostRedisplay();

        warpFlag = true;
        glutWarpPointer(mouseLocX, mouseLocY);
    }
}
