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
typedef float angle[3];

/*	
 *	W treœci zadania napisane jest, aby funkcjê "zoom" zrealizowaæ
 *	przy u¿yciu kó³ka myszki. Jednak wersja glut 3.7.6 nie wspiera obs³ugi kó³ka.
 *	Z tego wzglêdu zoom zrealizowany jest przez naciœniêcie kó³ka (middle button)
 *	oraz ruch myszy wzd³ó¿ osi OY.
 *
 *	ród³a:
 *	http://www.realmtech.net/opengl/glut
 *	http://openglut.sourceforge.net/group__input.html#ga5
 *	https://www.programmersought.com/article/8224783407/
 */


/*	CONTROLS:
 *	- "L", "P", "T" - draw eggs as Lines / Points / Triangles.
 *	- "X", "Y", "Z" - spin and rotate eggs around X / Y / Z axis.
 *	- "S" - stop rotation and spin and reset position.
 *	- "D" - change direction of spin and rotation.
 *	- "A" - enable/disable axes.
 *	- "LMB + MOUSE_MOVE_X" - move observer left/right.
 *	- "RMB + MOUSE_MOVE_X/MOUSE_MOVE_Y" - rotate observer spherically.
 *	- "MMB + MOUSE_MOVE_Y" - zoom in/out.
 *	- "R" - reset observer position.
 *
 *		SPIN - both small and big egg spin around own axis.
 *		ROTATE - small egg rotates around the big egg.
 */

const int WINDOW_SIZE = 800; // Size of the window in pixels.

int BIG_TO_SMALL_RATIO = 2; // How many times big egg is bigger than small egg.
int BIG_N = 100;	// Number of points of big egg.
int SMALL_N = BIG_N / BIG_TO_SMALL_RATIO; // Number of points of small egg.

angle theta = { 0.0, 0.0, 0.0 };	// Defines current spin of eggs.
angle theta_change = { 0.0, 0.0, 0.0 };	//Defines how fast eggs spin.
float rotation_angle = 0.0;	// Defines current rotation of small egg around the big one.
float rotation_angle_change = 0.0; // Defines how fast small egg rotates around the big one.
float direction = 1.0;	// Defines rotation and spin direction (left or right).


float observer_radius = 16.0; // Defines initial zoom value.
float observer_azimuth_angle = 30.0; // Defines initial observer position.
float observer_inclination_angle = 30.0; // Defines initial observer position.
point3 observer_position = { 0.0, 0.0, 0.0 }; // Observer cartesian coordinates.

float pixels_to_angle = 0;	// Pixels to angle ratio.
point2 old_mouse_position = { 0.0, 0.0 };  // Old mouse position used to calculate mouse movement.
point2 delta_mouse_position = { 0.0, 0.0 };	// Defines mouse move.

point3 lookat_target_position = { 0.0, 0.0, 0.0 }; // Defines target for glLookAt function

enum mouse_buttons_clicked // Defines mouse buttons
{
	none,
	left,
	right,
	middle
};
mouse_buttons_clicked mbc = none;

color** colors_big;	// Colors of big egg.
color** colors_small;	// Colors of small egg.	
point3** big_egg_points; // Points of big egg.
point3** small_egg_points; // Points of small egg.

bool show_axes = true;	// Defines if show axes on screen.

enum egg_begin_mode	// Defines draw mode of eggs.
{
	points,
	lines,
	triangles
};
egg_begin_mode ebm = points;


// --- Functions declarations ---
void keys(unsigned char key, int x, int y);
void mouse_buttons(int btn, int state, int x, int y);
void mouse_motion(GLsizei x, GLsizei y);
void set_theta_change(float x, float y, float z, float r);
void change_direction();
float pick_random_color();

void my_init();
void my_closing();
void render_scene();
void change_size(GLsizei horizontal, GLsizei vertical);

void draw_axes();
void draw_egg(point3** egg_points, int n, color** colors);

void spin_egg();
// --- Functions declarations END---

