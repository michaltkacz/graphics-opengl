#include <ctime>
#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>

#define PI 3.14159265358979323846

typedef float point3[3];
typedef float angle[3];

const int WINDOW_SIZE = 800;

int BIG_TO_SMALL_RATIO = 2;
int BIG_N = 50;
int SMALL_N = BIG_N / BIG_TO_SMALL_RATIO;

angle theta = { 0.0, 0.0, 0.0 };
angle theta_change = { 0.0, 0.0, 0.0 };

point3** big_egg_points;
point3** small_egg_points;

enum egg_begin_mode
{
	points,
	lines,
	triangles
};
egg_begin_mode egb = points;

void keys(unsigned char key, int x, int y);

void my_init();
void my_deinit();
void render_scene();
void change_size(GLsizei horizontal, GLsizei vertical);

void draw_axes();
void draw_egg(point3** egg_points, int n);
void spin_egg();


int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		BIG_N = atoi(argv[1]);
		SMALL_N = BIG_N / BIG_TO_SMALL_RATIO;
	}

	if (BIG_N < 0)
	{
		std::cout << "Number of points cannot be less than zero!" << std::endl;
		exit(0);
	}


	srand(time(NULL));

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WINDOW_SIZE) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - WINDOW_SIZE) / 2);
	glutCreateWindow("Egg");

	glutDisplayFunc(render_scene);
	glutReshapeFunc(change_size);
	glutKeyboardFunc(keys);
	glutIdleFunc(spin_egg);

	my_init();

	glEnable(GL_DEPTH_TEST);

	glutMainLoop();

	my_deinit();
}

void keys(unsigned char key, int x, int y)
{
	if (key == 'p') egb = points;
	if (key == 'l') egb = lines;
	if (key == 't') egb = triangles;

	if (key == 'x')
	{
		theta[0] = 0.0;
		theta[1] = 0.0;
		theta[2] = 0.0;

		theta_change[0] = 0.3;
		theta_change[1] = 0.0;
		theta_change[2] = 0.0;
	}
	else if (key == 'y')
	{
		theta[0] = 0.0;
		theta[1] = 0.0;
		theta[2] = 0.0;

		theta_change[0] = 0.0;
		theta_change[1] = 0.1;
		theta_change[2] = 0.0;
	}
	else if (key == 'z')
	{
		theta[0] = 0.0;
		theta[1] = 0.0;
		theta[2] = 0.0;

		theta_change[0] = 0.0;
		theta_change[1] = 0.0;
		theta_change[2] = 0.3;
	}

	else if (key == 's')
	{
		theta[0] = 0.0;
		theta[1] = 0.0;
		theta[2] = 0.0;

		theta_change[0] = 0.0;
		theta_change[1] = 0.0;
		theta_change[2] = 0.0;
	}

	render_scene();
}

void my_init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
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
}

void my_deinit()
{
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

}

void render_scene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();


	glRotated(20.0, 1.0, 0.0, 0.0);
	glRotated(10.0, 0.0, 1.0, 0.0);

	draw_axes();

	
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);
	
	glColor3f(1.0, 0.3, 0.8);
	glTranslated(0.0, -5.0, 0.0);
	draw_egg(big_egg_points, BIG_N);

	glColor3f(0.8f, 0.3, 1.0);
	glTranslated(6.0, 5.0, 0.0);
	glScaled(1.0 / BIG_TO_SMALL_RATIO, 1.0 / BIG_TO_SMALL_RATIO, 1.0 / BIG_TO_SMALL_RATIO);
	draw_egg(small_egg_points, SMALL_N);

	glFlush();
	glutSwapBuffers();

}

void change_size(GLsizei horizontal, GLsizei vertical)
{
	if (vertical == 0)
	{
		vertical = 1;
	}

	glViewport(0, 0, horizontal, vertical);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat aspect_ratio = (GLfloat)horizontal / (GLfloat)vertical;

	if (horizontal <= vertical)
	{
		glOrtho(-11, 11, -11 / aspect_ratio, 11 / aspect_ratio, 10.0, -10.0);
	}
	else
	{
		glOrtho(-11 * aspect_ratio, 11 * aspect_ratio, -11, 11, 10.0, -10.0);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void draw_axes()
{
	point3  x_min = { -1.0, 0.0, 0.0 };
	point3  x_max = { 1.0, 0.0, 0.0 };

	point3  y_min = { 0.0, -1.0, 0.0 };
	point3  y_max = { 0.0,  1.0, 0.0 };

	point3  z_min = { 0.0, 0.0, -1.0 };
	point3  z_max = { 0.0, 0.0,  1.0 };


	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);

	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);

	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();

}

void draw_egg(point3** egg_points, int n)
{
	switch (egb)
	{
	case points:
	{
		glBegin(GL_POINTS);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
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
			for (int j = 0; j < n; j++)
			{
				glVertex3fv(egg_points[j][i]);
			}
		}
		glEnd();
		break;
	}

	case triangles:
	{
		glBegin(GL_LINES);
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				glVertex3fv(egg_points[i][j]);
			}
		}
		glEnd();
		break;
	}

	}
}

void spin_egg()
{
	theta[0] += theta_change[0];
	if (abs(theta[0] > 360)) theta[0] = 0;
	theta[1] += theta_change[1];
	if (abs(theta[1] > 360)) theta[1] = 0;
	theta[2] += theta_change[2];
	if (abs(theta[2] > 360)) theta[2] = 0;

	glutPostRedisplay();
}