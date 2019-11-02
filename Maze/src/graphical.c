#include "graphical.h"

#ifdef _MAZE_GRAPHICAL_H

#include <stdio.h>
#include <stdlib.h>
#include <GL/freeglut.h>
#include "maze.h"
#include "util.h"
#include "config.h"

void drawFloor(int n) {
    glPushMatrix();
    glColor3f(floorcolor);
    glScalef(n, 0, n);
    glTranslatef(0.5, 0, 0.5);
    glutSolidCube(1);
    glPopMatrix();
}

/* Draws a single maze cell. Note that a Maze is composed by NxN
 * cells, where N is the maze complexity defined in 'config.h' */
void drawCell(int cell) {
    glPushMatrix();
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
        glTranslatef(2, 0, 0);
        if (cell & SOUTH) {
            glPushMatrix();
            glScalef(3, 1, 1);
            glutSolidCube(1);
            glPopMatrix();
        }
        // south-east corner (aways)
        glTranslatef(2, 0, 0);
        glutSolidCube(1);
        // east wall (not aways)
        glTranslatef(0, 0, 2);
        if (cell & EAST) {
            glPushMatrix();
            glScalef(1, 1, 3);
            glutSolidCube(1);
            glPopMatrix();
        }
        // north-east corner (aways)
        glTranslatef(0, 0, 2);
        glutSolidCube(1);
        // north wall (not aways)
        glTranslatef(-2, 0, 0);
        if (cell & NORTH) {
            glPushMatrix();
            glScalef(3, 1, 1);
            glutSolidCube(1);
            glPopMatrix();
        }
        // north-east corner (aways)
        glTranslatef(-2, 0, 0);
        glutSolidCube(1);
        // west corner (not aways)
        glTranslatef(0, 0, -2);
        if (cell & WEST) {
            glPushMatrix();
            glScalef(1, 1, 3);
            glutSolidCube(1);
            glPopMatrix();
        }
    }
    glPopMatrix();
}

/* Draws the map based on the maze matrix
 * 
 * Note that it follows the convension. see 'graphical.h'
 */
void drawMap(int map[complexity][complexity], int n) {
    int i, j;
    glPushMatrix();
    for (i = 0; i < n; i++) {
        glPushMatrix();
        for (j = 0; j < n; j++) {
            drawCell(map[i][j]);
            glTranslatef(1, 0, 0);
        }
        glPopMatrix();
        glTranslatef(0, 0, 1);
    }
    glPopMatrix();
}

void drawMaze(int map[complexity][complexity], int n) {
    glPushMatrix();
    drawFloor(n);
    glScalef(1, wallheight, 1);
    drawMap(map, n);
    glPopMatrix();
}

void setLights() {
    glPushMatrix();
    glTranslatef(10, 10, 10);
    glLightf(GL_LIGHT1, GL_DIFFUSE, 1.5);
    glPopMatrix();
}

#endif
