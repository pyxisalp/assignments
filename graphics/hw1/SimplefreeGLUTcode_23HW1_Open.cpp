#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define TO_RADIAN		0.017453292519943296 

int rightbuttonpressed = 0;
int leftbuttonpressed = 0, center_selected = 0;
int picking = 0;
int translation = 0;
int scaling = 0;
int reflection = 0;
int prev_x = 0, prev_y = 0;

float r, g, b; // Background color
float px, py, qx, qy; // Line (px, py) --> (qx, qy)
int n_object_points = 6;
int n_triangle_points = 3;
float object[6][2], object_center_x, object_center_y;
float triangle[3][2], triangle_center_x, triangle_center_y;
float rotation_angle_in_degree;
int window_width, window_height;

void draw_axes() {
	glLineWidth(3.0f);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.975f, 0.025f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(0.975f, -0.025f);
	glVertex2f(1.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(0.025f, 0.975f);
	glVertex2f(0.0f, 1.0f);
	glVertex2f(-0.025f, 0.975f);
	glVertex2f(0.0f, 1.0f);
	glEnd();
	glLineWidth(1.0f);

	glPointSize(7.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 0.0f);
	glEnd();
	glPointSize(1.0f);
}
void draw_line(float px, float py, float qx, float qy) {
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(px, py); 
	glVertex2f(qx, qy);
	glEnd();
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(px, py);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(qx, qy);
	glEnd();
	glPointSize(1.0f);
}

void draw_object(void) {
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 1.0f, 0.0f);
	for (int i = 0; i < 6; i++)
		glVertex2f(object[i][0], object[i][1]);
	glEnd();
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 6; i++)
		glVertex2f(object[i][0], object[i][1]);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(object_center_x, object_center_y);
	glEnd();
	glPointSize(1.0f);
}

void draw_triangle(void) {
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 3; i++)
		glVertex2f(triangle[i][0], triangle[i][1]);
	glEnd();
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	glColor3f(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < 3; i++)
		glVertex2f(triangle[i][0], triangle[i][1]);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(triangle_center_x, triangle_center_y);
	glEnd();
	glPointSize(1.0f);
}

void display(void) {
	glClearColor(r, g, b, 1.0f); 
	glClear(GL_COLOR_BUFFER_BIT);

	draw_axes();
	draw_line(px, py, qx, qy);
	draw_object();
	draw_triangle();
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'r':
		r = 1.0f; g = b = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'g':
		g = 1.0f; r = b = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 'b':
		b = 1.0f; r = g = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 's':
		r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case 't':
		triangle_center_x = triangle_center_y = 0.0f;
		if (reflection) {
			for (int i = 0; i < n_triangle_points; i++) {
				triangle[i][1] = -triangle[i][1];
				triangle_center_x += triangle[i][0];
				triangle_center_y += triangle[i][1];
			}
			reflection = 0;
		}
		else {
			for (int i = 0; i < n_triangle_points; i++) {
				triangle[i][0] = -triangle[i][0];
				triangle_center_x += triangle[i][0];
				triangle_center_y += triangle[i][1];
			}
			reflection = 1;
		}

		triangle_center_x /= n_triangle_points;
		triangle_center_y /= n_triangle_points;

		glutPostRedisplay();
		break;
	case 'q':
		glutLeaveMainLoop(); 
		break;
	}
}

void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		r -= 0.1f;
		if (r < 0.0f) r = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		r += 0.1f;
		if (r > 1.0f) r = 1.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		g -= 0.1f;
		if (g < 0.0f) g = 0.0f;
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		g += 0.1f;
		if (g > 1.0f) g = 1.0f;  
		fprintf(stdout, "$$$ The new window background color is (%5.3f, %5.3f, %5.3f).\n", r, g, b);
		glutPostRedisplay();
		break;
	}
}

int prevx, prevy;
void mousepress(int button, int state, int x, int y) {
	// (e) 기능
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)
		&& (glutGetModifiers() == GLUT_ACTIVE_SHIFT)) {
		float diff_x = fabs((px - (x - window_width / 2.0f) / 250.0f));
		float diff_y = fabs(py - ((window_height - y) - window_height / 2.0f) / 250.0f);

		if ((diff_x < 0.01) && (diff_y < 0.01)) picking = 1;
	}
	else picking = 0;

	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)
		&& (glutGetModifiers() == GLUT_ACTIVE_ALT)) {
		translation = 1;
		prev_x = x, prev_y = y;	
	}
	else {
		translation = 0;
	}

	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)
		&& (glutGetModifiers() == GLUT_ACTIVE_CTRL)) {
		scaling = 1;
		prev_x = x;		
	}
	else {
		scaling = 0;
	}
}

