#include <windows.h>

#include <cmath>
#include <cstdio>
#include <ctime>

#include <gl/gl.h>
#include <gl/glut.h>

#define PI 3.14159265358979323846
#define DEG_TO_RAD 0.0174

typedef GLfloat color[3];
typedef float point3[3];
typedef float point2[2];
typedef float angle[3];

/* CONTROLS:
        - "1"/"2" - Enable/Disable Light0.
        - "3"/"4" - Enable/Disable Light1.
        - "5"/"6" - Enable/Disable Light2.
        - "7"/"8" - Enable/Disable Light3.
        - "9"/"0" - Enable/Disable Light4.
        - "R" - Pick random color for Light4.
            (I recommend to hold R for disco-efect)
        - "X", "Y", "Z" - Rotate egg around X/Y/Z axis.
        - "S" - Stop rotation.
        - "LMB" + "MouseX/Y" - Move Light4.
        - "MMB" + "MouseX/Y" - Move Ligth4 closer/further.
        - "RMB" + "MouseX/Y" - Move observer (camera).
*/

// --- GLOBAL VARIABLES ---
// Size of the window in pixels.
const int WINDOW_SIZE = 800;

// Number of points of big egg.
int N = 50;

// Pixels to angle ratio for mouse move calculations.
float pixels_to_angle = 0;

// Mouse move variables.
point2 old_mouse_position = {0.0, 0.0};
point2 delta_mouse_position = {0.0, 0.0};

// Egg spin values.
angle spin_angle = {0.0, 0.0, 0.0};
angle spin_angle_change = {0.0, 0.0, 0.0};

// Observer initial position (camera position).
point3 observer_position = {0.0, 0.0, 0.0};
float observer_radius = 20.0;
float observer_azimuth_angle = 30.0;
float observer_inclination_angle = 30.0;

// Light1 initial position.
GLfloat light1_position[] = {0.0, 0.0, 0.0, 0.0};
float light1_radius = 20.0;
float light1_azimuth_angle = 90.0;
float light1_inclination_angle = 0.0;

// Light2 initial position.
GLfloat light2_position[] = {0.0, 0.0, 0.0, 0.0};
float light2_radius = 20.0;
float light2_azimuth_angle = 0.0;
float light2_inclination_angle = 0.0;

// Light3 initial position.
GLfloat light3_position[] = {0.0, 0.0, 0.0, 0.0};
float light3_radius = 20.0;
float light3_azimuth_angle = 0.0;
float light3_inclination_angle = 90.0;

// Light4 initial position.
GLfloat light4_position[] = {0.0, 0.0, 0.0, 1.0};
float light4_radius = 10.0;
float light4_azimuth_angle = 45.0;
float light4_inclination_angle = 45.0;

// Light 4 initial color components.
float light4_r = 0.0;
float light4_g = 0.0;
float light4_b = 0.0;

// Mouse buttons enum.
enum mouse_buttons_clicked { none, left, right, middle };

// Button currently clicked.
mouse_buttons_clicked mbc = none;

// Egg points coordinates.
point3** egg_points;

// Egg normal vectors.
point3** egg_normals;

// Show or hide axes.
bool show_axes = false;
// --- END GLOBAL VARIABLES ---

// --- Functions declarations ---
void keys(unsigned char key, int x, int y);
void mouse_buttons(int btn, int state, int x, int y);
void mouse_motion(GLsizei x, GLsizei y);

void my_init();
void my_closing();

void render_scene();
void change_size(GLsizei horizontal, GLsizei vertical);

void draw_axes();
void draw_egg(point3** egg_points, point3** egg_normals, int n);

void spin_egg();
float pick_random_color();
void look_at();
void move_light1();
void move_light2();
void move_light3();
void move_light4();
// --- Functions declarations END---

// --- MAIN ---
int main(int argc, char* argv[]) {
  // Check for starting parameter.
  if (argc > 1) {
    N = atoi(argv[1]);
  }

  // Validate given parameter.
  if (N <= 0) {
    printf("Number of points must be greater than zero!");
    exit(0);
  }

  // "Randomize" random function
  srand(time(NULL));

  // Initialize window.
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

  // Start glut program loop.
  glutMainLoop();

  // Do operation before closing program.
  my_closing();
}
// --- MAIN END ---

