//práctica 3: Modelado Geométrico y Cámara Sintética.
//Se agregan piezas auxiliares para repetir un mismo diseño sobre las seis caras.

//La figura se arma por instancias, de modo que una misma malla pueda reutilizarse varias veces.

#include <stdio.h>
#include <string.h>
#include<cmath>
#include<vector>
#include <glew.h>
#include <glfw3.h>
//glm
#include<glm.hpp>
#include<gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include <gtc\random.hpp>
//clases para dar orden y limpieza al c�digo
#include"Mesh.h"
#include"Shader.h"
#include"Sphere.h"
#include"Window.h"
#include"Camera.h"
//tecla E: Rotar sobre el eje X
//tecla R: Rotar sobre el eje Y
//tecla T: Rotar sobre el eje Z


using std::vector;

//Dimensiones de la ventana
const float toRadians = 3.14159265f / 180.0; //grados a radianes
const float PI = 3.14159265f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;
Camera camera;
Window mainWindow;
vector<Mesh*> meshList;
vector<Shader>shaderList;
//Vertex Shader
static const char* vShader = "shaders/shader.vert";
static const char* fShader = "shaders/shader.frag";
static const char* vShaderColor = "shaders/shadercolor.vert";
Sphere sp = Sphere(1.0, 20, 20); //recibe radio, slices, stacks


//Indices de las mallas utilizadas por la parte agregada.
const int IDX_CUBO = 0;
const int IDX_PIRAMIDE_TRI = 1;
const int IDX_CILINDRO = 2;
const int IDX_CONO = 3;
const int IDX_PIRAMIDE_CUAD = 4;
const int IDX_ESQUINA_COLOR = 5;

//Colores de las piezas del Holocron.
const glm::vec3 COL_AZUL(0.0f, 0.0f, 1.0f);
const glm::vec3 COL_AMARILLO(1.0f, 1.0f, 0.0f);
const glm::vec3 COL_ROJO(1.0f, 0.0f, 0.0f);
const glm::vec3 COL_VERDE(0.0f, 1.0f, 0.0f);
const glm::vec3 COL_MORADO(0.50f, 0.0f, 1.0f);
const glm::vec3 COL_OLIVA(0.50f, 0.50f, 0.0f);
const glm::vec3 COL_BORDE(0.15f, 0.15f, 0.15f);

//Dimensiones que controlan la forma general.
const float TAM_FIGURA = 3.6f;
const float SEMILADO_FIGURA = TAM_FIGURA / 2.0f;
const float POSICION_BORDE = 1.55f;
const float GROSOR_BORDE = 0.45f;
const float LIMITE_AZUL = POSICION_BORDE - GROSOR_BORDE / 2.0f;

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	GLfloat cubo_vertices[] = {
		// front
		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f
	};
	Mesh* cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 24, 36);
	meshList.push_back(cubo);
}

// Pir�mide triangular regular
void CrearPiramideTriangular()
{
	unsigned int indices_piramide_triangular[] = {
			0,1,2,
			1,3,2,
			3,0,2,
			1,0,3

	};
	GLfloat vertices_piramide_triangular[] = {
		-0.5f, -0.5f,0.0f,	//0
		0.5f,-0.5f,0.0f,	//1
		0.0f,0.5f, -0.25f,	//2
		0.0f,-0.5f,-0.5f,	//3

	};
	Mesh* piramidet = new Mesh();
	piramidet->CreateMesh(vertices_piramide_triangular, indices_piramide_triangular, 12, 12);
	meshList.push_back(piramidet);

}


//funci�n para crear pir�mide cuadrangular unitaria
void CrearPiramideCuadrangular()
{
	unsigned int piramidecuadrangular_indices[] = {
		0,3,4,//frontal
		3,2,4,//izquierda
		2,1,4,//trasera
		1,0,4,//derecha
		0,1,2,//abajo1
		0,2,3//abajo2

	};
	GLfloat piramidecuadrangular_vertices[] = {
		0.5f,-0.5f,0.5f,
		0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,
		0.0f,0.5f,0.0f,
	};
	Mesh* piramidec = new Mesh();
	piramidec->CreateMesh(piramidecuadrangular_vertices, piramidecuadrangular_indices, 15, 18);
	meshList.push_back(piramidec);
}

