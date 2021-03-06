#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

//Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

GLuint VAO, VBO, shader, uniformModel;

float moveAngle = 0.0f;
float yPos	    = 0.0f;
float xPos		= 0.0f;

//Vertex Shader
static const char* vShader = "				\
	#version 330							\n\
											\n\
	layout(location = 0) in vec3 pos;		\n\
											\n\
	uniform mat4 model;						\n\
											\n\
	void main()								\n\
	{										\n\
		gl_Position = model * vec4(pos * 0.1, 1.0);			\n\
	}										\n\
";

//Fragment Shader
static const char* fShader = "				\
	#version 330							\n\
											\n\
	in vec4 gl_FragCoord;										\n\
											\n\
	out vec4 colour;							\n\
											\n\
	void main()								\n\
	{										\n\
		colour = vec4(gl_FragCoord.x, gl_FragCoord.y, gl_FragCoord.z, 1.0f);					\n\
	}										\n\
";

void AddShader(GLuint shaderProgram, const char* shaderCode, GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint shaderLength[1];
	shaderLength[0] = strlen(shaderCode);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glShaderSource(theShader, 1, theCode, shaderLength);
	glCompileShader(theShader);
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
		printf("Error compiling the %d shader: %s \n", shaderType, eLog);
		return;
	}

	glAttachShader(shaderProgram, theShader);
}

void CompileShader() {
	shader = glCreateProgram();

	if (!shader) {
		printf("Error on creating shader program");
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking shader program: %s \n", eLog);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating shader program: %s\n", eLog);
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");
}

void CreateTriangle() {
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	
	//Initialize GLFW
	if (!glfwInit()) {
		printf("GLFW initialization error");
		glfwTerminate();
		return 1;
	}

	//Setup GLFW window properties
	//OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//Core Profile = No backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Alow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Main Window", NULL, NULL);
	if (!mainWindow) {
		printf("GLFW window creation failed");
		glfwTerminate();
		return 1;
	}

	//Get buffer size
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//Set context to glew use
	glfwMakeContextCurrent(mainWindow);

	//Allow modern extension features
	glewExperimental = GL_TRUE;

	//Initialize GLEW
	if (glewInit() != GLEW_OK) {
		printf("GLEW initializaztion failed");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	//Creating the triangle and compiling shader
	CreateTriangle();
	CompileShader();

	//Loop until window closed
	while (!glfwWindowShouldClose(mainWindow)) {
		//Get + Handle user input events
		glfwPollEvents();

		xPos = cos(moveAngle) / 2;
		yPos = sin(moveAngle) / 2;
		moveAngle += 0.001f;

		//Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
			
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(xPos, yPos, 0.0f));
			model = glm::rotate(model, moveAngle * toRadians * 1000, glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(xPos, yPos, 0.0f));

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

			glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}