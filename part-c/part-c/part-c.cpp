// part-a.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


// RotateCube.cpp
// Generate a rotating cube on a solid background

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "InitShader.h"
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>


GLuint vao;
int Index = 0;
GLuint program;
const int NumVertices = 36;
glm::vec4 points[NumVertices];
glm::vec4 colors[NumVertices];
//----------------------------------------------------------------------------

bool g_reverse_mode = false;

const float FOV_DEFAULT = 45.0f;
const float ASPECT_DEFAULT = 1.0f;
const float NCP_DEFAULT = 0.1f;
const float FCP_DEFAULT = 100.0f;

const float FOV_DELTA = 10.0f;
const float ASPECT_DELTA = 0.2f;
const float NCP_DELTA = 0.5f;
const float FCP_DELTA = 10.0f;

float g_fov = FOV_DEFAULT;
float g_aspect = ASPECT_DEFAULT;
float g_ncp = NCP_DEFAULT;
float g_fcp = FCP_DEFAULT;

const float MODEL_VALUE_DEFAULT = 1.0f;
const float MODEL_VALUE_DELTA = 0.1f;
const glm::mat4 MODEL_DEFAULT = glm::mat4(MODEL_VALUE_DEFAULT);

const float TRANSLATE_VALUE_DELTA = 0.1f;

const float ROTATE_VALUE_DELTA = 5.0f;

float g_model_value = MODEL_VALUE_DEFAULT;
glm::mat4 g_model = MODEL_DEFAULT;

// mouse_pos callback stuff
double oldX = 0.0;
double oldY = 0.0;
double hrotate = 0.0;
double vrotate = 0.0;
bool isPressed = false;

// camera zoom
const float ZOOM_DEFAULT = 2.0f;
const float ZOOM_DELTA = 0.1f;
float g_zoom = ZOOM_DEFAULT;

//----------------------------------------------------------------------------

glm::vec4 vertices[8] = {
	glm::vec4(-0.5, -0.5, 0.5, 1.0),
	glm::vec4(-0.5, 0.5, 0.5, 1.0),
	glm::vec4(0.5, 0.5, 0.5, 1.0),
	glm::vec4(0.5, -0.5, 0.5, 1.0),
	glm::vec4(-0.5, -0.5, -0.5, 1.0),
	glm::vec4(-0.5, 0.5, -0.5, 1.0),
	glm::vec4(0.5, 0.5, -0.5, 1.0),
	glm::vec4(0.5, -0.5, -0.5, 1.0)
};

glm::vec4 vertex_colors[8] = {
	glm::vec4(0.0, 0.0, 0.0, 1.0),  // black
	glm::vec4(1.0, 0.0, 0.0, 1.0),  // red
	glm::vec4(1.0, 1.0, 0.0, 1.0),  // yellow
	glm::vec4(0.0, 1.0, 0.0, 1.0),  // green
	glm::vec4(0.0, 0.0, 1.0, 1.0),  // blue
	glm::vec4(1.0, 0.0, 1.0, 1.0),  // magenta
	glm::vec4(1.0, 1.0, 1.0, 1.0),  // white
	glm::vec4(0.0, 1.0, 1.0, 1.0)   // cyan
};

enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Xaxis;
GLfloat  Theta[NumAxes] = { 0.0, 0.0, 0.0 };


GLuint  mvpi;
//GLuint  theta;	// The location of the "theta" shader uniform variable

void quad(int a, int b, int c, int d)
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void
colorcube()
{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}

void init(void)
{
	colorcube();

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors),
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	program = InitShader("vshader36.glsl", "fshader36.glsl");
	glUseProgram(program);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		NULL);

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		NULL);

	mvpi = glGetUniformLocation(program, "mvp");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

void mymouse(GLFWwindow* window, int button, int action, int mods)
{
	
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			isPressed = true;
			glfwGetCursorPos(window, &oldX, &oldY);
		} else if (action == GLFW_RELEASE) {
			isPressed = false;
			hrotate = 0.0;
			vrotate = 0.0;
		}
	}
}

