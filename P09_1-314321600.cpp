/*
Animación:
Simple o básica:Por banderas y condicionales (más de 1 transformación geométrica se ve modificada)
Compleja: Por medio de funciones y algoritmos.
Textura Animada
*/
//para cargar imagen
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
#include "Circuito.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
bool avanza = true;

//Nave en la pista
float navePos = 0.0f;                 // posición en el circuito
float naveVel = 0.4f;                 // velocidad por ciclo, en unidades de la pista
float naveRoll = 0.0f;                // inclinación lateral en curvas
float alturaNave = 0.3f;             // separación sobre el asfalto
float escalaPista = 3.0f;
float escalaNave = 0.70f;
glm::vec3 posPista(0.0f, -2.0f, -180.0f);
glm::vec3 naveP(0.0f, 0.0f, 0.0f);
glm::vec3 naveT(0.0f, 0.0f, -1.0f);

//Satélite TDRS en órbita
float satAnomalia = 0.0f;                    
float satMovMedio = 0.0020f;
float satExcentricidad = 0.25f;
float satSemiejeA = 80.0f;
float satInclinacion = 20.0f;                // grados
glm::vec3 satCentro(-15.0f, 120.0f, -160.0f);  // centro de la órbita, sobre la pista
float satRotPanel = 0.0f;
float satRotParabIzq = 0.0f;
float satRotParabDer = 0.0f;
float satRotEnlace = 0.0f;
float satVelParab = 3.0f;                    // grados por ciclo
float satVelEnlace = 3.0f;                  // grados por ciclo
glm::mat4 satMatriz(1.0f);


Window mainWindow;
std::vector<MeshModel*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;

//MODELOS

//NAVE Y PISTA
Model Nave_M;
Model Pista_M;