//Pieza plana usada para completar una esquina de cada cara.
//Se modela una sola vez y luego se gira para obtener las otras tres orientaciones.
void CrearPiezaEsquinaColor()
{
	float limite = LIMITE_AZUL / SEMILADO_FIGURA;

	unsigned int indicesPieza[] = {
		2,3,4, 2,4,3,
		2,4,0, 2,0,4,
		2,0,1, 2,1,0,

		7,8,9, 7,9,8,
		7,9,5, 7,5,9,
		7,5,6, 7,6,5,

		0,1,6, 0,6,1, 0,6,5, 0,5,6,
		1,2,7, 1,7,2, 1,7,6, 1,6,7,
		2,3,8, 2,8,3, 2,8,7, 2,7,8,
		3,4,9, 3,9,4, 3,9,8, 3,8,9,
		4,0,5, 4,5,0, 4,5,9, 4,9,5
	};

	GLfloat verticesPieza[] = {
		limite, 0.0f,  0.5f,
		1.0f,   0.0f,  0.5f,
		1.0f,   1.0f,  0.5f,
		0.0f,   1.0f,  0.5f,
		0.0f,   limite, 0.5f,

		limite, 0.0f, -0.5f,
		1.0f,   0.0f, -0.5f,
		1.0f,   1.0f, -0.5f,
		0.0f,   1.0f, -0.5f,
		0.0f,   limite, -0.5f
	};

	Mesh* piezaEsquina = new Mesh();
	piezaEsquina->CreateMesh(verticesPieza, indicesPieza, 30, 96);
	meshList.push_back(piezaEsquina);
}

