// Reporte de practica 2 - HGMT
// Proyeccion ortogonal, transformaciones, piramides, cubos y shaders separados.
#include <stdio.h>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

Window mainWindow;
std::vector<Mesh*> objetos;
std::vector<Shader> shaders;

enum Objeto { PIRAMIDE, CUBO, PIRAMIDE_ESQUINA };
enum ColorShader { ROJO, VERDE, AZUL, CAFE, MAGENTA };

void CrearPiramide()
{
    unsigned int indices[] = {
        0,1,2, 1,3,2, 3,0,2, 1,0,3
    };
    GLfloat vertices[] = {
        -0.5f,-0.5f, 0.0f,
         0.5f,-0.5f, 0.0f,
         0.0f, 0.5f,-0.25f,
         0.0f,-0.5f,-0.5f
    };
    Mesh* piramide = new Mesh();
    piramide->CreateMesh(vertices, indices, 12, 12);
    objetos.push_back(piramide);
}

void CrearCubo()
{
    unsigned int indices[] = {
        0,1,2, 2,3,0, 1,5,6, 6,2,1,
        7,6,5, 5,4,7, 4,0,3, 3,7,4,
        4,5,1, 1,0,4, 3,2,6, 6,7,3
    };
    GLfloat vertices[] = {
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
         0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f
    };
    Mesh* cubo = new Mesh();
    cubo->CreateMesh(vertices, indices, 24, 36);
    objetos.push_back(cubo);
}

// Piramide con cara frontal triangular recta para formar las cuatro
// esquinas de la composicion central sin que sobresalgan como estrella.
void CrearPiramideEsquina()
{
    unsigned int indices[] = {
        0,1,2, 0,3,1, 1,3,2, 2,3,0
    };
    GLfloat vertices[] = {
        -0.5f, 0.5f, 0.25f,
         0.5f, 0.5f, 0.25f,
        -0.5f,-0.5f, 0.25f,
         0.0f, 0.0f,-0.50f
    };
    Mesh* piramide = new Mesh();
    piramide->CreateMesh(vertices, indices, 12, 12);
    objetos.push_back(piramide);
}

void CrearShaders()
{
    const char* vertex[] = {
        "shaders/shaderrojo.vert", "shaders/shaderverde.vert",
        "shaders/shaderazul.vert", "shaders/shadercafe.vert",
        "shaders/shadermagenta.vert"
    };
    const char* fragment[] = {
        "shaders/shaderrojo.frag", "shaders/shaderverde.frag",
        "shaders/shaderazul.frag", "shaders/shadercafe.frag",
        "shaders/shadermagenta.frag"
    };
    for (int i=0; i<5; ++i) {
        Shader* shader = new Shader();
        shader->CreateFromFiles(vertex[i], fragment[i]);
        shaders.push_back(*shader);
    }
}

void Dibujar(Objeto objeto, ColorShader color, const glm::mat4& projection,
             float x,float y,float z,float sx,float sy,float sz,
             float rz=0.0f,float rx=0.0f)
{
    shaders[color].useShader();
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(x,y,z));
    model = glm::rotate(model, glm::radians(rx), glm::vec3(1,0,0));
    model = glm::rotate(model, glm::radians(rz), glm::vec3(0,0,1));
    model = glm::scale(model, glm::vec3(sx,sy,sz));
    glUniformMatrix4fv(shaders[color].getModelLocation(),1,GL_FALSE,glm::value_ptr(model));
    glUniformMatrix4fv(shaders[color].getProjectLocation(),1,GL_FALSE,glm::value_ptr(projection));
    objetos[objeto]->RenderMesh();
}

void Trazo(ColorShader c,const glm::mat4& p,float x,float y,float sx,float sy,float ang=0)
{ Dibujar(CUBO,c,p,x,y,0.0f,sx,sy,0.18f,ang,18.0f); }

