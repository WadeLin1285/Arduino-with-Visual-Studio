#include "stdafx.h"
#include <iostream>
#include <sstream>    // for stringstream
#include <string>     // for string
#include <math.h>     // for acos function
#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <iomanip>    // for setw function

#define L  7           // Define the length of std::setW()
#define PI 3.14159265

/* For serial port communication */
using namespace System;
using namespace System::IO::Ports;

/* Create a global structure with ref class SerialPort.
   Since SerialPort is ref class, it can't define globally. */
ref struct Port {

	static SerialPort^ port = gcnew SerialPort("COM3", 9600);

};

/* Define variables */
char title[] = "3D Shapes with animation";
float angle = 0.0;              // Rotational angle for cube
int   refreshMills = 10;        // refresh interval in milliseconds
float yaw, pitch, roll;         // Euler angle from Arudino 
float q0, q1, q2, q3;           // Quatarian value from Arduino
bool open = true;               // Define the usb serial open state

/* Define Functions */
void MarshalString(String ^ s, std::string& os);
void rotate();
void initGL();
void display();
void timer(int value);
void reshape(GLsizei width, GLsizei height);

/* Main function: GLUT runs as a console application starting at main() */
int main(int argc, char** argv) {
	
	glutInit(&argc, argv);            // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
	glutInitWindowSize(640, 480);     // Set the window's initial width & height
	glutInitWindowPosition(50, 50);   // Position the window's initial top-left corner

	glutCreateWindow(title);          // Create window with the given title
	glutDisplayFunc(display);         // Register callback handler for window re-paint event
	glutReshapeFunc(reshape);         // Register callback handler for window re-size event
	glutIdleFunc(rotate);             // Register callback handler for window idle event
	initGL();                         // Our own OpenGL initialization
	glutTimerFunc(0, timer, 0);       // First timer call immediately 
	glutMainLoop();                   // Enter the infinite event-processing loop
	
	return 0;
}

/* Transform String^ to string */
void MarshalString(String ^ s, std::string& os) {
	using namespace Runtime::InteropServices;
	const char* chars =
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	os = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
}

/* Read the Serial Port */
void rotate()
{
	while (open) // open serial port only once
	{
		Port::port->Open();
		open = false;
	}

	std::string message2;
	String^ message = Port::port->ReadLine();
	MarshalString(message, message2);          // translate String ^ into string
	std::stringstream  lineStream;

	lineStream << message2;                                      // send the serial message into stream
	lineStream >> yaw >> pitch >> roll >> q0 >> q1 >> q2 >> q3;  // take out the string as float type numbers
	
	// Determine the angle 
	angle = 2.0 * acos(q0) / PI * 180.0;

	// Print out in console
	std::cout << "Yaw: " << std::setw(L) << yaw << " Pitch: " << std::setw(L) << pitch << " Row: " << 
		std::setw(L) << roll << " Angle: " << std::setw(10) << angle << " Axis: ( " << std::setw(L - 1) << 
		q1 << "," << std::setw(L - 1) << q2 << "," << std::setw(L - 1) << q3 << ")" <<std::endl;
}

/* Initialize OpenGL Graphics */
void initGL() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	glClearDepth(1.0f);                   // Set background depth to farthest
	glEnable(GL_DEPTH_TEST);			  // Enable depth testing for z-culling
	glDepthFunc(GL_LEQUAL);				  // Set the type of depth-test
	glShadeModel(GL_SMOOTH);			  // Enable smooth shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

/* Handler for window-repaint event. Called back when the window first appears and whenever the window needs to be re-painted. */
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
	glMatrixMode(GL_MODELVIEW);       // To operate on model-view matrix

	// Render a color-cube consisting of 6 quads with different colors
	glLoadIdentity();                 // Reset the model-view matrix

	glTranslatef(0.0f, 0.0f, -7.0f);  // Move to center and into the screen
	glRotatef(angle, -q1, -q3, -q2);  // Rotate about axis

	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
	GLfloat x_length = 1.0f, y_length = 0.3f, z_length = 1.0f;  // Define each widths of cube 

	// Top face  (y = 1.0f)
	// Define vertices in counter-clockwise (CCW) order with normal pointing out
	glColor3f(0.0f, 1.0f, 0.0f);     // Green
	glVertex3f( x_length,  y_length, -z_length);
	glVertex3f(-x_length,  y_length, -z_length);
	glVertex3f(-x_length,  y_length,  z_length);
	glVertex3f( x_length,  y_length,  z_length);

	// Bottom face (y = -1.0f)
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange
	glVertex3f( x_length, -y_length,  z_length);
	glVertex3f(-x_length, -y_length,  z_length);
	glVertex3f(-x_length, -y_length, -z_length);
	glVertex3f( x_length, -y_length, -z_length);

	// Front face  (z = 1.0f)
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f( x_length,  y_length,  z_length);
	glVertex3f(-x_length,  y_length,  z_length);
	glVertex3f(-x_length, -y_length,  z_length);
	glVertex3f( x_length, -y_length,  z_length);

	// Back face  (z = -1.0f)
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
	glVertex3f( x_length, -y_length, -z_length);
	glVertex3f(-x_length, -y_length, -z_length);
	glVertex3f(-x_length,  y_length, -z_length);
	glVertex3f( x_length,  y_length, -z_length);

	// Left face  (x = -1.0f)
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	glVertex3f(-x_length,  y_length,  z_length);
	glVertex3f(-x_length,  y_length, -z_length);
	glVertex3f(-x_length, -y_length, -z_length);
	glVertex3f(-x_length, -y_length,  z_length);

	// Right face (x = 1.0f)
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
	glVertex3f( x_length,  y_length, -z_length);
	glVertex3f( x_length,  y_length,  z_length);
	glVertex3f( x_length, -y_length,  z_length);
	glVertex3f( x_length, -y_length, -z_length);

	glEnd();  // End of drawing color-cube

	glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}

/* Called back when timer expired */
void timer(int value) {
	glutPostRedisplay();      // Post re-paint request to activate display()
	glutTimerFunc(refreshMills, timer, 0); // next timer call milliseconds later
}

/* Handler for window re-size event. Called back when the window first appears and
whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
											   // Compute aspect ratio of the new window
	if (height == 0) height = 1.0f;  // To prevent divide by 0
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping volume to match the viewport
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();             // Reset
								  // Enable perspective projection with fovy, aspect, zNear and zFar
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}