// --- MAIN ---
int main(int argc, char* argv[])
{
	// Check for starting parameter.
	if (argc > 1)
	{
		BIG_N = atoi(argv[1]);
		SMALL_N = BIG_N / BIG_TO_SMALL_RATIO;
	}

	// Validate given parameter.
	if (BIG_N <= 0)
	{
		std::cout << "Number of points must be greater than zero!" << std::endl;
		exit(0);
	}

	// "Randomize" random function
	srand(time(NULL));

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
	glutIdleFunc(spin_egg);

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

	// Set eggs drawing mode.
	if (key == 'p') ebm = points;
	else if (key == 'l') ebm = lines;
	else if (key == 't') ebm = triangles;

	// Set rotation and spin.
	if (key == 's') {
		rotation_angle = 0.0;
		set_theta_change(0.0, 0.0, 0.0, 0.0);
	}
	else if (key == 'x')
	{
		set_theta_change(0.1, 0.0, 0.0, 0.25);
	}
	else if (key == 'y')
	{
		set_theta_change(0.0, 0.1, 0.0, 0.25);
	}
	else if (key == 'z')
	{
		set_theta_change(0.0, 0.0, 0.1, 0.25);
	}

	// Change rotation and spin direction.
	if (key == 'd') change_direction();

	// Reset observer position.
	if (key == 'r')
	{
		lookat_target_position[0] = 0.0;
		lookat_target_position[1] = 0.0;
		lookat_target_position[2] = 0.0;
		
		observer_radius = 16.0;
		observer_azimuth_angle = 30.0;
		observer_inclination_angle = 30.0;
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
		// Calculate observer position X and Z.
		float vec_x = observer_radius * cosf(observer_azimuth_angle * DEG_TO_RAD) * cosf(observer_inclination_angle * DEG_TO_RAD);
		float vec_z = observer_radius * sinf(observer_azimuth_angle * DEG_TO_RAD) * cosf(observer_inclination_angle * DEG_TO_RAD);

		// Calculate manitude of XZ vector.
		float magnitude = sqrt(vec_x * vec_x + vec_z * vec_z);

		// Normalize XZ vector.
		if(magnitude != 0.0)
		{
			vec_x /= magnitude;
			vec_z /= magnitude;
		}

		// Move lookat_target perpendicularly to XZ vector.
		lookat_target_position[0] -= vec_z * delta_mouse_position[0] * pixels_to_angle * 0.1;
		lookat_target_position[2] += vec_x * delta_mouse_position[0] * pixels_to_angle * 0.1;

		// Move lookat_target parallelly to XZ vector.
		lookat_target_position[0] -= vec_x * delta_mouse_position[1] * pixels_to_angle * 0.1;
		lookat_target_position[2] -= vec_z * delta_mouse_position[1] * pixels_to_angle * 0.1;
	}
	else if (mbc == right)
	{
		// Change azimuth angle with mouse X move.
		observer_azimuth_angle += delta_mouse_position[0] * pixels_to_angle;
		if (observer_azimuth_angle >= 360.0) observer_azimuth_angle = 0.0;
		else if (observer_azimuth_angle <= 0.0) observer_azimuth_angle = 360.0f;

		// Change inclination angle with mouse Y move.
		observer_inclination_angle += delta_mouse_position[1] * pixels_to_angle;
		if(observer_inclination_angle >= 360.0) observer_inclination_angle = 0.0;
		else if(observer_inclination_angle < 0.0) observer_inclination_angle = 360.0;
	}
	else if (mbc == middle)
	{
		// Zoom in/out with mouuse Y move.
		observer_radius += delta_mouse_position[1] * pixels_to_angle * 0.25;
		if (observer_radius >= 50) observer_radius = 50.0;
		else if (observer_radius <= 1.0) observer_radius = 1.0;
	}

	// Redisplay window.
	glutPostRedisplay();
}

