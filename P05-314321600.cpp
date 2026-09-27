/*
Práctica 5: Optimización y Carga de Modelos
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
#include "Shader_m.h"
#include "Camera.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

const float toRadians = 3.14159265f / 180.0f;
//float angulocola = 0.0f;
Window mainWindow;
std::vector<Mesh*> meshList; //solo recibe xyz
std::vector<MeshColor*> meshListColor; // recibe xyz rgb
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

//Lista de Modelos a importar
Model Rover_M;

//BRAZO
Model baseBrazo_M;
Model brazoParte1_M;
Model brazoParte2_M;
Model pinza_M;

//LLANTAS
Model llantaDI_M;
Model llantaDD_M;
Model llantaCI_M;
Model llantaCD_M;
Model llantaTI_M;
Model llantaTD_M;

//PATAS DE LAS LLANTAS
Model pataDI_M;
Model pataDD_M;
Model pataCI_M;
Model pataCD_M;
Model pataTI_M;
Model pataTD_M;

//HOLOCRON
Model holoCentro_M;
Model holoEsq1_M;
Model holoEsq2_M; 
Model holoEsq3_M; 
Model holoEsq4_M;
Model holoEsq5_M; 
Model holoEsq6_M;
Model holoEsq7_M; 
Model holoEsq8_M;

//SATELITE
Model tdrsCuerpo_M;
Model tdrsPanelIzq_M;
Model tdrsPanelDer_M;
Model tdrsParabIzq_M;
Model tdrsParabDer_M;
Model tdrsEnlace_M;


//Lista de Skybox a crear
Skybox skybox;


GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Vertex Shader
static const char* vShader = "shaders/shader_m.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_m.frag";


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

	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);


}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.5f, 7.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.3f);

	//Cargar modelos
	Rover_M = Model();
	Rover_M.LoadModel("Models/RoverCuerpo.obj");

	//BRAZO
	baseBrazo_M = Model();
	baseBrazo_M.LoadModel("Models/BaseBrazo.obj");

	brazoParte1_M = Model();
	brazoParte1_M.LoadModel("Models/BrazoParte1.obj");

	brazoParte2_M = Model();
	brazoParte2_M.LoadModel("Models/BrazoParte2.obj");

	pinza_M = Model();
	pinza_M.LoadModel("Models/Pinza.obj");

	//LLANTAS
	llantaDI_M = Model();
	llantaDI_M.LoadModel("Models/LlantaDelanteraIzquierda.obj");

	llantaDD_M = Model();
	llantaDD_M.LoadModel("Models/LlantaDelanteraDerecha.obj");

	llantaCI_M = Model();
	llantaCI_M.LoadModel("Models/LlantaCentralIzquierda.obj");

	llantaCD_M = Model();
	llantaCD_M.LoadModel("Models/LlantaCentralDerecha.obj");

	llantaTI_M = Model();
	llantaTI_M.LoadModel("Models/LlantaTraseraIzquierda.obj");

	llantaTD_M = Model();
	llantaTD_M.LoadModel("Models/LlantaTraseraDerecha.obj");

	//PATAS DE LAS LLANTAS
	pataDI_M = Model();
	pataDI_M.LoadModel("Models/PLDI.obj");

	pataDD_M = Model();
	pataDD_M.LoadModel("Models/PLDD.obj");

	pataCI_M = Model();
	pataCI_M.LoadModel("Models/PLCI.obj");

	pataCD_M = Model();
	pataCD_M.LoadModel("Models/PLCD.obj");

	pataTI_M = Model();
	pataTI_M.LoadModel("Models/PLTI.obj");

	pataTD_M = Model();
	pataTD_M.LoadModel("Models/PLTD.obj");

	//HOLOCRON
	holoCentro_M = Model();
	holoCentro_M.LoadModel("Models/HoloCentro.obj");

	holoEsq1_M = Model();
	holoEsq1_M.LoadModel("Models/HoloEsq1.obj");

	holoEsq2_M = Model();
	holoEsq2_M.LoadModel("Models/HoloEsq2.obj");

	holoEsq3_M = Model();
	holoEsq3_M.LoadModel("Models/HoloEsq3.obj");

	holoEsq4_M = Model();
	holoEsq4_M.LoadModel("Models/HoloEsq4.obj");

	holoEsq5_M = Model();
	holoEsq5_M.LoadModel("Models/HoloEsq5.obj");

	holoEsq6_M = Model();
	holoEsq6_M.LoadModel("Models/HoloEsq6.obj");

	holoEsq7_M = Model();
	holoEsq7_M.LoadModel("Models/HoloEsq7.obj");

	holoEsq8_M = Model();
	holoEsq8_M.LoadModel("Models/HoloEsq8.obj");

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

	//Crear Skybox con sus 6 texturas
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0, uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

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

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//Se dibuja el Skybox
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformColor = shaderList[0].getColorLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

		// INICIA DIBUJO DEL PISO
		color = glm::vec3(0.5f, 0.5f, 0.5f); //piso de color gris
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		meshListModel[2]->RenderMeshModel();

		//------------*INICIA DIBUJO DE NUESTROS DEMÁS OBJETOS-------------------*
		//Modelo Inicial
		color = glm::vec3(0.0f, 0.0f, 1.0f); //modelo de color azul
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, -1.5f));
		glm::mat4 modelChasis = model;

		//modelaux = model;
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Rover_M.RenderModel();//modificar por el modelo de solo cuerpo del Rover, para que se pueda separar el brazo y las llantas
		color = glm::vec3(0.0f, 0.0f, 1.0f);
		//En sesión se separara una parte del modelo y se unirá por jeraquía al cuerpo
		
		/* Ejercicio:
		1.- Separar las llantas
		2.- Hacer que al presionar una tecla cada pata de rueda pueda rotar un máximo de 45° "hacia adelante y hacia atrás"
		*/

		//ROVER

		//BRAZO
		modelaux = model;

		//BASE DEL BRAZO
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(0.0f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		baseBrazo_M.RenderModel();

		//PRIMERA PARTE DEL BRAZO
		model = modelaux;
		model = glm::translate(model, glm::vec3(.0f, 0.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion2()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoParte1_M.RenderModel();

		//SEGUNDA PARTE DEL BRAZO
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.7f, 2.5f, 0.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion3()), glm::vec3(0.0f, 0.0f, 1.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoParte2_M.RenderModel();

		//PINZA
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5f, 3.2f, 0.0f));
		model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion4()), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pinza_M.RenderModel();

		//LLANTAS

		//Ángulos limitados solo para las patas 
		float angPataDD = glm::clamp(mainWindow.getarticulacion5(), -45.0f, 45.0f);
		float angPataDI = glm::clamp(mainWindow.getarticulacion6(), -45.0f, 45.0f);
		float angPataCD = glm::clamp(mainWindow.getarticulacion7(), -45.0f, 45.0f);
		float angPataCI = glm::clamp(mainWindow.getarticulacion8(), -45.0f, 45.0f);
		float angPataTD = glm::clamp(mainWindow.getarticulacion9(), -45.0f, 45.0f);
		float angPataTI = glm::clamp(mainWindow.getarticulacion10(), -45.0f, 45.0f);
		
		float giroLlantas = mainWindow.getAvanceLlantas();

		//LLANTA DELANTERA DERECHA
		model = modelChasis;;
		model = glm::translate(model, glm::vec3(-3.5f, 0.0f, -2.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(angPataDD), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 modelPataDD = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pataDD_M.RenderModel();

		model = modelPataDD;
		model = glm::translate(model, glm::vec3(3.7f, -2.7f, -0.5f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaDD_M.RenderModel();

		//LLANTA DELANTERA IZQUIERDA
		model = modelChasis;;
		model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 3.5f));
		model = glm::rotate(model, glm::radians(angPataDI), glm::vec3(0.0f, 0.0f, -1.0f));
		glm::mat4 modelPataDI = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pataDI_M.RenderModel();

		model = modelPataDI;
		model = glm::translate(model, glm::vec3(-3.5f, -2.6f, -1.0f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaDI_M.RenderModel();

		//LLANTA CENTRAL DERECHA
		model = modelChasis;;
		model = glm::translate(model, glm::vec3(1.0f, -1.0f, -2.5f));
		model = glm::rotate(model, glm::radians(angPataCD), glm::vec3(0.0f, 0.0f, -1.0f));
		glm::mat4 modelPataCD = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pataCD_M.RenderModel();

		model = modelPataCD;
		model = glm::translate(model, glm::vec3(-2.5f, -2.0f, -0.5f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaCD_M.RenderModel();

		//LLANTA CENTRAL IZQUIERDA
		model = modelChasis;;
		model = glm::translate(model, glm::vec3(2.5f, -1.0f, 3.0f));
		model = glm::rotate(model, glm::radians(angPataCI), glm::vec3(0.0f, 0.0f, -1.0f));
		glm::mat4 modelPataCI = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pataCI_M.RenderModel();

		model = modelPataCI;
		model = glm::translate(model, glm::vec3(-2.5f, -2.0f, 0.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaCI_M.RenderModel();

		//LLANTA TRASERA DERECHA
		model = modelChasis;;
		model = glm::translate(model, glm::vec3(1.5f, -1.0f, -2.5f));
		model = glm::rotate(model, glm::radians(angPataTD), glm::vec3(0.0f, 0.0f, -1.0f));
		glm::mat4 modelPataTD = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pataTD_M.RenderModel();

		model = modelPataTD;
		model = glm::translate(model, glm::vec3(2.5f, -2.0f, -0.5f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTD_M.RenderModel();

		//LLANTA TRASERA IZQUIERDA
		model = modelChasis;;
		model = glm::translate(model, glm::vec3(2.5, -1.0f, 3.0f));
		model = glm::rotate(model, glm::radians(angPataTI), glm::vec3(0.0f, 0.0f, -1.0f));
		glm::mat4 modelPataTI = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pataTI_M.RenderModel();

		model = modelPataTI;
		model = glm::translate(model, glm::vec3(2.5f, -2.0f, 0.5f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTI_M.RenderModel();

		
		/* Práctica:
		1.- Holocron
		2.- Satélite
		*/

		//HOLOCRON
		
		//Centro del Holocron a un lado del Rover
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-25.0f, 3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glm::mat4 modelHolocron = model; //Matriz padre

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holoCentro_M.RenderModel();

		//LÓGICA DE MOVIMIENTO
		float maxApertura = 3.5f; //Distancia máxima a la que se separarán las esquinas
		float maxGiro = 90.0f;    //Límite de giro en grados

		//ESQUINA 1
		float ang1 = mainWindow.getAngEsquina1();
		float apertura1 = abs(sin(glm::radians(ang1)));
		float dist1 = apertura1 * maxApertura;
		float rot1 = apertura1 * maxGiro; //La pieza gira y regresa sola al cerrarse
		model = modelHolocron;
		model = glm::translate(model, glm::vec3(dist1, dist1, dist1));
		model = glm::rotate(model, glm::radians(rot1), glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holoEsq1_M.RenderModel();

		//ESQUINA 2
		float ang2 = mainWindow.getAngEsquina2();
		float apertura2 = abs(sin(glm::radians(ang2)));
		float dist2 = apertura2 * maxApertura;
		float rot2 = apertura2 * maxGiro;
		model = modelHolocron;
		model = glm::translate(model, glm::vec3(-dist2, dist2, dist2));
		model = glm::rotate(model, glm::radians(rot2), glm::vec3(-1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holoEsq2_M.RenderModel();

		//ESQUINA 3
		float ang3 = mainWindow.getAngEsquina3();
		float apertura3 = abs(sin(glm::radians(ang3)));
		float dist3 = apertura3 * maxApertura;
		float rot3 = apertura3 * maxGiro;
		model = modelHolocron;
		model = glm::translate(model, glm::vec3(dist3, -dist3, dist3));
		model = glm::rotate(model, glm::radians(rot3), glm::vec3(1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holoEsq3_M.RenderModel();

		//ESQUINA 4
		float ang4 = mainWindow.getAngEsquina4();
		float apertura4 = abs(sin(glm::radians(ang4)));
		float dist4 = apertura4 * maxApertura;
		float rot4 = apertura4 * maxGiro;
		model = modelHolocron;
		model = glm::translate(model, glm::vec3(-dist4, -dist4, dist4));
		model = glm::rotate(model, glm::radians(rot4), glm::vec3(-1.0f, -1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holoEsq4_M.RenderModel();

		//ESQUINA 5
		float ang5 = mainWindow.getAngEsquina5();
		float apertura5 = abs(sin(glm::radians(ang5)));
		float dist5 = apertura5 * maxApertura;
		float rot5 = apertura5 * maxGiro;
		model = modelHolocron;
		model = glm::translate(model, glm::vec3(dist5, dist5, -dist5));
		model = glm::rotate(model, glm::radians(rot5), glm::vec3(1.0f, 1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holoEsq5_M.RenderModel();

		//ESQUINA 6
		float ang6 = mainWindow.getAngEsquina6();
		float apertura6 = abs(sin(glm::radians(ang6)));
		float dist6 = apertura6 * maxApertura;
		float rot6 = apertura6 * maxGiro;
		model = modelHolocron;
		model = glm::translate(model, glm::vec3(-dist6, dist6, -dist6));
		model = glm::rotate(model, glm::radians(rot6), glm::vec3(-1.0f, 1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holoEsq6_M.RenderModel();

		//ESQUINA 7
		float ang7 = mainWindow.getAngEsquina7();
		float apertura7 = abs(sin(glm::radians(ang7)));
		float dist7 = apertura7 * maxApertura;
		float rot7 = apertura7 * maxGiro;
		model = modelHolocron;
		model = glm::translate(model, glm::vec3(dist7, -dist7, -dist7));
		model = glm::rotate(model, glm::radians(rot7), glm::vec3(1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holoEsq7_M.RenderModel();

		//ESQUINA 8
		float ang8 = mainWindow.getAngEsquina8();
		float apertura8 = abs(sin(glm::radians(ang8)));
		float dist8 = apertura8 * maxApertura;
		float rot8 = apertura8 * maxGiro;
		model = modelHolocron;
		model = glm::translate(model, glm::vec3(-dist8, -dist8, -dist8));
		model = glm::rotate(model, glm::radians(rot8), glm::vec3(-1.0f, -1.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holoEsq8_M.RenderModel();

		//SATÉLITE TDRS

		//MATRIZ BASE
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, 15.0f, 0.0f)); // Posición inicial
		model = glm::translate(model, glm::vec3(mainWindow.getMovSatX(), mainWindow.getMovSatY(), mainWindow.getMovSatZ()));
		glm::mat4 modelSatelite = model;

		//CUERPO CENTRAL
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsCuerpo_M.RenderModel();

		//PANELES SOLARES
		model = modelSatelite;
		model = glm::rotate(model, glm::radians(mainWindow.getRotPanelIzq()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsPanelIzq_M.RenderModel();

		model = modelSatelite;
		model = glm::rotate(model, glm::radians(mainWindow.getRotPanelDer()), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsPanelDer_M.RenderModel();

		//ANTENAS PARABÓLICAS

		//PARABÓLICA IZQUIERDA
		model = modelSatelite;
		model = glm::translate(model, glm::vec3(0.0f, 1.3f, -3.2f));
		model = glm::rotate(model, glm::radians(mainWindow.getRotParabIzq()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsParabIzq_M.RenderModel();

		//PARABÓLICA DERECHA
		model = modelSatelite;
		model = glm::translate(model, glm::vec3(0.0f, 1.3f, 3.2f));
		model = glm::rotate(model, glm::radians(mainWindow.getRotParabDer()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsParabDer_M.RenderModel();


		//ANTENA DE ENLACE 
		model = modelSatelite;
		model = glm::translate(model, glm::vec3(-0.9f, 1.2f, -0.5f));
		model = glm::rotate(model, glm::radians(mainWindow.getRotEnlace()), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tdrsEnlace_M.RenderModel();


		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
