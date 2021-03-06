#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/freeglut.h>    // gl*, glut*, glu*
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "util.h"       // ERROR
#include "maze.h"       // mazeMapInit, initCamera, initPlayer, maze.*, endded
#include "graphical.h"  // drawMaze, setCamera
#include "config.h"     // background, complexity, thename
#include "leader.h"     // addEntry, printLeader

// functions prototypes
void init(int);
void display();
void mouse(int, int, int, int);
void motion(int, int);
void special(int, int, int);
void keyboard(unsigned char, int, int);
void pause();
void messages(int);

// global variables
int firstmov = 1;
int option = 1;
char username[128];

int main(int argc, char *argv[]) {
    srand(time(NULL));
    printf("Digite seu nick: ");
    scanf("%[^\n]", username);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInit(&argc, argv);
    glutInitWindowSize(winwidth, winheight);
    glutInitWindowPosition(10, 10);
    glutCreateWindow(thename);
    glutFullScreen();
    glutDisplayFunc(display);
    init(4);
    camera.mode = SUP;
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(motion);
    glutMainLoop();

    return 0;
}

void init(int n) {
    complexity = n;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH); 
    glEnable(GL_SMOOTH);
    glEnable(GL_BLEND);
    // glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glClearColor(background, 1.0);
    mazeMapInit();  // create random maze
    initPlayer();   // default player values
    setCamera();   // default camera values
    timer.start = clock();
}

void display() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (camera.mode == PAUSE) {
        pause();
    } else {
        setCamera();
        glPushMatrix();
            setLights();
            drawMaze(maze.map, complexity);
            drawPlayer();
            if (complexity >= 8) {
                drawFred();
                if (colideFred(player.x, player.z)) {
                    keyboard('q', 0, 0);
                }
            }
        glPopMatrix();
    }
    if (endded(player.x, player.z)) {
        timer.last_time = (double)(clock() - timer.start) / CLOCKS_PER_SEC;
        complexity++;
        init(complexity);
    }
    fredDelay.end = clock();
    fredDelay.last_time = (double)(fredDelay.end - fredDelay.start) / CLOCKS_PER_SEC;
    if (fredDelay.last_time > 0.03) {
        fredDelay.start = fredDelay.end;
        setFred();
    }
    glutPostRedisplay();
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    firstmov = 1;
    glutPostRedisplay();
}

void motion(int x, int y) {
    static int x0 = 0, y0 = 0;
    int dx, dy;
    if (!firstmov) {
        dx = x - x0;
        dy = y - y0;
        if (camera.mode == THIRD) {
            camera.angle += (float)dx/2;
            camera.posy += (float)dy/5;
            if (camera.posy > maxcamheight)
                camera.posy = maxcamheight;
            else if (camera.posy < mincamheight)
                camera.posy = mincamheight;
        } else if (camera.mode == FIRST) {
            camera.angle -= dx;
        }
    }
    x0 = x;
    y0 = y;
    firstmov = 0;
    glutPostRedisplay();
}

void special(int key, int x, int y)  {
    if (key == GLUT_KEY_UP)
        keyboard('w', x, y);
    else if (key == GLUT_KEY_DOWN)
        keyboard('s', x, y);
    else if (key == GLUT_KEY_LEFT)
        keyboard('a', x, y);
    else if (key == GLUT_KEY_RIGHT)
        keyboard('d', x, y);
    else
        ERROR("Command not found");
}