void set_theta_change(float x, float y, float z, float r)
{
	// Reset position - spin angles.
	theta[0] = 0.0;
	theta[1] = 0.0;
	theta[2] = 0.0;

	// Set spin direction and speed.
	theta_change[0] = direction * x;
	theta_change[1] = direction * y;
	theta_change[2] = direction * z;

	// Set rotation direction and speed.
	rotation_angle_change = direction * r;
}

void change_direction()
{
	// Set direction to opposite.
	direction = -direction;

	// Set spin direction to opposite.
	theta_change[0] = -theta_change[0];
	theta_change[1] = -theta_change[1];
	theta_change[2] = -theta_change[2];

	// Set rotation direction to opposite.
	rotation_angle_change = -rotation_angle_change;
}

float pick_random_color()
{
	// Returns random component of RGB color.
	return (float)(rand() % 101 / 100.0f);
}

void my_init()
{
	// Clear window.
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Init points of big egg.
	big_egg_points = new point3 * [BIG_N];
	for (int i = 0; i < BIG_N; i++)
	{
		big_egg_points[i] = new point3[BIG_N];
	}

	float mesh_size = 1.0 / BIG_N;
	for (int i = 0; i < BIG_N; i++)
	{
		float  u = i * mesh_size;
		for (int j = 0; j < BIG_N; j++)
		{
			float v = j * mesh_size;
			big_egg_points[i][j][0] = (-90 * powf(u, 5) + 225 * powf(u, 4) - 270 * powf(u, 3) + 180 * powf(u, 2) - 45 * u) * cosf(PI * v);
			big_egg_points[i][j][1] = 160 * powf(u, 4) - 320 * powf(u, 3) + 160 * powf(u, 2);
			big_egg_points[i][j][2] = (-90 * powf(u, 5) + 225 * powf(u, 4) - 270 * powf(u, 3) + 180 * powf(u, 2) - 45 * u) * sinf(PI * v);
		}
	}

	// Init points of small egg.
	small_egg_points = new point3 * [SMALL_N];
	for (int i = 0; i < SMALL_N; i++)
	{
		small_egg_points[i] = new point3[SMALL_N];
	}

	mesh_size = 1.0 / SMALL_N;
	for (int i = 0; i < SMALL_N; i++)
	{
		float  u = i * mesh_size;
		for (int j = 0; j < SMALL_N; j++)
		{
			float v = j * mesh_size;
			small_egg_points[i][j][0] = (-90 * powf(u, 5) + 225 * powf(u, 4) - 270 * powf(u, 3) + 180 * powf(u, 2) - 45 * u) * cosf(PI * v);
			small_egg_points[i][j][1] = 160 * powf(u, 4) - 320 * powf(u, 3) + 160 * powf(u, 2);
			small_egg_points[i][j][2] = (-90 * powf(u, 5) + 225 * powf(u, 4) - 270 * powf(u, 3) + 180 * powf(u, 2) - 45 * u) * sinf(PI * v);
		}
	}

	// Init colors for big egg.
	colors_big = new color * [BIG_N];
	for (int i = 0; i < BIG_N; i++)
	{
		colors_big[i] = new point3[BIG_N];
	}

	for (int i = 0; i < BIG_N; i++)
	{
		for (int j = 0; j < BIG_N; j++)
		{
			colors_big[i][j][0] = pick_random_color();
			colors_big[i][j][1] = pick_random_color();
			colors_big[i][j][2] = pick_random_color();
		}
	}

	// Init colors for small egg.
	colors_small = new color * [SMALL_N];
	for (int i = 0; i < SMALL_N; i++)
	{
		colors_small[i] = new point3[SMALL_N];
	}

	for (int i = 0; i < SMALL_N; i++)
	{
		for (int j = 0; j < SMALL_N; j++)
		{
			colors_small[i][j][0] = pick_random_color();
			colors_small[i][j][1] = pick_random_color();
			colors_small[i][j][2] = pick_random_color();
		}
	}
}

