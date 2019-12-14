#include <iostream>
// OpenGL Helpers to reduce the clutter
#include "helpers.h"
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>
// Linear Algebra Library
#include <Eigen/Dense>
// Timer
#include <chrono>
#include <thread>
//Random number
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

////////////////////////////////////////////////////////////////////////////////

bool begun = false;
bool paused = false;
bool gaemover = false;
// VertexBufferObject wrapper
VertexBufferObject VBO;
VertexBufferObject VBO_C;
VertexBufferObject VBO_T;



int shapecols = 8*3; // 8 triangles per shape, 3 vertices each.

//Texture
Eigen::MatrixXf texturecoor(2,6);

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
int score = 0;
int shape = 0;
int orientation = 0;

int width, height;

int width_window, height_window;


//for collisions, 1 means will collide, 0 means safe

void updatescene(){

	scene.conservativeResize(2, 6+(200*2*3)); //6 for the background and then 200 squares, 2 triangles per square, 3 points per triangle.
	scene << scene.block(0,0,2,6), Eigen::MatrixXf::Zero(2, 200*2*3); //reset scene, keep the background

	//Redraw the entire scene
	for(int i = 0; i < 10; i++){
		for(int  j = 0; j < 20; j++){
			if(board(i,j) == 1){
				scene(0, 6+(60*j+6*i))   = i * 0.2 - 1; 				scene(1, 6+(60*j+6*i))   = j * 0.1 - 1;
				scene(0, 6+(60*j+6*i)+1) = i * 0.2 - 1; 				scene(1, 6+(60*j+6*i)+1) = j * 0.1 - 1 + 0.1;
				scene(0, 6+(60*j+6*i)+2) = i * 0.2 - 1 + 0.2;		scene(1, 6+(60*j+6*i)+2) = j * 0.1 - 1;
				scene(0, 6+(60*j+6*i)+3) = i * 0.2 - 1;					scene(1, 6+(60*j+6*i)+3) = j * 0.1 - 1 + 0.1;
				scene(0, 6+(60*j+6*i)+4) = i * 0.2 - 1 + 0.2; 	scene(1, 6+(60*j+6*i)+4) = j * 0.1 - 1;
				scene(0, 6+(60*j+6*i)+5) = i * 0.2 - 1 + 0.2; 	scene(1, 6+(60*j+6*i)+5) = j * 0.1 - 1 + 0.1;
			} else {
				scene(0, 6+(60*j+6*i))   = 0; 									scene(1, 6+(60*j+6*i))   = 0;
				scene(0, 6+(60*j+6*i)+1) = 0; 									scene(1, 6+(60*j+6*i)+1) = 0;
				scene(0, 6+(60*j+6*i)+2) = 0;										scene(1, 6+(60*j+6*i)+2) = 0;
				scene(0, 6+(60*j+6*i)+3) = 0;										scene(1, 6+(60*j+6*i)+3) = 0;
				scene(0, 6+(60*j+6*i)+4) = 0; 									scene(1, 6+(60*j+6*i)+4) = 0;
				scene(0, 6+(60*j+6*i)+5) = 0; 									scene(1, 6+(60*j+6*i)+5) = 0;
			}
		}
	}

	VBO.update(scene);
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

void update(){
	board << lastboard;
	for(int i = 0; i < 4; i++){
		if(currentsquares(i, 1) < 20){
			board(currentsquares(i,0),currentsquares(i,1)) = 1;
		} else {
			if(checkboard() == 0){ //hitting a block on the top
				begun = false;
				gaemover = true;
			}
		}
	}
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

void createshape(){ //craetes a new shape and sets it as the current shape.
	shape = rand() % 7 + 1;
	switch(shape){
		case 1:
			//square
		  currentsquares << 0, 20,
												0, 21,
											 	1, 20,
												1, 21;
			break;
		case 2:
			//T
			currentsquares << 0, 20,
												1, 21,
												1, 20,
												2, 20;
			break;
		case 3:
			//I
			currentsquares << 0, 20,
												0, 21,
												0, 22,
												0, 23;
		break;
		case 4:
			//Backwards L
			currentsquares << 0, 20,
												0, 21,
												0, 22,
												1, 22;
		break;
		case 5:
			//L
			currentsquares << 0, 20,
												1, 20,
												0, 21,
												0, 22;
		break;
		case 6:
			//S
			currentsquares << 0, 20,
												1, 20,
												1, 21,
												2, 21;
		break;
		case 7:
			//Z
			currentsquares << 0, 21,
												1, 21,
												1, 20,
												2, 20;
		break;
	}


	speed = 1;
}

void movedown(){

	currentsquares = currentsquares + fall;
	// currentsquares = currentsquares + speed*fall;

}



int won(){

	for(int j = 19; j >= 0; j--){
		for(int i = 0; i < 10; i++){
			if(board(i,j) == 0){ //not a complete row
				break;
			}
			if(i < 9){ //not at the end of the row yet, but everything so far is a 1.
				continue;
			}

			//CLEAR ROW I AND UPDATE BOARD (NEED TO MOVE EVERYTHING DOWN)
			return j;
		}
	}
	return -1;
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
	if(key == GLFW_KEY_P && action == GLFW_PRESS){
		paused = !paused;
	}

	if(!paused){
		if(key == GLFW_KEY_LEFT && action == GLFW_PRESS && inround == 1){
			if(shapecollisionleft() == 0){ //did not collide
				currentsquares <<	currentsquares + left;
			}
			update();
			updatescene();

			VBO.update(scene);

		}
		if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS && inround == 1){

				if(shapecollisionright() == 0){
					currentsquares <<	currentsquares + right;
				}
				update();
				updatescene();


			VBO.update(scene);

		}
		if(key == GLFW_KEY_DOWN && inround == 1){

				if(speed > 0){
					speed -= 0.01;
				}
				update();
				updatescene();


			VBO.update(scene);

		}
		if(key == GLFW_KEY_UP && inround == 1){

				if(speed > 0){
					speed += 0.01;
				}
				update();
				updatescene();


			VBO.update(scene);

		}
		if(key == GLFW_KEY_X && inround == 1){ //Clockwise

				switch(shape){
					case 2:
						switch(orientation){
							case 0:

								break;
							case 1:

								break;
							case 2:

								break;
							case 3:

								break;
						}
						break;
					case 3:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
					case 4:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
					case 5:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
					case 6:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
					case 7:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
				}

				update();
				updatescene();
			VBO.update(scene);

		}
		if(key == GLFW_KEY_Z && inround == 1){ //Counter Clockwise

				switch(shape){
					case 2:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
					case 3:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
					case 4:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
					case 5:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
					case 6:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
					case 7:
						switch(orientation){
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
						}
						break;
				}

				update();
				updatescene();
			VBO.update(scene);

		}


	}


	// Upload the change to the GPU
	updatescene();
	VBO.update(scene);
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

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int w, h, nrChannels;
	unsigned char *data = stbi_load(".././build/images/board.png", &w, &h, &nrChannels, 0);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

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
	VBO_T.init();

	color.resize(3,6);

	background.resize(2,6);

	background << -1, -1, 1, 1, -1, 1,
			 					-1,  1,-1, 1,  1,-1;
	texturecoor << 0, 0, 1, 1, 0, 1,
								0, 1,0 ,1, 1,0 ;

  color << 1,1,1, 1,1,1,
					1,1,1, 1,1,1,
					1,1,1, 1,1,1;

	scene.resize(2,6);

	scene << 0,0,0,0,0,0,
					0,0,0,0,0,0;

	updatescene();
	VBO_T.update(texturecoor);
  VBO_C.update(color);
	VBO.update(scene);

	// Initialize the OpenGL Program
	// A program controls the OpenGL pipeline and it must contains
	// at least a vertex shader and a fragment shader to be valid
	Program program;
	const GLchar* vertex_shader = R"(
		#version 150 core
		in vec2 position;
		in vec2 textcoor;

		in vec3 color;
			out vec3 f_color;
			out vec2 fragtext;

		void main() {
			gl_Position = vec4(position, 0.0, 1.0);
			f_color = color;
			fragtext = textcoor;
		}
	)";

	const GLchar* fragment_shader = R"(
		#version 150 core
		in vec3 f_color;
		in vec2 fragtext;

		uniform sampler2D ourTexture;

		out vec4 outColor;
		void main() {

				outColor = texture(ourTexture, fragtext);

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
	program.bindVertexAttribArray("textcoor",VBO_T);


	glUniform1i(program.uniform("sampler2D"),0);

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
	board << Eigen::MatrixXi::Zero(10, 20);

	lastboard << Eigen::MatrixXi::Zero(10, 20);

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


		if (begun && !paused){

			if(starttime == 0){
				scene.resize(2,6);
				scene << background;
				scene.conservativeResize(2, 6+(200*2*3)); //6 for the background and then 200 squares, 2 triangles per square, 3 points per triangle.
				scene << scene.block(0,0,2,6), Eigen::MatrixXf::Zero(2, 200*2*3);
				color.conservativeResize(3, 6+(200*2*3)); //6 for the background and then 200 squares, 2 triangles per square, 3 points per triangle.
				color << color.block(0,0,3,6), Eigen::MatrixXf::Zero(3, 200*2*3);
				starttime = time;
				prevtime = starttime;
			}


			if(inround == 0 && time - prevtime > speed){
				inround = 1;
				initializeround = 1;
			}

			if(initializeround){
				createshape();
				update();
				initializeround = 0;
			}


			if (inround == 1 && time - prevtime > speed){ //wait one second after a round finishes before lauching the next shape
				if(checkboard() == 0){
					while(won() != -1){
						int colm = won();
						board.col(colm) << 0,0,0,0,0,0,0,0,0,0;
						for(int k = colm; k < 19; k++){
							for(int l = 0; l < 10; l++){
								board(l,k) = board(l, k+1);
							}
						}
						board.col(19) << 0,0,0,0,0,0,0,0,0,0;

						lastboard << board;
						updatescene();
						std::this_thread::sleep_for(std::chrono::seconds(1));
						score += 1;
						std::cout << "score" << score << '\n';
						//HAVE TO MOVE THE ENTIRE SCENE DOWN
					}
					lastboard << board;
					inround = 0;
				} else {
					movedown();
					update();
				}

				prevtime = time; //will only update after each time the shape moves, which is every second
			}
		}

		if(gaemover){
			break;
		}

		updatescene();
		glDrawArrays(GL_TRIANGLES, 0, scene.cols());

		VBO_C.update(color);
		VBO.update(scene);
		VBO_T.update(texturecoor);



		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	if(gaemover){
		std::cout << "game over" << '\n';
		std::cout << "final score " << score << '\n';
	}

	// Deallocate opengl memory
	program.free();
	stbi_image_free(data);

	VAO.free();
	VBO.free();
	VBO_C.free();
	VBO_T.free();



	// Deallocate glfw internals
	glfwTerminate();
	return 0;
}
