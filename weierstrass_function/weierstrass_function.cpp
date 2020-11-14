#include <ctime>
#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

#define PI 3.14159265358979323846
#define DEG_TO_RAD 0.0174

typedef GLfloat color[3];
typedef float point3[3];
typedef float point2[2];

/*	CONTROLS:
 *	- "LMB + MOUSE MOVE X" - move camera X.
 *	- "RMB + MOUSE MOVE Y" - move camera Y.
 *	- "MMB + MOUSE MOVE Y" - zoom in/out.
 *	- "R" - reset view.
 *	- "A" - enable/disable axes.
 */

 // Size of the window in pixels.
const int WINDOW_SIZE = 800; 

// Defines Weierstrass  function parameters.
float A = 0.5;
float B = 3.0;
int C = 50;

// Defines min and max B value.
float min_b = 1.0;
float max_b = 6.0;

// Defines min and max X value for function calculation.
float min_x = 0.0;
float max_x = 0.0;
float x_step = 0.0;

// Defines how many points should be calculated and drawn on screen.
int points_count = 1000;
// Weierstrass function.
point3* function_values;

// Observer cartesian coordinates.
point3 observer_position = { 0.0, 0.0, 30.0 };

// Defines min and max zoom.
float min_zoom = 1.0;
float max_zoom = 30.0;
// Defines field of view angle.
float fov_angle = 70.0;

// Pixels to angle ratio.
float pixels_to_angle = 0;
// Old mouse position used to calculate mouse movement.
point2 old_mouse_position = { 0.0, 0.0 };
// Defines mouse move.
point2 delta_mouse_position = { 0.0, 0.0 };

// Defines mouse buttons
enum mouse_buttons_clicked 
{
	none,
	left,
	right,
	middle
};
mouse_buttons_clicked mbc = none;

// Defines if show axes on screen.
bool show_axes = true;	

// --- Functions declarations ---
void keys(unsigned char key, int x, int y);
void mouse_buttons(int btn, int state, int x, int y);
void mouse_motion(GLsizei x, GLsizei y);

void my_init();
void my_closing();
void render_scene();
void change_size(GLsizei horizontal, GLsizei vertical);

void draw_axes();
void draw_weierstrass_function();

void calculate_function();
float map_value(float n, float start1, float stop1, float start2, float stop2);
// --- Functions declarations END---

// --- MAIN ---
int main(int argc, char* argv[])
{
	// Initialize window
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WINDOW_SIZE) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - WINDOW_SIZE) / 2);
	glutCreateWindow("Egg");

	// Register callback functions.
	glutDisplayFunc(render_scene);
	glutReshapeFunc(change_size);
	glutKeyboardFunc(keys);
	glutMouseFunc(mouse_buttons);
	glutMotionFunc(mouse_motion);

	// Do initialization
	my_init();

	glEnable(GL_DEPTH_TEST);

	// Start glut program loop.
	glutMainLoop();

	// Do operation before closing program.
	my_closing();
}
// --- MAIN END ---


// --- Functions definitions ---
void keys(unsigned char key, int x, int y)
{
	// Disable / Enable axes.
	if (key == 'a') show_axes = !show_axes;

	// Reset observer position.
	if (key == 'r')
	{
		observer_position[0] = 0.0;
		observer_position[1] = 0.0;
		observer_position[2] = 30.0;

		calculate_function();
	}

	// Redisplay window.
	glutPostRedisplay();
}

void mouse_buttons(int btn, int state, int x, int y)
{
	// Handle mouse button events.
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		old_mouse_position[0] = x;
		old_mouse_position[1] = y;
		mbc = left;
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		old_mouse_position[0] = x;
		old_mouse_position[1] = y;
		mbc = right;
	}
	else if (btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
	{
		old_mouse_position[0] = x;
		old_mouse_position[1] = y;
		mbc = middle;
	}
	else
	{
		mbc = none;
	}
}

void mouse_motion(GLsizei x, GLsizei y)
{
	// Calculate mouse move.
	delta_mouse_position[0] = x - old_mouse_position[0];
	old_mouse_position[0] = x;
	delta_mouse_position[1] = y - old_mouse_position[1];
	old_mouse_position[1] = y;
	
	// Handle mouse movement.
	if (mbc == left)
	{
		// Move observer X.
		observer_position[0] += delta_mouse_position[0] * pixels_to_angle * 0.02;		
	}
	else if (mbc == right)
	{
		// Move observer Y.
		observer_position[1] -= delta_mouse_position[1] * pixels_to_angle * 0.02;
	}
	if (mbc == middle)
	{
		// Zoom in/out.
		observer_position[2] += delta_mouse_position[1] * pixels_to_angle * 0.05;
		if (observer_position[2] > max_zoom) observer_position[2] = max_zoom;
		else if (observer_position[2] < min_zoom) observer_position[2] = min_zoom;
	}

	// Redisplay window.
	glutPostRedisplay();
}

