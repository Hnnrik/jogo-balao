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
    float velocidadeNuvem = 0.01f;
    int quantidadeNuvens=0;
    nuvem nuvens[MAXIMO_NUVENS];
    bool colisao = false;

    void atualiza(int valor);
    void checaColisao();
    void desenhaBalaoManual();
    void display();
    void desenhaNuvem();
    void criaNuvem();

    void atualiza(int valor) {
        if (!colisao) {
            for(int i = 0; i<MAXIMO_NUVENS;i++){
                if(i!=0){
                    if(nuvens[i-1].nuvemX<0){
                        nuvens[i].velocidadeNuvem=0.01;
                    }
                }
                nuvens[i].nuvemX -= nuvens[i].velocidadeNuvem;
                if (nuvens[i].nuvemX < -1.5f){
                    nuvens[i].nuvemX = 1.5f;
                    nuvens[i].nuvemY = ((float)rand() / RAND_MAX) * 1.6f - 0.8f;

                }
                checaColisao();
            }

        }
        glutPostRedisplay();
        glutTimerFunc(16, atualiza, 0);
    }

    void desenhaChaoDeNuvem2() {
        glPushMatrix();
        glColor3f(0.8f, 0.8f, 0.8f); // Cor da nuvem

        glTranslatef(0.0f, -1.25f, 0.0f); // Z = 0.0f (posição base)
        glutSolidSphere(0.4f, 20, 20);

        glTranslatef(0.3f, 0.1f, 0.2f); // Z = 0.2f (mais perto)
        glutSolidSphere(0.3f, 20, 20);

        glTranslatef(-0.5f, 0.0f, -0.1f); // Z = -0.1f (mais longe que a base)
        glutSolidSphere(0.35f, 20, 20);

        glTranslatef(0.0f, -0.2f, 0.3f); // Z = 0.3f (ainda mais perto)
        glutSolidSphere(0.28f, 20, 20);

        glTranslatef(-0.4f, -0.1f, -0.2f); // Z = -0.2f (mais longe)
        glutSolidSphere(0.32f, 20, 20);

        glTranslatef(0.3f, 0.1f, 0.2f); // Z = 0.2f (mais perto)
        glutSolidSphere(0.25f, 20, 20);



        glPopMatrix();
    }


    void desenhaChaoDeNuvem() {

        for(int i = 0; i<6;i++){
            glPushMatrix();
            glColor3f(0.8f, 0.8f, 0.8f); // Cor da nuvem

            switch (i){
                case 0: glTranslatef(-0.9, -1.4, 0.0); break;
                case 1: glTranslatef(0.0, -1.45, 0.0); break;
                case 2: glTranslatef(0.7, -1.35, 0.0); break;
                case 3: glTranslatef(-0.5, -1.4, -0.1); break;
                case 4: glTranslatef(0.3, -1.45, -0.1); break;
                case 5: glTranslatef(1.0, -1.45, -0.1); break;

            }

            glutSolidSphere(0.4f, 20, 20);

            glTranslatef(0.3f, 0.1f, 0.2f); // Z = 0.2f (mais perto)
            glutSolidSphere(0.3f, 20, 20);

            glTranslatef(-0.5f, 0.0f, -0.1f); // Z = -0.1f (mais longe que a base)
            glutSolidSphere(0.35f, 20, 20);

            glTranslatef(0.0f, -0.2f, 0.3f); // Z = 0.3f (ainda mais perto)
            glutSolidSphere(0.28f, 20, 20);

            glTranslatef(-0.4f, -0.1f, -0.2f); // Z = -0.2f (mais longe)
            glutSolidSphere(0.32f, 20, 20);

            glTranslatef(0.3f, 0.1f, 0.2f); // Z = 0.2f (mais perto)
            glutSolidSphere(0.25f, 20, 20);
            glPopMatrix();
        }

    }

    void checaColisao() {
        for(int i=0;i<quantidadeNuvens;i++){
            float dx = balaoX - nuvens[i].nuvemX;
            float dy = balaoY - nuvens[i].nuvemY;
            float distancia = sqrt(dx * dx + dy * dy);
            colisao = distancia < 0.35f;
        }
    }

    void criaNuvem(){
            for(int i =0;i<MAXIMO_NUVENS;i++){
                nuvens[i].nuvemX = 1.5f;
                nuvens[i].nuvemY = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
                nuvens[i].velocidadeNuvem=0.0;
                printf("Criou nuvem\n");
            }
            nuvens[0].velocidadeNuvem=0.01f;
            quantidadeNuvens++;
    }
    void desenhaNuvem() {
        for(int i=0;i<MAXIMO_NUVENS;i++){
            printf("andou\n");
            glPushMatrix();
            glTranslatef(nuvens[i].nuvemX, nuvens[i].nuvemY, 0.0f);
            glColor3f(1.0f, 1.0f, 1.0f);
            glutSolidSphere(0.15, 20, 20);
            glTranslatef(-0.1f, 0.05f, 0.0f);
            glutSolidSphere(0.12, 20, 20);
            glTranslatef(0.2f, 0.0f, 0.0f);
            glutSolidSphere(0.13, 20, 20);
            glPopMatrix();
        }
    }

void desenhaSombraBalao() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glDisable(GL_LIGHTING); // Sombra sem iluminação
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f); // Cor preta semi-transparente


    glTranslatef(balaoX, -0.95f, 0.4f); // y bem perto do chão de nuvens
    glScalef(1.0f, 0.1f, 1.0f); // Achatar no eixo Y
    glutSolidSphere(0.2, 20, 20); // Mesmo tamanho do balão

    glEnable(GL_LIGHTING);
    glPopMatrix();

    glDisable(GL_BLEND);

}

void desenhaSombraNuvens() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f); // Mais transparente para as nuvens

    for (int i = 0; i < MAXIMO_NUVENS; i++) {
        glPushMatrix();
        glTranslatef(nuvens[i].nuvemX, -0.95f, 0.4f);
        glScalef(1.0f, 0.1f, 1.0f); // Achatar também
        glutSolidSphere(0.15, 20, 20); // Tamanho parecido com a nuvem principal
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
    glPopMatrix();
    glDisable(GL_BLEND);
}

    void desenhaBalaoManual() {
        glPushMatrix();
        glTranslatef(balaoX, balaoY, 0.0f); // Posição do balão manual
        glScalef(0.8f, 0.8f, 0.8f);

        // Parte principal do balão (esfera)
        glColor3f(0.9f, 0.2f, 0.2f); // Laranja
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat luzAmbiente[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    GLfloat luzDifusa[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat luzEspecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat posicaoLuz[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

    desenhaChaoDeNuvem();

    // <<< Adicione as sombras antes dos objetos
    desenhaSombraBalao();
    desenhaSombraNuvens();

    desenhaBalaoManual();
    desenhaNuvem();

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
                  0.0, 1.0, 0.0); // direcionamento da camera; ponto onde tá olhando; posição?(em pé ou deutado)
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
