#include "graphical.h"

#ifdef _MAZE_GRAPHICAL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/freeglut.h>
#include "maze.h"
#include "util.h"
#include "config.h" // *color

void drawFloor(int n) {
    glPushMatrix();
        glColor3f(floorcolor);
        glScalef(n, 0, n);
        glTranslatef(0.5, 0, 0.5);
    glPopMatrix();
}

/* Draws a single maze cell. Note that a Maze is composed by NxN
 * cells, where N is the maze complexity defined in 'config.h' */
void drawCell(int cell, int ferom) {
    float value = 0.8;
    glPushMatrix();
        glPushMatrix();
            //ferom -= (complexity/2);
            glColor3f((float)ferom/complexity, 0, (1.0f-ferom)/complexity);
            glBegin(GL_QUADS);
                glVertex3f(1, 0, 0);
                glVertex3f(1, 0, 1);
                glVertex3f(0, 0, 1);
                glVertex3f(0, 0, 0);
            glEnd();
        glPopMatrix();
        glColor3f(wallcolor);
        if ((cell & 0x0f) == 0x0f) {
            glTranslatef(0.5, 0.5, 0.5);
            glutSolidCube(1);
        } else {
            // one cell will be seen as divided by five:
            // then each little square is 0.20 the square.
            // note that .i is the order the wall is drawn
            //
            // .7           .6          .5
            //    ,---,---,---,---,---,
            //    |NW |   NORTH   | NE|
            //    |---+---+---+---+---|
            //    | W |   |   |   | E |
            //    |-E-+---+---+---+-A-|
            // .8 | S |   |   |   | S | .4
            //    |-T-+---+---+---+-T-|
            //    |   |   |   |   |   |
            //    |---+---+---+---+---|
            //    |SW |   SOUTH   | SE|
            //    '---'---'---'---'---'
            // .1           .2          .3
            //
            glScalef(0.2, 1, 0.2);
            // south-east corner (aways)
            glTranslatef(0.5, 0.5, 0.5);
            glutSolidCube(1);
            // south wall (not aways)
            glTranslatef(0, 0, 2);
            if (cell & SOUTH) {
                glPushMatrix();
                    glScalef(1, 1, 3);
                    glutSolidCube(1);
                glPopMatrix();
            }
            // south-east corner (aways)
            glTranslatef(0, 0, 2);
            glutSolidCube(1);
            // east wall (not aways)
            glTranslatef(2, 0, 0);
            if (cell & EAST) {
                glPushMatrix();
                    glScalef(3, 1, 1);
                    glutSolidCube(1);
                glPopMatrix();
            }
            // north-east corner (aways)
            glTranslatef(2, 0, 0);
            glutSolidCube(1);
            // north wall (not aways)
            glTranslatef(0, 0, -2);
            if (cell & NORTH) {
                glPushMatrix();
                    glScalef(1, 1, 3);
                    glutSolidCube(1);
                glPopMatrix();
            }
            // north-east corner (aways)
            glTranslatef(0, 0, -2);
            glutSolidCube(1);
            // west corner (not aways)
            glTranslatef(-2, 0, 0);
            if (cell & WEST) {
                glPushMatrix();
                    glScalef(3, 1, 1);
                    glutSolidCube(1);
                glPopMatrix();
            }
        }
    glPopMatrix();
}


void drawStartEnd() {
    glLineWidth(8);
    glPushMatrix();
        glColor3f(startcolor);
        glTranslatef((float)maze.start_i+0.5, 0.5, (float)maze.start_j+0.5);
        glScalef(0.6,1,0.6);
        glutWireCube(1);
    glPopMatrix();
    glPushMatrix();
        glColor3f(endcolor);
        glTranslatef((float)maze.end_i+0.5, 0.5, (float)maze.end_j+0.5);
        glScalef(0.6,1,0.6);
        glutWireCube(1);
    glPopMatrix();
    glLineWidth(1);
}

