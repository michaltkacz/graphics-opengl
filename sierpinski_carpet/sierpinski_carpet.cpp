#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <ctime>
#include <cmath>
#include <cstdio>

const GLint WINDOW_SIZE = 800;
GLint CARPET_LEVEL = 3;


GLfloat pick_random_peturbation(GLfloat square_half_size)
{
    // peturbation is 20% to 40% of square half size, positive or negative.
    GLint sign = rand() % 100 < 50 ? 1 : -1;
    return sign * (GLfloat)(10 + rand() % 21) / 100.0f * square_half_size;
}


GLfloat pick_random_color()
{
    return (GLfloat) (rand() % 101 / 100.0f);
}


void draw_carpet(GLfloat x, GLfloat y, GLfloat half_size, GLint level)
{
    if (level < 0) 
    {
        return;
    }

    if (level > 0)
    {
        // call reqursion
        draw_carpet(x - 2 * half_size / 3, y + 2 * half_size / 3, half_size / 3, level - 1);
        draw_carpet(x, y + 2 * half_size / 3, half_size / 3, level - 1);
        draw_carpet(x + 2 * half_size / 3, y + 2 * half_size / 3, half_size / 3, level - 1);

        draw_carpet(x - 2 * half_size / 3, y, half_size / 3, level - 1);
        draw_carpet(x + 2 * half_size / 3, y, half_size / 3, level - 1);

        draw_carpet(x - 2 * half_size / 3, y - 2 * half_size / 3, half_size / 3, level - 1);
        draw_carpet(x, y - 2 * half_size / 3, half_size / 3, level - 1);
        draw_carpet(x + 2 * half_size / 3, y - 2 * half_size / 3, half_size / 3, level - 1);
    }
    else if (level == 0)
    {
		// finally draw squares at lowest level
		glBegin(GL_POLYGON);
		glColor3f(pick_random_color(), pick_random_color(), pick_random_color());
		glVertex2f(x - half_size + pick_random_peturbation(half_size), y + half_size + pick_random_peturbation(half_size));
		glColor3f(pick_random_color(), pick_random_color(), pick_random_color());
		glVertex2f(x + half_size + pick_random_peturbation(half_size), y + half_size + pick_random_peturbation(half_size));
		glColor3f(pick_random_color(), pick_random_color(), pick_random_color());
		glVertex2f(x + half_size + pick_random_peturbation(half_size), y - half_size + pick_random_peturbation(half_size));
		glColor3f(pick_random_color(), pick_random_color(), pick_random_color());
		glVertex2f(x - half_size + pick_random_peturbation(half_size), y - half_size + pick_random_peturbation(half_size));
		glEnd();
    }

}


void RenderScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    // call function to draw carpet at given level
    draw_carpet(0.0f, 0.0f, 90.0f, CARPET_LEVEL);

    glFlush();

}


void MyInit(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1);

}


void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    if (vertical == 0)
    {
        vertical = 1;
    }
       
    glViewport(0, 0, horizontal, vertical);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    GLdouble obs_win = 100.0;
    if (horizontal <= vertical)
    {
        glOrtho(-obs_win, obs_win, -obs_win / AspectRatio, obs_win / AspectRatio, 1.0, -1.0);
    }
    else
    {
        glOrtho(-obs_win * AspectRatio, obs_win * AspectRatio, -obs_win, obs_win, 1.0, -1.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}


int main(int argc, char* argv[])
{
    if (argc > 1) 
    {
        CARPET_LEVEL = atoi(argv[1]);
    }

    srand(time(NULL));

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WINDOW_SIZE) / 2,
                           (glutGet(GLUT_SCREEN_HEIGHT) - WINDOW_SIZE) / 2);

    glutCreateWindow("Sierpinski Carpet");

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);

    MyInit();

    glutMainLoop();

}