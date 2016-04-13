#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



GLuint program;
GLint attribute_coord2d;
GLFWwindow* window;
unsigned char data[4];
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


// Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat lastX ;
GLfloat lastY ;
GLfloat alpha1 = 0.5f;
GLfloat alpha2 = 0.5f;
GLfloat hideAlpha1 = 1.0f;
GLfloat hideAlpha2 = 1.0f;
GLfloat colorChange = 0.0f;
GLfloat colorChange1 = 0.0f;
GLfloat numberAlpha1 = 0.0f;
GLfloat numberAlpha2 = 0.0f;
GLfloat fov = 45.0f;
GLfloat speed1 = 50.0f;
GLfloat speed2 = 50.0f;
GLboolean chooseStatus1;
GLboolean chooseStatus2;
bool keys[1024];

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

const char *vs_source =
"#version 410\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec4 color;\n"
"out vec4 ourColor;\n"
"uniform mat4 model;"
"uniform mat4 view; "
"uniform mat4 projection;"
"void main(void){"
"gl_Position = projection * view * model *vec4(position, 1.0);\n"
"ourColor = color;\n"
"}";

const char *fs_cource =
"#version 410\n"
"in vec4 ourColor;\n"
"out vec4 color;\n"
"void main(void){"
"color = vec4(ourColor);"
"}";


int init_resources()
{
	
	GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE)
	{

		GLint infoLogLength;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(vs, infoLogLength, NULL, strInfoLog);

		fprintf(stderr, "Compile failure in shader:\n%s\n", strInfoLog);
		delete[] strInfoLog;
		fprintf(stderr, "error in vertex error\n");
		return 0;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fs, 1, &fs_cource, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(fs, infoLogLength, NULL, strInfoLog);

		fprintf(stderr, "Compile failure in shader:\n%s\n", strInfoLog);
		delete[] strInfoLog;

		fprintf(stderr, "error in fragment shader\n");
		return 0;
	}

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok)
	{
		fprintf(stderr, "glinkprogram\n");
		return 0;
	}

	const char* attribute_name = "position";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1)
	{
		fprintf(stderr, "cannot bind attribute");
		return 0;
	}

	return 1;
}