void keyboard(unsigned char key, int x, int y) {
    static int pauseMode = PAUSE;
    float xi, zi;
    xi = player.x;
    zi = player.z;
    if (tolower(key) == 'w') {
        // superior view
        if (camera.mode == SUP) {
            xi += player.speed;
        // third person view
        } else if (camera.mode == THIRD) {
            xi += player.speed * cosf(camera.angle * M_PI / 180);
            zi += player.speed * sinf(camera.angle * M_PI / 180);
        } else if (camera.mode == FIRST){
            // TODO: resolve first person
            xi += player.speed * cosf(camera.angle * M_PI / 180);
            zi += player.speed * sinf(camera.angle * M_PI / 180);
        }
    } 
    if (tolower(key) == 's') {
        // superior view
        if (camera.mode == SUP) {
            xi -= player.speed;
        // third person view
        } else if (camera.mode == THIRD) {
            xi -= player.speed * cosf(camera.angle * M_PI / 180);
            zi -= player.speed * sinf(camera.angle * M_PI / 180);
        } else if (camera.mode == FIRST){
            // TODO: resolve first person
        }
    } 
    if (tolower(key) == 'a') {
        // superior view
        if (camera.mode == SUP) {
            zi -= player.speed;
        } else if (camera.mode == THIRD) {
            xi += player.speed * sinf(camera.angle * M_PI / 180);
            zi -= player.speed * cosf(camera.angle * M_PI / 180);
        } else if (camera.mode == FIRST){
            // TODO: resolve first person
        }
    } 
    if (tolower(key) == 'd') {
        if (camera.mode == SUP) {
            zi += player.speed;
        } else if (camera.mode == THIRD) {
            xi -= player.speed * sinf(camera.angle * M_PI / 180);
            zi += player.speed * cosf(camera.angle * M_PI / 180);
        } else if (camera.mode == FIRST){
            // TODO: resolve first person
        } 
    } else if (key == '\t') {
        camera.mode = (camera.mode + 1) % 2;
    } else if (tolower(key) == 'q') {
        addEntry("leaderboard.txt", username, complexity-4, timer.last_time);
        printEntry("leaderboard.txt");
        system("cat leaderboard.txt");
        glutExit();
        exit(0);
    } else if (tolower(key) == 'p'){
        option = 2;
        if (pauseMode == PAUSE) {
            pauseMode = camera.mode;
            camera.mode = PAUSE;
        } else {
            camera.mode = pauseMode;
            pauseMode = PAUSE;
        }
    } else if (key == ' ') {
        option = 0;
    }

    if (!colide(xi, player.z)) player.x = xi;
    if (!colide(player.x, zi)) player.z = zi;
    feromonSpread(player.x, player.z);
    glutPostRedisplay();
}
void text(float s, float d, float f, char *string){

    //Coloca o texto na janela utilizando-se das coordenadas x ,y, z
    glRasterPos3f(s,d,f);
    int len=strlen(string);
    for(int i = 0; i < len; i++){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);

    }
    glutPostRedisplay();

}


void messages(int flag){

    if (flag == 0){
        glPushMatrix();
            setLights();
            drawMaze(maze.map, complexity);
            drawPlayer();
        glPopMatrix();
    } else if(flag == 1){
        glPushMatrix();
            glColor3ub(255,255,255);
            text( 5, 0, 0,  "Pressione a barra da iniciar");
            text( 4, 0, 0,  "Controles:        ");
            text( 3, 0, 0,  "'w' - move para cima");
            text( 2, 0, 0,  "'s' - move para tras");
            text( 1, 0, 0,  "'a' - move para esquerda");
            text( 0, 0, 0,  "'d' - move para direita");
            text(-1, 0, 0,  "'q' - Sair do jogo");
        glPopMatrix();
    } else if(flag == 2) {
        glPushMatrix();
            glColor3ub(255,255,255);
            text( 5, 0, 0,  "Pause");
            text( 4, 0, 0,  "Controles:");
            text( 3, 0, 0,  "espaco - para retornar ao jogo");
            text( 2, 0, 0,  "'w' - move para cima");
            text( 1, 0, 0,  "'s' - move para tras");
            text( 0, 0, 0,  "'a' - move para esquerda");
            text(-1, 0, 0,  "'d' - move para direita");
            text(-2, 0, 0,  "'q' - Sair do jogo");
        glPopMatrix();
    }

}
