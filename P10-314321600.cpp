/* 
Animación por keyframes
La textura del skybox fue conseguida desde la página https ://opengameart.org/content/elyvisions-skyboxes?page=1
y edité en Gimp rotando 90 grados en sentido antihorario la imagen  sp2_up.png para poder ver continuidad.
Fuentes :
	https ://www.khronos.org/opengl/wiki/Keyframe_Animation
	http ://what-when-how.com/wp-content/uploads/2012/07/tmpcd0074_thumb.png
*/
//para cargar imagen
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture plainTexture;
Texture pisoTexture;

Model Pieza_M[4]; //0=etapa1, 1=etapa2, 2=etapa3, 3=capsula

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función para teclado de keyframes 
void inputKeyframes(bool* keys);

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	MeshModel* obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////
#define MAX_FRAMES 100
#define NUM_PIEZAS 4
int i_max_steps = 450;
int i_curr_steps = 0;

float posXcohete = 0.0f, posYcohete = 0.6f, posZcohete = 0.0f;
//altura del centro de cada pieza (para que gire sobre si misma)
float pivoteY[NUM_PIEZAS] = { -1.47f, 0.33f, 1.41f, 2.19f };
//0 = etapa 1, 1 = etapa 2, 2 = etapa 3, 3 = capsula
float movX[NUM_PIEZAS] = { 0.0f }, movY[NUM_PIEZAS] = { 0.0f }, giro[NUM_PIEZAS] = { 0.0f };
float orbita = 0.0f; //giro de la capsula alrededor del eje Y

int seleccion = -1;    //-1 = cohete unido, 0..3 = una sola pieza
int piezasSueltas = 0; //etapas que ya se separaron
const char* nombrePieza[NUM_PIEZAS] = { "ETAPA 1", "ETAPA 2", "ETAPA 3", "CAPSULA" };

