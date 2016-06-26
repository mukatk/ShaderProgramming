#define _USE_MATH_DEFINES

#include "maths_funcs.h"
#include <glew.h> // include GLEW and new version of GL on Windows
#include <glfw3.h> // GLFW helper library
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Mesh.h"
#include "Material.h"
#include "OBJReader.h"



using namespace std;

bool top = true;

enum objeto {
	Sonic = 0,
	Lua = 1
};

char* getPath(int enumObj) {
	char* path = "";
	switch (enumObj)
	{
	case Sonic:
		path = "Sonic/Sonic.obj";
		break;
	case Lua:
		path = "moon.obj";
		break;
	default:
		break;
	}
	return path;
}

int width = 640;
int height = 480;

int selectedModel = 0;

const char* vertex_shader =
"#version 400\n"

"layout(location = 0) in vec3 vertex_position;"
"layout(location = 1) in vec2 vt;"

"uniform mat4 view, proj, model;"

"out vec2 texture_coordinates;"
"void main() {"
	"texture_coordinates = vt;"
	"gl_Position = proj * view * model * vec4(vertex_position, 1.0);"
"}";

const char* fragment_shader =
"#version 400\n"
"in vec2 texture_coordinates;"
"uniform sampler2D basic_texture;"
"out vec4 frag_colour;"
"void main() {"
	"vec4 texel = texture(basic_texture, texture_coordinates);"
	"frag_colour = texel;"
"}";

Mesh* m = new Mesh();
vector<Material*> mat;
vector<Mesh*> models;

void reshape(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//void reshape(GLFWwindow* window, int w, int h) {
//	glMatrixMode(GL_PROJECTION_MATRIX);
//	glLoadIdentity();
//	gluPerspective(60, (float)w / (float)h, 1.0, 10000);
//	glViewport(0, 0, width = w, height = h);
//	glMatrixMode(GL_MODELVIEW_MATRIX);
//	glLoadIdentity();
//}

bool existsTexture(string textureName) {
	for (int i = 0; i < mat.size(); i++)
	{
		if (textureName == mat[i]->name && mat[i]->textureID > 0) {
			return true;
		}
	}
	return false;
}

void applyTexture() {
	// Create one OpenGL texture
	for (int l = 0; l < mat.size(); l++)
	{
		if (mat[l]->hasTexture()) {
			if (!existsTexture(mat[l]->name)) {
				GLuint textureID;
				glGenTextures(1, &textureID);
				mat[l]->textureID = textureID;
				glBindTexture(GL_TEXTURE_2D, mat[l]->textureID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mat[l]->img->getWidth(), mat[l]->img->getHeight(), 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, mat[l]->img->getPixels());
			}
		}
	}
}

void generateIDs(Mesh* m) {
	m->setTexturesIDs(mat);
	for (int j = 0; j < m->groups.size(); j++)
	{
		GLuint vbo = 0;
		glGenBuffers(1, &vbo);
		m->groups[j]->indexVBO = vbo;
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		m->groups[j]->indexVAO = vao;

		GLuint TextureVbo = 0;
		glGenBuffers(1, &TextureVbo);
		m->groups[j]->indexTextureVBO = TextureVbo;
	}
}

void generateIDs() {
	for (int i = 0; i < models.size(); i++)
	{
		models[i]->setTexturesIDs(mat);
		for (int j = 0; j < models[i]->groups.size(); j++)
		{
			GLuint vbo = 0;
			glGenBuffers(1, &vbo);
			models[i]->groups[j]->indexVBO = vbo;
			GLuint vao = 0;
			glGenVertexArrays(1, &vao);
			models[i]->groups[j]->indexVAO = vao;

			GLuint TextureVbo = 0;
			glGenBuffers(1, &TextureVbo);
			models[i]->groups[j]->indexTextureVBO = TextureVbo;
		}
	}
}

void bindBufferedData() {
	for (int i = 0; i < models.size(); i++)
	{
		for (int j = 0; j < models[i]->groups.size(); j++)
		{
			int vbo = models[i]->groups[j]->indexVBO;

			int vao = models[i]->groups[j]->indexVAO;

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			vector<float> v = models[i]->groups[j]->glfw_points;
			glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), &v[0], GL_STATIC_DRAW);

			glBindVertexArray(vao);
			// habilitado primeiro atributo do vbo bound atual
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo); // identifica vbo atual
												// associação do vbo atual com primeiro atributo
												// 0 identifica que o primeiro atributo está sendo definido
												// 3, GL_FLOAT identifica que dados são vec3 e estão a cada 3 float.
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			int TextureVbo = models[i]->groups[j]->indexTextureVBO;

			v = models[i]->groups[j]->glfw_texturePoints;

			glBindBuffer(GL_ARRAY_BUFFER, TextureVbo);
			glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), &v[0], GL_STATIC_DRAW);

			glBindVertexArray(vao);
			// habilitado primeiro atributo do vbo bound atual
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, TextureVbo); // identifica vbo atual
													   // associação do vbo atual com primeiro atributo
													   // 0 identifica que o primeiro atributo está sendo definido
													   // 3, GL_FLOAT identifica que dados são vec3 e estão a cada 3 float.
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		}
	}
}