// --- Functions definitions ---
void keys(unsigned char key, int x, int y) {
  // Disable / enable axes.
  if (key == 'a') show_axes = !show_axes;

  if (key == '1')
    glEnable(GL_LIGHT0);
  else if (key == '2')
    glDisable(GL_LIGHT0);
  else if (key == '3')
    glEnable(GL_LIGHT1);
  else if (key == '4')
    glDisable(GL_LIGHT1);
  else if (key == '5')
    glEnable(GL_LIGHT2);
  else if (key == '6')
    glDisable(GL_LIGHT2);
  else if (key == '7')
    glEnable(GL_LIGHT3);
  else if (key == '8')
    glDisable(GL_LIGHT3);
  else if (key == '9')
    glEnable(GL_LIGHT4);
  else if (key == '0')
    glDisable(GL_LIGHT4);

  // Set rotation and spin.
  if (key == 's') {
    // Reset position - spin angles.
    spin_angle[0] = 0.0;
    spin_angle[1] = 0.0;
    spin_angle[2] = 0.0;

    // Reset spin.
    spin_angle_change[0] = 0.0;
    spin_angle_change[1] = 0.0;
    spin_angle_change[2] = 0.0;
  } else if (key == 'x') {
    // Reset position - spin angles.
    spin_angle[0] = 0.0;
    spin_angle[1] = 0.0;
    spin_angle[2] = 0.0;

    // Set spin.
    spin_angle_change[0] = 0.1;
    spin_angle_change[1] = 0.0;
    spin_angle_change[2] = 0.0;
  } else if (key == 'y') {
    // Reset position - spin angles.
    spin_angle[0] = 0.0;
    spin_angle[1] = 0.0;
    spin_angle[2] = 0.0;

    // Set spin.
    spin_angle_change[0] = 0.0;
    spin_angle_change[1] = 0.1;
    spin_angle_change[2] = 0.0;
  } else if (key == 'z') {
    // Reset position - spin angles.
    spin_angle[0] = 0.0;
    spin_angle[1] = 0.0;
    spin_angle[2] = 0.0;

    // Set spin.
    spin_angle_change[0] = 0.0;
    spin_angle_change[1] = 0.0;
    spin_angle_change[2] = 0.1;
  }

  // Pick random color for light.
  if (key == 'r') {
    // Pick new colors.
    light4_r = pick_random_color();
    light4_g = pick_random_color();
    light4_b = pick_random_color();

    // Set Light4 colors.
    GLfloat light4_ambient[] = {light4_r / 10.0, light4_g / 10.0,
                                light4_b / 10.0, 1.0};
    GLfloat light4_diffuse[] = {light4_r, light4_g, light4_b, 1.0};
    GLfloat light4_specular[] = {light4_r, light4_g, light4_b, 1.0};
    glLightfv(GL_LIGHT4, GL_AMBIENT, light4_ambient);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, light4_diffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR, light4_specular);
  }

  // Redisplay window.
  glutPostRedisplay();
}

void mouse_buttons(int btn, int state, int x, int y) {
  // Handle mouse button events.
  if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    old_mouse_position[0] = x;
    old_mouse_position[1] = y;
    mbc = left;
  } else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
    old_mouse_position[0] = x;
    old_mouse_position[1] = y;
    mbc = right;
  } else if (btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
    old_mouse_position[0] = x;
    old_mouse_position[1] = y;
    mbc = middle;
  } else {
    mbc = none;
  }
}

void mouse_motion(GLsizei x, GLsizei y) {
  // Calculate mouse move.
  delta_mouse_position[0] = x - old_mouse_position[0];
  old_mouse_position[0] = x;
  delta_mouse_position[1] = y - old_mouse_position[1];
  old_mouse_position[1] = y;

  // Handle mouse movement.
  if (mbc == left) {
    // Change azimuth angle with mouse X move.
    light4_azimuth_angle += delta_mouse_position[0] * pixels_to_angle;
    if (light4_azimuth_angle >= 360.0)
      light4_azimuth_angle = 0.0;
    else if (light4_azimuth_angle <= 0.0)
      light4_azimuth_angle = 360.0f;

    // Change inclination angle with mouse Y move.
    light4_inclination_angle += delta_mouse_position[1] * pixels_to_angle;
    if (light4_inclination_angle >= 360.0)
      light4_inclination_angle = 0.0;
    else if (light4_inclination_angle < 0.0)
      light4_inclination_angle = 360.0;
  } else if (mbc == right) {
    // Change azimuth angle with mouse X move.
    observer_azimuth_angle += delta_mouse_position[0] * pixels_to_angle;
    if (observer_azimuth_angle >= 360.0)
      observer_azimuth_angle = 0.0;
    else if (observer_azimuth_angle <= 0.0)
      observer_azimuth_angle = 360.0f;

    // Change inclination angle with mouse Y move.
    observer_inclination_angle += delta_mouse_position[1] * pixels_to_angle;
    if (observer_inclination_angle >= 360.0)
      observer_inclination_angle = 0.0;
    else if (observer_inclination_angle < 0.0)
      observer_inclination_angle = 360.0;
  } else if (mbc == middle) {
    // Change light4 to egg distance.
    light4_radius += delta_mouse_position[1] * pixels_to_angle * 0.25;
    if (light4_radius >= 30)
      light4_radius = 30.0;
    else if (light4_radius <= 6.0)
      light4_radius = 6.0;
  }

  // Redisplay window.
  glutPostRedisplay();
}