typedef struct _frame
{
	float movX[NUM_PIEZAS], movY[NUM_PIEZAS], giro[NUM_PIEZAS];
	float movXInc[NUM_PIEZAS], movYInc[NUM_PIEZAS], giroInc[NUM_PIEZAS];
	float orbita, orbitaInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;
bool play = false;
int playIndex = 0;

void imprimirEstado(void)
{
	for (int i = 0; i < NUM_PIEZAS; i++)
		printf("   %-8s x=%6.2f  y=%6.2f  giro=%7.2f\n", nombrePieza[i], movX[i], movY[i], giro[i]);
	printf("   orbita capsula = %.2f\n", orbita);
}

void saveFrame(void) //tecla L
{
	if (FrameIndex >= MAX_FRAMES) { printf("Ya no caben mas frames\n"); return; }
	for (int i = 0; i < NUM_PIEZAS; i++)
	{
		KeyFrame[FrameIndex].movX[i] = movX[i];
		KeyFrame[FrameIndex].movY[i] = movY[i];
		KeyFrame[FrameIndex].giro[i] = giro[i];
	}
	KeyFrame[FrameIndex].orbita = orbita;
	printf("FRAME %d GUARDADO:\n", FrameIndex);
	imprimirEstado();
	FrameIndex++;
}

void guardarArchivo(void) //tecla G: escribe/sobreescribe
{
	FILE* f = fopen("keyframescohete_fases.txt", "w");
	if (f == NULL) { printf("No se pudo crear keyframescohete_fases.txt\n"); return; }
	fprintf(f, "%d\n", FrameIndex);
	for (int k = 0; k < FrameIndex; k++)
	{
		for (int i = 0; i < NUM_PIEZAS; i++)
			fprintf(f, "%f %f %f ", KeyFrame[k].movX[i], KeyFrame[k].movY[i], KeyFrame[k].giro[i]);
		fprintf(f, "%f\n", KeyFrame[k].orbita);
	}
	fclose(f);
	printf("Se escribieron %d frames en keyframescohete_fases.txt\n", FrameIndex);
}

void leerArchivo(void) //tecla C
{
	FILE* f = fopen("keyframescohete_fases.txt", "r");
	if (f == NULL) { printf("No existe keyframescohete_fases.txt\n"); return; }
	int n = 0;
	if (fscanf(f, "%d", &n) != 1 || n < 0 || n > MAX_FRAMES) { printf("Archivo invalido\n"); fclose(f); return; }
	for (int k = 0; k < n; k++)
	{
		for (int i = 0; i < NUM_PIEZAS; i++)
			fscanf(f, "%f %f %f", &KeyFrame[k].movX[i], &KeyFrame[k].movY[i], &KeyFrame[k].giro[i]);
		fscanf(f, "%f", &KeyFrame[k].orbita);
		printf("Frame %d leido:", k);
		for (int i = 0; i < NUM_PIEZAS; i++)
			printf(" [%.2f %.2f %.0f]", KeyFrame[k].movX[i], KeyFrame[k].movY[i], KeyFrame[k].giro[i]);
		printf(" orbita %.0f\n", KeyFrame[k].orbita);
	}
	fclose(f);
	FrameIndex = n;
	printf("Se leyeron %d frames de keyframescohete_fases.txt\n", FrameIndex);
}

void resetElements(void)
{
	for (int i = 0; i < NUM_PIEZAS; i++)
	{
		movX[i] = KeyFrame[0].movX[i];
		movY[i] = KeyFrame[0].movY[i];
		giro[i] = KeyFrame[0].giro[i];
	}
	orbita = KeyFrame[0].orbita;
}

void interpolation(void)
{
	FRAME& a = KeyFrame[playIndex];
	FRAME& b = KeyFrame[playIndex + 1];
	for (int i = 0; i < NUM_PIEZAS; i++)
	{
		a.movXInc[i] = (b.movX[i] - a.movX[i]) / i_max_steps;
		a.movYInc[i] = (b.movY[i] - a.movY[i]) / i_max_steps;
		a.giroInc[i] = (b.giro[i] - a.giro[i]) / i_max_steps;
	}
	a.orbitaInc = (b.orbita - a.orbita) / i_max_steps;
}

void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps)
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)
			{
				printf("Termino la animacion\n");
				playIndex = 0;
				play = false;
			}
			else
			{
				printf("Reproduciendo frame %d -> %d\n", playIndex, playIndex + 1);
				i_curr_steps = 0;
				interpolation();
			}
		}
		else
		{
			for (int i = 0; i < NUM_PIEZAS; i++)
			{
				movX[i] += KeyFrame[playIndex].movXInc[i];
				movY[i] += KeyFrame[playIndex].movYInc[i];
				giro[i] += KeyFrame[playIndex].giroInc[i];
			}
			orbita += KeyFrame[playIndex].orbitaInc;
			i_curr_steps++;
		}
	}
}
///////////////* FIN KEYFRAMES*////////////////////////////

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 9.0f, 32.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.5f, 0.5f);

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();

	Pieza_M[0].LoadModel("Models/cohete_etapa1.obj");
	Pieza_M[1].LoadModel("Models/cohete_etapa2.obj");
	Pieza_M[2].LoadModel("Models/cohete_etapa3.obj");
	Pieza_M[3].LoadModel("Models/cohete_capsula.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	printf("\nTECLAS KEYFRAMES:\nT: cambiar seleccion (cohete unido / etapa 1 / etapa 2 / etapa 3 / capsula)\nP: separar la siguiente etapa\n1/2: x +/-0.5  |  3/4: y +/-0.25  |  7/8: y +/-1  |  5/6: giro +/-15  |  9/0: orbita +/-15\nL: guardar frame  |  ESPACIO: reproducir\nG: escribir archivo  |  C: leer archivo  |  N: borrar todo\n");

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Para Keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[0]->RenderMeshModel();

		for (int i = 0; i < NUM_PIEZAS; i++)
		{
			model = glm::mat4(1.0);
			if (i == 3) //solo la capsula orbita alrededor del eje Y
				model = glm::rotate(model, orbita * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::translate(model, glm::vec3(posXcohete + movX[i], posYcohete + pivoteY[i] + movY[i], posZcohete));
			model = glm::rotate(model, giro[i] * toRadians, glm::vec3(0.0f, 0.0f, -1.0f));
			model = glm::translate(model, glm::vec3(0.0f, -pivoteY[i], 0.0f));
			model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
			model = glm::translate(model, glm::vec3(-0.14f, -10.1f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			Pieza_M[i].RenderModel();
		}

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}


bool teclaAnterior[1024] = { false };
bool presionada(bool* keys, int tecla)
{
	bool r = keys[tecla] && !teclaAnterior[tecla];
	teclaAnterior[tecla] = keys[tecla];
	return r;
}

void moverSeleccion(float dx, float dy, float dg)
{
	if (seleccion == -1) //cohete unido: se mueven todas las piezas que no se han separado
	{
		for (int i = piezasSueltas; i < NUM_PIEZAS; i++) { movX[i] += dx; movY[i] += dy; }
		if (dg != 0) printf("El cohete unido no gira, selecciona una pieza con T\n");
	}
	else
	{
		movX[seleccion] += dx; movY[seleccion] += dy; giro[seleccion] += dg;
	}
}

void inputKeyframes(bool* keys)
{
	bool kSpace = presionada(keys, GLFW_KEY_SPACE);
	bool kL = presionada(keys, GLFW_KEY_L);
	bool kG = presionada(keys, GLFW_KEY_G);
	bool kC = presionada(keys, GLFW_KEY_C);
	bool kN = presionada(keys, GLFW_KEY_N);
	bool kT = presionada(keys, GLFW_KEY_T);
	bool kP = presionada(keys, GLFW_KEY_P);
	bool k1 = presionada(keys, GLFW_KEY_1), k2 = presionada(keys, GLFW_KEY_2);
	bool k3 = presionada(keys, GLFW_KEY_3), k4 = presionada(keys, GLFW_KEY_4);
	bool k5 = presionada(keys, GLFW_KEY_5), k6 = presionada(keys, GLFW_KEY_6);
	bool k7 = presionada(keys, GLFW_KEY_7), k8 = presionada(keys, GLFW_KEY_8);
	bool k9 = presionada(keys, GLFW_KEY_9), k0 = presionada(keys, GLFW_KEY_0);

	if (kSpace && !play && FrameIndex > 1)
	{
		resetElements();
		playIndex = 0;
		i_curr_steps = 0;
		interpolation();
		play = true;
		printf("Reproduciendo animacion...\nReproduciendo frame 0 -> 1\n");
	}
	if (play) return;   //mientras reproduce no se edita

	if (kL) saveFrame();
	if (kG) guardarArchivo();
	if (kC) { leerArchivo(); resetElements(); }
	if (kN)
	{
		FrameIndex = 0; orbita = 0; piezasSueltas = 0; seleccion = -1;
		for (int i = 0; i < NUM_PIEZAS; i++) { movX[i] = 0; movY[i] = 0; giro[i] = 0; }
		printf("Frames borrados (nueva animacion)\n");
	}
	if (kP)
	{
		if (piezasSueltas < 3) { piezasSueltas++; printf("Se separo la %s\n", nombrePieza[piezasSueltas - 1]); }
		else printf("Ya no hay etapas por separar\n");
	}
	if (kT)
	{
		seleccion++;
		if (seleccion >= NUM_PIEZAS) seleccion = -1;
		if (seleccion == -1) printf("Seleccion: COHETE UNIDO\n");
		else printf("Seleccion: %s\n", nombrePieza[seleccion]);
	}

	bool cambio = true;
	if (k1) moverSeleccion(0.5f, 0, 0);
	else if (k2) moverSeleccion(-0.5f, 0, 0);
	else if (k3) moverSeleccion(0, 0.25f, 0);
	else if (k4) moverSeleccion(0, -0.25f, 0);
	else if (k7) moverSeleccion(0, 1.0f, 0);
	else if (k8) moverSeleccion(0, -1.0f, 0);
	else if (k5) moverSeleccion(0, 0, 15.0f);
	else if (k6) moverSeleccion(0, 0, -15.0f);
	else if (k9) orbita += 15.0f;
	else if (k0) orbita -= 15.0f;
	else cambio = false;
	if (cambio) imprimirEstado();
}