void onDisplay()
{	
	
	glm::vec3 cubePositions[] = {
		glm::vec3(-1.0f, 0.0f, 1.0f),
		//glm::vec3(0.5f, -0.4f, 1.2f)		
	};

	glm::vec3 numberPositions[] = {
		glm::vec3(-0.8f, -1.5f, -1.0f),		
	};
	glm::vec3 numberPositions1[] = {
		glm::vec3(-0.8f, -1.2f, 0.5f),
	};
	/*
	GLfloat points[] = {
	0.0, 0.8, 1.5,
	0.8, -0.8, 0.0,
	-0.8, -0.8, 0.0,
	};
	*/
	// cube ///////////////////////////////////////////////////////////////////////
	//    v6----- v5
	//   /|      /|
	//  v1------v0|
	//  | |     | |
	//  | |v7---|-|v4
	//  |/      |/
	//  v2------v3
	//glDrawArrays产生cube
	GLfloat points[] = {
		
		/*0.5, 0.5, 0.5, 1.0f, 0.0f, 0.0f, 0.0f, -0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f, 0.0f, -0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, 0.0f,    // v0-v1-v2 (front)
		-0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, 0.0f, 0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, 0.0f, 0.5, 0.5, 0.5, 1.0f, 0.0f, 0.0f, 0.0f,     // v2-v3-v0

		0.5, 0.5, 0.5, 1.0f, 0.0f, 0.0f, 0.0f, 0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, 0.0f, 0.5, -0.5, -0.5, 1.0f, 1.0f, 0.0f, 0.0f,    // v0-v3-v4 (right)
		0.5, -0.5, -0.5, 1.0f, 1.0f, 0.0f, 0.0f, 0.5, 0.5, -0.5, 0.0f, 1.0f, 1.0f, 0.0f, 0.5, 0.5, 0.5, 1.0f, 0.0f, 0.0f, 0.0f,       // v4-v5-v0
		
		0.5, 0.5, 0.5, 1.0f, 0.0f, 0.0f, 0.0f, 0.5, 0.5, -0.5, 0.0f, 1.0f, 1.0f, 0.0f, -0.5, 0.5, -0.5, 1.0f, 0.5f, 1.0f, 0.0f,      // v0-v5-v6 (top)
		-0.5, 0.5, -0.5, 1.0f, 0.5f, 1.0f, 0.0f, -0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f, 0.0f, 0.5, 0.5, 0.5, 1.0f, 0.0f, 0.0f, 0.0f,      // v6-v1-v0
		
		-0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f, 0.0f, -0.5, 0.5, -0.5, 1.0f, 0.5f, 1.0f, 0.0f, -0.5, -0.5, -0.5, 0.5f, 0.0f, 1.0f, 0.0f,       // v1-v6-v7 (left)
		-0.5, -0.5, -0.5, 0.5f, 0.0f, 1.0f, 0.0f, -0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, 0.0f, -0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f, 0.0f,     // v7-v2-v1
		
		-0.5, -0.5, -0.5, 0.5f, 0.0f, 1.0f, 0.0f,	0.5, -0.5, -0.5, 1.0f, 1.0f, 0.0f, 0.0f,	 0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, 0.0f,     // v7-v4-v3 (bottom)
		0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, 0.0f, -0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, 0.0f, -0.5,		-0.5, -0.5, 0.5f, 0.0f, 1.0f, 0.0f,       // v3-v2-v7
		
		-0.5, -0.5, -0.5, 0.5f, 0.0f, 1.0f, 0.0f, 0.5, -0.5, -0.5, 1.0f, 1.0f, 0.0f, 0.0f, 0.5, 0.5, -0.5, 0.0f, 1.0f, 1.0f, 0.0f,     // V7-v4-v5 (back)
		0.5, 0.5, -0.5, 0.0f, 1.0f, 1.0f, 0.0f, -0.5, 0.5, -0.5, 1.0f, 0.5f, 1.0f, 0.0f, -0.5, -0.5, -0.5, 0.5f, 0.0f, 1.0f, 0.0f	// v5-v6-v7
		*/ 
		 0.5, 0.5, 0.5,  1.0f, 0.3f, 0.2f, alpha1,	//0
		 -0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f, alpha1,	//1            
		 -0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, alpha1,	//2           
		 0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, alpha1,	//3           
		 0.5, -0.5, -0.5, 1.0f, 1.0f, 0.0f, alpha1,	//4           
		 0.5, 0.5, -0.5, 0.0f, 1.0f, 1.0f, alpha1,	//5           
		 -0.5, 0.5, -0.5, 1.0f, 0.5f, 1.0f, alpha1,	//6         
		 -0.5, -0.5, -0.5, 0.5f, 0.0f, 1.0f, alpha1//7            
		
	};
	GLfloat points1[] = { 
		0.5, 0.5, 0.5, 1.0f, 0.1f, 0.2f, alpha2,	//0
		-0.5, 0.5, 0.5, 0.0f, 1.0f, 0.0f, alpha2,	//1            
		-0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, alpha2,	//2           
		0.5, -0.5, 0.5, 0.0f, 0.0f, 1.0f, alpha2,	//3           
		0.5, -0.5, -0.5, 1.0f, 1.0f, 0.0f, alpha2,	//4           
		0.5, 0.5, -0.5, 0.0f, 1.0f, 1.0f, alpha2,	//5           
		-0.5, 0.5, -0.5, 1.0f, 0.5f, 1.0f, alpha2,	//6         
		-0.5, -0.5, -0.5, 0.5f, 0.0f, 1.0f, alpha2//7       
	};
	GLfloat points2[] = {
		0.5, 0.5, 0.5, 1.0f-colorChange, 0.0f , 0.0f + colorChange, hideAlpha1,	//0
		-0.5, 0.5, 0.5, 1.0f - colorChange, 0.0f, 0.0f + colorChange, hideAlpha1,	//1            
		-0.5, -0.5, 0.5, 1.0f - colorChange, 0.0f , 0.0f, hideAlpha1,	//2           
		0.5, -0.5, 0.5, 1.0f - colorChange, 0.0f , 0.0f + colorChange, hideAlpha1,	//3           
		0.5, -0.5, -0.5, 1.0f - colorChange, 0.0f , 0.0f, hideAlpha1,	//4           
		0.5, 0.5, -0.5, 1.0f - colorChange, 0.0f , 0.0f + colorChange, hideAlpha1,	//5           
		-0.5, 0.5, -0.5, 1.0f - colorChange, 0.0f, 0.0f , hideAlpha1,	//6         
		-0.5, -0.5, -0.5, 1.0f - colorChange, 0.0f , 0.0f + colorChange, hideAlpha1//7       
	};
	GLfloat points3[] = {
		0.5, 0.5, 0.5, 0.0f, 1.0f - colorChange1, 0.0f + colorChange1, hideAlpha2,	//0
		-0.5, 0.5, 0.5, 0.0f + colorChange1, 1.0f, 0.0f + colorChange1, hideAlpha2,	//1            
		-0.5, -0.5, 0.5, 0.0f, 1.0f - colorChange1, 0.0f + colorChange1, hideAlpha2,	//2           
		0.5, -0.5, 0.5, 0.0f + colorChange1, 1.0f, 0.0f + colorChange1, hideAlpha2,	//3           
		0.5, -0.5, -0.5, 0.0f, 1.0f - colorChange1, 0.0f + colorChange1, hideAlpha2,	//4           
		0.5, 0.5, -0.5, 0.0f + colorChange1, 1.0f, 0.0f + colorChange1, hideAlpha2,	//5           
		-0.5, 0.5, -0.5, 0.0f, 1.0f - colorChange1, 0.0f + colorChange1, hideAlpha2,	//6         
		-0.5, -0.5, -0.5, 0.0f + colorChange1, 1.0f, 0.0f + colorChange1, hideAlpha2//7       
	};
	GLfloat numbers1[] = {
		-0.5, 0.5, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//0
		0.5, 0.5, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//1   
		-0.5, 0.3, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//2   	
		0.3, 0.5, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//3
		0.5, 0.3, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//4 
		-0.5, 0.1, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//5  
		0.3, 0.1, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//6
		0.5, 0.1, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//7 
		-0.5, -0.1, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1, //8
		-0.3, -0.1, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//9
		0.5, -0.1, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//10
		-0.5, -0.3, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//11
		-0.3, -0.3, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//12
		0.5, -0.3, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//13
		-0.5, -0.5, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//14
		0.5, -0.5, 0.0, 1.0f, 0.3f, 0.2f, numberAlpha1,	//15
	};
	
	
	GLfloat numbers2[] = {
		-0.1, 0.45, 0.0, 1.0f, 0.1f, 0.2f, numberAlpha2,	//0
		0.1, 0.45, 0.0, 1.0f, 0.1f, 0.2f, numberAlpha2,	//1   
		0.1, -0.45, 0.0, 1.0f, 0.1f, 0.2f, numberAlpha2,	//2   	
		-0.1, -0.45, 0.0, 1.0f, 0.1f, 0.2f, numberAlpha2,//3	

	};
	GLuint indices[] = {
		0, 1, 2, 2, 3, 0,
		0, 3, 4, 4, 5, 0,
		0, 5, 6, 6, 1, 0,
		1, 6, 7, 7, 2, 1,
		7, 4, 3, 3, 2, 7,
		7, 4, 5, 5, 6, 7
	};
	GLuint indices1[] = {
		0, 1, 2, 2, 3, 0,
		0, 3, 4, 4, 5, 0,
		0, 5, 6, 6, 1, 0,
		1, 6, 7, 7, 2, 1,
		7, 4, 3, 3, 2, 7,
		7, 4, 5, 5, 6, 7
	};
	GLuint numberIndices[] = {
		0, 1, 4, 4, 2, 0,
		3, 4, 7, 7, 6, 3,
		5, 7, 10, 10, 8, 5,
		8, 9, 12, 12, 11, 8,
		11, 13, 15, 15, 14, 11
	};
	GLuint numberIndices1[] = {
		0, 1, 2, 2, 3, 0
	};
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);//创建VBO(顶点缓冲区对象)需要3个步骤：1.使用glGenBuffers()生成新缓存对象。2.使用glBindBuffer()绑定缓存对象。3.使用glBufferData()将顶点数据拷贝到缓存对象中。
	glBindBuffer(GL_ARRAY_BUFFER, vbo);//创建缓冲区对象
	glBufferData(GL_ARRAY_BUFFER, sizeof(points2), points2, GL_STATIC_DRAW);//分配size个存储单位（通常是字节）的OpenGL服务器内存，用于存储顶点数据或索引。以前所有与当前绑定对象相关联的数据都将删除
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);//①当buffer是一个首次使用的非零无符号整数时，它就创建一个新的缓冲区对象，并把buffer分配给这个缓冲区对象，作为它的名称。②当绑定到一个以前创建的缓冲区对象时，这个缓冲区对象便成为活动的缓冲区对象。③当绑定到一个值为零的buffer时，OpenGL就会停止使用缓冲区对象。
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7* sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	
	GLuint cubevbo = 0;
	glGenBuffers(1, &cubevbo);//创建VBO(顶点缓冲区对象)需要3个步骤：1.使用glGenBuffers()生成新缓存对象。2.使用glBindBuffer()绑定缓存对象。3.使用glBufferData()将顶点数据拷贝到缓存对象中。
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo);//创建缓冲区对象
	glBufferData(GL_ARRAY_BUFFER, sizeof(points3), points3, GL_STATIC_DRAW);//分配size个存储单位（通常是字节）的OpenGL服务器内存，用于存储顶点数据或索引。以前所有与当前绑定对象相关联的数据都将删除
	GLuint cubevao = 0;
	glGenVertexArrays(1, &cubevao);
	glBindVertexArray(cubevao);
	glBindBuffer(GL_ARRAY_BUFFER, cubevbo);//①当buffer是一个首次使用的非零无符号整数时，它就创建一个新的缓冲区对象，并把buffer分配给这个缓冲区对象，作为它的名称。②当绑定到一个以前创建的缓冲区对象时，这个缓冲区对象便成为活动的缓冲区对象。③当绑定到一个值为零的buffer时，OpenGL就会停止使用缓冲区对象。
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	GLuint numbervbo = 0;
	glGenBuffers(1, &numbervbo);
	glBindBuffer(GL_ARRAY_BUFFER, numbervbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(numbers1), numbers1, GL_STATIC_DRAW);
	GLuint numbervao = 0;
	glGenVertexArrays(1, &numbervao);
	glBindVertexArray(numbervao);
	glBindBuffer(GL_ARRAY_BUFFER, numbervao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);


	GLuint numbervbo1 = 0;
	glGenBuffers(1, &numbervbo1);
	glBindBuffer(GL_ARRAY_BUFFER, numbervbo1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(numbers2), numbers2, GL_STATIC_DRAW);
	GLuint numbervao1 = 0;
	glGenVertexArrays(1, &numbervao1);
	glBindVertexArray(numbervao1);
	glBindBuffer(GL_ARRAY_BUFFER, numbervao1);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
	

	GLfloat alphaSpeed = 5.0f * deltaTime;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)){
		






	}
	
	// Camera/View transformation
	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	// Projection 
	glm::mat4 projection;
	glm::mat4 model;

	projection = glm::perspective(fov, (GLfloat)640 / (GLfloat)480, 0.1f, 100.0f);
	// Get the uniform locations
	GLint modelLoc = glGetUniformLocation(program, "model");
	GLint viewLoc = glGetUniformLocation(program, "view");
	GLint projLoc = glGetUniformLocation(program, "projection");


	// Pass the matrices to the shader

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));




	glUseProgram(program);
	glBindVertexArray(vao);
	
	
	model = glm::rotate(model, (GLfloat)glfwGetTime() * speed1, glm::vec3(0.3f, 1.0f, 0.0f));
	view = glm::translate(view, glm::vec3(0.8f, 0.0f, -4.0f));
	
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices);
	glBindVertexArray(0);
	
	glBindVertexArray(cubevao);
	for (GLuint i = 0; i < 1; i++)
	{
		glm::mat4 model;
		model = glm::translate(model, cubePositions[i]);
		GLfloat angle = 20.0f * i;
		model = glm::rotate(model, (GLfloat)glfwGetTime() * speed2, glm::vec3(0.3f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, indices1);
	}
	glBindVertexArray(0);


	glBindVertexArray(numbervao);
	for (GLuint i = 0; i < 1; i++)
	{
		glm::mat4 model;
		model = glm::translate(model, numberPositions[i]);
		GLfloat angle = 20.0f * i;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, numberIndices);
	}
	glBindVertexArray(0);	
	
	glBindVertexArray(numbervao1);

	for (GLuint i = 0; i < 1; i++)
	{
		glm::mat4 model;
		model = glm::translate(model, numberPositions1[i]);
		GLfloat angle = 20.0f * i;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, numberIndices1);
	}
	glBindVertexArray(0);
	
	
	glFlush();
	glFinish();


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//读取鼠标所在像素点颜色
	glReadPixels(lastX, lastY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

	

	//glDrawArrays(GL_TRIANGLES, 0, 36);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)){
		int r = data[0];
		int  g = data[1];
		int  b = data[2];
	
		if (r==255)
		{
			//
			//hideAlpha2 = 0.5f;
			numberAlpha2 = 1.0f;
			numberAlpha1 = 0.0f;
			colorChange = 1.0f;
			colorChange1 = 0.0f;
		}
		if (g == 255)
		{
			//hideAlpha1 = 0.5f;
			numberAlpha1 = 1.0f;
			numberAlpha2 = 0.0f;
			colorChange1 = 1.0f;
			colorChange = 0.0f;
		}
		if (r==0 && g==0 && b==0)
		{
			//hideAlpha1 = 1.0f;
			//hideAlpha2 = 1.0f;
			numberAlpha2 = 0.0f;
			numberAlpha1 = 0.0f;
			colorChange = 0.0f;
			colorChange1 = 0.0f;
		}
	}
	
}
void free_resources()
{
	glDeleteProgram(program);
}

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}


int main(void)
{

	
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK)
	{
		//      throw std::runtime_error("glew fails to start.");
		fprintf(stderr, "glew error\n");
	}



	// Define the viewport dimensions
	glViewport(0, 0, 640, 480);


	//加载鼠标和键盘响应
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 640/ 2, 480/ 2);



	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	
	//混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//背面
	//glEnable(GL_CULL_FACE);
//	glCullFace(GL_FRONT);
	//glCullFace(GL_BACK);
	

	GLint maxV;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxV);
	fprintf(stderr, "maxv: %d\n", maxV);

	init_resources();


	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		do_movement();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		onDisplay();
	
		
		glfwSwapBuffers(window);
		

	}
	free_resources();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}
void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		cameraPos += cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_S])
		cameraPos -= cameraSpeed * cameraFront;
	if (keys[GLFW_KEY_A])
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (keys[GLFW_KEY_D])
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	
}



bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	lastX = xpos;
	lastY = ypos;
	
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

