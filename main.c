#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <locale.h>

typedef struct {
    float x, y, z;
} Vertex;

typedef struct {
    int v1, v2, v3;
} Face;

Vertex* vertices = NULL;
Face* faces = NULL;
int vertex_count = 0;
int face_count = 0;

float angle = 0.0;

typedef struct{
    float nuvemX;
    float nuvemY;
    float velocidadeNuvem;

} nuvem;

#define MAXIMO_NUVENS 2
float balaoX = -0.8, balaoY = 0.3;

float aviaoX = -0.6, aviaoY = 0.3;
int escolha=0;
float nuvemX, nuvemY;
float velocidadeBaseNuvem = 0.01;
int quantidadeNuvens = 0;
nuvem nuvens[MAXIMO_NUVENS];
bool colisao = false;
int pontos = 0;

bool jogando = false;

void atualiza(int valor);
void checaColisao();
void desenhaBalaoManual();
void display();
void desenhaNuvem();
void criaNuvem();
int melhor;
void salvarNovaMelhorPontuacao(const char *nome, int pontuacao);


void carregaImportado(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir arquivo OBJ: %s\n", filename);
        exit(1);
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "v ", 2) == 0) {
            Vertex v;
            sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
            vertices = realloc(vertices, sizeof(Vertex) * (vertex_count + 1));
            vertices[vertex_count++] = v;
        }
        else if (strncmp(line, "f ", 2) == 0) {
            Face f;
            int matches = sscanf(line, "f %d %d %d", &f.v1, &f.v2, &f.v3);
            if (matches != 3) {
                // Formato com barras: f v1/vt1/vn1 ...
                sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &f.v1, &f.v2, &f.v3);
            }
            faces = realloc(faces, sizeof(Face) * (face_count + 1));
            faces[face_count++] = f;
        }
    }

    fclose(file);
    printf("Carregado: %d vértices, %d faces\n", vertex_count, face_count);
}

void desenhaImportado() {

    glPushMatrix();

    glTranslatef(aviaoX, aviaoY, 0.0f);  // Posição na tela
    glScalef(0.001f, 0.001f, 0.001f);   // Escala razoável
    glRotatef(80.0f, -1, 0, 0);
    glRotatef(180.0f, 0, 0, 1);
    glColor3f(0.8, 0.1, 0.1);           // Cor do avião*/

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < face_count; i++) {
        Vertex v1 = vertices[faces[i].v1 - 1];
        Vertex v2 = vertices[faces[i].v2 - 1];
        Vertex v3 = vertices[faces[i].v3 - 1];
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
    glPopMatrix();
}

void reseta(){
    if(colisao){
        if(pontos> melhor){
            melhor=pontos;
            salvarNovaMelhorPontuacao("pontuacoes.csv",melhor);
        }
        colisao=false;
        balaoX=-0.8;
        balaoY = 0.3;

        aviaoX=-0.6;
        aviaoY = 0.3;
        pontos=0;
        velocidadeBaseNuvem=0.01;
        for(int i = 0; i<MAXIMO_NUVENS;i++){
            nuvens[i].nuvemX=1.5;
            nuvens[i].nuvemY = ((float)rand() / RAND_MAX) * 1.6 - 0.8;
            if(i==0){
                nuvens[i].velocidadeNuvem=velocidadeBaseNuvem;
                continue;
            }
            nuvens[i].velocidadeNuvem=0;
        }
    }
}

void atualiza(int valor){
    if (!colisao){
        for (int i = 0; i < MAXIMO_NUVENS; i++){
            if (nuvens[i].velocidadeNuvem == 0){
                if (nuvens[i - 1].nuvemX < 0 || nuvens[i + 1].nuvemX < 0){
                    nuvens[i].velocidadeNuvem = velocidadeBaseNuvem;
                }
            }

            nuvens[i].nuvemX -= nuvens[i].velocidadeNuvem;

            if (nuvens[i].nuvemX < -1.5){
                nuvens[i].nuvemX = 1.5 + ((float)(rand() % 50) / 100.0); // nasce entre 1.5 e 2.0
                nuvens[i].nuvemY = ((float)rand() / RAND_MAX) * 1.6 - 0.8;

                pontos++;
                printf("Pontos: %d\n", pontos);
                if (pontos % 10  == 0){
                    velocidadeBaseNuvem += 0.005;
                }
                nuvens[i].velocidadeNuvem=0;
            }
        }
        checaColisao();
    }
    glutPostRedisplay();
    glutTimerFunc(16, atualiza, 0);
}

