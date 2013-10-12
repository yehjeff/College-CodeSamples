#include "main.h"
#include <ctime>
#include <iostream>

using namespace std;


//****************************************************
// Some Classes
//****************************************************
class Viewport {
public:
    Viewport(): mousePos(0.0,0.0) { orientation = identity3D(); };
	int w, h; // width and height
	vec2 mousePos;
    mat4 orientation;
};

//****************************************************
// Global Variables
//****************************************************
Viewport viewport;
UCB::ImageSaver * imgSaver;
int frameCount = 0;
SplineCoaster *coaster;
enum {VIEW_FIRSTPERSON, VIEW_THIRDPERSON, VIEW_MAX};
int viewMode = VIEW_THIRDPERSON;
double x=0;
double lastY=1000;
double displacement=1;
double totalEnergy=1900;
double mass=0.01;
// A simple helper function to load a mat4 into opengl
void applyMat4(mat4 &m) {
	double glmat[16];
	int idx = 0;
	for (int j = 0; j < 4; j++) 
		for (int i = 0; i < 4; i++)
			glmat[idx++] = m[i][j];
	glMultMatrixd(glmat);
}

// replace this with something cooler!
void drawCart(double size) {
    glPushMatrix();
glTranslated(1.5 - ((frameCount+35)%70)/105.0, 0.6 + ((frameCount+35)%70)/105.0,
0);
glRotated(90,1,0,0);
glColor3d(1,1,1);
glutSolidTorus(.05, ((frameCount+35)%70)/210.0,50,50);
glPopMatrix();
glPushMatrix();
glTranslated(1.5 - (frameCount%70)/105.0, 0.6 + (frameCount%70)/105.0, 0);
glRotated(90,1,0,0);
glColor3d(1,1,1);
glutSolidTorus(.05, (frameCount%70)/210.0,50,50);
glPopMatrix();

glColor3d(0, 0, 0);
glPushMatrix();
glTranslated(0.7, .2, .4);
glutSolidSphere(.1,50,50);
glPopMatrix();
glPushMatrix();
glTranslated(0.7, .2, -.4);
glutSolidSphere(.1,50,50);
glPopMatrix();
glPushMatrix();
glTranslated(.76,-.15,0);
glScaled(.4,.4,.4);
glRotated(135,0,0,1);
glRotated(90,1,0,0);
glutSolidCone(1,.5,50,50);
glPopMatrix();

glPushMatrix();
glTranslated(.55,-.7,.85);
glScaled(.19, .19, .19);
glutSolidTorus(.6, 1, 50, 50);
glPopMatrix();
glPushMatrix();
glTranslated(-.55,-.7,.85);
glScaled(.19, .19, .19);
glutSolidTorus(.6, 1, 50, 50);
glPopMatrix();
glPushMatrix();
glTranslated(.55,-.7,-.85);
glScaled(.19, .19, .19);
glutSolidTorus(.6, 1, 50, 50);
glPopMatrix();
glPushMatrix();
glTranslated(-.55,-.7,-.85);
glScaled(.19, .19, .19);
glutSolidTorus(.6, 1, 50, 50);
glPopMatrix();

glColor3d(1.2, .1, .5);
glutSolidTeapot(1);
glRotated(-90, 0, 1, 0);
glColor3d(1.2, .1, .5);
glPushMatrix();
glRotated(sin(frameCount/7.0)*25, 0, 0, 1);
glScaled(1,.1,.5);
glTranslated(1, 0, 0);
   glutSolidSphere(1,40,40);
glPopMatrix();
glPushMatrix();
glRotated(sin(frameCount/7.0)*-25, 0, 0, 1);
glScaled(1,.1,.5);
glTranslated(-1, 0, 0);
   glutSolidSphere(1,40,40);
glPopMatrix();
}

double calcSpeed(double y){
	double KE=totalEnergy-mass*10000*y;
	if(KE<0){
		cout<<"not enough energy"<<endl;
	}
	double v=pow(2*KE/mass,0.5);
	return v;
}
//-------------------------------------------------------------------------------
/// You will be calling all of your drawing-related code from this function.
/// Nowhere else in your code should you use glBegin(...) and glEnd() except code
/// called from this method.
///
/// To force a redraw of the screen (eg. after mouse events or the like) simply call
/// glutPostRedisplay();
void display() {

	//Clear Buffers
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);				
	glLoadIdentity();							
    if (viewMode == VIEW_THIRDPERSON) {
		vec3 forward3D=coaster->sampleForward_nonzero(x).normalize(); 
		vec3 up3D=coaster->sampleUp(x).normalize();					 
		vec4 forward=vec4(forward3D,0);
		vec4 up=vec4(up3D,0);
		vec4 left=-vec4((forward3D^up3D).normalize(),0);
		vec4 position=vec4(coaster->sample(x).point,1);
		mat4 cartMat(forward,up,left,position);
		glPushMatrix();
        glTranslatef(0,-5,-55);
        applyMat4(viewport.orientation);
		applyMat4(cartMat.transpose());
		drawCart(1);
		glPopMatrix();
		glTranslatef(0,-5,-55);
        applyMat4(viewport.orientation);
		coaster->renderWithDisplayList(100,.3,3,.2,0);
		x=x+calcSpeed(position[VY])*0.000005;
    }
	if (viewMode == VIEW_FIRSTPERSON) {
		vec3 forward3D=coaster->sampleForward_nonzero(x).normalize(); 
		vec3 up3D=coaster->sampleUp(x).normalize();					 
		vec4 forward=vec4(forward3D,0);
		vec4 up=vec4(up3D,0);
		vec4 left=-vec4((forward3D^up3D).normalize(),0);
		vec4 position=vec4(coaster->sample(x).point,1);	
		mat4 cartMat(forward,up,left,position);
		vec3 u((up3D^(-forward3D)).normalize());
		vec3 v(-forward3D^u);
		mat4 camMat(vec4(u,0),vec4(v,0),-forward,position+1.2*up-1*forward);
        applyMat4(camMat.transpose().inverse());
		coaster->renderWithDisplayList(100,.3,3,.2,0);
		glPushMatrix();
		applyMat4(cartMat.transpose());
		drawCart(1);
		glPopMatrix();
		x=x+calcSpeed(position[VY])*0.000005;
	}
	glutSwapBuffers();

}