void my_init() {
  // Clear window.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Init points of big egg.
  egg_points = new point3*[N];
  for (int i = 0; i < N; i++) {
    egg_points[i] = new point3[N];
  }

  // Init normal vectors of big egg.
  egg_normals = new point3*[N];
  for (int i = 0; i < N; i++) {
    egg_normals[i] = new point3[N];
  }

  float mesh_size = 1.0 / N;
  for (int i = 0; i < N; i++) {
    float u = i * mesh_size;
    for (int j = 0; j < N; j++) {
      float v = j * mesh_size;
      // Calculate egg points.
      egg_points[i][j][0] =
          (-90.0 * powf(u, 5) + 225.0 * powf(u, 4) - 270.0 * powf(u, 3) +
           180.0 * powf(u, 2) - 45.0 * u) *
          cosf(PI * v);
      egg_points[i][j][1] =
          160.0 * powf(u, 4) - 320.0 * powf(u, 3) + 160.0 * powf(u, 2);
      egg_points[i][j][2] =
          (-90.0 * powf(u, 5) + 225.0 * powf(u, 4) - 270.0 * powf(u, 3) +
           180.0 * powf(u, 2) - 45.0 * u) *
          sinf(PI * v);

      // Derivatives.
      float x_u = (-450.0 * powf(u, 4) + 900.0 * powf(u, 3) -
                   810.0 * powf(u, 2) + 360.0 * u - 45.0) *
                  cosf(PI * v);
      float x_v = PI *
                  (90.0 * powf(u, 5) - 225.0 * powf(u, 4) + 270.0 * powf(u, 3) -
                   180.0 * powf(u, 2) + 45.0 * u) *
                  sinf(PI * v);

      float y_u = 640.0 * powf(u, 3) - 960.0 * powf(u, 2) + 320.0 * u;
      float y_v = 0.0;

      float z_u = (-450.0 * powf(u, 4) + 900.0 * powf(u, 3) -
                   810.0 * powf(u, 2) + 360.0 * u - 45) *
                  sinf(PI * v);
      float z_v = -PI *
                  (90.0 * powf(u, 5) - 225.0 * powf(u, 4) + 270.0 * powf(u, 3) -
                   180.0 * powf(u, 2) + 45.0 * u) *
                  cos(PI * v);

      // Calculate egg normal vectors.
      // Calculate vector coordiantes.
      point3 normal_vector = {y_u * z_v - z_u * y_v, z_u * x_v - x_u * z_v,
                              x_u * y_v - y_u * x_v};

      // Calculate vector length.
      float normal_vector_length =
          sqrt(powf(normal_vector[0], 2) + powf(normal_vector[1], 2) +
               powf(normal_vector[2], 2));

      // Normalize vector.
      if (normal_vector_length == 0) {
        // Special case for "top" and "bottom" of egg.
        if (i == 0 || i == N / 2) {
          normal_vector[1] = -1;
        }
        normal_vector_length = 1.0;
      }

      normal_vector[0] /= normal_vector_length;
      normal_vector[1] /= normal_vector_length;
      normal_vector[2] /= normal_vector_length;

      // Save normal vector.
      egg_normals[i][j][0] = normal_vector[0];
      egg_normals[i][j][1] = normal_vector[1];
      egg_normals[i][j][2] = normal_vector[2];

      // Vectors for one half of the egg must be inverted.
      if (i >= N / 2) {
        egg_normals[i][j][0] *= -1;
        egg_normals[i][j][1] *= -1;
        egg_normals[i][j][2] *= -1;
      }
    }
  }

  // Common lighting settings.
  // Material settings common for all lights.
  GLfloat mat_ambient[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_shininess = {20.0};
  // Attenuation settings common for all lights.
  GLfloat att_constant = {1.0};
  GLfloat att_linear = {0.05};
  GLfloat att_quadratic = {0.001};
  // Materials.
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

  // Light0 - Static white light.
  // Position.
  GLfloat light0_position[] = {0.0, 0.0, 10.0, 1.0};
  // Colors.
  GLfloat light0_ambient[] = {0.1, 0.1, 0.1, 1.0};
  GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light0_specular[] = {1.0, 1.0, 1.0, 1.0};
  // Setup.
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

  // Light1 - Red light, moving around X axis.
  GLfloat light1_ambient[] = {0.1, 0.0, 0.0, 1.0};
  GLfloat light1_diffuse[] = {1.0, 0.0, 0.0, 1.0};
  GLfloat light1_specular[] = {1.0, 0.0, 0.0, 1.0};
  // Setup
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, att_constant);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, att_linear);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att_quadratic);

  // Light2 - Green light, moving around Y axis.
  // Colors
  GLfloat light2_ambient[] = {0.0, 0.1, 0.0, 1.0};
  GLfloat light2_diffuse[] = {0.0, 1.0, 0.0, 1.0};
  GLfloat light2_specular[] = {0.0, 1.0, 0.0, 1.0};
  // Setup
  glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
  glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
  glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
  glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, att_constant);
  glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, att_linear);
  glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, att_quadratic);

  // Light3 - Green light, moving around Z axis.
  // Colors.
  GLfloat light3_ambient[] = {0.0, 0.0, 0.1, 1.0};
  GLfloat light3_diffuse[] = {0.0, 0.0, 1.0, 1.0};
  GLfloat light3_specular[] = {0.0, 0.0, 1.0, 1.0};
  // Setup
  glLightfv(GL_LIGHT3, GL_AMBIENT, light3_ambient);
  glLightfv(GL_LIGHT3, GL_DIFFUSE, light3_diffuse);
  glLightfv(GL_LIGHT3, GL_SPECULAR, light3_specular);
  glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
  glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, att_constant);
  glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, att_linear);
  glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, att_quadratic);

  // Light4 - Random color, position controlled with mouse.
  // Color.
  light4_r = pick_random_color();
  light4_g = pick_random_color();
  light4_b = pick_random_color();
  GLfloat light4_ambient[] = {light4_r / 10.0, light4_g / 10.0, light4_b / 10.0,
                              1.0};
  GLfloat light4_diffuse[] = {light4_r, light4_g, light4_b, 1.0};
  GLfloat light4_specular[] = {light4_r, light4_g, light4_b, 1.0};
  // Setup
  glLightfv(GL_LIGHT4, GL_AMBIENT, light4_ambient);
  glLightfv(GL_LIGHT4, GL_DIFFUSE, light4_diffuse);
  glLightfv(GL_LIGHT4, GL_SPECULAR, light4_specular);
  glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
  glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION, att_constant);
  glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, att_linear);
  glLightf(GL_LIGHT4, GL_QUADRATIC_ATTENUATION, att_quadratic);

  // Enable lights.
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);
  glEnable(GL_LIGHT3);
  glEnable(GL_LIGHT4);

  // Smooth ligting.
  glShadeModel(GL_SMOOTH);

  // Enable ligting.
  glEnable(GL_LIGHTING);

  // Enable z-buffor.
  glEnable(GL_DEPTH_TEST);
}