void desenhaChaoDeNuvem(){
    float kd[4];
    float ks[4];
    float ns;
    glPushAttrib(GL_LIGHTING_BIT);  // Salva o estado atual de material/luz

    kd[0] = 1.0; kd[1] = 1.0; kd[2] = 1.0; kd[3] = 1.0;
    ks[0] = 0.0; ks[1] = 0.0; ks[2] = 0.0; ks[3] = 1.0;
    ns = 128.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT, GL_SHININESS, ns);

    for (int i = 0; i < 6; i++)
    {
        glPushMatrix();
        glColor3f(0.8, 0.8, 0.8); // Cor da nuvem

        switch (i)
        {
        case 0:
            glTranslatef(-0.9, -1.4, 0.0);
            break;
        case 1:
            glTranslatef(0.0, -1.45, 0.0);
            break;
        case 2:
            glTranslatef(0.7, -1.35, 0.0);
            break;
        case 3:
            glTranslatef(-0.5, -1.4, -0.1);
            break;
        case 4:
            glTranslatef(0.3, -1.45, -0.1);
            break;
        case 5:
            glTranslatef(1.0, -1.45, -0.1);
            break;
        }

        glutSolidSphere(0.4, 20, 20);

        glTranslatef(0.3, 0.1, 0.2);
        glutSolidSphere(0.3, 20, 20);

        glTranslatef(-0.5, 0.0, -0.1);
        glutSolidSphere(0.35, 20, 20);

        glTranslatef(0.0, -0.2, 0.3);
        glutSolidSphere(0.28, 20, 20);

        glTranslatef(-0.4, -0.1, -0.2);
        glutSolidSphere(0.32, 20, 20);

        glTranslatef(0.3, 0.1, 0.2);
        glutSolidSphere(0.25f, 20, 20);
        glPopMatrix();
    }
    glPopAttrib();
}

void checaColisao(){
    colisao = false;
    if(escolha==0){
        for (int i = 0; i < MAXIMO_NUVENS; i++){
            float dx = balaoX - nuvens[i].nuvemX;
            float dy = balaoY - nuvens[i].nuvemY;
            float distancia = sqrt(dx * dx + dy * dy);
            if (distancia < 0.30){
                colisao = true;
                break;
            }
        }
    } else {
        for (int i = 0; i < MAXIMO_NUVENS; i++){
            float xmin = aviaoX - 0.54;
            float xmax = aviaoX + 0.24;
            float ymin = aviaoY - 0.12;
            float ymax = aviaoY + 0.12;

            if((nuvens[i].nuvemX > xmin) && (nuvens[i].nuvemX < xmax) && (nuvens[i].nuvemY > ymin) && (nuvens[i].nuvemY < ymax)){
                colisao = true;
                break;
            }
        }
    }
}

void desenhaSombraAviao()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(0.0, 0.0, 0.0, 0.4);

    glTranslatef(aviaoX, -0.95, 0.4);
    glScalef(1.5 + aviaoY/2, 0.1, 1.0);
    glutSolidSphere(0.2, 20, 20);

    glEnable(GL_LIGHTING);
    glPopMatrix();

    glDisable(GL_BLEND);
}



void desenhaSombraNuvens()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(0.0, 0.0, 0.0, 0.4);

    for (int i = 0; i < MAXIMO_NUVENS; i++)
    {
        glPushMatrix();
        glTranslatef(nuvens[i].nuvemX, -0.95, 0.4);
        glScalef(1.0 + nuvens[i].nuvemY/2, 0.1, 1.0);
        glutSolidSphere(0.15, 20, 20);
        glPopMatrix();
    }

    glEnable(GL_LIGHTING);
    glPopMatrix();
    glDisable(GL_BLEND);
}

void criaNuvem(){
    for (int i = 0; i < MAXIMO_NUVENS; i++){
        nuvens[i].nuvemX = 1.5;
        nuvens[i].nuvemY = ((float)rand() / RAND_MAX) * 2.0 - 1.0;
        nuvens[i].velocidadeNuvem = 0.0;
        printf("Criou nuvem\n");
    }
}

