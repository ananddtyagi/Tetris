// This example is heavily based on the tutorial at https://open.gl

////////////////////////////////////////////////////////////////////////////////
#include <iostream>
// OpenGL Helpers to reduce the clutter
#include "helpers.h"
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
// Linear Algebra Library
#include <Eigen/Dense>
// Timer
#include <chrono>
////////////////////////////////////////////////////////////////////////////////

// VertexBufferObject wrapper
VertexBufferObject VBO;
VertexBufferObject VBO_C;


using namespace Eigen;

Matrix4f view(4,4);

// Contains the vertex positions
MatrixXf V(2,3);
MatrixXf C(3,3);
MatrixXf tempV(2,3);
MatrixXf tempC(3,3);

MatrixXf point(2,1);
MatrixXf midpoint(2,1);
MatrixXf clickpoint(2,1);
MatrixXf rotationclockwise(4,4);
MatrixXf rotationcounterclockwise(4,4);

std::string mode = "";
std::string rotate = "";
std::string scale = "";
std::string title = "";

int insertion = 0;
int triangletomove = -1;
int selectedtriangle = -1;
int triangletodelete = -1;
int triangletorotate = -1;
int triangletoscale = -1;
int numberoftriangles = 0;
int closestvertex = -1;

int closest(MatrixXf p, MatrixXf v1, MatrixXf v2, MatrixXf v3){
	float dist1 = std::sqrt((v1(0,0) - p(0,0))*(v1(0,0) - p(0,0)) + (v1(1,0) - p(1,0))*(v1(1,0) - p(1,0)));
	float dist2 = std::sqrt((v2(0,0) - p(0,0))*(v2(0,0) - p(0,0)) + (v2(1,0) - p(1,0))*(v2(1,0) - p(1,0)));
	float dist3 = std::sqrt((v3(0,0) - p(0,0))*(v3(0,0) - p(0,0)) + (v3(1,0) - p(1,0))*(v3(1,0) - p(1,0)));

	if (dist1 < dist2 && dist1 < dist3){
		return 0;
	}
	if(dist2 < dist1 && dist2 < dist3){
		return 1;
	}
	return 2;

}

