#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

typedef struct{
    float nuvemX;
    float nuvemY;
    float velocidadeNuvem;

} nuvem;

#define MAXIMO_NUVENS 2
float balaoX = -0.8f, balaoY = 0.3f;
float nuvemX, nuvemY;
float velocidadeBaseNuvem = 0.01f;
int quantidadeNuvens=0;
nuvem nuvens[MAXIMO_NUVENS];
bool colisao = false;
int pontos = 0;


void atualiza(int valor);
void checaColisao();
void desenhaBalaoManual();
void display();
void desenhaNuvem();
void criaNuvem();

void atualiza(int valor) {
    if (!colisao) {
        for (int i = 0; i < MAXIMO_NUVENS; i++) {
            if (nuvens[i].velocidadeNuvem == 0) {
                if (nuvens[i-1].nuvemX < 0) {
                    nuvens[i].velocidadeNuvem = 0.01f; // Velocidade inicial
                }
            }

            // Atualiza a posi��o da nuvem
            nuvens[i].nuvemX -= nuvens[i].velocidadeNuvem;

            // Se a nuvem sair da tela
            if (nuvens[i].nuvemX < -1.5f) {
                nuvens[i].nuvemX = 1.5f + ((float)(rand() % 50) / 100.0f); // nasce entre 1.5 e 2.0
                nuvens[i].nuvemY = ((float)rand() / RAND_MAX) * 2.0f - 1.0f; // Reposiciona a nuvem


                pontos++; // Incrementa pontos ao passar a nuvem
                printf("Pontos: %d\n", pontos);
                if (pontos % 2==0) {
                        velocidadeBaseNuvem += 0.05f; // S� aumenta a velocidade base
                        if (velocidadeBaseNuvem > 0.05f) {
                            velocidadeBaseNuvem = 0.05f;
                        }
                    for (int g=0;g<MAXIMO_NUVENS;g++){
                                    nuvens[g].velocidadeNuvem = velocidadeBaseNuvem; // Aumento mais gradual na velocidade
                 }
                }

            }
        }

        checaColisao(); // Checa colis�es com o bal�o
    }

    glutPostRedisplay();
    glutTimerFunc(16, atualiza, 0); // Chama a fun��o de atualiza��o de 16 em 16 milissegundos
}


void desenhaChaoDeNuvem() {
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f); // Cor da nuvem

    // Parte principal da nuvem (mais central)
    glTranslatef(0.0f, -1.25f, 0.0f); // Z = 0.0f (posi��o base)
    glutSolidSphere(0.4f, 20, 20);

    // Adicionando "lobos" � nuvem - movendo alguns para frente (Z positivo)
    glTranslatef(0.3f, 0.1f, 0.2f); // Z = 0.2f (mais perto)
    glutSolidSphere(0.3f, 20, 20);

    glTranslatef(-0.5f, 0.0f, -0.1f); // Z = -0.1f (mais longe que a base)
    glutSolidSphere(0.35f, 20, 20);

    glTranslatef(0.0f, -0.2f, 0.3f); // Z = 0.3f (ainda mais perto)
    glutSolidSphere(0.28f, 20, 20);

    glTranslatef(-0.4f, -0.1f, -0.2f); // Z = -0.2f (mais longe)
    glutSolidSphere(0.32f, 20, 20);

    glTranslatef(0.6f, 0.05f, 0.1f); // Z = 0.1f (um pouco mais perto)
    glutSolidSphere(0.25f, 20, 20);

    glPopMatrix();
}


void checaColisao() {
    colisao = false;
    for (int i = 0; i < MAXIMO_NUVENS; i++) {
        float dx = balaoX - nuvens[i].nuvemX;
        float dy = balaoY - nuvens[i].nuvemY;
        float distancia = sqrt(dx * dx + dy * dy);
        if (distancia < 0.35f) {
            colisao = true;
            break; // N�o precisa continuar verificando se j� colidiu
        }
    }
}


