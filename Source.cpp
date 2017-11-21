#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtx/transform.hpp>
#include <GLI/gli.hpp>


void errorCallbackGLFW(int error, const char* description);
void hintsGLFW();
void endProgram();
void render(GLfloat currentTime);
void update(GLfloat currentTime); 
void setupRender();
void startup();
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
void debugGL();
static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);
string readShader(string name);
void checkErrorShader(GLuint shader);
void readObj(string name, struct modelObject *obj);
void readTexture(string name, GLuint texture);


// VARIABLES
GLFWwindow*		window;
int				windowWidth = 800;
int				windowHeight = 600;

float			pi = 3.141592f;

bool			running = true;
glm::mat4		proj_matrix;

glm::vec3		tableAngle = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		tableDisp = glm::vec3(0.0f, 0.0f, 0.0f);


glm::vec3		righthumanAngle = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		lefthumanAngle = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		humanDisp = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3		mugAngle = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		mugDisp = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3		floorAngle = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		floorDisp = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3		sawAngle = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3		sawDisp = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3		cameraPosition = glm::vec3(-0.04f, 1.0f, 0.35f);
glm::vec3		cameraFront = glm::vec3(0.0f, -0.8f, -1.0f);
glm::vec3		cameraUp = glm::vec3(0.0f, 0.2f, 0.0f);
float           aspect = (float)windowWidth / (float)windowHeight;
float			fovy = 45.0f;
bool			keyStatus[1024];
GLfloat			deltaTime = 0.0f;	
GLfloat			lastTime = 0.0f;  

// FPS camera variables
GLfloat			yaw = -90.0f;	// init pointing to inside
GLfloat			pitch = 0.0f;	// start centered
GLfloat			lastX = (GLfloat)windowWidth / 2.0f;	// start middle screen
GLfloat			lastY = (GLfloat)windowHeight / 2.0f;	// start middle screen
bool			firstMouse = true;

// OBJ Variables
struct modelObject {
	std::vector < glm::vec3 > out_vertices;
	std::vector < glm::vec2 > out_uvs;
	std::vector < glm::vec3 > out_normals;
	GLuint*		texture;
	GLuint      program;
	GLuint      vao;
	GLuint      buffer[2];
	GLint       mv_location;
	GLint       proj_location;
	GLint		tex_location;

	// extra variables for this example
	GLuint		matColor_location;
	GLuint		lightColor_location;

} tableModel, sawModel, rightHumanModel, leftHumanModel, mugModel, lightModel, floorModel;

glm::vec3		*tablePositions;
glm::vec3		*tableRotations;

glm::vec3		*mugPositions;
glm::vec3		*mugRotations;

glm::vec3		*floorPositions;
glm::vec3		*floorRotations;


glm::vec3		*sawPositions;
glm::vec3		*sawRotations;

glm::vec3		*rightHumanPositions;
glm::vec3		*rightHumanRotations;

glm::vec3		*leftHumanPositions;
glm::vec3		*leftHumanRotations;



// Light
bool			movingLight = true;
int				movingLightNumber = 0;
int				timer = 0;
glm::vec3		lightDisp;
glm::vec3		lightDirection = glm::vec3(0.0f, -1.0f, 0.0f);;
//glm::vec3		lightDisp = glm::vec3(-1.0f, -1.0f, 0.0f);
glm::vec3		ia = glm::vec3(0.0f, 0.0f, 0.8f);
GLfloat			ka = 0.01f;
//glm::vec3		id = glm::vec3(0.93f, 0.75f, 0.32f);
glm::vec3		id;
GLfloat			kd = 1.0f;
glm::vec3		is = glm::vec3(1.00f, 1.00f, 1.0f);
GLfloat			ks = 0.01f;

float			innerLight = 17.0f;
float			outerLight = 52.0f;

int main()
{
	if (!glfwInit()) {							// Checking for GLFW
		cout << "Could not initialise GLFW...";
		return 0;
	}

	glfwSetErrorCallback(errorCallbackGLFW);	// Setup a function to catch and display all GLFW errors.

	hintsGLFW();								// Setup glfw with various hints.		

	// Start a window using GLFW
	string title = "Magic Trick Gone Wrong!";
	
	// Fullscreen
	//const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//windowWidth = mode->width; windowHeight = mode->height;
	//window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL);

	// Window
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {								// Window or OpenGL context creation failed
		cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}

	glfwMakeContextCurrent(window);				// making the OpenGL context current

												// Start GLEW (note: always initialise GLEW after creating your window context.)
	glewExperimental = GL_TRUE;					// hack: catching them all - forcing newest debug callback (glDebugMessageCallback)
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {					// Problems starting GLEW?
		cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}

	debugGL();									// Setup callback to catch openGL errors.	

	// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);		// Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);					// Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback);	// Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);		// Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);		// Set callback for mouse wheel.
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor. Fullscreen
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS.

	setupRender();								// setup some render variables.
	startup();									// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	do {										// run until the window is closed
		GLfloat currentTime = (GLfloat)glfwGetTime();		// retrieve timelapse
		deltaTime = currentTime - lastTime;		// Calculate delta time
		lastTime = currentTime;					// Save for next frame calculations.
		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		render(currentTime);					// call render function.

		glfwSwapBuffers(window);				// swap buffers (avoid flickering and tearing)
		
		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram();			// Close and clean everything up...

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.
	
	return 0; 
}

void errorCallbackGLFW(int error, const char* description) {
	cout << "Error GLFW: " << description << "\n";
}

void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);			// Create context in debug mode - for debug message callback
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
}

void endProgram() {
	glfwMakeContextCurrent(window);		// destroys window handler
	glfwTerminate();	// destroys all windows and releases resources.

	// tidy heap memory
	delete[] tableModel.texture;
	delete[] rightHumanModel.texture;
	delete[] mugModel.texture;
	delete[] lightModel.texture;
	delete[] tablePositions;
	delete[] tableRotations;
	delete[] mugPositions;
	delete[] mugRotations;
	delete[] sawPositions;
	delete[] sawRotations;
	//delete[] lightDisp;
	//delete[] id;
}

