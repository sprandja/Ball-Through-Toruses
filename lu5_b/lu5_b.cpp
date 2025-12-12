// lu5_b.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "lu5_b.h"
/*
   АНИМАЦИЯ: Топче през 5 торуса с физика и отражение
   Библиотека: FreeGLUT
*/

#include <GL/freeglut.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <time.h> // for srand
#include <stdlib.h> // for rand
#pragma comment(linker, "/SUBSYSTEM:CONSOLE")

// --- КОНСТАНТИ ---
#define M_PI 3.14159265
const float BALL_RADIUS = 0.8f;
const float TORUS_THICKNESS = 0.8f;
const float TORUS_RADIUS = 3.0f;
const float SPEED = 0.4f;

// --- ГЛОБАЛНИ ПРОМЕНЛИВИ ---
struct Torus {
    float z;            // Позиция по Z
    bool isRotating;    // Дали се върти
    float angle;        // Текущ ъгъл
    float rotationSpeed;
    bool isHit;         // Дали е ударен
};

std::vector<Torus> toruses;
float ballZ;
float ballSpeed;

// --- ИНИЦИАЛИЗАЦИЯ НА ИГРАТА ---
void InitGame() {
    srand((unsigned int)time(NULL));

    // Топчето започва отдясно
    ballZ = 40.0f;
    ballSpeed = -SPEED; // Движи се наляво

    toruses.clear();
    for (int i = 0; i < 5; i++) {
        Torus t;
        t.z = 20.0f - (i * 10.0f);
        t.isHit = false;

        // 2-ри и 4-ти се въртят
        if (i == 1 || i == 3) {
            t.isRotating = true;
            t.angle = (float)(rand() % 360); // Случаен старт
            t.rotationSpeed = 2.5f;
        }
        else {
            t.isRotating = false;
            t.angle = 0.0f;
            t.rotationSpeed = 0.0f;
        }
        toruses.push_back(t);
    }
}

// --- ОТРИСУВАНЕ ---
void DrawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // КАМЕРА (Страничен изглед)
    gluLookAt(50.0, 0.0, 0.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0);

    // ОСВЕТЛЕНИЕ (Позиционирано така, че да осветява добре отстрани)
    GLfloat lightPos[] = { 10.0f, 20.0f, 20.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // 1. РИСУВАНЕ НА ТОРУСИТЕ
    for (size_t i = 0; i < toruses.size(); i++) {
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, toruses[i].z);

        // ЦВЯТ: ТУК Е КЛЮЧЪТ КЪМ ЯРКИТЕ ЦВЕТОВЕ
        if (toruses[i].isHit) {
            glColor3f(0.1f, 0.1f, 0.1f); // Тъмно ЧЕРНО при удар
        }
        else {
            glColor3f(0.0f, 0.0f, 1.0f); // Ярко СИНЬО
        }

        // Въртене
        glRotatef(toruses[i].angle, 0.0f, 1.0f, 0.0f);

        glutSolidTorus(TORUS_THICKNESS, TORUS_RADIUS, 30, 30);
        glPopMatrix();
    }

    // 2. РИСУВАНЕ НА ТОПЧЕТО
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, ballZ);

    glColor3f(1.0f, 0.0f, 0.0f); // Ярко ЧЕРВЕНО

    glutSolidSphere(BALL_RADIUS, 30, 30);
    glPopMatrix();

    glutSwapBuffers();
}

// --- ЛОГИКА ---
void Update(int value) {
    // Движение на топчето
    ballZ += ballSpeed;

    // Въртене на торусите
    for (auto& t : toruses) {
        if (t.isRotating && !t.isHit) {
            t.angle += t.rotationSpeed;
            if (t.angle > 360) t.angle -= 360;
        }
    }

    // ПРОВЕРКА ЗА УДАР
    for (auto& t : toruses) {
        if (abs(ballZ - t.z) < TORUS_THICKNESS && !t.isHit) {

            float rad = t.angle * M_PI / 180.0f;
            float blockage = abs(sin(rad)); // 0 = отворено, 1 = затворено

            // Ако е затворено -> УДАР
            if (blockage > 0.45f) {
                t.isHit = true;       // Става черно
                ballSpeed *= -1.0f;   // Отразява се
                ballZ += ballSpeed * 2.0f; // Отскача леко
            }
        }
    }

    // Рестарт за демонстрация (ако излезе от екрана)
    if (ballZ < -50.0f || ballZ > 50.0f) {
        if (ballZ < -50.0f) InitGame(); // Успех -> Нова игра
        else ballSpeed = -SPEED;        // Удар -> Връща се напред
    }

    glutPostRedisplay();
    glutTimerFunc(16, Update, 0);
}

// --- INIT ---
void Init() {
    // БЯЛ ФОН
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Настройка на светлината, за да не е прекалено ярка (избелваща)
    GLfloat ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.33, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);

    InitGame();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 400);
    glutCreateWindow("Red Ball vs Blue Torus");

    Init();

    glutDisplayFunc(DrawScene);
    glutTimerFunc(0, Update, 0);

    glutMainLoop();
    return 0;
}