void my_init()
{
	// Clear window.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Initially calculate function.
	calculate_function();
}

void my_closing()
{
	// Deallocate memory
	delete[] function_values;
}

void render_scene()
{
	// Clear screen.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Clear matrix.
	glLoadIdentity();

	// Set observer position, look target and orientation.
	gluLookAt(observer_position[0], observer_position[1], observer_position[2], observer_position[0], observer_position[1], 0.0, 0.0, 1.0, 0.0);

	// Show axes.
	if (show_axes)
	{
		// Push matrix on stack.
		glPushMatrix();
		// Draw axes.
		draw_axes();
		// Pop matrix from stack.
		glPopMatrix();
	}

	// Recalculate function.
	calculate_function();

	// Push matrix on stack.
	glPushMatrix();
	// Draw function.
	draw_weierstrass_function();
	// Pop matrix from stack.
	glPopMatrix();
	
	// Do drawing.
	glFlush();
	// Swaps the buffers of the current window if double buffered.
	glutSwapBuffers();
}

void change_size(GLsizei horizontal, GLsizei vertical)
{
	// Set pixels to angle ratio.
	pixels_to_angle = 360.0 / (float)horizontal;

	// Set matrix to projection matrix.
	glMatrixMode(GL_PROJECTION);

	// Clear matrix.
	glLoadIdentity();

	// Set perspective parameters
	gluPerspective(fov_angle, 1.0, 1.0, 100.0);

	// Set viewport
	if (horizontal <= vertical)
	{
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	}
	else
	{
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	}

	// Set matrix to modelview matrix.
	glMatrixMode(GL_MODELVIEW);

	// Clear matrix.
	glLoadIdentity();
}

void draw_axes()
{
	// X axis points.
	point3  x_min = { -10.0, 0.0, 0.0 };
	point3  x_max = { 10.0, 0.0, 0.0 };

	// Y axis points.
	point3  y_min = { 0.0, -10.0, 0.0 };
	point3  y_max = { 0.0,  10.0, 0.0 };

	// Z axis points.
	point3  z_min = { 0.0, 0.0, -10.0 };
	point3  z_max = { 0.0, 0.0,  10.0 };

	// Draw red X axis.
	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	// Draw green Y axis.
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	// Draw blue Z axis.
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

void draw_weierstrass_function()
{
	// Draw line based on points stored in function_values.
	glColor3f(0.4f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < points_count-1; i++)
	{
		glVertex3fv(function_values[i]);
		glVertex3fv(function_values[i+1]);
	}
	glEnd();
}

void calculate_function()
{
	// Calculate x step.
	x_step = ((max_x - min_x) / points_count);

	// Clear old function values.
	if (function_values != nullptr)
	{
		delete[] function_values;
	}

	// Allocate memory.
	function_values = new point3[points_count];

	// Calculate x on edge of screen.
	float x_edge = observer_position[2] * tanf(fov_angle * DEG_TO_RAD / 2.0);

	// Set min and max x (on left and right edge of screen).
	max_x = observer_position[0] + x_edge;
	min_x = observer_position[0] - x_edge;

	// Calculate Weierstrass function B parameter.
	B = min_b + max_b - map_value(observer_position[2], min_zoom, max_zoom, min_b, max_b);

	std::cout << min_x << std::endl;
	// Calculate function values.
	for (int i = 0; i < points_count; i++)
	{
		// Current X value.
		float x = min_x + i * x_step;

		// Initial point.
		function_values[i][0] = x;
		function_values[i][1] = 0;
		function_values[i][2] = 0;

		// Actual Weierstrass function formula.
		for (int n = 0; n < C; n++)
		{
			function_values[i][1] += powf(A, n) * cosf(powf(B, n) * PI * x);
		}
	}
}

float map_value(float n, float start1, float stop1, float start2, float stop2)
{
	// Maps value N: start1 <= n <= stop1 to value RETURN: start2 <= RETURN <= stop2.
	return  (n - start1) / (stop1 - start1) * (stop2 - start2) + start2;
};
// --- Functions definitions END ---