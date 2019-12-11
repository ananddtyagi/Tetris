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

bool begun = false;

// VertexBufferObject wrapper
VertexBufferObject VBO;
VertexBufferObject VBO_C;


int shapecols = 8*3; // 8 triangles per shape, 3 vertices each.

// Contains the vertex positions
Eigen::MatrixXf background(2,6);
Eigen::MatrixXf scene(2,6);
Eigen::MatrixXf tempscene(2,shapecols);
// Eigen::MatrixXf view(4,4);


Eigen::MatrixXi board(10,20);
Eigen::MatrixXi lastboard(10,20);

Eigen::MatrixXf currentsquares(4,2);
Eigen::MatrixXf tempsquares(4,2);

Eigen::MatrixXf currentshape(2,shapecols);
Eigen::MatrixXf tempshape(2,6);



Eigen::MatrixXf color(3,6);
Eigen::MatrixXf shapecolor(3,shapecols);


Eigen::MatrixXf fall(4,2);
Eigen::MatrixXf left(4,2);
Eigen::MatrixXf right(4,2);



int inround = 0;
double speed = 1;

int width, height;

int width_window, height_window;


//for collisions, 1 means will collide, 0 means safe

void updatescene(){

	std::cout << board << '\n';

	tempsquares << currentsquares(0,0) * 0.2 - 1, currentsquares(0,1) * 0.1 - 1,
								 currentsquares(1,0) * 0.2 - 1, currentsquares(1,1) * 0.1 - 1,
								 currentsquares(2,0) * 0.2 - 1, currentsquares(2,1) * 0.1 - 1,
								 currentsquares(3,0) * 0.2 - 1, currentsquares(3,1) * 0.1 - 1;

	currentshape.resize(2,shapecols);
	for(int i = 0; i < 4; i++){ //enter one square at a time
		tempshape << tempsquares(i,0), tempsquares(i,0), tempsquares(i,0) + 0.2, tempsquares(i,0), tempsquares(i,0) + 0.2, tempsquares(i,0) + 0.2,
								tempsquares(i,1), tempsquares(i,1)+0.1, tempsquares(i,1), tempsquares(i,1)+0.1, tempsquares(i,1), tempsquares(i,1) + 0.1,
		currentshape << currentshape.block(0,0,2,i*6), tempshape;
	}



	scene << scene.block(0,0,2,scene.cols()-shapecols) , currentshape;

}



int shapecollisionleft(){
	if(currentsquares(0,0) - 1 < 0 || currentsquares(1,0) - 1 < 0 || currentsquares(2,0) - 1 < 0 || currentsquares(3,0) - 1 < 0){
		return 1;
	}
	for(int i = 0; i < 4; i++){
		 if(lastboard(currentsquares(i,0)-1, currentsquares(i,1)) == 1){
			 return 1;
		 }
	}

	return 0;
}

int shapecollisionright(){
	if(currentsquares(0,0) + 1 >= 10 || currentsquares(1,0) + 1 >= 10 || currentsquares(2,0) + 1 >= 10 || currentsquares(3,0) + 1 >= 10){
		return 1;
	}
	for(int i = 0; i < 4; i++){
		 if(lastboard(currentsquares(i,0)+1, currentsquares(i,1)) == 1){
			 return 1;
		 }
	}

	return 0;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	// Get viewport size (canvas in number of pixels)

	// Get the size of the window (may be different than the canvas size on retina displays)

	// Get the position of the mouse in the window
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Deduce position of the mouse in the viewport
	double highdpi = (double) width / (double) width_window;
	xpos *= highdpi;
	ypos *= highdpi;

	// Convert screen position to world coordinates
	double xworld = ((xpos/double(width))*2)-1;
	double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw

	// Upload the change to the GPU
	updatescene();

	VBO.update(scene);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Update the position of the first vertex if the keys 1,2, or 3 are pressed
	if(key == GLFW_KEY_B){
		begun = true;
	}
	if(key == GLFW_KEY_LEFT && action == GLFW_PRESS && inround == 1){

		if(shapecollisionleft() == 0){ //did not collide
			currentsquares <<	currentsquares + left;
		}
		updatescene();
		VBO.update(scene);

	}
	if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS && inround == 1){

			if(shapecollisionright() == 0){
				currentsquares <<	currentsquares + right;
			}
			updatescene();


		VBO.update(scene);

	}
	if(key == GLFW_KEY_DOWN && action == GLFW_PRESS && inround == 1){

			speed += + 0.1;
			std::cout << speed << '\n';
			updatescene();


		VBO.update(scene);

	}


	// Upload the change to the GPU
	VBO.update(scene);
}




void createshape(){ //craetes a new shape and sets it as the current shape.

	//square
  currentsquares << 0, 19,
										0, 20,
									 	1, 19,
										1, 20;
	scene.conservativeResize(2,scene.cols() + shapecols);

	color.conservativeResize(3, color.cols() + shapecols);
	shapecolor << 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
								0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
								0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0;
	tempscene << 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
								0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0;

	scene << scene.block(0,0,2,scene.cols()-shapecols), tempscene;
	color << color.block(0,0,3,color.cols()-shapecols), shapecolor;

	speed = 1;
}