void bindBufferedData(Mesh* m) {
	for (int j = 0; j < m->groups.size(); j++)
	{
		int vbo = m->groups[j]->indexVBO;

		int vao = m->groups[j]->indexVAO;

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		vector<float> v = m->groups[j]->glfw_points;
		glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), &v[0], GL_STATIC_DRAW);

		glBindVertexArray(vao);
		// habilitado primeiro atributo do vbo bound atual
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // identifica vbo atual
											// associação do vbo atual com primeiro atributo
											// 0 identifica que o primeiro atributo está sendo definido
											// 3, GL_FLOAT identifica que dados são vec3 e estão a cada 3 float.
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		int TextureVbo = m->groups[j]->indexTextureVBO;

		v = m->groups[j]->glfw_texturePoints;

		glBindBuffer(GL_ARRAY_BUFFER, TextureVbo);
		glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), &v[0], GL_STATIC_DRAW);

		glBindVertexArray(vao);
		// habilitado primeiro atributo do vbo bound atual
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, TextureVbo); // identifica vbo atual
													// associação do vbo atual com primeiro atributo
													// 0 identifica que o primeiro atributo está sendo definido
													// 3, GL_FLOAT identifica que dados são vec3 e estão a cada 3 float.
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}
}

void drawObject(int s_program) {
	for (int i = 0; i < models.size(); i++)
	{
		for (int j = 0; j < models[i]->groups.size(); j++)
		{
			GLint model_mat_location = glGetUniformLocation(s_program, "model");
			mat4 M = translate(identity_mat4(), vec3(models[i]->posX, models[i]->posY, models[i]->posZ));
			glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, M.m);

			int vao = models[i]->groups[j]->indexVAO;
			glBindTexture(GL_TEXTURE_2D, models[i]->groups[j]->indexMaterial);
			glBindVertexArray(vao);

			// draw points 0-3 from the currently bound VAO with current in-use shader
			glDrawArrays(GL_TRIANGLES, 0, models[i]->groups[j]->glfw_points.size());
			// update other events like input handling 
			glfwPollEvents();
		}
	}
}

void duplicateObject(int model) {
	m = new Mesh();
	OBJReader* objReader = new OBJReader();
	objReader->readObj(getPath(model), m, mat);

	m->generatePoints();

	generateIDs(m);
	bindBufferedData(m);
	models.push_back(m);
}

void keyboardFuncion(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action != GLFW_PRESS) return;

	switch (key)
	{
	case GLFW_KEY_SPACE:
		//duplicateObject(models[selectedModel]->indexObject);
		break;
	case GLFW_KEY_P:
		selectedModel++;
		break;
	case GLFW_KEY_O:
		selectedModel--;
		break;		
	}
}

void init(GLFWwindow* window) {
	glfwSetFramebufferSizeCallback(window, reshape);
	glfwSetKeyCallback(window, keyboardFuncion);
	glfwMakeContextCurrent(window);
	// inicia manipulador da extensão GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	// obtenção de versão suportada da OpenGL e renderizador
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL (versão suportada) %s\n", version);

	OBJReader* reader = new OBJReader();
	m->indexObject = Sonic;
	reader->readObj(getPath(m->indexObject), m, mat);
	models.push_back(m);

	m = new Mesh();
	m->indexObject = Lua;
	reader->readObj(getPath(m->indexObject), m, mat);
	models.push_back(m);

	applyTexture();
	for (int i = 0; i < models.size(); i++)
	{
		models[i]->generatePoints();
	}
	generateIDs();
}