void desenhaNuvem(){
    float kd[4];
    float ks[4];
    float ns;
    glPushAttrib(GL_LIGHTING_BIT);  // Salva o estado atual de material/luz

    kd[0] = 1.0; kd[1] = 1.0; kd[2] = 1.0; kd[3] = 1.0;
    ks[0] = 0.0; ks[1] = 0.0; ks[2] = 0.0; ks[3] = 1.0;
    ns = 128.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT, GL_SHININESS, ns);

    for (int i = 0; i < MAXIMO_NUVENS; i++)
    {
        glPushMatrix();
        glTranslatef(nuvens[i].nuvemX, nuvens[i].nuvemY, 0.0);
        glColor3f(1.0, 1.0, 1.0);
        glutSolidSphere(0.15, 20, 20);
        glTranslatef(-0.1, 0.05, 0.0);
        glutSolidSphere(0.12, 20, 20);
        glTranslatef(0.2, 0.0, 0.0);
        glutSolidSphere(0.13, 20, 20);
        glPopMatrix();
    }
    glPopAttrib();
}

void desenhaBalaoManual()
{
    glPushMatrix();

    float kd[4];
    float ks[4];
    float ns;

    glTranslatef(balaoX, balaoY, 0.0);
    glScalef(0.8, 0.8, 0.8);
    glColor3f(1.0, 0.0, 0.0);

    glPushAttrib(GL_LIGHTING_BIT);  // Salva o estado atual de material/luz

    kd[0] = 1.0; kd[1] = 1.0; kd[2] = 1.0; kd[3] = 1.0;
    ks[0] = 0.7; ks[1] = 0.7; ks[2] = 0.7; ks[3] = 1.0;
    ns = 45.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT, GL_SHININESS, ns);


    glutSolidSphere(0.2, 20, 20);

    glPopAttrib();

    glColor3f(0.6, 0.3, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.05, -0.2, 0.05);
    glVertex3f(0.02, -0.3, 0.02);

    glVertex3f(-0.05, -0.2, 0.05);
    glVertex3f(-0.02, -0.3, 0.02);

    glVertex3f(0.05, -0.2, -0.05);
    glVertex3f(0.02, -0.3, -0.02);

    glVertex3f(-0.05, -0.2, -0.05);
    glVertex3f(-0.02, -0.3, -0.02);
    glEnd();

    glColor3f(0.4, 0.2, 0.0);
    glTranslatef(0.0, -0.32, 0.0);
    glutSolidCube(0.08);


    glPopMatrix();
}

void desenhaMenu() {
    if (!jogando) {

        char texto1[50];
        char texto2[50];
        char texto3[50];
        sprintf(texto1, "Para iniciar comece a se mover", pontos);
        sprintf(texto2, "A melhor pontuação foi %d",melhor);
        sprintf(texto3, "Pressione 'e' para mudar o transporte");

        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 600, 0, 600);

        glColor3f(0.0, 0.0, 0.0);

        // Primeira linha
        glRasterPos2i(200, 310);
        for (char *c = texto1; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24 , *c);
        }

        // Segunda linha
        glRasterPos2i(200, 280);  // um pouco abaixo da primeira
        for (char *c = texto2; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24 , *c);
        }

        glRasterPos2i(200, 250);  // um pouco abaixo da primeira
        for (char *c = texto3; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24 , *c);
        }

        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
}

void desenhaGameOver() {
    if (colisao) {
        char texto1[50];
        char texto2[50];
        sprintf(texto1, "Voce fez %d pontos.", pontos);
        sprintf(texto2, "Pressione R para reiniciar.");

        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, 600, 0, 600);

        glColor3f(0.0f, 0.0f, 0.0f);

        // Primeira linha
        glRasterPos2i(200, 310);
        for (char *c = texto1; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }

        // Segunda linha
        glRasterPos2i(200, 280);  // um pouco abaixo da primeira
        for (char *c = texto2; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }

        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
}

void desenhaSombraBalao()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();


    glDisable(GL_LIGHTING);

    glColor4f(0.0, 0.0, 0.0, 0.4);


    glTranslatef(balaoX, -0.95, 0.4);
    glScalef(1.0 + balaoY / 2, 0.1, 1.0);

    glutSolidSphere(0.2, 20, 20);

    glEnable(GL_LIGHTING);

    glPopMatrix();

    glDisable(GL_BLEND);


    glColor4f(1.0, 1.0, 1.0, 1.0);
}