void my_closing() {
  // Deallocate memory
  for (int i = 0; i < N; i++) {
    delete[] egg_points[i];
    delete[] egg_normals[i];
  }
  delete[] egg_points;
  delete[] egg_normals;
}

void render_scene() {
  // Clear screen.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Clear matrix.
  glLoadIdentity();

  // Set up observer.
  look_at();

  // Move lights automatically.
  move_light1();
  move_light2();
  move_light3();

  // Move light4 - controlled with mouse.
  move_light4();

  // -- AXES --
  if (show_axes) {
    // Push matrix on stack.
    glPushMatrix();
    // Draw axes.
    draw_axes();
    // Pop matrix from stack.
    glPopMatrix();
  }

  // -- BIG EGG --
  // Push matrix on stack.
  glPushMatrix();
  // Spin X.
  glRotatef(spin_angle[0], 1.0, 0.0, 0.0);
  // Spin Y.
  glRotatef(spin_angle[1], 0.0, 1.0, 0.0);
  // Spin Z.
  glRotatef(spin_angle[2], 0.0, 0.0, 1.0);
  // Move egg "down".
  glTranslated(0.0, -5.0, 0.0);
  // Draw big egg.
  draw_egg(egg_points, egg_normals, N);
  // Pop matrix from stack.
  glPopMatrix();

  // Do drawing.
  glFlush();
  // Swaps the buffers of the current window if double buffered.
  glutSwapBuffers();
}