void movedown(){
	currentsquares = currentsquares + 4*fall;
	// currentsquares = currentsquares + speed*fall;

}

void update(){

	board << lastboard;
	board(currentsquares(0,0),currentsquares(0,1)) = 1;
	board(currentsquares(1,0),currentsquares(1,1)) = 1;
	board(currentsquares(2,0),currentsquares(2,1)) = 1;
	board(currentsquares(3,0),currentsquares(3,1)) = 1;

}

int checkboard(){ //1 = did NOT hit, 0 = hit
		for (int i = 0; i < 4; i++){
			if(lastboard(currentsquares(i,0),currentsquares(i,1)-1) == 1){ //if this next move down in that column will be a collision, end round
				return 0;
			}
			if(currentsquares(i,1) == 0){
				return 0;
			}
		}

return 1;
}

void won(){

	for(int i = 0; i < 20; i++){
		for(int j = 0; j < 10; j++){
			if(board(j,i) == 0){ //not a complete row
				break;
			}
			if(j < 9){ //not at the end of the row yet, but everything so far is a 1.
				continue;
			}
			//CLEAR ROW I AND UPDATE BOARD
			board.col(i) << 0,0,0,0,0,0,0,0,0,0;
			updatescene();
			//HAVE TO REGENERATE SCENE EVERYTIME
		}
	}

}

int main(void) {
	// Initialize the GLFW library
	if (!glfwInit()) {
		return -1;
	}

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

	// Don't allow the user to change the size of the window
	// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Create a windowed mode window and its OpenGL context

		GLFWwindow * window = glfwCreateWindow(400, 800, "Tetris", NULL, NULL);

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

	color.resize(3,6);

	background.resize(2,6);

	background << -1, -1, 1, 1, -1, 1,
			 					-1,  1,-1, 1,  1,-1;

  color << 1,1,1, 1,1,1,
					1,1,1, 1,1,1,
					1,1,1, 1,1,1;

	scene.resize(2,6);

	scene << 0,0,0,0,0,0,
					0,0,0,0,0,0;

  VBO_C.update(color);

	VBO.update(scene);

	// Initialize the OpenGL Program
	// A program controls the OpenGL pipeline and it must contains
	// at least a vertex shader and a fragment shader to be valid
	Program program;
	const GLchar* vertex_shader = R"(
		#version 150 core
		in vec2 position;

		in vec3 color;
			out vec3 f_color;

		void main() {
			gl_Position = vec4(position, 0.0, 1.0);
			f_color = color;

		}
	)";

	const GLchar* fragment_shader = R"(
		#version 150 core
		in vec3 f_color;


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

	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwGetWindowSize(window, &width_window, &height_window);

	int starttime = 0;
	int prevtime = 0;
	int initializeround = 0;

	fall << 0,-1,
					0,-1,
					0,-1,
					0,-1;

	left << -1, 0,
					-1, 0,
					-1, 0,
					-1, 0;

	right << 1, 0,
					 1, 0,
					 1, 0,
					 1, 0;


	//THE ISSUE IS THAT THE SCALING IS OFF, I NEED TO MAKE A 10X20 BOARD
	board << 0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0;

	lastboard << 0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0,
					0,0,0,0,0,0,0,0,0,0;

	tempsquares << 0,0,0,0,
								 0,0,0,0;

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		// Set the size of the viewport (canvas) to the size of the application window (framebuffer)


		// Bind your VAO (not necessary if you have only one)
		VAO.bind();

		// Bind your program
		program.bind();

		// Set the uniform value depending on the time difference
		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		// float aspect_ratio = float(height)/float(width); // corresponds to the necessary width scaling
		//
    //     view <<
    //     aspect_ratio,0, 0, 0,
    //     0,           1, 0, 0,
    //     0,           0, 1, 0,
    //     0,           0, 0, 1;
		//
    //     glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, view.data());

		// Clear the framebuffer
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		if (begun){

			if(starttime == 0){
				scene.resize(2,6);
				scene << background;


				starttime = time;
				prevtime = starttime;
			}


			if(inround == 0 && time - prevtime > 1){
				inround = 1;
				initializeround = 1;
			}

			if(initializeround){
				createshape();
				initializeround = 0;
			}


			if (inround == 1 && time - prevtime > 1){ //wait one second after a round finishes before lauching the next shape
				if(checkboard() == 0){

					won();
					lastboard << board;

					inround = 0;
				} else {
					movedown();
					update();
				}

				prevtime = time; //will only update after each time the shape moves, which is every second
			}
			if(scene.cols() > 6){
				updatescene();

			}
			glDrawArrays(GL_TRIANGLES, 0, scene.cols());

		}



		VBO_C.update(color);
		VBO.update(scene);


		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Deallocate opengl memory
	program.free();
	VAO.free();
	VBO.free();

	// Deallocate glfw internals
	glfwTerminate();
	return 0;
}