void my_closing()
{
	// Deallocate memory
	for (int i = 0; i < BIG_N; i++)
	{
		delete[] big_egg_points[i];
	}
	delete[] big_egg_points;

	for (int i = 0; i < SMALL_N; i++)
	{
		delete[] small_egg_points[i];
	}
	delete[] small_egg_points;

	for (int i = 0; i < BIG_N; i++)
	{
		delete[] colors_big[i];
	}
	delete[] colors_big;

	for (int i = 0; i < SMALL_N; i++)
	{
		delete[] colors_small[i];
	}
	delete[] colors_small;
}

void render_scene()
{
	// Clear screen.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Clear matrix.
	glLoadIdentity();

	// Calculate observer cartesian position.
	observer_position[0] = observer_radius * cosf(observer_azimuth_angle * DEG_TO_RAD) * cosf(observer_inclination_angle * DEG_TO_RAD) + lookat_target_position[0];
	observer_position[1] = observer_radius * sinf(observer_inclination_angle * DEG_TO_RAD) + lookat_target_position[1];
	observer_position[2] = observer_radius * sinf(observer_azimuth_angle * DEG_TO_RAD) * cosf(observer_inclination_angle * DEG_TO_RAD) + lookat_target_position[2];
	// Set observer orientation UP_Y vector.
	float up_y = (observer_inclination_angle >= 90.0 && observer_inclination_angle < 270.0) ? -1.0 : 1.0;
	// Set observer position, look target and orientation.
	gluLookAt(observer_position[0], observer_position[1], observer_position[2], lookat_target_position[0], lookat_target_position[1], lookat_target_position[2], 0.0, up_y, 0.0);

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

	// Push matrix on stack.
	glPushMatrix();
	// Set color of "crosshair"
	glColor3f(0.8, 0.2, 0.5);
	// Set position of "crosshair" to lookat_target_position.
	glTranslatef(lookat_target_position[0], lookat_target_position[1], lookat_target_position[2]);
	// Draw "crosshair" as a solid sphere.
	glutSolidSphere(0.12, 10, 10);
	// Pop matrix from stack.
	glPopMatrix();
	

	// -- BIG EGG --
	// Push matrix on stack.
	glPushMatrix();
	// Set spin angles.
	// Spin X.
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	// Spin Y.
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	// Spin Z.
	glRotatef(theta[2], 0.0, 0.0, 1.0);
	// Move egg "down".
	glTranslated(0.0, -5.0, 0.0);
	// Draw big egg.
	draw_egg(big_egg_points, BIG_N, colors_big);
	// Pop matrix from stack.
	glPopMatrix();

	// -- SMALL EGG --
	// Push matrix on stack.
	glPushMatrix();
	// Set spin angles.
	// Spin X.
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	// Spin Z.
	glRotatef(theta[2], 0.0, 0.0, 1.0);
	// Rotate around big egg Y - rotate around big egg.
	glRotatef(rotation_angle, 0.0, 1.0, 0.0);
	// Move egg "aside and down".
	glTranslated(6.0, -2.5, 0.0);
	// Spin Y - must be done after translation.
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	// Scale egg down.
	glScaled(1.0 / BIG_TO_SMALL_RATIO, 1.0 / BIG_TO_SMALL_RATIO, 1.0 / BIG_TO_SMALL_RATIO);
	// Draw small egg.
	draw_egg(small_egg_points, SMALL_N, colors_small);
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
	gluPerspective(70, 1.0, 1.0, 100.0);

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

void draw_egg(point3** egg_points, int n, color** colors)
{
	// Draw eggs in chosen draw mode.
	switch (ebm)
	{
	case points:
	{
		// Draw all points.
		glBegin(GL_POINTS);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				glColor3fv(colors[i][j]);
				glVertex3fv(egg_points[i][j]);
			}
		}
		glEnd();
		break;
	}

	case lines:
	{
		glBegin(GL_LINES);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n - 1; j++)
			{
				// Line vertical:
				// Point A
				glColor3fv(colors[i][j]);
				glVertex3fv(egg_points[i][j]);
				// Point B
				glColor3fv(colors[(i + 1) % n][j]);
				glVertex3fv(egg_points[(i + 1) % n][j]);

				// Line horizontal:
				// Point A:
				glColor3fv(colors[i][j]);
				glVertex3fv(egg_points[i][j]);
				// Point C:
				glColor3fv(colors[i][j + 1]);
				glVertex3fv(egg_points[i][j + 1]);
			}

			// Line vertical connecting endings of two semicircles:
			// Point A
			glColor3fv(colors[i][n - 1]);
			glVertex3fv(egg_points[i][n - 1]);
			// Point B
			glColor3fv(colors[(i + 1) % n][n - 1]);
			glVertex3fv(egg_points[(i + 1) % n][n - 1]);

			// Line horizontal connecting two semicircles:
			// Point A:
			glColor3fv(colors[i][n - 1]);
			glVertex3fv(egg_points[i][n - 1]);
			// Point C:
			glColor3fv(colors[(n - i) % n][0]);
			glVertex3fv(egg_points[(n - i) % n][0]);
		}
		glEnd();
		break;
	}

	case triangles:
	{

		glBegin(GL_TRIANGLES);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n - 1; j++)
			{
				/*	Points:
				 *	 C - D
				 *	 | \ |
				 *	 A - B
				 */

				 // Triangle ABC connecting two semicircles:
				 // Point A:
				glColor3fv(colors[i][j]);
				glVertex3fv(egg_points[i][j]);
				// Point B:
				glColor3fv(colors[i][j + 1]);
				glVertex3fv(egg_points[i][j + 1]);
				// Point C:
				glColor3fv(colors[(i + 1) % n][j]);
				glVertex3fv(egg_points[(i + 1) % n][j]);

				// Triangle BCD connecting two semicircles:
				// Point B:
				glColor3fv(colors[i][j + 1]);
				glVertex3fv(egg_points[i][j + 1]);
				// Point C:
				glColor3fv(colors[(i + 1) % n][j]);
				glVertex3fv(egg_points[(i + 1) % n][j]);
				// Point D:
				glColor3fv(colors[(i + 1) % n][j + 1]);
				glVertex3fv(egg_points[(i + 1) % n][j + 1]);
			}

			// Triangle ABC:
			// Point A:
			glColor3fv(colors[i][n - 1]);
			glVertex3fv(egg_points[i][n - 1]);
			// Point B:
			glColor3fv(colors[(n - i) % n][0]);
			glVertex3fv(egg_points[(n - i) % n][0]);
			// Point C:
			glColor3fv(colors[(i + 1) % n][n - 1]);
			glVertex3fv(egg_points[(i + 1) % n][n - 1]);

			// Triangle BCD:
			// Point B:
			glColor3fv(colors[(n - i) % n][0]);
			glVertex3fv(egg_points[(n - i) % n][0]);
			// Point C:
			glColor3fv(colors[(i + 1) % n][n - 1]);
			glVertex3fv(egg_points[(i + 1) % n][n - 1]);
			// Point D:
			glColor3fv(colors[(n - i - 1) % n][0]);
			glVertex3fv(egg_points[(n - i - 1) % n][0]);

		}
		glEnd();
		break;
	}
	}
}

void spin_egg()
{
	// Set spin angles.
	theta[0] += theta_change[0];
	if (abs(theta[0] > 360.0)) theta[0] = 0.0;

	theta[1] += theta_change[1];
	if (abs(theta[1] > 360.0)) theta[1] = 0.0;

	theta[2] += theta_change[2];
	if (abs(theta[2] > 360.0)) theta[2] = 0.0;

	// Set rotation angle.
	rotation_angle += rotation_angle_change;
	if (abs(rotation_angle > 360.0)) rotation_angle = 0.0;

	// Redisplay window.
	glutPostRedisplay();
}
// --- Functions definitions END ---