void salvarNovaMelhorPontuacao(const char* nomeArquivo, int novaPontuacao) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }
    fprintf(arquivo, "%d\n", novaPontuacao);  // Escreve a pontuação
    fclose(arquivo);
}


int obterPrimeiraPontuacao(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return -1;
    }

    int pontuacao = 0;
    fscanf(arquivo, "%d", &pontuacao);

    fclose(arquivo);
    return pontuacao;
}


void desenhaPontuacao(){
    char texto[20];
    sprintf(texto, "Pontos: %d", pontos);

    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 600, 0, 600);
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(10, 570);
    for (char *c = texto; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void desenhaMelhor(){
    char texto[20];
    sprintf(texto, "O melhor foi: %d", melhor);

    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 600, 0, 600);
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(10, 553);
    for (char *c = texto; *c != '\0'; c++){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    desenhaChaoDeNuvem();
    if(escolha==0){
        desenhaSombraBalao();
        desenhaBalaoManual();
    }else{
        desenhaSombraAviao();
         desenhaImportado();
    }

    desenhaSombraNuvens();


    desenhaNuvem();
    desenhaPontuacao();
    desenhaMelhor();

    desenhaMenu();
    desenhaGameOver();


    glutSwapBuffers();
}


void teclado(unsigned char tecla, int x, int y){
    if(tecla == 'r'){
        reseta();
    }else if(tecla== 'e' && !jogando){
        escolha=1-escolha;
    }
}

void tecladoEspecial(int tecla, int x, int y){
    if(tecla == GLUT_KEY_UP || tecla == GLUT_KEY_DOWN || tecla == GLUT_KEY_LEFT|| tecla == GLUT_KEY_RIGHT){
        nuvens[0].velocidadeNuvem=velocidadeBaseNuvem;
        jogando=true;
    }
   if(escolha==0){
        switch (tecla){
        case GLUT_KEY_UP:
            balaoY += balaoY < 1.1 ? 0.06 : 0.0;
            break;
        case GLUT_KEY_DOWN:
            balaoY -= balaoY > -0.55 ? 0.06 : 0.0;
            break;
        case GLUT_KEY_LEFT:
            balaoX -= balaoX > -1.1 ? 0.06 : 0.0;
            break;
        case GLUT_KEY_RIGHT:
            balaoX += balaoX < 1.1 ? 0.06 : 0.0;
            break;
        }
   }else{
        switch (tecla){
        case GLUT_KEY_UP:
            aviaoY += aviaoY < 1.1 ? 0.06 : 0.0;
            break;
        case GLUT_KEY_DOWN:
            aviaoY -= aviaoY > -0.55 ? 0.06 : 0.0;
            break;
        case GLUT_KEY_LEFT:
            aviaoX -= aviaoX > -1.1 ? 0.06 : 0.0;
            break;
        case GLUT_KEY_RIGHT:
            aviaoX += aviaoX < 1.1 ? 0.06 : 0.0;
            break;
        }

   }
    printf("O aviao se moveu para (%.2f,%.2f)\n", aviaoX, aviaoY);
    glutPostRedisplay();

}

void luzes(){

    float luzAmbiente[] = {0.2, 0.2, 0.2, 1.0};
    float luzDifusa[] = {0.8, 0.8, 0.8, 1.0};
    float luzEspecular[] = {1.0, 1.0, 1.0, 1.0};
    float posicaoLuz[] = {1.5, 2.0, 2.0, 0.0};

    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void init(){
    melhor = obterPrimeiraPontuacao("pontuacoes.csv");
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    luzes();
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(0.5, 0.8, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    criaNuvem();
    gluLookAt(0.0, 0.0, 3.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);
}

int main(int argc, char **argv){
    carregaImportado("Low-Poly-Airplane/11804_Airplane_v2_l2.obj");
    setlocale(LC_ALL, "");
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Jogo do Balao - OpenGL");
    init();
    glutDisplayFunc(display);
    glutSpecialFunc(tecladoEspecial);
    glutKeyboardFunc(teclado);
    glutTimerFunc(16, atualiza, 0);
    glutMainLoop();
    return 0;
}