void mousewheel(int button, int dir, int x, int y) {
	// T(-px, -py)
	qx -= px;
	qy -= py;

	// R(dir * TO_RADIAN)
	qx = qx * cos(dir * TO_RADIAN) - qy * sin(dir * TO_RADIAN);
	qy = qx * sin(dir * TO_RADIAN) + qy * cos(dir * TO_RADIAN);

	// T(px, py)
	qx += px;
	qy += py;

	glutPostRedisplay();
}

void mousemove(int x, int y) {
	// (d) 기능
	if (picking) {
		px = (x - window_width / 2.0f) / 250.0f;
		py = ((window_height - y) - window_height / 2.0f) / 250.0f;

		glutPostRedisplay();
	}
	else if (translation) {
		int del_x = x - prev_x, del_y = y - prev_y;

		object_center_x = object_center_y = 0.0f;
		for (int i = 0; i < n_object_points; i++) {
			object[i][0] = object[i][0] + del_x / 250.0f;
			object[i][1] = object[i][1] - del_y / 250.0f;

			object_center_x += object[i][0];
			object_center_y += object[i][1];
		}
		object_center_x /= n_object_points;
		object_center_y /= n_object_points;
		prev_x = x, prev_y = y;

		glutPostRedisplay();
	}
	else if (scaling) {
		int del_x = x - prev_x;

		for (int i = 0; i < n_object_points; i++) {
			// T(-object_center_x, -object_center_y)
			object[i][0] -= object_center_x;
			object[i][1] -= object_center_y;

			// S(1 + del_x / 100.0f)
			object[i][0] *= (1 + del_x / 100.0f);
			object[i][1] *= (1 + del_x / 100.0f);
			
			// T(-object_center_x, -object_center_y)
			object[i][0] += object_center_x;
			object[i][1] += object_center_y;
		}
		prev_x = x;

		glutPostRedisplay();
	}
	
	
}
	
void reshape(int width, int height) {
	// DO NOT MODIFY THIS FUNCTION!!!
	window_width = width, window_height = height;
	glViewport(0.0f, 0.0f, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-window_width / 500.0f, window_width / 500.0f,  -window_height / 500.0f, window_height / 500.0f, -1.0f, 1.0f);

	glutPostRedisplay();
}


void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMouseWheelFunc(mousewheel);
	glutMotionFunc(mousemove);
	glutReshapeFunc(reshape);
 	glutCloseFunc(close);
}

void initialize_renderer(void) {
	register_callbacks();
	r = 250.0f / 255.0f, g = 128.0f / 255.0f, b = 114.0f / 255.0f; // Background color = Salmon
	px = -0.60f, py = 0.40f, qx = -0.40f, qy = 0.25f;
	rotation_angle_in_degree = 1.0f; // 1 degree
	
	float sq_cx = 0.50f, sq_cy = -0.50f, sq_side = 0.10f;
	object[0][0] = sq_cx + sq_side;
	object[0][1] = sq_cy + sq_side;
	object[1][0] = sq_cx + 2.5 * sq_side;
	object[1][1] = sq_cy + 4.0 * sq_side;
	object[2][0] = sq_cx + 5.5 * sq_side;
	object[2][1] = sq_cy + 4.0 * sq_side;
	object[3][0] = sq_cx + 7.0 * sq_side;
	object[3][1] = sq_cy + sq_side;
	object[4][0] = sq_cx + 5.5 * sq_side;
	object[4][1] = sq_cy - 2.0 * sq_side;
	object[5][0] = sq_cx + 2.5 * sq_side;
	object[5][1] = sq_cy - 2.0 * sq_side;
	object_center_x = object_center_y = 0.0f;

	for (int i = 0; i < n_object_points; i++) {
		object_center_x += object[i][0];
		object_center_y += object[i][1];
	}
	object_center_x /= n_object_points;
	object_center_y /= n_object_points;

	float tr_cx = -1.00f, tr_cy = -1.00f, tr_side = 0.15f;
	triangle[0][0] = tr_cx + tr_side;
	triangle[0][1] = tr_cy + tr_side;
	triangle[1][0] = tr_cx + 4.0 * tr_side;
	triangle[1][1] = tr_cy + tr_side;
	triangle[2][0] = tr_cx + 2.5 * tr_side;
	triangle[2][1] = tr_cy + 3.0 * tr_side;
	triangle_center_x = triangle_center_y = 0.0f;

	for (int i = 0; i < n_triangle_points; i++) {
		triangle_center_x += triangle[i][0];
		triangle_center_y += triangle[i][1];
	}
	triangle_center_x /= n_triangle_points;
	triangle_center_y /= n_triangle_points;
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 4
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 Simple 2D Transformations";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'r', 'g', 'b', 's', 'q'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: SHIFT/L-click and move, ALT/R-click and move, CTRL/R-click and move",
		"    - Wheel used: up and down scroll"
		"    - Other operations: window size change"
	};

	glutInit(&argc, argv);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!
 //	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(750, 750);
	glutInitWindowPosition(500, 200);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

   // glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT); // default
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	
	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
}