void criaNuvem(){
        for(int i =0;i<MAXIMO_NUVENS;i++){
            nuvens[i].nuvemX = 1.5f;
            nuvens[i].nuvemY = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
            nuvens[i].velocidadeNuvem=0.0;
            printf("Criou nuvem\n");
        }
        nuvens[0].velocidadeNuvem = velocidadeBaseNuvem; // S� a primeira come�a andando
}
void desenhaNuvem() {
    for(int i=0;i<MAXIMO_NUVENS;i++){
        printf("andou\n");
        glPushMatrix();
        glTranslatef(nuvens[i].nuvemX, nuvens[i].nuvemY, 0.0f);
        glColor3f(0.7f, 0.7f, 0.7f);
        glutSolidSphere(0.15, 20, 20);
        glTranslatef(-0.1f, 0.05f, 0.0f);
        glutSolidSphere(0.12, 20, 20);
        glTranslatef(0.2f, 0.0f, 0.0f);
        glutSolidSphere(0.13, 20, 20);
        glPopMatrix();
    }
}


void desenhaBalaoManual() {
    glPushMatrix();
    glTranslatef(balaoX, balaoY, 0.0f); // Posi��o do bal�o manual
    glScalef(0.8f, 0.8f, 0.8f);

    // Parte principal do bal�o (esfera)
    glColor3f(1.0f, 0.5f, 0.0f); // Laranja
    glutSolidSphere(0.2, 20, 20);

    // Cordinhas (linhas finas)
    glColor3f(0.6f, 0.3f, 0.0f); // Marrom
    glBegin(GL_LINES);
        glVertex3f( 0.05f, -0.2f,  0.05f);
        glVertex3f( 0.02f, -0.3f,  0.02f);

        glVertex3f(-0.05f, -0.2f,  0.05f);
        glVertex3f(-0.02f, -0.3f,  0.02f);

        glVertex3f( 0.05f, -0.2f, -0.05f);
        glVertex3f( 0.02f, -0.3f, -0.02f);

        glVertex3f(-0.05f, -0.2f, -0.05f);
        glVertex3f(-0.02f, -0.3f, -0.02f);
    glEnd();

    // Cestinha (cubo pequeno)
    glColor3f(0.4f, 0.2f, 0.0f); // Marrom escuro
    glTranslatef(0.0f, -0.32f, 0.0f);
    glutSolidCube(0.08);

    glPopMatrix();
}
void desenhaPontuacao() {
    char texto[20];
    sprintf(texto, "Pontos: %d", pontos);

    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 600, 0, 600);
    glColor3f(0.0f, 0.0f, 0.0f); // Cor preta para o texto
    glRasterPos2i(10, 570); // Posi��o no canto superior esquerdo
    for (char* c = texto; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat luzAmbiente[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);
    /*
    GLfloat luzDifusa[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat luzEspecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat posicaoLuz[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);


    desenhaPlanoComTextura();
    desenhaBalao();

    desenhaBalaoManual(); // <<< Adicionado aqui
    */
    desenhaChaoDeNuvem();
    desenhaBalaoManual();
    desenhaNuvem();
    desenhaPontuacao();
    glutSwapBuffers();
}

void tecladoEspecial(int tecla, int x, int y) {
    switch (tecla) {
        case GLUT_KEY_UP: balaoY += 0.05f; break;
        case GLUT_KEY_DOWN: balaoY -= 0.05f; break;
        case GLUT_KEY_LEFT: balaoX -= 0.05f; break;
        case GLUT_KEY_RIGHT: balaoX += 0.05f; break;
    }
    glutPostRedisplay();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    criaNuvem();
    gluLookAt(0.0, 0.0, 3.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0); // direcionamento da camera; ponto onde t� olhando; posi��o?(em p� ou deutado)
}

int main(int argc, char** argv) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Jogo do Balao - OpenGL");
    init();
    glutDisplayFunc(display);
    glutSpecialFunc(tecladoEspecial);
    glutTimerFunc(16, atualiza, 0);
    glutMainLoop();
    return 0;
}
