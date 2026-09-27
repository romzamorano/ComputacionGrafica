#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	GLfloat getrotay() { return rotay; }
	GLfloat getrotax() { return rotax; }
	GLfloat getrotaz() { return rotaz; }
	GLfloat getarticulacion1() { return articulacion1; }
	GLfloat getarticulacion2() { return articulacion2; }
	GLfloat getarticulacion3() { return articulacion3; }
	GLfloat getarticulacion4() { return articulacion4; }
	GLfloat getarticulacion5() { return articulacion5; }
	GLfloat getarticulacion6() { return articulacion6; }
	GLfloat getarticulacion7() { return articulacion7; } 
	GLfloat getarticulacion8() { return articulacion8; } 
	GLfloat getarticulacion9() { return articulacion9; }
	GLfloat getarticulacion10() { return articulacion10; }
	GLfloat getAvanceLlantas() { return avanceLlantas; }
	GLfloat getAngEsquina1() { return angEsq1; }
	GLfloat getAngEsquina2() { return angEsq2; }
	GLfloat getAngEsquina3() { return angEsq3; }
	GLfloat getAngEsquina4() { return angEsq4; }
	GLfloat getAngEsquina5() { return angEsq5; }
	GLfloat getAngEsquina6() { return angEsq6; }
	GLfloat getAngEsquina7() { return angEsq7; }
	GLfloat getAngEsquina8() { return angEsq8; }
	GLfloat getMovSatX() { return movSatX; }
	GLfloat getMovSatY() { return movSatY; }
	GLfloat getMovSatZ() { return movSatZ; }
	GLfloat getRotPanelIzq() { return rotPanelIzq; }
	GLfloat getRotPanelDer() { return rotPanelDer; }
	GLfloat getRotParabIzq() { return rotParabIzq; }
	GLfloat getRotParabDer() { return rotParabDer; }
	GLfloat getRotEnlace() { return rotEnlace; }

	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	GLfloat rotax,rotay,rotaz, articulacion1, articulacion2, articulacion3, articulacion4, articulacion5, articulacion6, articulacion7, articulacion8, articulacion9, articulacion10, avanceLlantas;
	GLfloat angEsq1, angEsq2, angEsq3, angEsq4, angEsq5, angEsq6, angEsq7, angEsq8;
	GLfloat movSatX, movSatY, movSatZ, rotPanelIzq, rotPanelDer, rotParabIzq, rotParabDer, rotEnlace;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;
	void createCallbacks();
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
};