void pseudoAIEvent() {
	float dy = (models[Sonic]->posZ - models[Lua]->posZ);
	float dx = (models[Sonic]->posX - models[Lua]->posX);
	if ((int)dx == 0) {
		models[Lua]->posZ += (dy > 0) ? 0.05f : -0.05f;
	}
	else {
		float m = (float)dy / dx;
		float b = models[Lua]->posZ - (m * models[Lua]->posX);
		models[Lua]->posX += (dx > 0) ? 0.05f : -0.05f;
		models[Lua]->posZ = (m * models[Lua]->posX) + b;
	}
}

int main() {
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}
	
	/* Caso necessário, definições específicas para SOs, p. e. Apple OSX *
	/* Definir como 3.2 para Apple OS X */
	/*glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/
	GLFWwindow *window = glfwCreateWindow(
		width, height, "Teste de versão OpenGL", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}

	init(window);

	GLuint s_program = glCreateProgram();	

	bindBufferedData();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);

	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);
	int params = -1;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		printf("ERROR: GL shader index %d did not compile\n", fs);
	}

	glAttachShader(s_program, fs);
	glAttachShader(s_program, vs);

	glLinkProgram(s_program);

	glGetProgramiv(s_program, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(
			stderr,
			"ERROR: could not link shader programme GL index %i\n",
			s_program
		);
		printf("\n%d\n", s_program);
	}

	/* create PROJECTION MATRIX */
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444
	// input variables
	float pnear = 0.1f; // clipping plane
	float pfar = 1000.0f; // clipping plane
	float fov = 67.0f * ONE_DEG_IN_RAD; // convert 67 degrees to radians
	float aspect = (float)width / (float)height; // aspect ratio
												 // matrix components
	float range = tan(fov * 0.5f) * pnear;
	float Sx = (2.0f * pnear) / (range * aspect + range * aspect);
	float Sy = pnear / range;
	float Sz = -(pfar + pnear) / (pfar - pnear);
	float Pz = -(2.0f * pfar * pnear) / (pfar - pnear);
	GLfloat proj_mat[] = {
		Sx, 0.0f, 0.0f, 0.0f,
		0.0f, Sy, 0.0f, 0.0f,
		0.0f, 0.0f, Sz, -1.0f,
		0.0f, 0.0f, Pz, 0.0f
	};

	/* create VIEW MATRIX */
	float cam_speed = 10.0f; // 1 unit per second
	float cam_yaw_speed = 20.0f; // 10 degrees per second
	float cam_pos[] = { 0.0f, 265.0f, 0.0f }; // don't start at zero, or we will be too close
	float cam_yaw = 0.0f; // y-rotation in degrees
	float cam_xaw = -90.0f;
	mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw);
	mat4 R2 = rotate_x_deg(identity_mat4(), -cam_xaw);
	mat4 view_mat = R * R2 * T;

	/* get location numbers of matrices in shader programme */
	GLint view_mat_location = glGetUniformLocation(s_program, "view");
	GLint proj_mat_location = glGetUniformLocation(s_program, "proj");
	/* use program (make current in state machine) and set default matrix values*/
	glUseProgram(s_program);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, proj_mat);

	/*------------------------------rendering loop--------------------------------*/
	/* some rendering defaults */
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
	
	while (!glfwWindowShouldClose(window)) {
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glUseProgram(s_program);

		drawObject(s_program);

		bool cam_moved = false;

		if (glfwGetKey(window, GLFW_KEY_R)) {                   //Resets everything
			cam_pos[0] = models[Sonic]->posX + 35.0f;
			cam_pos[1] = models[Sonic]->posY + 30.0f;
			cam_pos[2] = models[Sonic]->posZ - 35.0f;
			cam_yaw = 140.0f;
			cam_xaw = 0.0f;
			cam_moved = true;
			top = false;
		}

		if (glfwGetKey(window, GLFW_KEY_T)) {                   //Resets everything
			cam_pos[0] = 0.0f;
			cam_pos[1] = 265.0f;
			cam_pos[2] = 0.0f;
			cam_yaw = 0.0f;
			cam_xaw = -90.0f;
			cam_moved = true;
			top = true;
		}

		if (glfwGetKey(window, GLFW_KEY_A)) {
			cam_pos[0] -= cam_speed * elapsed_seconds;

			printf("\ndif pos[0]: %f", models[Sonic]->posX - cam_pos[0]);
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			cam_pos[0] += cam_speed * elapsed_seconds;

			printf("\ndif pos[0]: %f", models[Sonic]->posX - cam_pos[0]);
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_PAGE_UP)) {
			cam_pos[1] += cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN)) {
			cam_pos[1] -= cam_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_W)) {
			cam_pos[2] -= cam_speed * elapsed_seconds;

			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			cam_pos[2] += cam_speed * elapsed_seconds;

			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			cam_yaw += cam_yaw_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			cam_yaw -= cam_yaw_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_UP)) {
			cam_xaw += cam_yaw_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			cam_xaw -= cam_yaw_speed * elapsed_seconds;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_I)) {
			models[selectedModel]->posZ -= 0.2f;
			if (top == false) {
				cam_pos[0] = models[Sonic]->posX + 35.0f;
				cam_pos[1] = models[Sonic]->posY + 30.0f;
				cam_pos[2] = models[Sonic]->posZ - 35.0f;
				cam_yaw = 140.0f;
				cam_xaw = 0.0f;
				cam_moved = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_K)) {
			models[selectedModel]->posZ += 0.2f;
			if (top == false) {
				cam_pos[0] = models[Sonic]->posX + 35.0f;
				cam_pos[1] = models[Sonic]->posY + 30.0f;
				cam_pos[2] = models[Sonic]->posZ - 35.0f;
				cam_yaw = 140.0f;
				cam_xaw = 0.0f;
				cam_moved = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_N)) {
			models[selectedModel]->posY -= 0.2f;
			if (top == false) {
				cam_pos[0] = models[Sonic]->posX + 35.0f;
				cam_pos[1] = models[Sonic]->posY + 30.0f;
				cam_pos[2] = models[Sonic]->posZ - 35.0f;
				cam_yaw = 140.0f;
				cam_xaw = 0.0f;
				cam_moved = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_M)) {
			models[selectedModel]->posY += 0.2f;
			if (top == false) {
				cam_pos[0] = models[Sonic]->posX + 35.0f;
				cam_pos[1] = models[Sonic]->posY + 30.0f;
				cam_pos[2] = models[Sonic]->posZ - 35.0f;
				cam_yaw = 140.0f;
				cam_xaw = 0.0f;
				cam_moved = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_J)) {
			models[selectedModel]->posX -= 0.2f;
			if (top == false) {
				cam_pos[0] = models[Sonic]->posX + 35.0f;
				cam_pos[1] = models[Sonic]->posY + 30.0f;
				cam_pos[2] = models[Sonic]->posZ - 35.0f;
				cam_yaw = 140.0f;
				cam_xaw = 0.0f;
				cam_moved = true;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_L)) {
			models[selectedModel]->posX += 0.2f;
			if (top == false) {
				cam_pos[0] = models[Sonic]->posX + 35.0f;
				cam_pos[1] = models[Sonic]->posY + 30.0f;
				cam_pos[2] = models[Sonic]->posZ - 35.0f;
				cam_yaw = 140.0f;
				cam_xaw = 0.0f;
				cam_moved = true;
			}
		}
		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
			double x = 0.0f;
			double y = 0.0f;
			glfwGetCursorPos(window, &x, &y);
			models[selectedModel]->posX = (x * (450.0f / (float)width)) - 225;
			models[selectedModel]->posZ = (y * (344.0f / (float)height)) - 172;
		}
		/* update view matrix */
		if (cam_moved) {
			mat4 T = translate(identity_mat4(), vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2])); // cam translation
			mat4 R = rotate_y_deg(identity_mat4(), -cam_yaw); // 
			mat4 R2 = rotate_x_deg(identity_mat4(), -cam_xaw);
			mat4 view_mat = R * R2 * T;
			glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view_mat.m);
		}

		pseudoAIEvent();

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}