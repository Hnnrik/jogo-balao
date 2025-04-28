// Jogo do Balão com modelo OBJ, textura e sombra
// Compilar com: gcc -o balao balao.c -lGL -lGLU -lglut -lm

#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

float balaoX = 0.0f, balaoY = 0.0f;
float nuvemX = 1.5f;
float velocidadeNuvem = 0.01f;
bool colisao = false;
GLuint texturaID;

// Dados do modelo OBJ
typedef struct {
    float x, y, z;
} Vertice;

Vertice vertices[1000];
int faces[1000][3];
int totalVertices = 0;
int totalFaces = 0;

void carregaOBJ(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir arquivo OBJ: %s\n", nomeArquivo);
        exit(1); // Encerra se o arquivo não for encontrado
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (linha[0] == 'v' && linha[1] == ' ') {
            if (totalVertices < 1000) {
                sscanf(linha, "v %f %f %f",
                       &vertices[totalVertices].x,
                       &vertices[totalVertices].y,
                       &vertices[totalVertices].z);
                totalVertices++;
            }
        } else if (linha[0] == 'f') {
            if (totalFaces < 1000) {
                int v1, v2, v3;
                // Suporta faces com ou sem slashes (ex: f 1/1/1 2/2/2 3/3/3 ou f 1 2 3)
                if (sscanf(linha, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &v1, &v2, &v3) == 3 ||
                    sscanf(linha, "f %d %d %d", &v1, &v2, &v3) == 3) {
                    faces[totalFaces][0] = v1;
                    faces[totalFaces][1] = v2;
                    faces[totalFaces][2] = v3;
                    totalFaces++;
                }
            }
        }
    }

    fclose(arquivo);
    printf("Modelo carregado: %d vertices, %d faces\n", totalVertices, totalFaces);
}

void desenhaModeloOBJ() {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < totalFaces; i++) {
        for (int j = 0; j < 3; j++) {
            int idx = faces[i][j] - 1;
            glVertex3f(vertices[idx].x * 0.3, vertices[idx].y * 0.3, vertices[idx].z * 0.3);
        }
    }
    glEnd();
}

GLuint carregaTextura(const char* nomeArquivo) {
    FILE* f = fopen(nomeArquivo, "rb");
    if (!f) return 0;
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f);
    int largura = *(int*)&info[18];
    int altura = *(int*)&info[22];
    int tamanho = 3 * largura * altura;
    unsigned char* dados = (unsigned char*)malloc(tamanho);
    fread(dados, sizeof(unsigned char), tamanho, f);
    fclose(f);
    for (int i = 0; i < tamanho; i += 3) {
        unsigned char tmp = dados[i];
        dados[i] = dados[i + 2];
        dados[i + 2] = tmp;
    }
    GLuint textura;
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, dados);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    free(dados);
    return textura;
}

void desenhaPlanoComTextura() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaID);
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(-2, -1, -1);
        glTexCoord2f(1, 0); glVertex3f( 2, -1, -1);
        glTexCoord2f(1, 1); glVertex3f( 2, -1, 1);
        glTexCoord2f(0, 1); glVertex3f(-2, -1, 1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void desenhaBalaoManual() {
    glPushMatrix();
    glTranslatef(-0.8f, 0.3f, 0.0f); // Posição do balão manual
    glScalef(0.8f, 0.8f, 0.8f);

    // Parte principal do balão (esfera)
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

void desenhaBalao() {
    glPushMatrix();
    glTranslatef(balaoX, balaoY, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    desenhaModeloOBJ();
    glPopMatrix();
}

void desenhaSombra() {
    GLfloat sombraMatriz[16] = {
        1, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
    glDisable(GL_LIGHTING);
    glColor4f(0.1f, 0.1f, 0.1f, 0.5f);
    glPushMatrix();
    glMultMatrixf(sombraMatriz);
    glTranslatef(nuvemX, 0.5f, 0.0f);
    glutSolidSphere(0.15, 20, 20);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void desenhaNuvem() {
    glPushMatrix();
    glTranslatef(nuvemX, 0.5f, 0.0f);
    glColor3f(0.7f, 0.7f, 0.7f);
    glutSolidSphere(0.15, 20, 20);
    glTranslatef(-0.1f, 0.05f, 0.0f);
    glutSolidSphere(0.12, 20, 20);
    glTranslatef(0.2f, 0.0f, 0.0f);
    glutSolidSphere(0.13, 20, 20);
    glPopMatrix();
    desenhaSombra();
}

void checaColisao() {
    float dx = balaoX - nuvemX;
    float dy = balaoY - 0.5f;
    float distancia = sqrt(dx * dx + dy * dy);
    colisao = distancia < 0.35f;
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

    desenhaPlanoComTextura();
    desenhaBalao();
    desenhaNuvem();
    desenhaBalaoManual(); // <<< Adicionado aqui

    glutSwapBuffers();
}


void atualiza(int valor) {
    if (!colisao) {
        nuvemX -= velocidadeNuvem;
        if (nuvemX < -1.5f) nuvemX = 1.5f;
        checaColisao();
    }
    glutPostRedisplay();
    glutTimerFunc(16, atualiza, 0);
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

void inicializa() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    texturaID = carregaTextura("ceu.bmp");
    carregaOBJ("balao.obj");
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 3.0,   0.0, 0.0, 0.0,   0.0, 1.0, 0.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Jogo do Balao - OpenGL");
    inicializa();
    glutDisplayFunc(display);
    glutSpecialFunc(tecladoEspecial);
    glutTimerFunc(16, atualiza, 0);
    glutMainLoop();
    return 0;
}