void DibujarIniciales(const glm::mat4& p)
{
    // H roja
    Trazo(ROJO,p,-6.6f,2.65f,0.18f,1.45f); Trazo(ROJO,p,-5.45f,2.65f,0.18f,1.45f);
    Trazo(ROJO,p,-6.02f,2.65f,1.15f,0.18f);
    // G verde
    Trazo(VERDE,p,-3.65f,3.25f,1.20f,0.18f); Trazo(VERDE,p,-4.16f,2.65f,0.18f,1.25f);
    Trazo(VERDE,p,-3.65f,2.05f,1.20f,0.18f); Trazo(VERDE,p,-3.15f,2.35f,0.18f,0.60f);
    Trazo(VERDE,p,-3.40f,2.65f,0.50f,0.18f);
    // M azul
    Trazo(AZUL,p,-1.65f,2.65f,0.18f,1.45f); Trazo(AZUL,p,-0.20f,2.65f,0.18f,1.45f);
    Trazo(AZUL,p,-1.28f,3.00f,0.90f,0.18f,-48.0f); Trazo(AZUL,p,-0.57f,3.00f,0.90f,0.18f,48.0f);
    // T magenta
    Trazo(MAGENTA,p,1.30f,3.25f,1.55f,0.18f); Trazo(MAGENTA,p,1.30f,2.65f,0.18f,1.25f);
}

void DibujarTresFiguras(const glm::mat4& p)
{
    // Piso y figura izquierda.
    Dibujar(CUBO,AZUL,p,0.15f,-2.32f,-0.7f,14.4f,0.50f,0.35f);
    Dibujar(CUBO,CAFE,p,-5.69f,0.00f,0,0.28f,4.18f,0.28f);
    Dibujar(CUBO,CAFE,p,-3.99f,0.00f,0,0.28f,4.18f,0.28f);
    Dibujar(PIRAMIDE,ROJO,p,-4.84f,0.86f,0,1.42f,1.16f,0.80f,180);
    Dibujar(PIRAMIDE,VERDE,p,-4.84f,-0.29f,0,1.42f,1.16f,0.80f,180);
    Dibujar(PIRAMIDE,MAGENTA,p,-4.84f,-1.50f,0,1.42f,1.34f,0.80f,180);

    // Figura central: cuatro piramides de esquina forman un cuadrado cerrado.
    Dibujar(PIRAMIDE_ESQUINA,ROJO,p,-0.75f,0.20f,0,1.5f,1.5f,0.75f,0);
    Dibujar(PIRAMIDE_ESQUINA,VERDE,p,0.75f,0.20f,0,1.5f,1.5f,0.75f,-90);
    Dibujar(PIRAMIDE_ESQUINA,AZUL,p,0.75f,-1.30f,0,1.5f,1.5f,0.75f,180);
    Dibujar(PIRAMIDE_ESQUINA,MAGENTA,p,-0.75f,-1.30f,0,1.5f,1.5f,0.75f,90);
    Dibujar(CUBO,AZUL,p,0,-0.55f,0.45f,2.12f,2.12f,0.35f,45);
    Dibujar(CUBO,CAFE,p,0,-0.55f,0.85f,1.18f,1.18f,0.42f,45);

    // Figura derecha: cuatro piramides.
    Dibujar(PIRAMIDE,VERDE,p,3.84f,-1.33f,0,1.82f,1.56f,1.0f);
    Dibujar(PIRAMIDE,ROJO,p,5.44f,-1.33f,0,1.82f,1.56f,1.0f);
    Dibujar(PIRAMIDE,CAFE,p,4.64f,-1.33f,0.3f,1.82f,1.56f,1.0f,180);
    Dibujar(PIRAMIDE,MAGENTA,p,4.64f,0.23f,0,1.82f,1.56f,1.0f);
}

int main()
{
    mainWindow=Window(1200,700);
    if(mainWindow.Initialise()!=0) return 1;
    CrearPiramide(); CrearCubo(); CrearPiramideEsquina(); CrearShaders();
    glm::mat4 projection=glm::ortho(-8.0f,8.0f,-4.5f,4.5f,-20.0f,20.0f);
    while(!mainWindow.getShouldClose()) {
        glfwPollEvents();
        glClearColor(0.83f,0.86f,0.91f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        DibujarIniciales(projection);
        DibujarTresFiguras(projection);
        glUseProgram(0); mainWindow.swapBuffers();
    }
    return 0;
}