/*
Crear cilindro, cono y esferas con arreglos din�micos vector creados en el Semestre 2023 - 1 : por S�nchez P�rez Omar Alejandro
*/
void CrearCilindro(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//c�lculo del paso interno en la circunferencia y variables que almacenar�n cada coordenada de cada v�rtice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//ciclo for para crear los v�rtices de las paredes del cilindro
	for (n = 0; n <= (res); n++) {
		if (n != res) {
			x = R * cos((n)*dt);
			z = R * sin((n)*dt);
		}
		//caso para terminar el c�rculo
		else {
			x = R * cos((0) * dt);
			z = R * sin((0) * dt);
		}
		for (i = 0; i < 6; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			case 3:
				vertices.push_back(x);
				break;
			case 4:
				vertices.push_back(0.5);
				break;
			case 5:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia inferior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(-0.5f);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//ciclo for para crear la circunferencia superior
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(0.5);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}

	//Se generan los indices de los v�rtices
	for (i = 0; i < vertices.size(); i++) indices.push_back(i);

	//se genera el mesh del cilindro
	Mesh* cilindro = new Mesh();
	cilindro->CreateMeshGeometry(vertices, indices, vertices.size(), indices.size());
	meshList.push_back(cilindro);
}

//funci�n para crear un cono
void CrearCono(int res, float R) {

	//constantes utilizadas en los ciclos for
	int n, i;
	//c�lculo del paso interno en la circunferencia y variables que almacenar�n cada coordenada de cada v�rtice
	GLfloat dt = 2 * PI / res, x, z, y = -0.5f;

	vector<GLfloat> vertices;
	vector<unsigned int> indices;

	//caso inicial para crear el cono
	vertices.push_back(0.0);
	vertices.push_back(0.5);
	vertices.push_back(0.0);

	//ciclo for para crear los v�rtices de la circunferencia del cono
	for (n = 0; n <= (res); n++) {
		x = R * cos((n)*dt);
		z = R * sin((n)*dt);
		for (i = 0; i < 3; i++) {
			switch (i) {
			case 0:
				vertices.push_back(x);
				break;
			case 1:
				vertices.push_back(y);
				break;
			case 2:
				vertices.push_back(z);
				break;
			}
		}
	}
	vertices.push_back(R * cos(0) * dt);
	vertices.push_back(-0.5);
	vertices.push_back(R * sin(0) * dt);


	for (i = 0; i < res + 2; i++) indices.push_back(i);

	//se genera el mesh del cono
	Mesh* cono = new Mesh();
	cono->CreateMeshGeometry(vertices, indices, vertices.size(), res + 2);
	meshList.push_back(cono);
}

// ============================================================================
// Organizacion de las piezas agregadas al Holocron.
// ============================================================================

struct ElementoHolocron
{
	int malla;
	glm::mat4 matriz;
	glm::vec3 color;
};

vector<ElementoHolocron> elementosHolocron;

//Guarda una pieza ya transformada para dibujarla despues en el ciclo principal.
void RegistrarElemento(int indiceMalla, const glm::mat4& matrizFinal, const glm::vec3& color)
{
	ElementoHolocron elemento;
	elemento.malla = indiceMalla;
	elemento.matriz = matrizFinal;
	elemento.color = color;
	elementosHolocron.push_back(elemento);
}

//Construye la matriz local de una pieza dentro de una cara.
glm::mat4 CrearMatrizPieza(glm::vec3 posicion, glm::vec3 escala, float giroX, float giroZ)
{
	glm::mat4 m(1.0f);
	m = glm::translate(m, posicion);
	m = glm::rotate(m, glm::radians(giroZ), glm::vec3(0.0f, 0.0f, 1.0f));
	m = glm::rotate(m, glm::radians(giroX), glm::vec3(1.0f, 0.0f, 0.0f));
	m = glm::scale(m, escala);
	return m;
}

//Agrega una instancia relativa a la orientacion de una cara.
void PonerEnCara(
	const glm::mat4& cara,
	int indiceMalla,
	glm::vec3 posicion,
	glm::vec3 escala,
	float giroZ,
	glm::vec3 color,
	float giroX = 0.0f)
{
	glm::mat4 local = CrearMatrizPieza(posicion, escala, giroX, giroZ);
	RegistrarElemento(indiceMalla, cara * local, color);
}

//Orienta el sistema local para trabajar sobre una de las seis caras.
glm::mat4 OrientarCara(glm::vec3 normal, float giroY, float giroX)
{
	glm::mat4 cara(1.0f);
	cara = glm::translate(cara, normal * SEMILADO_FIGURA);
	cara = glm::rotate(cara, glm::radians(giroY), glm::vec3(0.0f, 1.0f, 0.0f));
	cara = glm::rotate(cara, glm::radians(giroX), glm::vec3(1.0f, 0.0f, 0.0f));
	return cara;
}

//Marco rectangular de una cara.
void AgregarMarcoCara(const glm::mat4& cara)
{
	const float salida = 0.05f;

	PonerEnCara(cara, IDX_CUBO,
		glm::vec3(0.0f, POSICION_BORDE, salida),
		glm::vec3(TAM_FIGURA, GROSOR_BORDE, 0.30f),
		0.0f, COL_BORDE);

	PonerEnCara(cara, IDX_CUBO,
		glm::vec3(0.0f, -POSICION_BORDE, salida),
		glm::vec3(TAM_FIGURA, GROSOR_BORDE, 0.30f),
		0.0f, COL_BORDE);

	PonerEnCara(cara, IDX_CUBO,
		glm::vec3(-POSICION_BORDE, 0.0f, salida),
		glm::vec3(GROSOR_BORDE, TAM_FIGURA, 0.30f),
		0.0f, COL_BORDE);

	PonerEnCara(cara, IDX_CUBO,
		glm::vec3(POSICION_BORDE, 0.0f, salida),
		glm::vec3(GROSOR_BORDE, TAM_FIGURA, 0.30f),
		0.0f, COL_BORDE);
}

//Las cuatro esquinas usan la misma malla y cambian solo giro y color.
void AgregarColoresEsquina(const glm::mat4& cara)
{
	const float salida = 0.12f;
	const float espesor = 0.20f;

	const float giros[4] = { 0.0f, 90.0f, 180.0f, 270.0f };
	const glm::vec3 colores[4] = {
		COL_ROJO,
		COL_AMARILLO,
		COL_MORADO,
		COL_VERDE
	};

	for (int i = 0; i < 4; i++)
	{
		PonerEnCara(
			cara,
			IDX_ESQUINA_COLOR,
			glm::vec3(0.0f, 0.0f, salida),
			glm::vec3(SEMILADO_FIGURA, SEMILADO_FIGURA, espesor),
			giros[i],
			colores[i]);
	}
}

//Aproxima el circulo central mediante pequeñas barras del mismo cubo.
void AgregarAroCara(const glm::mat4& cara, float radio, float salida, int segmentos)
{
	float paso = 360.0f / segmentos;

	for (int i = 0; i < segmentos; i++)
	{
		float angulo = i * paso + paso / 2.0f;
		float radianes = glm::radians(angulo);

		float x = radio * cos(radianes);
		float y = radio * sin(radianes);
		float longitud = 2.0f * radio * sin(glm::radians(paso / 2.0f)) * 1.25f;

		PonerEnCara(
			cara,
			IDX_CUBO,
			glm::vec3(x, y, salida),
			glm::vec3(0.10f, longitud, 0.16f),
			angulo,
			COL_BORDE);
	}
}

//Pequeña pieza colocada en el centro del panel.
void AgregarCentroCara(const glm::mat4& cara)
{
	PonerEnCara(
		cara,
		IDX_CUBO,
		glm::vec3(0.0f, 0.0f, 0.34f),
		glm::vec3(0.42f, 0.42f, 0.22f),
		45.0f,
		COL_OLIVA);
}

//Reune todas las partes visibles de una cara.
void CompletarCara(const glm::mat4& cara)
{
	AgregarColoresEsquina(cara);
	AgregarMarcoCara(cara);
	AgregarAroCara(cara, 0.42f, 0.28f, 16);
	AgregarCentroCara(cara);
}

//Prepara el cuerpo y aplica el mismo patron a las seis orientaciones.
void PrepararHolocron()
{
	RegistrarElemento(
		IDX_CUBO,
		glm::scale(glm::mat4(1.0f), glm::vec3(TAM_FIGURA, TAM_FIGURA, TAM_FIGURA)),
		COL_AZUL);

	glm::mat4 caras[6] = {
		OrientarCara(glm::vec3(0.0f, 0.0f, 1.0f),   0.0f,   0.0f),
		OrientarCara(glm::vec3(0.0f, 0.0f, -1.0f), 180.0f,  0.0f),
		OrientarCara(glm::vec3(1.0f, 0.0f, 0.0f),  90.0f,   0.0f),
		OrientarCara(glm::vec3(-1.0f, 0.0f, 0.0f), -90.0f,  0.0f),
		OrientarCara(glm::vec3(0.0f, 1.0f, 0.0f),   0.0f, -90.0f),
		OrientarCara(glm::vec3(0.0f, -1.0f, 0.0f),  0.0f,  90.0f)
	};

	for (int i = 0; i < 6; i++)
	{
		CompletarCara(caras[i]);
	}
}

// ============================================================================

void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);

	Shader* shader2 = new Shader();
	shader2->CreateFromFiles(vShaderColor, fShader);
	shaderList.push_back(*shader2);
}

