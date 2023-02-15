#include <iostream>
#include "GL/glew.h"
#include "GL/3dgl.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Include GLM extensions
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")


using namespace std;
using namespace _3dgl;
using namespace glm;

int Amb1 = 0;
int Pot1 = 0;
int Amb2 = 0;
int Pot2 = 0;
int Amb3 = 0;

// 3D models
C3dglModel camera;
C3dglModel teapot;
C3dglModel table;
C3dglModel chair;
C3dglModel vase;
C3dglModel cat;
C3dglModel lamp; 
GLuint idTexWood;
GLuint idTexNone;


C3dglProgram Program;

// camera position (for first person type camera navigation)
mat4 matrixView;			// The View Matrix
float angleTilt = 15;		// Tilt Angle
float angleRot = 0.1f;		// Camera orbiting angle
vec3 cam(0);				// Camera movement values

float vertices[] = { 4, 0, 4, -4, 0, 4, 0, -7, 0, 4, 0, -4, -4, 0, -4
, 0, -7, 0, 4, 0, 4, 4, 0, -4, 0, -7, 0, -4, 0, 4, -4, 0, -4, 0, -7, 0,4
, 0, 4, 4, 0, -4, -4, 0, 4, -4, 0, -4 }; 
float normals[] = { 0, 4, -7, 0, 4, -7, 0, 4, -7, 0, 4, 7, 0, 4, 7, 0
,4, 7,-7, 4, 0, -7, 4, 0, -7, 4, 0, 7, 4, 0, 7, 4, 0, 7, 4, 0,0, -1, 0
, 0, -1, 0, 0, -1, 0, 0, -1, 0 }; 
unsigned indices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
, 13, 14, 15 };

// buffers names
unsigned vertexBuffer = 0;
unsigned normalBuffer = 0;
unsigned indexBuffer = 0;



bool init()
{
	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!
	 
	// setup lighting
	C3dglShader VertexShader;
	C3dglShader FragmentShader;
	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/basic.vert")) return false;
	if (!VertexShader.Compile()) return false;
	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/basic.frag")) return false;
	if (!FragmentShader.Compile()) return false;
	if (!Program.Create()) return false;
	if (!Program.Attach(VertexShader)) return false;
	if (!Program.Attach(FragmentShader)) return false;
	if (!Program.Link()) return false;
	if (!Program.Use(true)) return false;

	// prepare vertex data
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// prepare normal data
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	// prepare indices array
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	

	// load your 3D models here!
	if (!camera.load("models\\camera.3ds")) return false;
	if (!table.load("models\\table.obj")) return false;
	if (!chair.load("models\\table.obj")) return false;
	if (!teapot.load("models\\utah_teapot_hires.obj")) return false;
	if (!vase.load("models\\vase.obj")) return false;
	if (!cat.load("models\\fat_cat_obj.obj")) return false;
	if (!lamp.load("models\\lamp.obj")) return false;

	

	Program.SendUniform("lightAmbient.on", Amb2);
	Program.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1); 

	Program.SendUniform("lightDir.on", 1); 
	Program.SendUniform("lightDir.direction", 1.0, 0.5, 1.0); 
	Program.SendUniform("lightDir.diffuse", 0.2, 0.2, 0.2);// dimmed white light
	Program.SendUniform("lightPoint1.on", Pot1);
	Program.SendUniform("lightPoint1.position", -5.75f, 3.95f, 3.0f);
	Program.SendUniform("lightPoint1.diffuse" , 0.5, 0.5, 0.5);
	Program.SendUniform("lightPoint1.specular", 0.6, 0.6, 1.0);

	Program.SendUniform("lightPoint2.on", Pot2);
	Program.SendUniform("lightPoint2.position", 0.25f, 3.95f, -3.0f);
	Program.SendUniform("lightPoint2.diffuse", 0.5, 0.5, 0.5);
	Program.SendUniform("lightPoint2.specular", 0.6, 0.6, 1.0);
	
	Program.SendUniform("shininess", 3.0);
	
	C3dglBitmap bm; bm.Load("models/oak.bmp", GL_RGBA); if (!bm.GetBits()) return false;
	
	glActiveTexture(GL_TEXTURE0); glGenTextures(1, &idTexWood);
	glBindTexture(GL_TEXTURE_2D, idTexWood);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());

	glGenTextures(1, &idTexNone);
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	BYTE bytes[] = { 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);

	
	Program.SendUniform("texture0", 0);

	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));
	matrixView *= lookAt(
		vec3(0.0, 5.0, 10.0),
		vec3(0.0, 5.0, 0.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.18f, 0.25f, 0.22f, 1.0f);   // deep grey background

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Shift+AD or arrow key to auto-orbit" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;

	return true;
}

void done()
{
}