//-------------------------------------------------------------------------------
/// \brief	Called when the screen gets resized.
/// This gives you the opportunity to set up all the relevant transforms.
///
void reshape(int w, int h) {
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, ((double)w / MAX(h, 1)), 1.0, 100.0);
	//glOrtho(-10,10,-10,10,1,100);

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//-------------------------------------------------------------------------------
/// Called to handle keyboard events.
void myKeyboardFunc (unsigned char key, int x, int y) {
	switch (key) {
		case 27:			// Escape key
			exit(0);
			break;
        case 'S':
        case 's':
    	    imgSaver->saveFrame();
            break;
        case 'V':
        case 'v':
            viewMode = (viewMode+1)%VIEW_MAX;
            break;
		case 'g':
			for(int t=0;t<459;t++){
				display();
				frameCount++;
				imgSaver->saveFrame();
			}
			break;
	}
}

//-------------------------------------------------------------------------------
/// Called whenever the mouse moves while a button is pressed
void myActiveMotionFunc(int x, int y) {

    // Rotate viewport orientation proportional to mouse motion
    vec2 newMouse = vec2((double)x / glutGet(GLUT_WINDOW_WIDTH),(double)y / glutGet(GLUT_WINDOW_HEIGHT));
    vec2 diff = (newMouse - viewport.mousePos);
    double len = diff.length();
    if (len > .001) {
        vec3 axis = vec3(diff[1]/len, diff[0]/len, 0);
        viewport.orientation = rotation3D(axis, 180 * len) * viewport.orientation;
    }

    //Record the mouse location for drawing crosshairs
    viewport.mousePos = newMouse;

    //Force a redraw of the window.
    //glutPostRedisplay();
}


//-------------------------------------------------------------------------------
/// Called whenever the mouse moves without any buttons pressed.
void myPassiveMotionFunc(int x, int y) {
    //Record the mouse location for drawing crosshairs
    viewport.mousePos = vec2((double)x / glutGet(GLUT_WINDOW_WIDTH),(double)y / glutGet(GLUT_WINDOW_HEIGHT));

    //Force a redraw of the window.
    //glutPostRedisplay();
}

//-------------------------------------------------------------------------------
/// Called to update the screen at 30 fps.
void frameTimer(int value) {
    frameCount++;
    glutPostRedisplay();
    glutTimerFunc(1000/30, frameTimer, 1);
}



//-------------------------------------------------------------------------------
/// Initialize the environment
int main(int argc,char** argv) {
	//Initialize OpenGL
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);

	//Set up global variables
	viewport.w = 600;
	viewport.h = 600;

	if (argc < 2) {
	    cout << "USAGE: coaster coaster.trk" << endl;
	    return -1;
    } else {
        // Create the coaster
        coaster = new SplineCoaster(argv[1]);
        if (coaster->bad()) {
            cout << "Coaster file appears to not have a proper coaster in it" << endl;
            return -1;
        }
    }

	//Initialize the screen capture class to save BMP captures
	//in the current directory, with the prefix "coaster"
	imgSaver = new UCB::ImageSaver("./", "coaster");

	//Create OpenGL Window
	glutInitWindowSize(viewport.w,viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow("CS184 Framework");

	//Register event handlers with OpenGL.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(myKeyboardFunc);
	glutMotionFunc(myActiveMotionFunc);
	glutPassiveMotionFunc(myPassiveMotionFunc);
    frameTimer(0);

    glClearColor(.2,.5,.3,0);

    // set some lights
    {
       float ambient[3] = { .1f, .1f, .1f };
       float diffuse[3] = { .2f, .5f, .5f };
       float pos[4] = { 0, 5, -5, 0 };
       glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
       glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
       glLightfv(GL_LIGHT1, GL_POSITION, pos);
       glEnable(GL_LIGHT1);
    }
    {
       float ambient[3] = { .1f, .1f, .1f };
       float diffuse[3] = { .5f, .2f, .5f };
       float pos[4] = { 5, 0, -5, 0 };
       glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
       glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
       glLightfv(GL_LIGHT2, GL_POSITION, pos);
       glEnable(GL_LIGHT2);
    }
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);


	//And Go!
	glutMainLoop();
}
