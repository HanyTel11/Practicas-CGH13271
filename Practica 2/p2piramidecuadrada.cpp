// Practica 2: proyeccion ortogonal y transformaciones geometricas.
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
std::vector<MeshColor*> figuras;
std::vector<Shader> shaders;

static const char* vShaderColor = "shaders/shadercolor.vert";
static const char* fShaderColor = "shaders/shadercolor.frag";

enum Figura { TRI_AMARILLO, TRI_VERDE, TRI_ROJO, CUADRO_CAFE,
              CUADRO_AZUL, TRI_MAGENTA, CUADRO_NEGRO,
              ESQUINA_AMARILLA, ESQUINA_ROJA,
              ESQUINA_VERDE, ESQUINA_MAGENTA };

void CrearTriangulo(float r, float g, float b)
{
    // Vertices copiados del triangulo magenta proporcionado.
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, r, g, b,
         1.0f, -1.0f, 0.0f, r, g, b,
         0.0f,  1.0f, 0.0f, r, g, b
    };
    MeshColor* triangulo = new MeshColor();
    triangulo->CreateMeshColor(vertices, 18);
    figuras.push_back(triangulo);
}

void CrearCuadrado(float r, float g, float b)
{
    // Vertices copiados del cuadrado azul proporcionado.
    GLfloat vertices[] = {
        -0.5f,-0.5f,0.0f, r,g,b,   0.5f,-0.5f,0.0f, r,g,b,
         0.5f, 0.5f,0.0f, r,g,b,  -0.5f,-0.5f,0.0f, r,g,b,
         0.5f, 0.5f,0.0f, r,g,b,  -0.5f, 0.5f,0.0f, r,g,b
    };
    MeshColor* cuadrado = new MeshColor();
    cuadrado->CreateMeshColor(vertices, 36);
    figuras.push_back(cuadrado);
}

// Triangulo recto centrado en el origen. Se usa solamente para las cuatro
// esquinas del cuadrado central; las demas figuras conservan el triangulo base.
void CrearEsquina(float r, float g, float b)
{
    GLfloat vertices[] = {
        -0.5f,  0.5f, 0.0f, r, g, b,
         0.5f,  0.5f, 0.0f, r, g, b,
        -0.5f, -0.5f, 0.0f, r, g, b
    };
    MeshColor* esquina = new MeshColor();
    esquina->CreateMeshColor(vertices, 18);
    figuras.push_back(esquina);
}

void CrearFiguras()
{
    CrearTriangulo(1.0f, 1.0f, 0.0f);       // amarillo
    CrearTriangulo(0.0f, 1.0f, 0.0f);       // verde
    CrearTriangulo(1.0f, 0.0f, 0.0f);       // rojo
    CrearCuadrado(0.478f, 0.255f, 0.067f);  // cafe solicitado
    CrearCuadrado(0.0f, 0.0f, 1.0f);        // azul
    CrearTriangulo(0.55f, 0.0f, 1.0f);      // magenta/violeta
    CrearCuadrado(0.0f, 0.0f, 0.0f);        // base negra
    CrearEsquina(1.0f, 1.0f, 0.0f);         // esquina amarilla
    CrearEsquina(1.0f, 0.0f, 0.0f);         // esquina roja
    CrearEsquina(0.0f, 1.0f, 0.0f);         // esquina verde
    CrearEsquina(0.55f, 0.0f, 1.0f);        // esquina magenta
}

void CrearShaders()
{
    Shader* shader = new Shader();
    shader->CreateFromFiles(vShaderColor, fShaderColor);
    shaders.push_back(*shader);
}

void Dibujar(Figura figura, GLuint uniformModel, float x, float y,
             float sx, float sy, float angulo = 0.0f, float z = 0.0f)
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(x, y, z));
    model = glm::rotate(model, glm::radians(angulo), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(sx, sy, 1.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    figuras[figura]->RenderMeshColor();
}

int main()
{
    mainWindow = Window(1200, 700);
    if (mainWindow.Initialise() != 0) return 1;

    CrearFiguras();
    CrearShaders();

    shaders[0].useShader();
    GLuint uniformModel = shaders[0].getModelLocation();
    GLuint uniformProjection = shaders[0].getProjectLocation();
    glm::mat4 projection = glm::ortho(-8.0f, 8.0f, -4.5f, 4.5f, -10.0f, 10.0f);
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

    while (!mainWindow.getShouldClose())
    {
        glfwPollEvents();
        glClearColor(0.84f, 0.86f, 0.90f, 1.0f); // diferente de blanco/negro/figuras
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaders[0].useShader();

        // Piso negro.
        Dibujar(CUADRO_NEGRO, uniformModel, 0.15f, -2.32f, 14.40f, 0.50f, 0.0f, -0.5f);

        // Figura izquierda: dos postes cafe y tres triangulos invertidos.
        Dibujar(CUADRO_CAFE, uniformModel, -5.69f,  0.00f, 0.28f, 4.18f);
        Dibujar(CUADRO_CAFE, uniformModel, -3.99f,  0.00f, 0.28f, 4.18f);
        Dibujar(TRI_AMARILLO, uniformModel, -4.84f,  0.86f, 0.71f, 0.58f, 180.0f);
        Dibujar(TRI_ROJO,     uniformModel, -4.84f, -0.29f, 0.71f, 0.58f, 180.0f);
        Dibujar(TRI_VERDE,    uniformModel, -4.84f, -1.50f, 0.71f, 0.67f, 180.0f);

        // Figura central: cuatro esquinas triangulares, sin mezclar colores.
        Dibujar(ESQUINA_AMARILLA, uniformModel, -0.75f,  0.20f, 1.5f, 1.5f,   0.0f);
        Dibujar(ESQUINA_ROJA,     uniformModel,  0.75f,  0.20f, 1.5f, 1.5f, -90.0f);
        Dibujar(ESQUINA_VERDE,    uniformModel,  0.75f, -1.30f, 1.5f, 1.5f, 180.0f);
        Dibujar(ESQUINA_MAGENTA,  uniformModel, -0.75f, -1.30f, 1.5f, 1.5f,  90.0f);
        // Los rombos son cuadrados rotados 45 grados sobre el eje Z.
        Dibujar(CUADRO_AZUL,  uniformModel,  0.0f, -0.55f, 2.12f, 2.12f, 45.0f, 0.2f);
        Dibujar(CUADRO_CAFE,  uniformModel,  0.0f, -0.55f, 1.18f, 1.18f, 45.0f, 0.4f);

        // Figura derecha: piramide formada por cuatro triangulos.
        Dibujar(TRI_VERDE,    uniformModel,  3.84f, -1.33f, 0.91f, 0.78f);
        Dibujar(TRI_ROJO,     uniformModel,  5.44f, -1.33f, 0.91f, 0.78f);
        Dibujar(TRI_AMARILLO, uniformModel,  4.64f, -1.33f, 0.91f, 0.78f, 180.0f, 0.2f);
        Dibujar(TRI_MAGENTA,  uniformModel,  4.64f,  0.23f, 0.91f, 0.78f);

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    for (MeshColor* figura : figuras) delete figura;
    return 0;
}