bool inside(MatrixXf p, MatrixXf v1, MatrixXf v2, MatrixXf v3){
	Vector3d x1;
	Vector3d y1;
	Vector3d x2;
	Vector3d y2;
	Vector3d x3;
	Vector3d y3;


	x1 << v2(0,0) - v1(0,0), v2(1,0) - v1(1,0),0;
	y1 << p(0,0) - v1(0,0), p(1,0) - v1(1,0),0;

	x2 << v3(0,0) - v2(0,0), v3(1,0) - v2(1,0),0;
	y2 << p(0,0) - v2(0,0), p(1,0) - v2(1,0),0;

	x3 << v1(0,0) - v3(0,0), v1(1,0) - v3(1,0),0;
	y3 << p(0,0) - v3(0,0), p(1,0) - v3(1,0),0;


	if(x1.cross(y1)(2) < 0 && x2.cross(y2)(2) < 0 && x3.cross(y3)(2) < 0){
		return true;
	}
	if(x1.cross(y1)(2) > 0 && x2.cross(y2)(2) > 0 && x3.cross(y3)(2) > 0){
		return true;
	}

	return false;

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	// Get viewport size (canvas in number of pixels)
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	// Get the size of the window (may be different than the canvas size on retina displays)
	int width_window, height_window;
	glfwGetWindowSize(window, &width_window, &height_window);

	// Get the position of the mouse in the window
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Deduce position of the mouse in the viewport
	double highdpi = (double) width / (double) width_window;
	xpos *= highdpi;
	ypos *= highdpi;

	// Convert screen position to world coordinates
	// double xworld = ((xpos/double(width))*2)-1;
	// double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw
	Vector4f p_screen(xpos,height-1-ypos,0,1);
	Vector4f p_canonical((p_screen[0]/width)*2-1,(p_screen[1]/height)*2-1,0,1);
	Vector4f p_world = view.inverse()*p_canonical;

	// Update the position of the first vertex if the left button is pressed
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if(mode.compare("i") == 0){
			insertion+=1;
			if(insertion == 1){
				V.conservativeResize(2,V.cols()+3);
				C.conservativeResize(3,C.cols()+3);
				C.col(C.cols()-3) << 0,0,0;
				C.col(C.cols()-2) << 0,0,0;
				C.col(C.cols()-1) << 0,0,0;
				V.col(V.cols()-3) << p_world[0], p_world[1];
				V.col(V.cols()-2) << p_world[0], p_world[1];
				V.col(V.cols()-1) << p_world[0], p_world[1];
			}
			else if (insertion == 2){
				V.col(V.cols()-2) << p_world[0], p_world[1];
				V.col(V.cols()-1) << p_world[0], p_world[1];
			}
			else if (insertion == 3){
				V.col(V.cols()-1) << p_world[0], p_world[1];
			}

			if (insertion == 3){
				insertion = 0;
				mode = "";

			}
		}
		else if (mode.compare("c") == 0 && selectedtriangle != -1){
			point.col(0) << p_world[0], p_world[1];
			closestvertex = closest(point, V.col(selectedtriangle), V.col(selectedtriangle+1), V.col(selectedtriangle+2));

		}
		else if((mode.compare("o") == 0 && triangletomove == -1) || mode.compare("o") != 0 ){
			for(int i = V.cols(); i > 0; i = i - 3) {
				point.col(0) << p_world[0], p_world[1];
				if (inside(point, V.col(i-3), V.col(i-2), V.col(i-1))){
					clickpoint << p_world[0], p_world[1];
					triangletomove = i-3;
					selectedtriangle = i-3;
					tempC.col(0) << C.col(triangletomove);
					tempC.col(1) << C.col(triangletomove+1);
					tempC.col(2) << C.col(triangletomove+2);
					break;
				} else {
					// std::cout << "was not inside" << '\n';
					selectedtriangle = -1;
				}
			}
		}

	}



	if(mode.compare("o") == 0 && action != GLFW_PRESS){
		if(triangletomove != -1){
			C.col(triangletomove) << tempC.col(0);
			C.col(triangletomove+1) << tempC.col(1);
			C.col(triangletomove+2) << tempC.col(2);
			triangletomove=-1;
		}
	}

	if(mode.compare("p") == 0 && action != GLFW_PRESS){
		if(selectedtriangle != -1){
			tempV.resize(2,V.cols()-3);
			tempC.resize(3,C.cols()-3);

			if(selectedtriangle == V.cols()-3){
				tempV << V.block(0,0,2,selectedtriangle);
				tempC << C.block(0,0,3,selectedtriangle);

				V.resize(2,V.cols()-3);
				V << tempV;
				C.resize(3,C.cols()-3);
				C << tempC;

			} else {
				tempV << V.block(0,0,2,selectedtriangle) , V.block(0,selectedtriangle+3,2,V.cols());
				V.resize(2,V.cols()-3);
				V << tempV;
				tempC << C.block(0,0,3,selectedtriangle) , C.block(0,selectedtriangle+3,3,C.cols());
				C.resize(3,C.cols()-3);
				C << tempC;
			}
			selectedtriangle = -1;

		}
	}



	// Upload the change to the GPU
	VBO.update(V);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Update the position of the first vertex if the keys 1,2, or 3 are pressed
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	int width_window, height_window;
	glfwGetWindowSize(window, &width_window, &height_window);

	if(numberoftriangles == 0){
		mode = "";
		if (GLFW_KEY_I == key){
			mode = "i";
			selectedtriangle = -1;
			triangletomove = -1;
		}
	} else {
		switch (key) {
			case GLFW_KEY_I:
				mode = "i";
				selectedtriangle = -1;
				triangletomove = -1;
				break;
			case GLFW_KEY_O:
				mode = "o";
				if(insertion != 0){ //switched in the middle of insertion
					insertion = 0;
					V.conservativeResize(2,V.cols()-3);
					C.conservativeResize(3,C.cols()-3);
				}
				selectedtriangle = -1;
				triangletomove = -1;
				break;
			case GLFW_KEY_P:
				mode = "p";
				if(insertion != 0){ //switched in the middle of insertion
					insertion = 0;
					V.conservativeResize(2,V.cols()-3);
					C.conservativeResize(3,C.cols()-3);
				}
				break;
			case GLFW_KEY_C:
				mode = "c";
				if(insertion != 0){ //switched in the middle of insertion
					insertion = 0;
					V.conservativeResize(2,V.cols()-3);
					C.conservativeResize(3,C.cols()-3);
				}
				selectedtriangle = -1;
				triangletomove = -1;
				break;
			case GLFW_KEY_ESCAPE:
				mode = "";
				rotate = "";
				scale = "";
				selectedtriangle = -1;
				if(insertion != 0){ //switched in the middle of insertion
					insertion = 0;
					V.conservativeResize(2,V.cols()-3);
					C.conservativeResize(3,C.cols()-3);
				}
				break;
			default:
				break;
		}
	}

	// std::cout << selectedtriangle << "selectedtriangle" << '\n';
	if(key == GLFW_KEY_EQUAL && action == GLFW_PRESS){
		view(0,0) = view(0,0)*1.2;
		view(1,1) = view(1,1)*1.2;
	}
	if(key == GLFW_KEY_MINUS && action == GLFW_PRESS){
		view(0,0) = view(0,0)*0.8;
		view(1,1) = view(1,1)*0.8;

	}


	if(key == GLFW_KEY_W && action == GLFW_PRESS){
		view(1,3) =  view(1,3) - (0.2)*(height / height_window);

	}
	if(key == GLFW_KEY_S && action == GLFW_PRESS){
		view(1,3) =  view(1,3) + (0.2)*(height / height_window);

	}
	if(key == GLFW_KEY_A && action == GLFW_PRESS){
		view(0,3) =  view(0,3) + (0.2)*(width / width_window);

	}
	if(key == GLFW_KEY_D && action == GLFW_PRESS){
		view(0,3) =  view(0,3) - (0.2)*(width / width_window);

	}

	if(mode.compare("c") == 0 && selectedtriangle != -1){
		switch (key) {
			case GLFW_KEY_1:
				C(0, selectedtriangle + closestvertex) = 1;
				C(1, selectedtriangle + closestvertex) = 0;
				C(2, selectedtriangle + closestvertex) = 0;
				break;
			case GLFW_KEY_2:
				C(0, selectedtriangle + closestvertex) = 0;
				C(1, selectedtriangle + closestvertex) = 1;
				C(2, selectedtriangle + closestvertex) = 0;
				break;
			case GLFW_KEY_3:
				C(0, selectedtriangle + closestvertex) = 0;
				C(1, selectedtriangle + closestvertex) = 0;
				C(2, selectedtriangle + closestvertex) = 1;
				break;
			case GLFW_KEY_4:
				C(0, selectedtriangle + closestvertex) = 1;
				C(1, selectedtriangle + closestvertex) = 1;
				C(2, selectedtriangle + closestvertex) = 0;
				break;
			case GLFW_KEY_5:
				C(0, selectedtriangle + closestvertex) = 1;
				C(1, selectedtriangle + closestvertex) = 0;
				C(2, selectedtriangle + closestvertex) = 1;
				break;
			case GLFW_KEY_6:
				C(0, selectedtriangle + closestvertex) = 0;
				C(1, selectedtriangle + closestvertex) = 1;
				C(2, selectedtriangle + closestvertex) = 1;
				break;
			case GLFW_KEY_7:
				C(0, selectedtriangle + closestvertex) = 0.5;
				C(1, selectedtriangle + closestvertex) = 0.5;
				C(2, selectedtriangle + closestvertex) = 0.5;
				break;
			case GLFW_KEY_8:
				C(0, selectedtriangle + closestvertex) = 0.3;
				C(1, selectedtriangle + closestvertex) = 0.6;
				C(2, selectedtriangle + closestvertex) = 0.9;
				break;
			case GLFW_KEY_9:
				C(0, selectedtriangle + closestvertex) = 0.9;
				C(1, selectedtriangle + closestvertex) = 0.6;
				C(2, selectedtriangle + closestvertex) = 0.3;
				break;
			case GLFW_KEY_0:
				C.col(selectedtriangle + closestvertex) << 0,0,0;
			default:
				break;
		}
	}

	if(key == GLFW_KEY_H && action == GLFW_PRESS && mode.compare("o") == 0 && selectedtriangle != -1){
		midpoint << (V(0, selectedtriangle+1) + V(0, selectedtriangle+2)) / 2 , (V(1, selectedtriangle+1) + V(1, selectedtriangle+2))/2;
		midpoint << V(0, selectedtriangle) + 2 * (midpoint(0,0) - V(0, selectedtriangle)) / 3, V(1, selectedtriangle) + 2 * (midpoint(1,0) - V(1, selectedtriangle)) / 3;

		point.col(0) << V(0,selectedtriangle) - midpoint(0,0), V(1,selectedtriangle) - midpoint(1,0);

		V.col(selectedtriangle) << rotationclockwise * point + midpoint;

		point << V(0,selectedtriangle+1) - midpoint(0,0), V(1,selectedtriangle+1) - midpoint(1,0);
		V.col(selectedtriangle+1) << rotationclockwise * point + midpoint;

		point << V(0,selectedtriangle+2) - midpoint(0,0), V(1,selectedtriangle+2) - midpoint(1,0);
		V.col(selectedtriangle+2) << rotationclockwise * point + midpoint;

		VBO.update(V);

	}
	if(key == GLFW_KEY_J && action == GLFW_PRESS && mode.compare("o") == 0 && selectedtriangle != -1){
		midpoint << (V(0, selectedtriangle+1) + V(0, selectedtriangle+2)) / 2 , (V(1, selectedtriangle+1) + V(1, selectedtriangle+2))/2;
		midpoint << V(0, selectedtriangle) + 2 * (midpoint(0,0) - V(0, selectedtriangle)) / 3, V(1, selectedtriangle) + 2 * (midpoint(1,0) - V(1, selectedtriangle)) / 3;

		point.col(0) << V(0,selectedtriangle) - midpoint(0,0), V(1,selectedtriangle) - midpoint(1,0);

		V.col(selectedtriangle) << rotationcounterclockwise * point + midpoint;

		point << V(0,selectedtriangle+1) - midpoint(0,0), V(1,selectedtriangle+1) - midpoint(1,0);
		V.col(selectedtriangle+1) << rotationcounterclockwise * point + midpoint;

		point << V(0,selectedtriangle+2) - midpoint(0,0), V(1,selectedtriangle+2) - midpoint(1,0);
		V.col(selectedtriangle+2) << rotationcounterclockwise * point + midpoint;

		VBO.update(V);
	}
	if(key == GLFW_KEY_K && action == GLFW_PRESS && mode.compare("o") == 0 && selectedtriangle != -1){
		midpoint << (V(0, selectedtriangle+1) + V(0, selectedtriangle+2)) / 2 , (V(1, selectedtriangle+1) + V(1, selectedtriangle+2))/2;
		midpoint << V(0, selectedtriangle) + 2 * (midpoint(0,0) - V(0, selectedtriangle)) / 3, V(1, selectedtriangle) + 2 * (midpoint(1,0) - V(1, selectedtriangle)) / 3;

		point.col(0) << V(0,selectedtriangle) - midpoint(0,0), V(1,selectedtriangle) - midpoint(1,0);

		V.col(selectedtriangle) << 0.75*point + midpoint;

		point << V(0,selectedtriangle+1) - midpoint(0,0), V(1,selectedtriangle+1) - midpoint(1,0);
		V.col(selectedtriangle+1) << 0.75*point + midpoint;

		point << V(0,selectedtriangle+2) - midpoint(0,0), V(1,selectedtriangle+2) - midpoint(1,0);
		V.col(selectedtriangle+2) << 0.75*point + midpoint;

		VBO.update(V);
	}
	if(key == GLFW_KEY_L && action == GLFW_PRESS && mode.compare("o") == 0 && selectedtriangle != -1){
		midpoint << (V(0, selectedtriangle+1) + V(0, selectedtriangle+2)) / 2 , (V(1, selectedtriangle+1) + V(1, selectedtriangle+2))/2;
		midpoint << V(0, selectedtriangle) + 2 * (midpoint(0,0) - V(0, selectedtriangle)) / 3, V(1, selectedtriangle) + 2 * (midpoint(1,0) - V(1, selectedtriangle)) / 3;

		point.col(0) << V(0,selectedtriangle) - midpoint(0,0), V(1,selectedtriangle) - midpoint(1,0);

		V.col(selectedtriangle) << 1.25*point + midpoint;

		point << V(0,selectedtriangle+1) - midpoint(0,0), V(1,selectedtriangle+1) - midpoint(1,0);
		V.col(selectedtriangle+1) << 1.25*point  + midpoint;

		point << V(0,selectedtriangle+2) - midpoint(0,0), V(1,selectedtriangle+2) - midpoint(1,0);
		V.col(selectedtriangle+2) << 1.25*point  + midpoint;

		VBO.update(V);

	}
	// std::cout << "mode" << mode << '\n';


	// Upload the change to the GPU
	VBO.update(V);
}

