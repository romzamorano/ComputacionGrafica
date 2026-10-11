/*
Práctica 7: Iluminación 1 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <map>
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
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Model Blackhawk_M;

//Texturas del dado y del holocron
Texture dadoTexture;
Texture holocronTexture;

//Lista de Modelos a importar

//AVION
Model Avion_M;

//CUERPO DE ROVER
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


//función de calculo de normales por promedio de vértices 
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
	
	MeshModel *obj1 = new MeshModel();
	obj1->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj1);

	MeshModel *obj2 = new MeshModel();
	obj2->CreateMeshModel(vertices, indices, 32, 12);
	meshListModel.push_back(obj2);

	MeshModel *obj3 = new MeshModel();
	obj3->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	meshListModel.push_back(obj3);

	MeshModel *obj4 = new MeshModel();
	obj4->CreateMeshModel(vegetacionVertices, vegetacionIndices, 64, 12);
	meshListModel.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


void CrearDado() //sustituir con su propio dado, para que tenga la textura de la práctica 6
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,

		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,

		// right
		4, 5, 6,
		6, 7, 4,

	};	

	GLfloat cubo_vertices[] = {
		// Cara frontal
		// x       y       z        S        T       NX      NY      NZ
		-0.5f,  -0.5f,   0.5f,    0.0000f, 0.5000f,  0.0f,   0.0f,  -1.0f,
		 0.5f,  -0.5f,   0.5f,    0.3333f, 0.5000f,  0.0f,   0.0f,  -1.0f,
		 0.5f,   0.5f,   0.5f,    0.3333f, 1.0000f,  0.0f,   0.0f,  -1.0f,
		-0.5f,   0.5f,   0.5f,    0.0000f, 1.0000f,  0.0f,   0.0f,  -1.0f,

		// Cara derecha
		// x       y       z        S        T       NX      NY      NZ
		 0.5f,  -0.5f,   0.5f,    0.3333f, 0.5000f, -1.0f,   0.0f,   0.0f,
		 0.5f,  -0.5f,  -0.5f,    0.6666f, 0.5000f, -1.0f,   0.0f,   0.0f,
		 0.5f,   0.5f,  -0.5f,    0.6666f, 1.0000f, -1.0f,   0.0f,   0.0f,
		 0.5f,   0.5f,   0.5f,    0.3333f, 1.0000f, -1.0f,   0.0f,   0.0f,

		 // Cara trasera
		 // x       y       z        S        T       NX      NY      NZ
		 -0.5f,  -0.5f,  -0.5f,    0.6666f, 0.5000f,  0.0f,   0.0f,   1.0f,
		  0.5f,  -0.5f,  -0.5f,    1.0000f, 0.5000f,  0.0f,   0.0f,   1.0f,
		  0.5f,   0.5f,  -0.5f,    1.0000f, 1.0000f,  0.0f,   0.0f,   1.0f,
		 -0.5f,   0.5f,  -0.5f,    0.6666f, 1.0000f,  0.0f,   0.0f,   1.0f,

		 // Cara izquierda
		 // x       y       z        S        T       NX      NY      NZ
		 -0.5f,  -0.5f,  -0.5f,    0.0000f, 0.0000f,  1.0f,   0.0f,   0.0f,
		 -0.5f,  -0.5f,   0.5f,    0.3333f, 0.0000f,  1.0f,   0.0f,   0.0f,
		 -0.5f,   0.5f,   0.5f,    0.3333f, 0.5000f,  1.0f,   0.0f,   0.0f,
		 -0.5f,   0.5f,  -0.5f,    0.0000f, 0.5000f,  1.0f,   0.0f,   0.0f,

		 // Cara inferior
		 // x       y       z        S        T       NX      NY      NZ
		 -0.5f,  -0.5f,   0.5f,    0.3333f, 0.0000f,  0.0f,   1.0f,   0.0f,
		  0.5f,  -0.5f,   0.5f,    0.6666f, 0.0000f,  0.0f,   1.0f,   0.0f,
		  0.5f,  -0.5f,  -0.5f,    0.6666f, 0.5000f,  0.0f,   1.0f,   0.0f,
		 -0.5f,  -0.5f,  -0.5f,    0.3333f, 0.5000f,  0.0f,   1.0f,   0.0f,

		 // Cara superior
		 // x       y       z        S        T       NX      NY      NZ
		 -0.5f,   0.5f,   0.5f,    0.6666f, 0.0000f,  0.0f,  -1.0f,   0.0f,
		  0.5f,   0.5f,   0.5f,    1.0000f, 0.0000f,  0.0f,  -1.0f,   0.0f,
		  0.5f,   0.5f,  -0.5f,    1.0000f, 0.5000f,  0.0f,  -1.0f,   0.0f,
		 -0.5f,   0.5f,  -0.5f,    0.6666f, 0.5000f,  0.0f,  -1.0f,   0.0f,
	};


	MeshModel* dado = new MeshModel();
	dado->CreateMeshModel(cubo_vertices, cubo_indices, 192, 36);
	meshListModel.push_back(dado);

}


void CrearHolocron()
{
	unsigned int holocron_indices[] = {
		12, 1, 0,
		12, 4, 15,
		14, 9, 2,
		2, 12, 15,
		8, 3, 4,
		15, 4, 3,
		3, 6, 14,
		4, 5, 8,
		6, 7, 13,
		9, 10, 11,
		11, 2, 9,
		10, 7, 5,
		13, 14, 6,
		2, 3, 14,
		2, 15, 3,
		11, 12, 2,
		13, 9, 14,
		8, 6, 3,
		12, 0, 4,
		12, 11, 1,
		4, 0, 5,
		6, 8, 7,
		9, 13, 10,
		5, 0, 1,
		1, 11, 10,
		10, 13, 7,
		7, 8, 5,
		5, 1, 10
	};

	GLfloat pos[16][3] = {
		{-2.517274, -2.579095,  0.002115},	//0
		{-1.185055, -2.530499, -1.198261},	//1
		{ 0.049649,  2.440128, -2.417217},	//2
		{ 0.051152,  2.399781,  2.545043},	//3
		{-2.646786, -0.139047,  2.493757},	//4
		{-1.328995, -2.578386,  1.297838},	//5
		{ 2.593003, -0.085222,  2.505755},	//6
		{ 1.385028, -2.549476,  1.308677},	//7
		{-0.140696, -2.601934,  2.558561},	//8
		{ 2.534944, -0.061146, -2.580280},	//9
		{ 1.344182, -2.542571, -1.294387},	//10
		{ 0.081432, -2.513204, -2.426005},	//11
		{-2.453980, -0.063388, -2.477451},	//12
		{ 2.632201, -2.553892,  0.007226},	//13
		{ 2.521206,  2.448241,  0.063388},	//14
		{-2.451372,  2.421759,  0.077538}	//15
	};

	struct Cara { std::vector<int> tris; int logo; };
	std::vector<Cara> caras = {
		{ {4, 17},                  0 },	// Frente (cuadrado)
		{ {10, 15},                 1 },	// Atras (cuadrado)
		{ {1, 18},                  2 },	// Izquierda (cuadrado)
		{ {12, 16},                 3 },	// Derecha (cuadrado)
		{ {13, 14},                 4 },	// Arriba (cuadrado)
		{ {11, 23, 24, 25, 26, 27}, 5 },	// Abajo (cuadrado, 6 triangulos por los vertices extra)
		{ {0, 19},                  6 },	// Triangulo abajo atras-izquierda
		{ {9, 22},                  7 },	// Triangulo abajo atras-derecha
		{ {7, 20},                  8 },	// Triangulo abajo frente-izquierda
		{ {8, 21},                  9 },	// Triangulo abajo frente-derecha
		{ {2},                     10 },	// Triangulo arriba atras-derecha
		{ {3},                     11 },	// Triangulo arriba atras-izquierda
		{ {5},                     12 },	// Triangulo arriba frente-izquierda
		{ {6},                     13 }		// Triangulo arriba frente-derecha
	};

	const float zoomCuadrados = 1.15f;
	const float zoomTriangulos = 1.3f;
	const bool invertirV = true;

	auto P = [&](int i) { return glm::vec3(pos[i][0], pos[i][1], pos[i][2]); };

	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;

	for (auto& cara : caras)
	{
		// Vertices unicos de la cara
		std::vector<int> ids;
		for (int t : cara.tris)
			for (int k = 0; k < 3; k++)
			{
				int v = holocron_indices[t * 3 + k];
				if (std::find(ids.begin(), ids.end(), v) == ids.end()) ids.push_back(v);
			}

		// Centro
		glm::vec3 centro(0.0f);
		float areaTotal = 0.0f;
		for (int t : cara.tris)
		{
			glm::vec3 a = P(holocron_indices[t * 3]);
			glm::vec3 b = P(holocron_indices[t * 3 + 1]);
			glm::vec3 c = P(holocron_indices[t * 3 + 2]);
			float area = glm::length(glm::cross(b - a, c - a)) * 0.5f;
			centro += (a + b + c) / 3.0f * area;
			areaTotal += area;
		}
		centro /= areaTotal;

		// Normal de la cara, apuntando hacia afuera
		glm::vec3 n(0.0f);
		for (int t : cara.tris)
		{
			glm::vec3 a = P(holocron_indices[t * 3]);
			glm::vec3 b = P(holocron_indices[t * 3 + 1]);
			glm::vec3 c = P(holocron_indices[t * 3 + 2]);
			n += glm::cross(b - a, c - a);
		}
		n = glm::normalize(n);
		if (glm::dot(n, centro) < 0.0f) n = -n;

		// Ejes del plano: vv = "arriba" del logo, uu = "derecha" del logo
		glm::vec3 arriba(0.0f, 1.0f, 0.0f);
		glm::vec3 vv = arriba - n * glm::dot(arriba, n);
		if (glm::length(vv) < 0.2f)	// cara de arriba o abajo
		{
			arriba = glm::vec3(0.0f, 0.0f, -1.0f);
			vv = arriba - n * glm::dot(arriba, n);
		}
		vv = glm::normalize(vv);
		glm::vec3 uu = glm::cross(vv, n);

		// Radio de la cara
		float R = 0.0f;
		for (int v : ids) R = std::max(R, glm::length(P(v) - centro));
		R /= (cara.logo <= 5) ? zoomCuadrados : zoomTriangulos;

		// Celda del logo dentro de la imagen de 4x4
		int col = cara.logo % 4;
		int fila = cara.logo / 4;
		float cu = (col + 0.5f) / 4.0f;
		float cv = (fila + 0.5f) / 4.0f;
		float mitad = 0.125f;	// mitad de una celda

		// Vertices nuevos (uno por vertice por cara)
		std::map<int, unsigned int> nuevo;
		for (int vi : ids)
		{
			glm::vec3 d = P(vi) - centro;
			float s = cu + (glm::dot(d, uu) / R) * mitad;
			float t = cv - (glm::dot(d, vv) / R) * mitad;
			if (invertirV) t = 1.0f - t;

			nuevo[vi] = (unsigned int)(vertices.size() / 8);
			GLfloat dato[8] = { pos[vi][0], pos[vi][1], pos[vi][2], s, t, -n.x, -n.y, -n.z };
			vertices.insert(vertices.end(), dato, dato + 8);
		}

		// Indices
		for (int t : cara.tris)
			for (int k = 0; k < 3; k++)
				indices.push_back(nuevo[holocron_indices[t * 3 + k]]);
	}

	MeshModel* holocron = new MeshModel();
	holocron->CreateMeshModel(vertices.data(), indices.data(), (unsigned int)vertices.size(), (unsigned int)indices.size());
	meshListModel.push_back(holocron);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	CreateObjects();
	CrearDado();
	CrearHolocron();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();


	// ----Texturas Ejercicio de clase ----
	
	//TEXTURA DE DADO
	dadoTexture = Texture("Textures/dado_starwars.png");
	dadoTexture.LoadTextureA();

	//TEXTURA DE HOLOCRON
	holocronTexture = Texture("Textures/holo_starwars.png");
	holocronTexture.LoadTextureA();

	/*Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	*/
	
	// ---- Modelos para Practica ----

	//MODELO DE AVION
	Avion_M = Model();
	Avion_M.LoadModel("Models/Avion.obj");

	//MODELO DE ROVER
	
	//CUERPO
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


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(1.0f, 32);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.2f, 0.2f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	
	/*pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		-6.0f, 1.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;
	*/

	/*
	// Luces blancas en las 8 esquinas de una caja alrededor del dado y el holocron
	glm::vec3 centroObjetos(-6.5f, 6.5f, 6.0f);
	float dx = 4.0f, dy = 4.0f, dz = 4.0f;
	for (int sx = -1; sx <= 1; sx += 2)
		for (int sy = -1; sy <= 1; sy += 2)
			for (int sz = -1; sz <= 1; sz += 2)
			{
				glm::vec3 p = centroObjetos + glm::vec3(sx * dx, sy * dy, sz * dz);
				pointLights[pointLightCount] = PointLight(1.0f, 1.0f, 1.0f,
					0.0f, 1.0f,
					p.x, p.y, p.z,
					1.0f, 0.05f, 0.01f);
				pointLightCount++;
			}
	*/

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//se crean mas luces puntuales y spotlight
	
	//faro frontal del rover
	spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 5.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, -0.1f, 0.0f,
		1.0f, 0.02f, 0.0f,
		35.0f);
	spotLightCount++;

	//faro del avion hacia abajo
	spotLights[2] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.5f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.0f,
		30.0f);
	spotLightCount++;

	//faro del avion hacia adelante
	spotLights[3] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f,
		1.0f, 0.05f, 0.0f,
		10.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
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
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());


		//ROVER
		float luzAvanceRover = mainWindow.getAvanceLlantas() * 0.05f;
		glm::vec3 posChasisLuz = glm::vec3(0.0f - luzAvanceRover, 3.0f, -1.5f);
		glm::vec3 posFaroRover = posChasisLuz + glm::vec3(-6.5f, 1.0f, 0.5f);
		spotLights[1].SetFlash(posFaroRover, glm::vec3(-1.0f, -0.1f, 0.0f));

		// AVION
		float luzAvionY = 4.5f + mainWindow.getMovSatY();
		float luzAvionZ = -20.0f + mainWindow.getMovSatZ();
		glm::vec3 posAvionLuz = glm::vec3(20.0f, luzAvionY, luzAvionZ);
		spotLights[2].SetFlash(posAvionLuz + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		spotLights[3].SetFlash(posAvionLuz + glm::vec3(0.0f, -0.5f, 1.0f), glm::vec3(0.0f, -0.5f, 1.0f));

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[2]->RenderMeshModel();
	
		/*
		
		//BLACKHAWK
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		//Agave ¿qué sucede si lo renderizan antes del helicóptero ?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[3]->RenderMeshModel();
		glDisable(GL_BLEND);

		*/

		/*
		//EJERCICIO DE CLASE

		//Dado
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.0f, 5.0f, 6.0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dadoTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[4]->RenderMeshModel();

		//Holocron
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-7.0f, 7.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		holocronTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshListModel[5]->RenderMeshModel();
		*/

		//PRACTICA

		//ROVER
		color = glm::vec3(0.6f, 0.6f, 0.6f);
		model = glm::mat4(1.0);
		float avanceLlantasRover = mainWindow.getAvanceLlantas();
		float avanceLineal = avanceLlantasRover * 0.05f;
		model = glm::translate(model, glm::vec3(0.0f - avanceLineal, 3.0f, -1.5f));
		glm::mat4 modelChasis = model;

		//CUERPO
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Rover_M.RenderModel();
		color = glm::vec3(0.0f, 0.0f, 1.0f);

		//BRAZO
		modelaux = model;

		//BASE DEL BRAZO
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.5f, 1.0f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		baseBrazo_M.RenderModel();

		//PRIMERA PARTE DEL BRAZO
		model = modelaux;
		model = glm::translate(model, glm::vec3(.0f, 0.5f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoParte1_M.RenderModel();

		//SEGUNDA PARTE DEL BRAZO
		model = modelaux;
		model = glm::translate(model, glm::vec3(-2.7f, 2.5f, 0.0f));
		modelaux = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brazoParte2_M.RenderModel();

		//PINZA
		model = modelaux;
		model = glm::translate(model, glm::vec3(3.5f, 3.2f, 0.0f));
		model = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pinza_M.RenderModel();

		//LLANTAS
		float factorGiro = -5.0f;
		float giroLlantas = mainWindow.getAvanceLlantas() * factorGiro;

		//LLANTA DELANTERA DERECHA
		model = modelChasis;;
		model = glm::translate(model, glm::vec3(-3.5f, 0.0f, -2.5f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 modelPataDD = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pataDD_M.RenderModel();

		model = modelPataDD;
		model = glm::translate(model, glm::vec3(3.7f, -2.7f, -0.5f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(0.0f, 0.0f, -1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaDD_M.RenderModel();

		//LLANTA DELANTERA IZQUIERDA
		model = modelChasis;
		model = glm::translate(model, glm::vec3(-4.0f, 0.0f, 3.5f));
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
		glm::mat4 modelPataTI = model;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pataTI_M.RenderModel();

		model = modelPataTI;
		model = glm::translate(model, glm::vec3(2.5f, -2.0f, 0.5f));
		model = glm::rotate(model, glm::radians(giroLlantas), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		llantaTI_M.RenderModel();

		//AVION
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		model = glm::mat4(1.0);
		float avionY = 4.5f + mainWindow.getMovSatY();
		float avionZ = -20.0f + mainWindow.getMovSatZ();
		model = glm::translate(model, glm::vec3(20.0f, avionY, avionZ));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(350.0f, 350.0f, 350.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Avion_M.RenderModel();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