void mykey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// reset all values
		if (key == GLFW_KEY_R) {
			// reverse mode
			g_reverse_mode = false;

			// camera values
			g_fov = FOV_DEFAULT;
			g_aspect = ASPECT_DEFAULT;
			g_ncp = NCP_DEFAULT;
			g_fcp = FCP_DEFAULT;
			g_zoom = ZOOM_DEFAULT;

			// scale values
			g_model_value = MODEL_VALUE_DEFAULT;
			g_model = MODEL_DEFAULT;
		}

		// enable reverse mode
		if (key == GLFW_KEY_LEFT_SHIFT) {
			if (!g_reverse_mode) {
				g_reverse_mode = true;
			} else {
				g_reverse_mode = false;
			}
		}

		// zoom camera in and out
		if (key == GLFW_KEY_I) {
			g_zoom -= ZOOM_DELTA;
		}

		if (key == GLFW_KEY_O) {
			g_zoom += ZOOM_DELTA;
		}

		// increase and decrease field of view with up/down arrow keys
		if (key == GLFW_KEY_UP) {
			g_fov += FOV_DELTA;
		}

		if (key == GLFW_KEY_DOWN) {
			g_fov -= FOV_DELTA;
		}

		// increase and decrease aspect ratio with left/right arrow keys
		if (key == GLFW_KEY_RIGHT) {
			g_aspect += ASPECT_DELTA;
		}

		if (key == GLFW_KEY_LEFT) {
			g_aspect -= ASPECT_DELTA;
		}

		// increase and decrease near clipping plane with </> keys
		if (key == GLFW_KEY_PERIOD) {
			g_ncp += NCP_DELTA;
			printf("ncp increased to: %f\n", g_ncp);
		}

		if (key == GLFW_KEY_COMMA) {
			g_ncp -= NCP_DELTA;
			printf("ncp decreased to: %f\n", g_ncp);
		}

		// increase and decrease far clipping plane with [/] keys
		if (key == GLFW_KEY_RIGHT_BRACKET) {
			g_fcp += FCP_DELTA;
			printf("fcp increased to: %f\n", g_fcp);
		}

		if (key == GLFW_KEY_LEFT_BRACKET) {
			g_fcp -= FCP_DELTA;
			printf("fcp decreased to: %f\n", g_fcp);
		}

		// increase and decrease object scale
		if (key == GLFW_KEY_EQUAL) {
			g_model_value += MODEL_VALUE_DELTA;
			if (g_model_value > 2.0f)
				g_model_value = 0.0f;
			printf("fcp increased to: %f\n", g_fcp);
			g_model = glm::scale(MODEL_DEFAULT, glm::vec3(g_model_value));
		}

		if (key == GLFW_KEY_MINUS) {
			g_model_value -= MODEL_VALUE_DELTA;
			if (g_model_value < 0.0f)
				g_model_value = MODEL_VALUE_DEFAULT;
			printf("fcp decreased to: %f\n", g_fcp);
			g_model = glm::scale(MODEL_DEFAULT, glm::vec3(g_model_value));
		}

		
		if (!g_reverse_mode) {
			// translate in x,y,z
			// A == x
			// S == y
			// D == z
			if (key == GLFW_KEY_A) {
				g_model = glm::translate(g_model, glm::vec3(TRANSLATE_VALUE_DELTA, 0.0f, 0.0f));
			}
			if (key == GLFW_KEY_S) {
				g_model = glm::translate(g_model, glm::vec3(0.0f, TRANSLATE_VALUE_DELTA, 0.0f));
			}
			if (key == GLFW_KEY_D) {
				g_model = glm::translate(g_model, glm::vec3(0.0f, 0.0f, TRANSLATE_VALUE_DELTA));
			}

			// rotate in x,y,z
			// X == x
			// Y == y
			// Z == z
			if (key == GLFW_KEY_X) {
				g_model = glm::rotate(g_model, ROTATE_VALUE_DELTA, glm::vec3(1.0f, 0.0f, 0.0f));
			}
			if (key == GLFW_KEY_Y) {
				g_model = glm::rotate(g_model, ROTATE_VALUE_DELTA, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (key == GLFW_KEY_Z) {
				g_model = glm::rotate(g_model, ROTATE_VALUE_DELTA, glm::vec3(0.0f, 0.0f, 1.0f));
			}
		} else {
			// reverse translation
			if (key == GLFW_KEY_A) {
				g_model = glm::translate(g_model, glm::vec3(-TRANSLATE_VALUE_DELTA, 0.0f, 0.0f));
			}
			if (key == GLFW_KEY_S) {
				g_model = glm::translate(g_model, glm::vec3(0.0f, -TRANSLATE_VALUE_DELTA, 0.0f));
			}
			if (key == GLFW_KEY_D) {
				g_model = glm::translate(g_model, glm::vec3(0.0f, 0.0f, -TRANSLATE_VALUE_DELTA));
			}

			// reverse rotation
			if (key == GLFW_KEY_X) {
				g_model = glm::rotate(g_model, -ROTATE_VALUE_DELTA, glm::vec3(1.0f, 0.0f, 0.0f));
			}
			if (key == GLFW_KEY_Y) {
				g_model = glm::rotate(g_model, -ROTATE_VALUE_DELTA, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (key == GLFW_KEY_Z) {
				g_model = glm::rotate(g_model, -ROTATE_VALUE_DELTA, glm::vec3(0.0f, 0.0f, 1.0f));
			}
		}
	}
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (isPressed) {
		hrotate += 0.00003*(xpos - oldX);
		vrotate += 0.00003*(ypos - oldY);

		g_model = glm::rotate(g_model, GLfloat(hrotate), glm::vec3(0.0, 1.0, 0.0));
		g_model = glm::rotate(g_model, GLfloat(vrotate), glm::vec3(1.0, 0.0, 0.0));
	}
}
//----------------------------------------------------------------------------

int main(int argc, char **argv)
{
	GLFWwindow *window = NULL;
	const GLubyte *renderer;
	const GLubyte *version;
	/* start GL context and O/S window using the GLFW helper library */
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	/* We must specify 3.2 core if on Apple OS X -- other O/S can specify
	anything here. I defined 'APPLE' in the makefile for OS X

	Remove the #ifdef #endif and play around with this - you should be starting
	an explicit version anyway, and some non-Apple drivers will require this too.
	*/
#ifdef APPLE
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	window = glfwCreateWindow(640, 640, "simple - kfjmbb - part c", NULL, NULL);

	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	/* start GLEW extension handler */
	glewExperimental = GL_TRUE;
	glewInit();
	renderer = glGetString(GL_RENDERER); /* get renderer string */
	version = glGetString(GL_VERSION);	 /* version as a string */
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);
	init();

	glfwSetKeyCallback(window, mykey);
	glfwSetMouseButtonCallback(window, mymouse);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	//


	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window

		glm::mat4 model = g_model;
		glm::vec3 eye(0.0f, 0.0f, g_zoom);
		glm::vec3 at(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0, 1.0f, 0.0f);
		glm::mat4 view = glm::lookAt(eye, at, up);

		glm::mat4 Projection = glm::perspective(glm::radians(g_fov), g_aspect, g_ncp, g_fcp);
		glm::mat4 mvp = Projection * view * model;

		glUniformMatrix4fv(mvpi, 1, GL_FALSE, glm::value_ptr(mvp));

		glDrawArrays(GL_TRIANGLES, 0, NumVertices);    // draw the points
													   /* update other events like input handling */
		glfwPollEvents();
		glfwSwapBuffers(window);
	} while (!glfwWindowShouldClose(window));

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;

}