void setupRender() {
	glfwSwapInterval(1);	// Ony render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}

void startup() {
	
	// Load main object model and shaders

	// --------------Table Model---------------------
	tableModel.program = glCreateProgram();

	string vs_text = readShader("vs_model.glsl"); static const char* vs_source = vs_text.c_str(); 
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	checkErrorShader(vs);
	glAttachShader(tableModel.program, vs);

	string fs_text = readShader("fs_model.glsl"); static const char* fs_source = fs_text.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	checkErrorShader(fs);
	glAttachShader(tableModel.program, fs);

	glLinkProgram(tableModel.program);
	glUseProgram(tableModel.program);

	readObj("table.obj", &tableModel);
	
	glCreateBuffers(3, tableModel.buffer);		// Create a new buffer

	// Store the vertices
	glNamedBufferStorage(tableModel.buffer[0], tableModel.out_vertices.size() * sizeof(glm::vec3), &tableModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, tableModel.buffer[0]);	// Bind Buffer

	// Store the texture coordinates
	glNamedBufferStorage(tableModel.buffer[1], tableModel.out_uvs.size() * sizeof(glm::vec2), &tableModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, tableModel.buffer[1]);	// Bind Buffer

	// Store the normal Vectors
	glNamedBufferStorage(tableModel.buffer[2], tableModel.out_normals.size() * sizeof(glm::vec3), &tableModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, tableModel.buffer[3]);	// Bind Buffer

	glCreateVertexArrays(1, &tableModel.vao);		// Create Vertex Array Object
	
	// Bind vertex position buffer to the vao and format
	glVertexArrayVertexBuffer(tableModel.vao, 0, tableModel.buffer[0], 0, sizeof(GLfloat)*3);
	glVertexArrayAttribFormat(tableModel.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(tableModel.vao, 0);	// Enable Vertex Array Attribute

	// Bind texture coordinate buffer to the vao and format
	glVertexArrayVertexBuffer(tableModel.vao, 1, tableModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(tableModel.vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(tableModel.vao, 1);	// Enable Vertex Array Attribute

	// Bind the normals buffer to the vao and format
	glVertexArrayVertexBuffer(tableModel.vao, 2, tableModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(tableModel.vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(tableModel.vao, 2);	// Enable Vertex Array Attribute

	glBindVertexArray(tableModel.vao);				// Bind VertexArrayObject

	tableModel.mv_location = glGetUniformLocation(tableModel.program, "mv_matrix");
	tableModel.proj_location = glGetUniformLocation(tableModel.program, "proj_matrix");
	tableModel.tex_location = glGetUniformLocation(tableModel.program, "tex");
	tableModel.lightColor_location = glGetUniformLocation(tableModel.program, "ia");
	tableModel.lightColor_location = glGetUniformLocation(tableModel.program, "ka");

	// --------------Human Model---------------------
	rightHumanModel.program = glCreateProgram();

	
	glAttachShader(rightHumanModel.program, vs);


	glAttachShader(rightHumanModel.program, fs);

	glLinkProgram(rightHumanModel.program);
	glUseProgram(rightHumanModel.program);

	readObj("rightPerson.obj", &rightHumanModel);

	glCreateBuffers(3, rightHumanModel.buffer);		// Create a new buffer

												// Store the vertices
	glNamedBufferStorage(rightHumanModel.buffer[0], rightHumanModel.out_vertices.size() * sizeof(glm::vec3), &rightHumanModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, rightHumanModel.buffer[0]);	// Bind Buffer

															// Store the texture coordinates
	glNamedBufferStorage(rightHumanModel.buffer[1], rightHumanModel.out_uvs.size() * sizeof(glm::vec2), &rightHumanModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, rightHumanModel.buffer[1]);	// Bind Buffer

															// Store the normal Vectors
	glNamedBufferStorage(rightHumanModel.buffer[2], rightHumanModel.out_normals.size() * sizeof(glm::vec3), &rightHumanModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, rightHumanModel.buffer[3]);	// Bind Buffer

	glCreateVertexArrays(1, &rightHumanModel.vao);		// Create Vertex Array Object

													// Bind vertex position buffer to the vao and format
	glVertexArrayVertexBuffer(rightHumanModel.vao, 0, rightHumanModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(rightHumanModel.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(rightHumanModel.vao, 0);	// Enable Vertex Array Attribute

													// Bind texture coordinate buffer to the vao and format
	glVertexArrayVertexBuffer(rightHumanModel.vao, 1, rightHumanModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(rightHumanModel.vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(rightHumanModel.vao, 1);	// Enable Vertex Array Attribute

													// Bind the normals buffer to the vao and format
	glVertexArrayVertexBuffer(rightHumanModel.vao, 2, rightHumanModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(rightHumanModel.vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(rightHumanModel.vao, 2);	// Enable Vertex Array Attribute

	glBindVertexArray(rightHumanModel.vao);				// Bind VertexArrayObject

	rightHumanModel.mv_location = glGetUniformLocation(rightHumanModel.program, "mv_matrix");
	rightHumanModel.proj_location = glGetUniformLocation(rightHumanModel.program, "proj_matrix");
	rightHumanModel.tex_location = glGetUniformLocation(rightHumanModel.program, "tex");
	rightHumanModel.lightColor_location = glGetUniformLocation(rightHumanModel.program, "ia");
	rightHumanModel.lightColor_location = glGetUniformLocation(rightHumanModel.program, "ka");



	///  left person

	leftHumanModel.program = glCreateProgram();


	glAttachShader(leftHumanModel.program, vs);


	glAttachShader(leftHumanModel.program, fs);

	glLinkProgram(leftHumanModel.program);
	glUseProgram(leftHumanModel.program);

	readObj("leftPerson.obj", &leftHumanModel);

	glCreateBuffers(3, leftHumanModel.buffer);		// Create a new buffer

													// Store the vertices
	glNamedBufferStorage(leftHumanModel.buffer[0], leftHumanModel.out_vertices.size() * sizeof(glm::vec3), &leftHumanModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, leftHumanModel.buffer[0]);	// Bind Buffer

																// Store the texture coordinates
	glNamedBufferStorage(leftHumanModel.buffer[1], leftHumanModel.out_uvs.size() * sizeof(glm::vec2), &leftHumanModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, leftHumanModel.buffer[1]);	// Bind Buffer

																// Store the normal Vectors
	glNamedBufferStorage(leftHumanModel.buffer[2], leftHumanModel.out_normals.size() * sizeof(glm::vec3), &leftHumanModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, leftHumanModel.buffer[3]);	// Bind Buffer

	glCreateVertexArrays(1, &leftHumanModel.vao);		// Create Vertex Array Object

														// Bind vertex position buffer to the vao and format
	glVertexArrayVertexBuffer(leftHumanModel.vao, 0, leftHumanModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(leftHumanModel.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(leftHumanModel.vao, 0);	// Enable Vertex Array Attribute

														// Bind texture coordinate buffer to the vao and format
	glVertexArrayVertexBuffer(leftHumanModel.vao, 1, leftHumanModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(leftHumanModel.vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(leftHumanModel.vao, 1);	// Enable Vertex Array Attribute

														// Bind the normals buffer to the vao and format
	glVertexArrayVertexBuffer(leftHumanModel.vao, 2, leftHumanModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(leftHumanModel.vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(leftHumanModel.vao, 2);	// Enable Vertex Array Attribute

	glBindVertexArray(leftHumanModel.vao);				// Bind VertexArrayObject

	leftHumanModel.mv_location = glGetUniformLocation(leftHumanModel.program, "mv_matrix");
	leftHumanModel.proj_location = glGetUniformLocation(leftHumanModel.program, "proj_matrix");
	leftHumanModel.tex_location = glGetUniformLocation(leftHumanModel.program, "tex");
	leftHumanModel.lightColor_location = glGetUniformLocation(leftHumanModel.program, "ia");
	leftHumanModel.lightColor_location = glGetUniformLocation(leftHumanModel.program, "ka");


	// --------------Floor Model---------------------
	floorModel.program = glCreateProgram();


	glAttachShader(floorModel.program, vs);


	glAttachShader(floorModel.program, fs);

	glLinkProgram(floorModel.program);
	glUseProgram(floorModel.program);

	readObj("floor.obj", &floorModel);

	glCreateBuffers(3, floorModel.buffer);		// Create a new buffer

													// Store the vertices
	glNamedBufferStorage(floorModel.buffer[0], floorModel.out_vertices.size() * sizeof(glm::vec3), &floorModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, floorModel.buffer[0]);	// Bind Buffer

																// Store the texture coordinates
	glNamedBufferStorage(floorModel.buffer[1], floorModel.out_uvs.size() * sizeof(glm::vec2), &floorModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, floorModel.buffer[1]);	// Bind Buffer

																// Store the normal Vectors
	glNamedBufferStorage(floorModel.buffer[2], floorModel.out_normals.size() * sizeof(glm::vec3), &floorModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, floorModel.buffer[3]);	// Bind Buffer

	glCreateVertexArrays(1, &floorModel.vao);		// Create Vertex Array Object

														// Bind vertex position buffer to the vao and format
	glVertexArrayVertexBuffer(floorModel.vao, 0, floorModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(floorModel.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(floorModel.vao, 0);	// Enable Vertex Array Attribute

														// Bind texture coordinate buffer to the vao and format
	glVertexArrayVertexBuffer(floorModel.vao, 1, floorModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(floorModel.vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(floorModel.vao, 1);	// Enable Vertex Array Attribute

														// Bind the normals buffer to the vao and format
	glVertexArrayVertexBuffer(floorModel.vao, 2, floorModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(floorModel.vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(floorModel.vao, 2);	// Enable Vertex Array Attribute

	glBindVertexArray(floorModel.vao);				// Bind VertexArrayObject

	floorModel.mv_location = glGetUniformLocation(floorModel.program, "mv_matrix");
	floorModel.proj_location = glGetUniformLocation(floorModel.program, "proj_matrix");
	floorModel.tex_location = glGetUniformLocation(floorModel.program, "tex");
	floorModel.lightColor_location = glGetUniformLocation(floorModel.program, "ia");
	floorModel.lightColor_location = glGetUniformLocation(floorModel.program, "ka");



	// --------------Saw Model---------------------
	sawModel.program = glCreateProgram();


	glAttachShader(sawModel.program, vs);


	glAttachShader(sawModel.program, fs);

	glLinkProgram(sawModel.program);
	glUseProgram(sawModel.program);

	readObj("saw.obj", &sawModel);

	glCreateBuffers(3, sawModel.buffer);		// Create a new buffer

												// Store the vertices
	glNamedBufferStorage(sawModel.buffer[0], sawModel.out_vertices.size() * sizeof(glm::vec3), &sawModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, sawModel.buffer[0]);	// Bind Buffer

														// Store the texture coordinates
	glNamedBufferStorage(sawModel.buffer[1], sawModel.out_uvs.size() * sizeof(glm::vec2), &sawModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, sawModel.buffer[5]);	// Bind Buffer

														// Store the normal Vectors
	glNamedBufferStorage(sawModel.buffer[2], sawModel.out_normals.size() * sizeof(glm::vec3), &sawModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, sawModel.buffer[3]);	// Bind Buffer

	glCreateVertexArrays(1, &sawModel.vao);		// Create Vertex Array Object

												// Bind vertex position buffer to the vao and format
	glVertexArrayVertexBuffer(sawModel.vao, 0, sawModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(sawModel.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(sawModel.vao, 0);	// Enable Vertex Array Attribute

												// Bind texture coordinate buffer to the vao and format
	glVertexArrayVertexBuffer(sawModel.vao, 1, sawModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(sawModel.vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(sawModel.vao, 1);	// Enable Vertex Array Attribute

												// Bind the normals buffer to the vao and format
	glVertexArrayVertexBuffer(sawModel.vao, 2, sawModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(sawModel.vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(sawModel.vao, 2);	// Enable Vertex Array Attribute

	glBindVertexArray(sawModel.vao);				// Bind VertexArrayObject

	sawModel.mv_location = glGetUniformLocation(sawModel.program, "mv_matrix");
	sawModel.proj_location = glGetUniformLocation(sawModel.program, "proj_matrix");
	sawModel.tex_location = glGetUniformLocation(sawModel.program, "tex");
	sawModel.lightColor_location = glGetUniformLocation(sawModel.program, "ia");
	sawModel.lightColor_location = glGetUniformLocation(sawModel.program, "ka");

	// --------------Mug Model---------------------
	mugModel.program = glCreateProgram();

	
	glAttachShader(mugModel.program, vs);

	glAttachShader(mugModel.program, fs);

	glLinkProgram(mugModel.program);
	glUseProgram(mugModel.program);

	readObj("mug.obj", &mugModel);

	glCreateBuffers(3, mugModel.buffer);		// Create a new buffer

												// Store the vertices
	glNamedBufferStorage(mugModel.buffer[0], mugModel.out_vertices.size() * sizeof(glm::vec3), &mugModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, mugModel.buffer[0]);	// Bind Buffer

															// Store the texture coordinates
	glNamedBufferStorage(mugModel.buffer[1], mugModel.out_uvs.size() * sizeof(glm::vec2), &mugModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, mugModel.buffer[5]);	// Bind Buffer

															// Store the normal Vectors
	glNamedBufferStorage(mugModel.buffer[2], mugModel.out_normals.size() * sizeof(glm::vec3), &mugModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, mugModel.buffer[3]);	// Bind Buffer

	glCreateVertexArrays(1, &mugModel.vao);		// Create Vertex Array Object

													// Bind vertex position buffer to the vao and format
	glVertexArrayVertexBuffer(mugModel.vao, 0, mugModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(mugModel.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(mugModel.vao, 0);	// Enable Vertex Array Attribute

													// Bind texture coordinate buffer to the vao and format
	glVertexArrayVertexBuffer(mugModel.vao, 1, mugModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(mugModel.vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(mugModel.vao, 1);	// Enable Vertex Array Attribute

													// Bind the normals buffer to the vao and format
	glVertexArrayVertexBuffer(mugModel.vao, 2, mugModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(mugModel.vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(mugModel.vao, 2);	// Enable Vertex Array Attribute

	glBindVertexArray(mugModel.vao);				// Bind VertexArrayObject

	mugModel.mv_location = glGetUniformLocation(mugModel.program, "mv_matrix");
	mugModel.proj_location = glGetUniformLocation(mugModel.program, "proj_matrix");
	mugModel.tex_location = glGetUniformLocation(mugModel.program, "tex");
	mugModel.lightColor_location = glGetUniformLocation(mugModel.program, "ia");
	mugModel.lightColor_location = glGetUniformLocation(mugModel.program, "ka");
	
	//--------------Light Model--------------------------
	lightModel.program = glCreateProgram();

	string vs_textLight = readShader("vs_light.glsl"); static const char* vs_sourceLight = vs_textLight.c_str();
	GLuint vsLight = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsLight, 1, &vs_sourceLight, NULL);
	glCompileShader(vsLight);
	checkErrorShader(vsLight);
	glAttachShader(lightModel.program, vsLight);

	string fs_textLight = readShader("fs_light.glsl"); static const char* fs_sourceLight = fs_textLight.c_str();
	GLuint fsLight = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsLight, 1, &fs_sourceLight, NULL);
	glCompileShader(fsLight);
	checkErrorShader(fsLight);
	glAttachShader(lightModel.program, fsLight);

	glLinkProgram(lightModel.program);

	readObj("sphere.obj", &lightModel);

	glCreateBuffers(3, lightModel.buffer);		// Create a new buffer     

	// Store the vertices
	glNamedBufferStorage(lightModel.buffer[0], lightModel.out_vertices.size() * sizeof(glm::vec3), &lightModel.out_vertices[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[0]);	// Bind Buffer

	// Store the texture coordinates
	glNamedBufferStorage(lightModel.buffer[1], lightModel.out_uvs.size() * sizeof(glm::vec2), &lightModel.out_uvs[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[1]);	// Bind Buffer

	// Store the normal Vectors
	glNamedBufferStorage(lightModel.buffer[2], lightModel.out_normals.size() * sizeof(glm::vec3), &lightModel.out_normals[0], GL_DYNAMIC_STORAGE_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, lightModel.buffer[3]);	// Bind Buffer

	glCreateVertexArrays(1, &lightModel.vao);		// Create Vertex Array Object

	// Bind vertex position buffer to the vao and format
	glVertexArrayVertexBuffer(lightModel.vao, 0, lightModel.buffer[0], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(lightModel.vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(lightModel.vao, 0);	// Enable Vertex Array Attribute

	// Bind texture coordinate buffer to the vao and format
	glVertexArrayVertexBuffer(lightModel.vao, 1, lightModel.buffer[1], 0, sizeof(GLfloat) * 2);
	glVertexArrayAttribFormat(lightModel.vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(lightModel.vao, 1);	// Enable Vertex Array Attribute

	// Bind the normals buffer to the vao and format
	glVertexArrayVertexBuffer(lightModel.vao, 2, lightModel.buffer[2], 0, sizeof(GLfloat) * 3);
	glVertexArrayAttribFormat(lightModel.vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(lightModel.vao, 2);	// Enable Vertex Array Attribute

	glBindVertexArray(lightModel.vao);				// Bind VertexArrayObject

	lightModel.mv_location = glGetUniformLocation(lightModel.program, "mv_matrix");
	lightModel.proj_location = glGetUniformLocation(lightModel.program, "proj_matrix");
	lightModel.tex_location = glGetUniformLocation(lightModel.program, "tex");


	//--------------------------------------------

	mugPositions = new  glm::vec3(-0.5f, 0.45f, 0.0f);
	mugRotations = new glm::vec3(0.0f, 0.0f, 0.0f);

	floorPositions = new glm::vec3(-0.2f, 0.1f, -0.55f);
	floorRotations = new glm::vec3(0.0f, 0.0f, 0.0f);

	sawPositions = new  glm::vec3(-0.04f, 0.5f, 0.5f);
	sawRotations = new glm::vec3(0.0f, 0.0f, 0.0f);

	tablePositions = new glm::vec3(0.0f, 0.0f, 0.0f);
	tableRotations = new glm::vec3(0.0f, 0.0f, 0.0f);

	rightHumanPositions = new  glm::vec3(-0.04f, 0.35f, 0.35f);
	rightHumanRotations = new glm::vec3(-1.55f, 0.0f, 0.0f);
	
	leftHumanPositions = new  glm::vec3(-0.04f, 0.35f, 0.35f);
	leftHumanRotations = new glm::vec3(-1.55f, 0.0f, 0.0f);

	lightDisp = glm::vec3(0.0f, 1.2f, 0.0f);

	id =  glm::vec3(1.0f, 1.0f, 1.0f);

	
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void update(GLfloat currentTime) {

	// calculate movement
	GLfloat cameraSpeed = 1.0f * deltaTime;
	if (keyStatus[GLFW_KEY_W]) cameraPosition += cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_S]) cameraPosition -= cameraSpeed * cameraFront;
	if (keyStatus[GLFW_KEY_A]) cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keyStatus[GLFW_KEY_D]) cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (keyStatus[GLFW_KEY_L] && (movingLight == false)) {
		cout << "Change mode to moving light...\n";
		movingLight = true;
	}
	if (keyStatus[GLFW_KEY_M] && (movingLight == true)) {
		cout << "Change mode to moving object...\n";
		movingLight = false;
	}


	if (timer >= 10 && timer < 20) cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (timer >= 10 && timer < 60) {
	//	cameraPosition -= cameraSpeed * cameraFront;
		//make it facing the table
		
		//cameraFront += glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)) - cameraPosition;
		cameraPosition -= cameraSpeed * cameraFront;
	}

	//if (timer = 11 ) {
//		cameraPosition -= cameraSpeed * cameraFront;
	//}

	//resets all the object's positions
	if (keyStatus[GLFW_KEY_R]) {
		//reset positions
		mugPositions[0].x = 0.0f;
		mugPositions[0].y = 0.5f;
		mugPositions[0].z = 0.0f;

		sawPositions[0].x = -0.04f;
		sawPositions[0].y = 0.5f;
		sawPositions[0].z = 0.5f;

		rightHumanPositions[0].x = -0.04f;
		rightHumanPositions[0].y = 0.35f;
		rightHumanPositions[0].z = 0.35f;

		leftHumanPositions[0].x = -0.04f;
		leftHumanPositions[0].y = 0.35f;
		leftHumanPositions[0].z = 0.35f;

		//reset rotations
		mugRotations[0].x = 0.0f;
		mugRotations[0].y = 0.0f;
		mugRotations[0].z = 0.0f;

		sawRotations[0].x = 0.0f;
		sawRotations[0].y = 0.0f;
		sawRotations[0].z = 0.0f;

		rightHumanRotations[0].x = -1.55f;
		rightHumanRotations[0].y = 0.0f;
		rightHumanRotations[0].z = 0.0f;

		leftHumanRotations[0].x = -1.55f;
		leftHumanRotations[0].y = 0.0f;
		leftHumanRotations[0].z = 0.0f;

		mugAngle.x = 0.0f;
		mugAngle.y = 0.0f;
		mugAngle.z = 0.0f;

		cameraPosition.x = -0.04f;
		cameraPosition.y = 0.7f;
		cameraPosition.z = 0.35f;

		cameraFront.x = 0.0f;
		cameraFront.y = 0.0f;
		cameraFront.z = -1.0f;

		cameraUp.x = 0.0f;
		cameraUp.y = 1.0f;
		cameraUp.z = 0.0f;

		timer = 0;
	}

	if (timer < 170) {
		mugAngle.y += 0.01f;
	}

	if (timer > 150 && timer < 163) {
	
		mugPositions[0].y -= 0.03f;
	
	}
	
		if(timer > 60 && timer <= 160)
		{ 
		sawPositions[0].z -= 0.01f;
		}

		timer++;

		if (timer >= 130 && timer < 170)
		{
			rightHumanPositions[0].x -= 0.01f;
			leftHumanPositions[0].x += 0.01f;

		}

		if (timer >= 140 && timer < 165)
		{
			rightHumanRotations[0].y -= 0.04f;
			leftHumanRotations[0].y += 0.04f;

			rightHumanPositions[0].y -= 0.01f;
			leftHumanPositions[0].y -= 0.01f;
		}

		if (timer > 170 && timer < 200) cameraPosition -= cameraSpeed * cameraFront;

		sawAngle.x += 0.1f;


	



	/*if (keyStatus[GLFW_KEY_0] && (movingLight == true)) {
		movingLightNumber = 0;  cout << "Moving light 0...\n";
	}
	if (keyStatus[GLFW_KEY_1] && (movingLight == true)) {
		movingLightNumber = 1;  cout << "Moving light 1...\n";
	}
	if (keyStatus[GLFW_KEY_2] && (movingLight == true)) {
		movingLightNumber = 2;  cout << "Moving light 2...\n";
	}
	if (keyStatus[GLFW_KEY_3] && (movingLight == true)) {
		movingLightNumber = 3;  cout << "Moving light 3...\n";
	}*/

	//if (leftHumanRotations[0].y < 1.1) {
	//	leftHumanRotations[0].y += 0.01f;
	//}
	//
	//if (leftHumanRotations[0].y > 1) {
	//	leftHumanRotations[0].x += 0.01f;
	//}




	if (movingLight == false) {		// moving object rotation and z displacement
		if (keyStatus[GLFW_KEY_LEFT])			tableAngle.y += 0.05f;
		if (keyStatus[GLFW_KEY_RIGHT])			tableAngle.y -= 0.05f;
		if (keyStatus[GLFW_KEY_UP])				tableAngle.x += 0.05f;
		if (keyStatus[GLFW_KEY_DOWN])			tableAngle.x -= 0.05f;
		if (keyStatus[GLFW_KEY_Z])				tableDisp.z += 0.10f;
		if (keyStatus[GLFW_KEY_X])				tableDisp.z -= 0.10f;
	}
	else {							// moving light displacement x y z
		//if (keyStatus[GLFW_KEY_LEFT])			lightDisp[movingLightNumber].x -= 0.05f;
		//if (keyStatus[GLFW_KEY_RIGHT])			lightDisp[movingLightNumber].x += 0.05f;
		//if (keyStatus[GLFW_KEY_UP])				lightDisp[movingLightNumber].y += 0.05f;
		//if (keyStatus[GLFW_KEY_DOWN])			lightDisp[movingLightNumber].y -= 0.05f;
		//if (keyStatus[GLFW_KEY_KP_ADD])			lightDisp[movingLightNumber].z += 0.05f;
		//if (keyStatus[GLFW_KEY_KP_SUBTRACT])	lightDisp[movingLightNumber].z -= 0.05f;
	}
}

void render(GLfloat currentTime) {
	glViewport(0, 0, windowWidth, windowHeight);

	// Clear colour buffer
	//this changes the background colour
	glm::vec4 backgroundColor = glm::vec4(0.28f, 0.28f, 0.28f, 1.0f); glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear Deep buffer
	static const GLfloat one = 1.0f; glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// ----------draw table model------------
	glUseProgram(tableModel.program);
	glBindVertexArray(tableModel.vao);
	glUniformMatrix4fv(tableModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

	glUniform4f(glGetUniformLocation(tableModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(tableModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
	glUniform1f(glGetUniformLocation(tableModel.program, "ka"), ka);
	
	glUniform1f(glGetUniformLocation(tableModel.program, "kd"), 1.0f);
	glUniform4f(glGetUniformLocation(tableModel.program, "is"), is.r, is.g, is.b, 1.0);
	glUniform1f(glGetUniformLocation(tableModel.program, "ks"), 1.0f);
	glUniform1f(glGetUniformLocation(tableModel.program, "shininess"), 32.0f);

	glUniform1f(glGetUniformLocation(tableModel.program, "lightConstant"), 1.0f);
	glUniform1f(glGetUniformLocation(tableModel.program, "lightLinear"), 0.7f);
	glUniform1f(glGetUniformLocation(tableModel.program, "lightQuadratic"), 1.8f);

	//for (int i = 0; i < 1; i++) {
	//	glUniform4f(glGetUniformLocation(tableModel.program, ("lights[" + to_string(0) + "].lightPosition").c_str()), lightDisp.x, lightDisp.y, lightDisp.z, 1.0f);
		glUniform4f(glGetUniformLocation(tableModel.program, "id"), id.r, id.g, id.b, 1.0f);
	//}

	glUniform4f(glGetUniformLocation(tableModel.program, "lightPosition"), lightDisp.x, lightDisp.y, lightDisp.z, 1.0);
    glUniform4f(glGetUniformLocation(tableModel.program, "lightSpotDirection"), lightDirection.x, lightDirection.y, lightDirection.z, 0.0);
	glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotCutOff"), glm::cos(glm::radians(innerLight)));
	glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(outerLight)));

	// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tableModel.texture[0]);
	glUniform1i(tableModel.tex_location, 0);

	glm::mat4 viewMatrix = glm::lookAt( cameraPosition,					// eye
										cameraPosition + cameraFront,	// centre
										cameraUp);						// up

	//for (int i = 0; i < 1; i++) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));// modelDisp.z));
		modelMatrix = glm::translate(modelMatrix, tablePositions[0]);
		modelMatrix = glm::rotate(modelMatrix, tableAngle.x + tableRotations[0].x, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, tableAngle.y + tableRotations[0].y, glm::vec3(0.0f, 1.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
		
		glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(tableModel.program, "model_matrix"), 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(tableModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, tableModel.out_vertices.size());
	//}

		///draw human

		glUseProgram(rightHumanModel.program);
		glBindVertexArray(rightHumanModel.vao);
		glUniformMatrix4fv(rightHumanModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

		glUniform4f(glGetUniformLocation(rightHumanModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
		glUniform4f(glGetUniformLocation(rightHumanModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
		glUniform1f(glGetUniformLocation(rightHumanModel.program, "ka"), ka);

		glUniform1f(glGetUniformLocation(rightHumanModel.program, "kd"), 2.0f);
		glUniform4f(glGetUniformLocation(rightHumanModel.program, "is"), is.r, is.g, is.b, 2.0);
		glUniform1f(glGetUniformLocation(rightHumanModel.program, "ks"), 2.0f);
		glUniform1f(glGetUniformLocation(rightHumanModel.program, "shininess"), 32.0f);

		glUniform1f(glGetUniformLocation(rightHumanModel.program, "lightConstant"), 1.0f);
		glUniform1f(glGetUniformLocation(rightHumanModel.program, "lightLinear"), 0.7f);
		glUniform1f(glGetUniformLocation(rightHumanModel.program, "lightQuadratic"), 1.8f);

		glUniform4f(glGetUniformLocation(tableModel.program, "id"), id.r, id.g, id.b, 1.0f);
		

		glUniform4f(glGetUniformLocation(tableModel.program, "lightPosition"), lightDisp.x, lightDisp.y, lightDisp.z, 1.0);
		glUniform4f(glGetUniformLocation(tableModel.program, "lightSpotDirection"), lightDirection.x, lightDirection.y, lightDirection.z, 0.0);
		glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotCutOff"), glm::cos(glm::radians(innerLight)));
		glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(outerLight)));

		// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rightHumanModel.texture[0]);
		glUniform1i(rightHumanModel.tex_location, 0);

		/*glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);						// up
		*/

		//for (int i = 0; i < 1; i++) {
		glm::mat4 humanMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));// modelDisp.z));
		humanMatrix = glm::translate(humanMatrix, rightHumanPositions[0]);
		humanMatrix = glm::rotate(humanMatrix, righthumanAngle.z + rightHumanRotations[0].z, glm::vec3(0.0f, 0.0f, 1.0f));
		humanMatrix = glm::rotate(humanMatrix, righthumanAngle.x + rightHumanRotations[0].x, glm::vec3(1.0f, 0.0f, 0.0f));
		humanMatrix = glm::rotate(humanMatrix, righthumanAngle.y + rightHumanRotations[0].y, glm::vec3(0.0f, 1.0f, 0.0f));
		humanMatrix = glm::scale(humanMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

		//glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(rightHumanModel.program, "model_matrix"), 1, GL_FALSE, &humanMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(rightHumanModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, rightHumanModel.out_vertices.size());
		//}


		///draw left sidehuman

		glUseProgram(leftHumanModel.program);
		glBindVertexArray(leftHumanModel.vao);
		glUniformMatrix4fv(leftHumanModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

		glUniform4f(glGetUniformLocation(leftHumanModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
		glUniform4f(glGetUniformLocation(leftHumanModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
		glUniform1f(glGetUniformLocation(leftHumanModel.program, "ka"), ka);

		glUniform1f(glGetUniformLocation(leftHumanModel.program, "kd"), 2.0f);
		glUniform4f(glGetUniformLocation(leftHumanModel.program, "is"), is.r, is.g, is.b, 2.0);
		glUniform1f(glGetUniformLocation(leftHumanModel.program, "ks"), 2.0f);
		glUniform1f(glGetUniformLocation(leftHumanModel.program, "shininess"), 32.0f);

		glUniform1f(glGetUniformLocation(leftHumanModel.program, "lightConstant"), 1.0f);
		glUniform1f(glGetUniformLocation(leftHumanModel.program, "lightLinear"), 0.7f);
		glUniform1f(glGetUniformLocation(leftHumanModel.program, "lightQuadratic"), 1.8f);

		glUniform4f(glGetUniformLocation(tableModel.program, "id"), id.r, id.g, id.b, 1.0f);


		glUniform4f(glGetUniformLocation(tableModel.program, "lightPosition"), lightDisp.x, lightDisp.y, lightDisp.z, 1.0);
		glUniform4f(glGetUniformLocation(tableModel.program, "lightSpotDirection"), lightDirection.x, lightDirection.y, lightDirection.z, 0.0);
		glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotCutOff"), glm::cos(glm::radians(innerLight)));
		glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(outerLight)));

		// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, leftHumanModel.texture[0]);
		glUniform1i(leftHumanModel.tex_location, 0);

		/*glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);						// up
		*/

		//for (int i = 0; i < 1; i++) {
		humanMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));// modelDisp.z));
		humanMatrix = glm::translate(humanMatrix, leftHumanPositions[0]);
		humanMatrix = glm::rotate(humanMatrix, lefthumanAngle.z + leftHumanRotations[0].z, glm::vec3(0.0f, 0.0f, 1.0f));
		humanMatrix = glm::rotate(humanMatrix, lefthumanAngle.x + leftHumanRotations[0].x, glm::vec3(1.0f, 0.0f, 0.0f));
		humanMatrix = glm::rotate(humanMatrix, lefthumanAngle.y + leftHumanRotations[0].y, glm::vec3(0.0f, 1.0f, 0.0f));
		humanMatrix = glm::scale(humanMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

		//glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(leftHumanModel.program, "model_matrix"), 1, GL_FALSE, &humanMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(leftHumanModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, leftHumanModel.out_vertices.size());
		




	// ----------draw mug model------------
	glUseProgram(mugModel.program);
	glBindVertexArray(mugModel.vao);
	glUniformMatrix4fv(mugModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

	glUniform4f(glGetUniformLocation(mugModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(mugModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
	glUniform1f(glGetUniformLocation(mugModel.program, "ka"), ka);

	glUniform1f(glGetUniformLocation(mugModel.program, "kd"), 2.0f);
	glUniform4f(glGetUniformLocation(mugModel.program, "is"), is.r, is.g, is.b, 2.0);
	glUniform1f(glGetUniformLocation(mugModel.program, "ks"), 2.0f);
	glUniform1f(glGetUniformLocation(mugModel.program, "shininess"), 32.0f);

	glUniform1f(glGetUniformLocation(mugModel.program, "lightConstant"), 1.0f);
	glUniform1f(glGetUniformLocation(mugModel.program, "lightLinear"), 0.7f);
	glUniform1f(glGetUniformLocation(mugModel.program, "lightQuadratic"), 1.8f);

	glUniform4f(glGetUniformLocation(tableModel.program, "id"), id.r, id.g, id.b, 1.0f);


	glUniform4f(glGetUniformLocation(tableModel.program, "lightPosition"), lightDisp.x, lightDisp.y, lightDisp.z, 1.0);
	glUniform4f(glGetUniformLocation(tableModel.program, "lightSpotDirection"), lightDirection.x, lightDirection.y, lightDirection.z, 0.0);
	glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotCutOff"), glm::cos(glm::radians(innerLight)));
	glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(outerLight)));

	// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mugModel.texture[0]);
	glUniform1i(mugModel.tex_location, 0);

	/*glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);						// up
		*/

	//for (int i = 0; i < 1; i++) {
		glm::mat4 mugMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));// modelDisp.z));
		mugMatrix = glm::translate(mugMatrix, mugPositions[0]);
		mugMatrix = glm::rotate(mugMatrix, mugAngle.x + mugRotations[0].x, glm::vec3(1.0f, 0.0f, 0.0f));
		mugMatrix = glm::rotate(mugMatrix, mugAngle.y + mugRotations[0].y, glm::vec3(0.0f, 1.0f, 0.0f));
		mugMatrix = glm::scale(mugMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

		//glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(mugModel.program, "model_matrix"), 1, GL_FALSE, &mugMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(mugModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, mugModel.out_vertices.size());
	//}


	// ----------draw floor model------------
		glUseProgram(floorModel.program);
		glBindVertexArray(floorModel.vao);
		glUniformMatrix4fv(floorModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

		glUniform4f(glGetUniformLocation(floorModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
		glUniform4f(glGetUniformLocation(floorModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
		glUniform1f(glGetUniformLocation(floorModel.program, "ka"), ka);

		glUniform1f(glGetUniformLocation(floorModel.program, "kd"), 2.0f);
		glUniform4f(glGetUniformLocation(floorModel.program, "is"), is.r, is.g, is.b, 2.0);
		glUniform1f(glGetUniformLocation(floorModel.program, "ks"), 2.0f);
		glUniform1f(glGetUniformLocation(floorModel.program, "shininess"), 32.0f);

		glUniform1f(glGetUniformLocation(floorModel.program, "lightConstant"), 1.0f);
		glUniform1f(glGetUniformLocation(floorModel.program, "lightLinear"), 0.7f);
		glUniform1f(glGetUniformLocation(floorModel.program, "lightQuadratic"), 1.8f);

		glUniform4f(glGetUniformLocation(tableModel.program, "id"), id.r, id.g, id.b, 1.0f);


		glUniform4f(glGetUniformLocation(tableModel.program, "lightPosition"), lightDisp.x, lightDisp.y, lightDisp.z, 1.0);
		glUniform4f(glGetUniformLocation(tableModel.program, "lightSpotDirection"), lightDirection.x, lightDirection.y, lightDirection.z, 0.0);
		glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotCutOff"), glm::cos(glm::radians(innerLight)));
		glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(outerLight)));

		// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorModel.texture[0]);
		glUniform1i(floorModel.tex_location, 0);

		/*glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);						// up
		*/

		//for (int i = 0; i < 1; i++) {
		glm::mat4 floorMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));// modelDisp.z));
		floorMatrix = glm::translate(floorMatrix, floorPositions[0]);
		floorMatrix = glm::rotate(floorMatrix, floorAngle.x + floorRotations[0].x, glm::vec3(1.0f, 0.0f, 0.0f));
		floorMatrix = glm::rotate(floorMatrix, floorAngle.y + floorRotations[0].y, glm::vec3(0.0f, 1.0f, 0.0f));
		floorMatrix = glm::scale(floorMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

		//glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(floorModel.program, "model_matrix"), 1, GL_FALSE, &floorMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(floorModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, floorModel.out_vertices.size());
		//}


	// ----------draw saw model------------
		glUseProgram(sawModel.program);
		glBindVertexArray(sawModel.vao);
		glUniformMatrix4fv(sawModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

		glUniform4f(glGetUniformLocation(sawModel.program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
		glUniform4f(glGetUniformLocation(sawModel.program, "ia"), ia.r, ia.g, ia.b, 1.0);
		glUniform1f(glGetUniformLocation(sawModel.program, "ka"), ka);

		glUniform1f(glGetUniformLocation(sawModel.program, "kd"), 2.0f);
		glUniform4f(glGetUniformLocation(sawModel.program, "is"), is.r, is.g, is.b, 2.0);
		glUniform1f(glGetUniformLocation(sawModel.program, "ks"), 2.0f);
		glUniform1f(glGetUniformLocation(sawModel.program, "shininess"), 32.0f);

		glUniform1f(glGetUniformLocation(sawModel.program, "lightConstant"), 1.0f);
		glUniform1f(glGetUniformLocation(sawModel.program, "lightLinear"), 0.7f);
		glUniform1f(glGetUniformLocation(sawModel.program, "lightQuadratic"), 1.8f);

		glUniform4f(glGetUniformLocation(tableModel.program, "id"), id.r, id.g, id.b, 1.0f);


		glUniform4f(glGetUniformLocation(tableModel.program, "lightPosition"), lightDisp.x, lightDisp.y, lightDisp.z, 1.0);
		glUniform4f(glGetUniformLocation(tableModel.program, "lightSpotDirection"), lightDirection.x, lightDirection.y, lightDirection.z, 0.0);
		glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotCutOff"), glm::cos(glm::radians(innerLight)));
		glUniform1f(glGetUniformLocation(tableModel.program, "lightSpotOuterCutOff"), glm::cos(glm::radians(outerLight)));

		// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sawModel.texture[0]);
		glUniform1i(sawModel.tex_location, 0);

		/*glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);						// up
		*/

		//for (int i = 0; i < 1; i++) {
		glm::mat4 sawMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));// modelDisp.z));
		sawMatrix = glm::translate(sawMatrix, sawPositions[0]);
		sawMatrix = glm::rotate(sawMatrix, sawAngle.z + sawRotations[0].z, glm::vec3(0.0f, 0.0f, 1.0f));
		sawMatrix = glm::rotate(sawMatrix, sawAngle.x + sawRotations[0].x, glm::vec3(1.0f, 0.0f, 0.0f));
		sawMatrix = glm::rotate(sawMatrix, sawAngle.y + sawRotations[0].y, glm::vec3(0.0f, 1.0f, 0.0f));
		sawMatrix = glm::scale(sawMatrix, glm::vec3(0.2f, 0.2f, 0.2f));

		//glm::mat4 mv_matrix = viewMatrix * modelMatrix;

		glUniformMatrix4fv(glGetUniformLocation(sawModel.program, "model_matrix"), 1, GL_FALSE, &sawMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(sawModel.program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, sawModel.out_vertices.size());
		//}

	// ----------draw light------------
	glUseProgram(lightModel.program);
	glBindVertexArray(lightModel.vao);
	glUniformMatrix4fv(lightModel.proj_location, 1, GL_FALSE, &proj_matrix[0][0]);

	// Bind textures and samplers - using 0 as I know there is only one texture - need to extend.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lightModel.texture[0]);
	glUniform1i(lightModel.tex_location, 0);

	

	for (int i = 0; i < 1; i++) {
		glm::mat4 modelMatrixLight = glm::translate(glm::mat4(1.0f), glm::vec3(lightDisp.x, lightDisp.y, lightDisp.z));
		modelMatrixLight = glm::scale(modelMatrixLight, glm::vec3(0.2f, 0.2f, 0.2f));
		glm::mat4 mv_matrixLight = viewMatrix * modelMatrixLight;

		glUniformMatrix4fv(lightModel.mv_location, 1, GL_FALSE, &mv_matrixLight[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, lightModel.out_vertices.size());
	}

	
}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;

	aspect = (float)w / (float)h;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);

	if (firstMouse){
		lastX = (GLfloat) mouseX; lastY = (GLfloat)mouseY; firstMouse = false;
	}

	GLfloat xoffset = mouseX - lastX;
	GLfloat yoffset = lastY - mouseY; // Reversed
	lastX = (GLfloat)mouseX; lastY = (GLfloat)mouseY;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity; yoffset *= sensitivity;

	yaw += xoffset; pitch += yoffset;

	// check for pitch out of bounds otherwise screen gets flipped
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	cameraFront = glm::normalize(front);

}

static void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	int yoffsetInt = static_cast<int>(yoffset);

	fovy += yoffsetInt;
	if (fovy >= 1.0f && fovy <= 45.0f) fovy -= (GLfloat)yoffset;
	if (fovy <= 1.0f) fovy = 1.0f;
	if (fovy >= 45.0f) fovy = 45.0f;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void debugGL() {
	//Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;

	// Enable Opengl Debug
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	cout << "---------------------opengl-callback------------" << endl;
	cout << "Message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << " --- ";

	cout << "id: " << id << " --- ";
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		cout << "NOTIFICATION";
	}
	cout << endl;
	cout << "-----------------------------------------" << endl;
}

string readShader(string name) {
	string vs_text;
	std::ifstream vs_file(name);

	string vs_line;
	if (vs_file.is_open()) {

		while (getline(vs_file, vs_line)) {
			vs_text += vs_line;
			vs_text += '\n';
		}
		vs_file.close();
	}
	return vs_text;
}

void  checkErrorShader(GLuint shader) {
	// Get log lenght
	GLint maxLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	// Init a string for it
	std::vector<GLchar> errorLog(maxLength);

	if (maxLength > 0) {
		// Get the log file
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		cout << "--------------Shader compilation error-------------\n";
		cout << errorLog.data();
	}

}

void readObj(string name, struct modelObject *obj) {
	cout << "Loading model " << name << "\n";

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< string > materials, textures;
	std::vector< glm::vec3 > obj_vertices;
	std::vector< glm::vec2 > obj_uvs;
	std::vector< glm::vec3 > obj_normals;

	std::ifstream dataFile(name);

	string rawData;		// store the raw data.
	int countLines = 0;
	if (dataFile.is_open()) {
		string dataLineRaw;	
		while (getline(dataFile, dataLineRaw)) {
			rawData += dataLineRaw;
			rawData += "\n";
			countLines++;
		}
		dataFile.close();
	}

	cout << "Finished reading model file " << name << "\n";

	istringstream rawDataStream(rawData);
	string dataLine;
	int linesDone = 0;
	while (std::getline(rawDataStream, dataLine)) {
		if (dataLine.find("v ") != string::npos) {	// does this line have a vector?
			glm::vec3 vertex;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart+1);
			vertex.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart+1);
			vertex.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart+1);
			vertex.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_vertices.push_back(vertex);
		}
		else if (dataLine.find("vt ") != string::npos) {	// does this line have a uv coordinates?
			glm::vec2 uv;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			uv.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			uv.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_uvs.push_back(uv);
		}
		else if (dataLine.find("vn ") != string::npos) { // does this line have a normal coordinates?
			glm::vec3 normal;

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			normal.x = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			normal.y = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			normal.z = stof(dataLine.substr(foundStart, foundEnd - foundStart));

			obj_normals.push_back(normal);
		}
		else if (dataLine.find("f ") != string::npos) { // does this line defines a triangle face?
			string parts[3];

			int foundStart = dataLine.find(" ");  int foundEnd = dataLine.find(" ", foundStart + 1);
			parts[0] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			parts[1] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			foundStart = foundEnd; foundEnd = dataLine.find(" ", foundStart + 1);
			parts[2] = dataLine.substr(foundStart + 1, foundEnd - foundStart - 1);

			for (int i = 0; i < 3; i++) {		// for each part

				vertexIndices.push_back( stoul(parts[i].substr(0, parts[i].find("/"))) );

				int firstSlash = parts[i].find("/"); int secondSlash = parts[i].find("/", firstSlash + 1);

				if ((firstSlash+1) != (secondSlash)) {	// there are texture coordinates.
					uvIndices.push_back(stoul(parts[i].substr(firstSlash+1, secondSlash - firstSlash +1)));
				}

				
				normalIndices.push_back( stoul(parts[i].substr(secondSlash+1)) );
					
			}
		}
		else if (dataLine.find("mtllib ") != string::npos) { // does this object have a material?
			materials.push_back(dataLine.substr(dataLine.find(" ") + 1));
		}

		linesDone++;

		if (linesDone % 50000 == 0) {
			cout << "Parsed " << linesDone << " of " << countLines << " from model...\n";
		}
			
	}

	// Double check here in which coordinate system you exported your models - and flip or not the vertices...
	
/*	for (unsigned int i = 0; i < vertexIndices.size(); i += 3) {
		(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i+2] - 1]);
		(*obj).out_normals.push_back(obj_normals[normalIndices[i+2] - 1]);
		(*obj).out_uvs.push_back(obj_uvs[uvIndices[i+2] - 1]);

		(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i+1] - 1]);
		(*obj).out_normals.push_back(obj_normals[normalIndices[i+1] - 1]);
		(*obj).out_uvs.push_back(obj_uvs[uvIndices[i + 1] - 1]);

		(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i] - 1]);
		(*obj).out_normals.push_back(obj_normals[normalIndices[i] - 1]);
		(*obj).out_uvs.push_back(obj_uvs[uvIndices[i + 0] - 1]);
	}*/
	

	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		(*obj).out_vertices.push_back(obj_vertices[vertexIndices[i] - 1]);
		(*obj).out_normals.push_back(obj_normals[normalIndices[i] - 1]);
		(*obj).out_uvs.push_back(obj_uvs[uvIndices[i] - 1]);
	}
	

	// Load Materials
	for (unsigned int i = 0; i < materials.size(); i++) {
		
		std::ifstream dataFileMat(materials[i]);

		string rawDataMat;		// store the raw data.
		int countLinesMat = 0;
		if (dataFileMat.is_open()) {
			string dataLineRawMat;
			while (getline(dataFileMat, dataLineRawMat)) {
				rawDataMat += dataLineRawMat;
				rawDataMat += "\n";
			}
			dataFileMat.close();
		}

		istringstream rawDataStreamMat(rawDataMat);
		string dataLineMat;
		while (std::getline(rawDataStreamMat, dataLineMat)) {
			if (dataLineMat.find("map_Kd ") != string::npos) {	// does this line have a texture map?
				textures.push_back(dataLineMat.substr(dataLineMat.find(" ") + 1));
			}
		}
	}

	(*obj).texture = new GLuint[textures.size()];		// Warning possible memory leak here - there is a new here, where is your delete?
	glCreateTextures(GL_TEXTURE_2D, textures.size(), (*obj).texture);

	for (unsigned int i = 0; i < textures.size(); i++) {
		readTexture(textures[i], (*obj).texture[i]);
	}

	cout << "done";
}

void readTexture(string name, GLuint textureName) {
	
	cout <<" reading textutre called "+ name << endl;
	gli::texture tex = gli::load(name);
	if (tex.empty()) {
		cout << "Unable to load file " << name;
	}

	gli::gl texGl(gli::gl::PROFILE_GL33);
	gli::gl::format const texFormat = texGl.translate(tex.format(), tex.swizzles());
	//GLenum texTarget = texGl.translate(tex.target());

	// Load and create a texture 
	glBindTexture(GL_TEXTURE_2D, textureName); // All upcoming operations now have effect on this texture object

	glm::tvec3<GLsizei> const texExtent(tex.extent());
	GLsizei const texFaceTotal = static_cast<GLsizei>(tex.layers() * tex.faces());

	// Note: This only loads GL_TEXTURE_2D - for the complete code please visit >> http://gli.g-truc.net/
	glTexStorage2D(GL_TEXTURE_2D, static_cast<GLint>(tex.levels()), texFormat.Internal, texExtent.x, texExtent.y);
		
	for (std::size_t Layer = 0; Layer < tex.layers(); ++Layer){
		for (std::size_t Face = 0; Face < tex.faces(); ++Face){
			for (std::size_t Level = 0; Level < tex.levels(); ++Level) {
				glTextureSubImage2D(textureName, static_cast<GLint>(Level),
							0, 0,
							texExtent.x, texExtent.y,
							texFormat.External, texFormat.Type, tex.data(Layer, Face, Level));

			}
		}
	}

	// This only works for 2D Textures...
	// Set the texture wrapping parameters
	glTextureParameteri(textureName, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(textureName, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters
	glTextureParameteri(textureName, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(textureName, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);// Unbind texture when done, so we won't accidentily mess up our texture.
	

}