int main(void) {
	// Initialize the GLFW library
	if (!glfwInit()) {
		return -1;
	}

	//Initializations
	V.resize(2,0);
	rotationclockwise <<
			cos(EIGEN_PI / 18), -sin(EIGEN_PI / 18), 0,0,
			sin(EIGEN_PI / 18), cos(EIGEN_PI / 18), 0,0,
			0,0,1,0,
			0,0,0,1;
	// rotationclockwise << cos(2* EIGEN_PI / 6), -sin(2 * EIGEN_PI / 6), sin(2 * EIGEN_PI / 6), cos(2 * EIGEN_PI / 6); //test rotation
	rotationcounterclockwise <<
			cos(35 * EIGEN_PI / 18), -sin(35 * EIGEN_PI / 18), 0,0,
			sin(35 * EIGEN_PI / 18), cos(35 * EIGEN_PI / 18), 0,0,
			0,0,1,0,
			0,0,0,1;


	// Activate supersampling
	glfwWindowHint(GLFW_SAMPLES, 8);

	// Ensure that we get at least a 3.2 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create a windowed mode window and its OpenGL context
	GLFWwindow * window = glfwCreateWindow(640, 480, "[Float] Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Load OpenGL and its extensions
	if (!gladLoadGL()) {
		printf("Failed to load OpenGL and its extensions");
		return(-1);
	}
	printf("OpenGL Version %d.%d loaded", GLVersion.major, GLVersion.minor);

	int major, minor, rev;
	major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
	minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
	rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
	printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
	printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
	printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Initialize the VAO
	// A Vertex Array Object (or VAO) is an object that describes how the vertex
	// attributes are stored in a Vertex Buffer Object (or VBO). This means that
	// the VAO is not the actual object storing the vertex data,
	// but the descriptor of the vertex data.
	VertexArrayObject VAO;
	VAO.init();
	VAO.bind();

	// Initialize the VBO with the vertices data
	// A VBO is a data container that lives in the GPU memory
	VBO.init();
	VBO_C.init();

	view <<
	1,0, 0, 0,
	0,1, 0, 0,
	0,0, 1, 0,
	0,0, 0, 1;


	C.resize(3,0);
	 // C <<
	 // 0,  0, 0,
	 // 0,  0, 0,
 	 // 0,  0, 0;
	VBO_C.update(C);

	  // V.resize(2,3);
	  // V << -0.8660254038, 0, 0.8660254038,  -0.5, 1, -0.5;
	 // std::cout << "INIT" << V << '\n';
	 VBO.update(V);


	// Initialize the OpenGL Program
	// A program controls the OpenGL pipeline and it must contains
	// at least a vertex shader and a fragment shader to be valid
	Program program;
	const GLchar* vertex_shader = R"(
		#version 150 core

		in vec2 position;

		uniform mat4 view;
		in vec3 color;
        out vec3 f_color;

		void main() {

			gl_Position = view * vec4(position, 0.0, 1.0);
			f_color = color;
		}
	)";

	const GLchar* fragment_shader = R"(
		#version 150 core
		in vec3 f_color;

		uniform vec3 triangleColor;
		out vec4 outColor;

		void main() {
			outColor = vec4(f_color, 1.0);
		}
	)";

	// Compile the two shaders and upload the binary to the GPU
	// Note that we have to explicitly specify that the output "slot" called outColor
	// is the one that we want in the fragment buffer (and thus on screen)
	program.init(vertex_shader, fragment_shader, "outColor");
	program.bind();

	// The vertex shader wants the position of the vertices as an input.
	// The following line connects the VBO we defined above with the position "slot"
	// in the vertex shader
	program.bindVertexAttribArray("position", VBO);
	program.bindVertexAttribArray("color",VBO_C);


	// Save the current time --- it will be used to dynamically change the triangle color
	auto t_start = std::chrono::high_resolution_clock::now();

	// Register the keyboard callback
	glfwSetKeyCallback(window, key_callback);

	// Register the mouse callback
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		title = "Mode: " + mode;
		glfwSetWindowTitle(window, title.c_str());
		// Set the size of the viewport (canvas) to the size of the application window (framebuffer)
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Bind your VAO (not necessary if you have only one)
		VAO.bind();

		// Bind your program
		program.bind();

		// Set the uniform value depending on the time difference
		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
		//glUniform3f(program.uniform("triangleColor"), (float)(sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);

		// std::cout << view << '\n';
		glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());


		glEnable(GL_DEPTH_TEST);
		// Clear the framebuffer

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// // Draw a triangle
		// glUniform3f(program.uniform("triangleColor"),1.0f, 0.0f, 0.0f);
		// glDrawArrays(GL_TRIANGLES, 0, 3);

		// Draw a triangle
		 glUniform3f(program.uniform("triangleColor"),0.0f, 0.0f, 1.0f);

		 if (V.size() >= 3)
		 {
		 glDrawArrays(GL_TRIANGLES, 0, V.cols());
		 glUniform3f(program.uniform("triangleColor"),0.0f, 0.0f, 0.0f);
		 glDrawArrays(GL_LINE_LOOP, V.cols()-3, 3);
	 	}

		glfwGetFramebufferSize(window, &width, &height);

		// Get the size of the window (may be different than the canvas size on retina displays)
		int width_window, height_window;
		glfwGetWindowSize(window, &width_window, &height_window);

		// Get the position of the mouse in the window
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// Deduce position of the mouse in the viewport
		double highdpi = (double) width / (double) width_window;
		xpos *= highdpi;
		ypos *= highdpi;

		// Convert screen position to world coordinates
		// double xworld = ((xpos/double(width))*2)-1;
		// double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw
		std::cout << xpos << '\n';
		Vector4f p_screen(xpos,height-1-ypos,0,1);
		Vector4f p_canonical((p_screen[0]/width)*2-1,(p_screen[1]/height)*2-1,0,1);
		Vector4f p_world = view.inverse()*p_canonical;


		if(mode.compare("i") == 0){
		 	if (insertion == 1){
				V.col(V.cols()-2) << p_world[0], p_world[1];
				V.col(V.cols()-1) << p_world[0], p_world[1];
			}
			else if (insertion == 2){
				V.col(V.cols()-1) << p_world[0], p_world[1];
			}
			VBO.update(V);
		}

		midpoint.resize(2,1);

		if(mode.compare("o") == 0 && triangletomove != -1){
			C.col(triangletomove) << 1,1,1;
			C.col(triangletomove+1) << 1,1,1;
			C.col(triangletomove+2) << 1,1,1;

			V.col(triangletomove) << V(0,triangletomove) + p_world[0] - clickpoint(0,0), V(1,triangletomove) + p_world[1] - clickpoint(1,0);
			V.col(triangletomove+1) << V(0,triangletomove+1) + p_world[0] - clickpoint(0,0), V(1,triangletomove+1) + p_world[1] - clickpoint(1,0);
			V.col(triangletomove+2) << V(0,triangletomove+2) + p_world[0] - clickpoint(0,0), V(1,triangletomove+2) + p_world[1] - clickpoint(1,0);
			clickpoint << p_world[0], p_world[1];


			VBO.update(V);
		}



		if(selectedtriangle == -1){ //have to fix this to prevent the triangle form jumping if I press "o" RIGHT after clicking the last insertion
			rotate = "";
			scale = "";
		}
		numberoftriangles = V.cols() / 3;

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();

		VBO_C.update(C);
		VBO.update(V);

	}

	// Deallocate opengl memory
	program.free();
	VAO.free();
	VBO.free();

	// Deallocate glfw internals
	glfwTerminate();

	// exit(0);
	return 0;
}