//SATELITE
Model tdrsCuerpo_M;
Model tdrsPanelIzq_M;
Model tdrsPanelDer_M;
Model tdrsParabIzq_M;
Model tdrsParabDer_M;
Model tdrsEnlace_M;

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
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

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
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	MeshModel* obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	MeshModel* obj5 = new MeshModel();
	obj5->CreateMeshModel(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	MeshModel* obj6 = new MeshModel();
	obj6->CreateMeshModel(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6); // todos los números

	MeshModel* obj7 = new MeshModel();
	obj7->CreateMeshModel(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7); // solo un número

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

//punto del circuito (índice i, circular)
glm::vec3 pc(int i)
{
	int n = NUM_PUNTOS_CIRCUITO;
	int k = ((i % n) + n) % n;
	return glm::vec3(circuito[k][0], circuito[k][1], circuito[k][2]);
}

//posición y derivada (dirección) sobre el circuito 
void puntoCircuito(float u, glm::vec3& pos, glm::vec3& der)
{
	int i1 = (int)floor(u);
	float t = u - floor(u);
	glm::vec3 p0 = pc(i1 - 1), p1 = pc(i1), p2 = pc(i1 + 1), p3 = pc(i1 + 2);
	pos = 0.5f * ((2.0f * p1) + (-p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
	der = 0.5f * ((-p0 + p2) +
		2.0f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t +
		3.0f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t);
}


int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	

	// ---- Modelos para Práctica ----

	//NAVE
	Nave_M = Model();
	Nave_M.LoadModel("Models/Nave.obj");

	//PISTA
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");

	//SATELITE
	tdrsCuerpo_M = Model();
	tdrsCuerpo_M.LoadModel("Models/tdrsCuerpo.obj");

	tdrsPanelIzq_M = Model();
	tdrsPanelIzq_M.LoadModel("Models/tdrsPanelIzq.obj");

	tdrsPanelDer_M = Model();
	tdrsPanelDer_M.LoadModel("Models/tdrsPanelDer.obj");

	tdrsParabIzq_M = Model();
	tdrsParabIzq_M.LoadModel("Models/tdrsParabIzq.obj");

	tdrsParabDer_M = Model();
	tdrsParabDer_M.LoadModel("Models/tdrsParabDer.obj");

	tdrsEnlace_M = Model();
	tdrsEnlace_M.LoadModel("Models/tdrsEnlace.obj");


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

	glm::vec3 lowerLight(0.0f,0.0f,0.0f);
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime; //Estandarizar el tiempo entre diferentes hardware
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Nave: recorre el circuito a velocidad constante
		glm::vec3 derNave;
		puntoCircuito(navePos, naveP, derNave);
		navePos += (naveVel * deltaTime) / glm::length(derNave);
		if (navePos >= NUM_PUNTOS_CIRCUITO) navePos -= NUM_PUNTOS_CIRCUITO;
		puntoCircuito(navePos, naveP, naveT);
		naveT = glm::normalize(naveT);

		float yawNave = atan2(-naveT.x, -naveT.z) / toRadians;
		float pitchNave = asin(naveT.y) / toRadians;

		//inclinación lateral según cuánto gira el rumbo unos puntos adelante
		glm::vec3 posAdelante, derAdelante;
		puntoCircuito(fmod(navePos + 5.0f, (float)NUM_PUNTOS_CIRCUITO), posAdelante, derAdelante);
		float rumboAdelante = atan2(-derAdelante.x, -derAdelante.z) / toRadians;
		float difRumbo = rumboAdelante - yawNave;
		while (difRumbo > 180.0f) difRumbo -= 360.0f;
		while (difRumbo < -180.0f) difRumbo += 360.0f;
		float rollObjetivo = glm::clamp(difRumbo, -35.0f, 35.0f);
		naveRoll += (rollObjetivo - naveRoll) * 0.05f * deltaTime;

		//Satélite: órbita elíptica 
		satAnomalia += satMovMedio * deltaTime;
		if (satAnomalia > 2.0f * 3.14159265f) satAnomalia -= 2.0f * 3.14159265f;

		float satE = satAnomalia;                         
		for (int k = 0; k < 6; k++)
			satE -= (satE - satExcentricidad * sin(satE) - satAnomalia) / (1.0f - satExcentricidad * cos(satE));

		float satB = satSemiejeA * sqrt(1.0f - satExcentricidad * satExcentricidad);
		glm::vec3 ejeA(1.0f, 0.0f, 0.0f);
		glm::vec3 ejeB(0.0f, sin(satInclinacion * toRadians), cos(satInclinacion * toRadians));
		glm::vec3 normalOrbita = glm::normalize(glm::cross(ejeA, ejeB));

		glm::vec3 satPos = satCentro + ejeA * (satSemiejeA * cos(satE)) + ejeB * (satB * sin(satE));

		//orientación: las antenas apuntan al centro de la órbita, los paneles van sobre la normal de la órbita
		glm::vec3 satY = glm::normalize(satCentro - satPos);
		glm::vec3 satX = normalOrbita;
		glm::vec3 satZ = glm::normalize(glm::cross(satX, satY));
		satMatriz = glm::mat4(glm::vec4(satX, 0.0f), glm::vec4(satY, 0.0f), glm::vec4(satZ, 0.0f), glm::vec4(satPos, 1.0f));

		//paneles: giran al revés que el cuerpo para seguir apuntando al sol
		satRotPanel = -atan2(satB * sin(satE), satSemiejeA * cos(satE)) / toRadians;

		//parabólicas y antena de enlace: giro sobre su propio eje
		satRotParabIzq += satVelParab * deltaTime;
		satRotParabDer -= satVelParab * deltaTime;
		satRotEnlace += satVelEnlace * deltaTime;
		if (satRotParabIzq > 360.0f) satRotParabIzq -= 360.0f;
		if (satRotParabDer < -360.0f) satRotParabDer += 360.0f;
		if (satRotEnlace > 360.0f) satRotEnlace -= 360.0f;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		//Clear the window
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
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//Reinicializando variables cada ciclo de reloj
		model= glm::mat4(1.0);
		modelaux= glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMeshModel();
		
		//PISTA
		color = glm::vec3(0.35f, 0.35f, 0.40f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glm::mat4 modelPista(1.0);
		modelPista = glm::translate(modelPista, posPista);
		modelPista = glm::scale(modelPista, glm::vec3(escalaPista));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelPista));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Pista_M.RenderModel();

		//NAVE
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = modelPista;
		model = glm::translate(model, naveP + glm::vec3(0.0f, alturaNave, 0.0f));
		model = glm::rotate(model, glm::radians(yawNave), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(pitchNave), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(naveRoll), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(escalaNave));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Nave_M.RenderModel();

		//SATÉLITE TDRS

		//MATRIZ BASE
		model = satMatriz;
		glm::mat4 modelSatelite = model;

		//CUERPO CENTRAL
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsCuerpo_M.RenderModel();

		//PANELES SOLARES
		model = modelSatelite;
		model = glm::rotate(model, glm::radians(satRotPanel), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsPanelIzq_M.RenderModel();

		model = modelSatelite;
		model = glm::rotate(model, glm::radians(satRotPanel), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsPanelDer_M.RenderModel();

		//ANTENAS PARABÓLICAS

		//PARABÓLICA IZQUIERDA
		model = modelSatelite;
		model = glm::translate(model, glm::vec3(0.0f, 1.3f, -3.2f));
		model = glm::rotate(model, glm::radians(satRotParabIzq), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsParabIzq_M.RenderModel();

		//PARABÓLICA DERECHA
		model = modelSatelite;
		model = glm::translate(model, glm::vec3(0.0f, 1.3f, 3.2f));
		model = glm::rotate(model, glm::radians(satRotParabDer), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsParabDer_M.RenderModel();

		//ANTENA DE ENLACE 
		model = modelSatelite;
		model = glm::translate(model, glm::vec3(-0.9f, 1.2f, -0.5f));
		model = glm::rotate(model, glm::radians(satRotEnlace), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsEnlace_M.RenderModel();

		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