/* Draws the map based on the maze matrix
 * 
 * Note that it follows the convension. see 'graphical.h'
 */
void drawMap() {
    int i, j;
    // draws start and end
    drawStartEnd();
    // draw each cell
    glPushMatrix();
    for (i = 0; i < complexity; i++) {
        glPushMatrix();
        for (j = 0; j < complexity; j++) {
            drawCell(maze.map[i][j], ferom[i][j]);
            glTranslatef(0, 0, 1);
        }
        glPopMatrix();
        glTranslatef(1, 0, 0);
    }
    glPopMatrix();
}

void drawMaze() {
    glPushMatrix();
    drawFloor(complexity);
    glScalef(1, wallheight, 1);
    drawMap();
    glPopMatrix();
}

void setLights() {
    glPushMatrix();
    glTranslatef(10, 10, 10);
    glLightf(GL_LIGHT1, GL_DIFFUSE, 0.3);
    glPopMatrix();
}

/* It draws the player */
void drawPlayer() {
    glPushMatrix();
        glColor3f(playercolor);
        glTranslatef(player.x, player.radius, player.z);
        glScalef(player.radius, player.radius, player.radius);
        glutSolidSphere(1, hrings, vrings);
    glPopMatrix();
}

void drawFred() {
    glPushMatrix();
        glColor3f(fredcolor);
        glTranslatef(fred.x, fred.radius, fred.z);
        glScalef(fred.radius, fred.radius, fred.radius);
        glutSolidSphere(1, hrings, vrings);
    glPopMatrix();
}

/* Set up the camera */
void setCamera() {
    int h;
    // game in third person mode
    camera.aperture = 45;
    if (camera.mode == THIRD) {
        h = camheight/2; 
        // camera.posy = h;
        camera.posx = player.x - h/2 * cosf(camera.angle * M_PI / 180);
        camera.posz = player.z - h/2 * sinf(camera.angle * M_PI / 180);
        camera.lx = player.x;
        camera.ly = 0;
        camera.lz = player.z;
        camera.min = 1;
        camera.max = 200;
        camera.ux = 0;
        camera.uy = 1;
        camera.uz = 0;
    // game in first person mode
    } else if (camera.mode == FIRST) {
        camera.posy = 2*ballradius;
        camera.posx = player.x;
        camera.posz = player.z;
        camera.ly = camera.posy;
        camera.lx = player.x + sinf(camera.angle * M_PI / 180);
        camera.lz = player.z + cosf(camera.angle * M_PI / 180);
        camera.min = ballradius;
        camera.max = 2 * complexity;
        camera.ux = 0;
        camera.uy = 1;
        camera.uz = 0;
    // game view in superior mode
    } else if (camera.mode == SUP) {
        camera.prop = ((float) glutGet(GLUT_WINDOW_WIDTH)) / glutGet(GLUT_WINDOW_HEIGHT);
        camera.posy = camheight;
        camera.posx = player.x;
        camera.posz = player.z;
        camera.ly = 0;
        camera.lx = player.x;
        camera.lz = player.z;
        camera.xmin = -8 * camera.prop;
        camera.xmax = 8 * camera.prop;
        camera.ymin = -8;
        camera.ymax = 8;
        camera.zmin = -32;
        camera.zmax = 32;
        camera.ux = 1;
        camera.uy = 0;
        camera.uz = 0;
    } else {
        ERROR("Invalid camera mode");
    } 

    if (camera.mode == THIRD || camera.mode == FIRST) {
        gluPerspective(camera.aperture, camera.prop, camera.min, camera.max);
    } else if (camera.mode == SUP) {
        glOrtho(
            camera.xmin, camera.xmax,
            camera.ymin, camera.ymax,
            camera.zmin, camera.zmax
        );
    } else {
        ERROR("Invalid camera mode");
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        camera.posx, camera.posy, camera.posz,
        camera.lx, camera.ly, camera.lz,
        camera.ux, camera.uy, camera.uz
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

#endif