int main()
{
	mainWindow = Window(800, 600);
	mainWindow.Initialise();
	//Cilindro y cono reciben resoluci�n (slices, rebanadas) y Radio de circunferencia de la base y tapa

	CrearCubo();//�ndice 0 en MeshList
	CrearPiramideTriangular();//�ndice 1 en MeshList
	CrearCilindro(10, 1.0f);//�ndice 2 en MeshList
	CrearCono(25, 2.0f);//�ndice 3 en MeshList
	CrearPiramideCuadrangular();//�ndice 4 en MeshList
	CrearPiezaEsquinaColor();//pieza agregada para las esquinas
	CreateShaders();

	//Se preparan una sola vez las piezas que componen la figura.
	PrepararHolocron();

	/*C�mara se usa el comando: glm::lookAt(vector de posici�n, vector de orientaci�n, vector up));
	En la clase Camera se reciben 5 datos:
	glm::vec3 vector de posici�n,
	glm::vec3 vector up,
	GlFloat yaw rotaci�n para girar hacia la derecha e izquierda
	GlFloat pitch rotaci�n para inclinar hacia arriba y abajo
	GlFloat velocidad de desplazamiento,
	GlFloat velocidad de vuelta o de giro
	Se usa el Mouse y las teclas WASD y su posici�n inicial est� en 0,0,1 y ve hacia 0,0,-1.
	*/

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);


	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	//glm::mat4 projection = glm::ortho(-1, 1, -1, 1, 1, 10);

	//Loop mientras no se cierra la ventana
	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader

	glm::mat4 model(1.0);//Inicializar matriz de Modelo 4x4

	while (!mainWindow.getShouldClose())
	{

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;
		//Recibir eventos del usuario
		glfwPollEvents();
		//C�mara
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Limpiar la ventana
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Se agrega limpiar el buffer de profundidad
		shaderList[0].useShader();
		uniformModel = shaderList[0].getModelLocation();
		uniformProjection = shaderList[0].getProjectLocation();
		uniformView = shaderList[0].getViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		//La proyeccion y la vista son comunes para toda la figura.
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		//Se recorren las piezas preparadas y se aplica la transformacion general.
		for (size_t i = 0; i < elementosHolocron.size(); i++)
		{
			ElementoHolocron& elemento = elementosHolocron[i];

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -9.5f));

			model = glm::rotate(model, glm::radians(mainWindow.getrotax()), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, glm::radians(mainWindow.getrotay()), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, glm::radians(mainWindow.getrotaz()), glm::vec3(0.0f, 0.0f, 1.0f));

			model = model * elemento.matriz;

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3fv(uniformColor, 1, glm::value_ptr(elemento.color));

			meshList[elemento.malla]->RenderMesh();
		}

		glUseProgram(0);
		mainWindow.swapBuffers();
	}
	return 0;
}