void change_size(GLsizei horizontal, GLsizei vertical) {
  // Set pixels to angle ratio.
  pixels_to_angle = 360.0 / static_cast<float>(horizontal);

  // Set matrix to projection matrix.
  glMatrixMode(GL_PROJECTION);

  // Clear matrix.
  glLoadIdentity();

  // Set perspective parameters
  gluPerspective(70, 1.0, 1.0, 100.0);

  // Set viewport
  if (horizontal <= vertical) {
    glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
  } else {
    glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
  }

  // Set matrix to modelview matrix.
  glMatrixMode(GL_MODELVIEW);

  // Clear matrix.
  glLoadIdentity();
}

void draw_axes() {
  // X axis points.
  point3 x_min = {-10.0, 0.0, 0.0};
  point3 x_max = {10.0, 0.0, 0.0};

  // Y axis points.
  point3 y_min = {0.0, -10.0, 0.0};
  point3 y_max = {0.0, 10.0, 0.0};

  // Z axis points.
  point3 z_min = {0.0, 0.0, -10.0};
  point3 z_max = {0.0, 0.0, 10.0};

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

void draw_egg(point3** egg_points, point3** egg_normals, int n) {
  glBegin(GL_TRIANGLES);
  glColor3f(1.0, 1.0, 1.0);
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n - 1; j++) {
      /*	Points:
       *	 C - D
       *	 | \ |
       *	 A - B
       */

      // Triangle ABC connecting two semicircles:
      // Point A:
      glNormal3fv(egg_normals[i][j]);
      glVertex3fv(egg_points[i][j]);
      // Point B:
      glNormal3fv(egg_normals[i][j + 1]);
      glVertex3fv(egg_points[i][j + 1]);
      // Point C:
      glNormal3fv(egg_normals[(i + 1) % n][j]);
      glVertex3fv(egg_points[(i + 1) % n][j]);

      // Triangle BCD connecting two semicircles:
      // Point B:
      glNormal3fv(egg_normals[i][j + 1]);
      glVertex3fv(egg_points[i][j + 1]);
      // Point C:
      glNormal3fv(egg_normals[(i + 1) % n][j]);
      glVertex3fv(egg_points[(i + 1) % n][j]);
      // Point D:
      glNormal3fv(egg_normals[(i + 1) % n][j + 1]);
      glVertex3fv(egg_points[(i + 1) % n][j + 1]);
    }

    // Triangle ABC:
    // Point A:
    glNormal3fv(egg_normals[i][n - 1]);
    glVertex3fv(egg_points[i][n - 1]);
    // Point B:
    glNormal3fv(egg_normals[(n - i) % n][0]);
    glVertex3fv(egg_points[(n - i) % n][0]);
    // Point C:
    glNormal3fv(egg_normals[(i + 1) % n][n - 1]);
    glVertex3fv(egg_points[(i + 1) % n][n - 1]);

    // Triangle BCD:
    // Point B:
    glNormal3fv(egg_normals[(n - i) % n][0]);
    glVertex3fv(egg_points[(n - i) % n][0]);
    // Point C:
    glNormal3fv(egg_normals[(i + 1) % n][n - 1]);
    glVertex3fv(egg_points[(i + 1) % n][n - 1]);
    // Point D:
    glNormal3fv(egg_normals[(n - i - 1) % n][0]);
    glVertex3fv(egg_points[(n - i - 1) % n][0]);
  }
  glEnd();
}