void render()
{
	// this global variable controls the animation
	float theta = glutGet(GLUT_ELAPSED_TIME) * 0.01f;

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	mat4 m = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));// switch tilt off
	m = translate(m, cam);												// animate camera motion (controlled by WASD keys)
	m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));			// switch tilt on
	m = m * matrixView;
	m = rotate(m, radians(angleRot), vec3(0.f, 1.f, 0.f));				// animate camera orbiting
	matrixView = m;

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, idTexWood);;

	Program.SendUniform("matrixView", matrixView);
	// setup materials - grey

	// table
	Program.SendUniform("materialAmbient", 0.4f, 0.25f, 0.15f);
	Program.SendUniform("materialDiffuse", 0.4f, 0.25f, 0.15f);
	Program.SendUniform("materialSpecular", 0.0f, 0.0f, 0.0f);

	m = matrixView;
	m = translate(m, vec3(-3.0f, -6.85, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.01f, 0.01f, 0.01f));
	table.render(1,m);

	//chairs
	Program.SendUniform("materialAmbient", 0.0f, 0.2f, 0.2f);
	Program.SendUniform("materialDiffuse", 0.0f, 0.2f, 0.2f);
	Program.SendUniform("materialSpecular", 0.0f, 0.0f, 0.0f);
	m = matrixView;
	m = translate(m, vec3(-3.0f, -6.85, 0.0f));
	m = rotate(m, radians(0.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.01f, 0.01f, 0.01f));
	chair.render(0, m);

	m = matrixView;
	m = translate(m, vec3(-3.0f, -6.85, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.01f, 0.01f, 0.01f));
	chair.render(0, m);

	m = matrixView;
	m = translate(m, vec3(-3.0f, -6.85, 0.0f));
	m = rotate(m, radians(270.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.01f, 0.01f, 0.01f));
	chair.render(0, m);

	m = matrixView;
	m = translate(m, vec3(-3.0f, -6.85, 0.0f));
	m = rotate(m, radians(90.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.01f, 0.01f, 0.01f));
	chair.render(0, m);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexNone);

	// teapot
	Program.SendUniform("materialAmbient", 1.0f, 0.2f, 0.2f);
	Program.SendUniform("materialDiffuse", 1.0f, 0.2f, 0.2f);
	Program.SendUniform("materialSpecular", 0.6f, 0.6f, 1.0f);
	

	m = matrixView;
	m = translate(m, vec3(1.0f, 0.72, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(1.0f, 1.0f, 1.0f));
	teapot.render(m);
	//vase

	Program.SendUniform("materialAmbient", 0.2f, 0.2f, 0.8f);
	Program.SendUniform("materialDiffuse", 0.2f, 0.2f, 0.8f);
	Program.SendUniform("materialSpecular", 0.6f, 0.6f, 1.0f);
	m = matrixView;
	m = translate(m, vec3(-3.0f, 0.76, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.25f, 0.25f, 0.25f));
	vase.render(m);

	//cat
	Program.SendUniform("materialAmbient", 0.6f, 0.6f, 0.6f);
	Program.SendUniform("materialDiffuse", 0.6f, 0.6f, 0.6f);
	Program.SendUniform("materialSpecular", 0.6f, 0.6f, 1.0f);
	m = matrixView;
	m = translate(m, vec3(-7.5f, 1.65, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	cat.render(m);

	//lamp1
	Program.SendUniform("materialAmbient", 0.6f, 0.6f, 0.6f);
	Program.SendUniform("materialDiffuse", 0.6f, 0.6f, 0.6f);
	Program.SendUniform("materialSpecular", 0.6f, 0.6f, 1.0f);
	m = matrixView;
	m = translate(m, vec3(-7.5f, 0.775, 3.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.05f, 0.05f, 0.05f));
	lamp.render(m);

	//bulb1
	Program.SendUniform("materialAmbient", 1.0f, 1.0f, 1.0f);
	Program.SendUniform("materialDiffuse", 1.0f, 1.0f, 1.0f);
	Program.SendUniform("materialSpecular", 0.0f, 0.0f, 0.0f);
	m = matrixView;
	m = translate(m, vec3(-5.75f, 3.95f, 3.0f));
	m = scale(m, vec3(0.25f, 0.25f, 0.25f));
	Program.SendUniform("matrixModelView", m);
	Program.SendUniform("lightAmbient2.on", Amb2);
	Program.SendUniform("lightAmbient2.color", 1.0, 1.0, 1.0);
	glutSolidSphere(1, 32, 32);
	Program.SendUniform("lightAmbient2.color", 0.0, 0.0, 0.0);

	 //lamp2
	Program.SendUniform("materialAmbient", 0.6f, 0.6f, 0.6f);
	Program.SendUniform("materialDiffuse", 0.6f, 0.6f, 0.6f);
	Program.SendUniform("materialSpecular", 0.6f, 0.6f, 1.0f);
	m = matrixView;
	m = translate(m, vec3(2.0f, 0.775, -3.0f));
	m = rotate(m, radians(0.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.05f, 0.05f, 0.05f));
	lamp.render(m);

	//bulb2
	Program.SendUniform("materialAmbient", 1.0f, 1.0f, 1.0f);
	Program.SendUniform("materialDiffuse", 1.0f, 1.0f, 1.0f);
	Program.SendUniform("materialSpecular", 0.0f, 0.0f, 0.0f);
	m = matrixView;
	m = translate(m, vec3(0.25f, 3.95f, - 3.0f));
	m = scale(m, vec3(0.25f, 0.25f, 0.25f));
	Program.SendUniform("matrixModelView", m);
	Program.SendUniform("matrixModelView", m);
	Program.SendUniform("lightAmbient3.on", Amb3);
	Program.SendUniform("lightAmbient3.color", 1.0, 1.0, 1.0);
	glutSolidSphere(1, 32, 32);
	Program.SendUniform("lightAmbient3.color", 0.0, 0.0, 0.0);
	
	 //pym
	m = matrixView;
	m = translate(m, vec3(-0.9f, 3.7f, -1.2f));
	m = rotate(m, radians(180.f), vec3(1, 0, 0));
	m = rotate(m, radians(-4 * theta), vec3(0, 1, 0));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	Program.SendUniform("matrixModelView", m);

	GLuint attribVertex = Program.GetAttribLocation("aVertex");
	GLuint attribNormal = Program.GetAttribLocation("aNormal");
	// Enable vertex attribute arrays
	glEnableVertexAttribArray(attribVertex);
	glEnableVertexAttribArray(attribNormal);
	// Bind (activate) the vertex buffer and set the pointer to it
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Bind (activate) the normal buffer and set the pointer to it
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Draw triangles–using index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	// Disable arrays
	glDisableVertexAttribArray(attribVertex);
	glDisableVertexAttribArray(attribNormal);

	
												
	glutSetVertexAttribCoord3(Program.GetAttribLocation("aVertex")); 
    glutSetVertexAttribNormal(Program.GetAttribLocation("aNormal"));

	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}

// called before window opened or resized - to setup the Projection Matrix
void reshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 matrixProjection = perspective(radians(60.f), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix
	Program.SendUniform("matrixProjection", matrixProjection);
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': cam.z = std::max(cam.z * 1.05f, 0.01f); break;
	case 's': cam.z = std::min(cam.z * 1.05f, -0.01f); break;
	case 'a': cam.x = std::max(cam.x * 1.05f, 0.01f); angleRot = 0.1f; break;
	case 'd': cam.x = std::min(cam.x * 1.05f, -0.01f); angleRot = -0.1f; break;
	case 'e': cam.y = std::max(cam.y * 1.05f, 0.01f); break;
	case 'q': cam.y = std::min(cam.y * 1.05f, -0.01f); break;

	case '1': Pot1 = 1 - Pot1; Program.SendUniform("lightPoint1.on", Pot1);
		Amb2 = 1 - Amb2; Program.SendUniform("lightAmbient2.on", Amb2); break;

	case '2': Pot2 = 1 - Pot2; Program.SendUniform("lightPoint2.on", Pot2);
		Amb3 = 1 - Amb3; Program.SendUniform("lightAmbient3.on", Amb3); break;
	}
	// speed limit
	cam.x = std::max(-0.15f, std::min(0.15f, cam.x));
	cam.y = std::max(-0.15f, std::min(0.15f, cam.y));
	cam.z = std::max(-0.15f, std::min(0.15f, cam.z));
	// stop orbiting
	if ((glutGetModifiers() & GLUT_ACTIVE_SHIFT) == 0) angleRot = 0;
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': cam.z = 0; break;
	case 'a':
	case 'd': cam.x = 0; break;
	case 'q':
	case 'e': cam.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
bool bJustClicked = false;
void onMouse(int button, int state, int x, int y)
{
	bJustClicked = (state == GLUT_DOWN);
	glutSetCursor(bJustClicked ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
}

// handle mouse move
void onMotion(int x, int y)
{
	if (bJustClicked)
		bJustClicked = false;
	else
	{
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// find delta (change to) pan & tilt
		float deltaPan = 0.25f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
		float deltaTilt = 0.25f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// View = Tilt * DeltaPan * Tilt^-1 * DeltaTilt * View;
		angleTilt += deltaTilt;
		mat4 m = mat4(1.f);
		m = rotate(m, radians(angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaPan), vec3(0.f, 1.f, 0.f));
		m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaTilt), vec3(1.f, 0.f, 0.f));
		matrixView = m * matrixView;
	}
}

int main(int argc, char **argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("CI5520 3D Graphics Programming");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "GLEW Error: " << glewGetErrorString(err) << endl;
		return 0;
	}
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	// register callbacks
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);

	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Version: " << glGetString(GL_VERSION) << endl;

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		cerr << "Application failed to initialise" << endl;
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	done();

	return 1;
}