void spin_egg() {
  // Set spin angles.
  spin_angle[0] += spin_angle_change[0];
  if (abs(spin_angle[0] > 360.0)) spin_angle[0] = 0.0;

  spin_angle[1] += spin_angle_change[1];
  if (abs(spin_angle[1] > 360.0)) spin_angle[1] = 0.0;

  spin_angle[2] += spin_angle_change[2];
  if (abs(spin_angle[2] > 360.0)) spin_angle[2] = 0.0;

  // Redisplay window.
  glutPostRedisplay();
}

float pick_random_color() {
  // Returns random component of RGB color.
  return static_cast<float>(rand() % 101 / 100.0f);
}

void look_at() {
  // Calculate observer cartesian position.
  observer_position[0] = observer_radius *
                         cosf(observer_azimuth_angle * DEG_TO_RAD) *
                         cosf(observer_inclination_angle * DEG_TO_RAD);
  observer_position[1] =
      observer_radius * sinf(observer_inclination_angle * DEG_TO_RAD);
  observer_position[2] = observer_radius *
                         sinf(observer_azimuth_angle * DEG_TO_RAD) *
                         cosf(observer_inclination_angle * DEG_TO_RAD);
  // Set observer orientation UP_Y vector.
  float up_y =
      (observer_inclination_angle >= 90.0 && observer_inclination_angle < 270.0)
          ? -1.0
          : 1.0;
  // Set observer position, look target and orientation.
  gluLookAt(observer_position[0], observer_position[1], observer_position[2],
            0.0, 0.0, 0.0, 0.0, up_y, 0.0);
}

void move_light1() {
  // Calculate Light1 cartesian position.
  light1_position[0] = light1_radius * cosf(light1_azimuth_angle * DEG_TO_RAD) *
                       cosf(light1_inclination_angle * DEG_TO_RAD);
  light1_position[1] =
      light1_radius * sinf(light1_inclination_angle * DEG_TO_RAD);
  light1_position[2] = light1_radius * sinf(light1_azimuth_angle * DEG_TO_RAD) *
                       cosf(light1_inclination_angle * DEG_TO_RAD);

  // Set Light1 position.
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

  // Move light1.
  light1_inclination_angle += 0.25;
  if (light1_inclination_angle >= 360.0) {
    light1_inclination_angle = 0.0;
  }
}

void move_light2() {
  // Calculate Light2 cartesian position.
  light2_position[0] = light2_radius * cosf(light2_azimuth_angle * DEG_TO_RAD) *
                       cosf(light2_inclination_angle * DEG_TO_RAD);
  light2_position[1] =
      light2_radius * sinf(light2_inclination_angle * DEG_TO_RAD);
  light2_position[2] = light2_radius * sinf(light2_azimuth_angle * DEG_TO_RAD) *
                       cosf(light2_inclination_angle * DEG_TO_RAD);

  // Set Light2 position.
  glLightfv(GL_LIGHT2, GL_POSITION, light2_position);

  // Move Light2.
  light2_azimuth_angle += 0.25;
  if (light2_azimuth_angle >= 360.0) {
    light2_azimuth_angle = 0.0;
  }
}

void move_light3() {
  // Calculate Light3 cartesian position.
  light3_position[0] = light3_radius * cosf(light3_azimuth_angle * DEG_TO_RAD) *
                       cosf(light3_inclination_angle * DEG_TO_RAD);
  light3_position[1] =
      light3_radius * sinf(light3_inclination_angle * DEG_TO_RAD);
  light3_position[2] = light3_radius * sinf(light3_azimuth_angle * DEG_TO_RAD) *
                       cosf(light3_inclination_angle * DEG_TO_RAD);

  // Set Light3 position.
  glLightfv(GL_LIGHT3, GL_POSITION, light3_position);

  // Move Light3
  light3_inclination_angle += 0.25;
  if (light3_inclination_angle >= 360.0) {
    light3_inclination_angle = 0.0;
  }
}

void move_light4() {
  // Calculate Light4 cartesian position.
  light4_position[0] = light4_radius * cosf(light4_azimuth_angle * DEG_TO_RAD) *
                       cosf(light4_inclination_angle * DEG_TO_RAD);
  light4_position[1] =
      light4_radius * sinf(light4_inclination_angle * DEG_TO_RAD);
  light4_position[2] = light4_radius * sinf(light4_azimuth_angle * DEG_TO_RAD) *
                       cosf(light4_inclination_angle * DEG_TO_RAD);

  // Set Light4 position.
  glLightfv(GL_LIGHT4, GL_POSITION, light4_position);
}
// --- Functions definitions END ---
