#include "main.h"

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
Sweep *sweep;
Mesh *mesh;
Shader *shade;
Skeleton *skel;
Animation *anim;
Creature *creature;
// these variables track which joint is under IK, and where
int ikJoint;
double ikDepth;
// ui modes
bool playanim = false;
int ik_mode = IK_CCD;
// A simple helper function to load a mat4 into opengl
void applyMat4(mat4 &mat) {
    double glmat[16];
    int k = 0;
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            glmat[k++] = mat[j][i];
        }
    }
    glMultMatrixd(glmat);
}
void setupView() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glTranslatef(0,0,-3);
    applyMat4(viewport.orientation);
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
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"

    glTranslatef(0,0,-20); //used to be 20
    applyMat4(viewport.orientation);
	glClearColor(.4,.2,1,0);
	//glPushMatrix();
	//glTranslatef(creature->aboveSurfacePoint(frameCount/4,frameCount/2,0.3)[0],creature->aboveSurfacePoint(frameCount/4,frameCount/2,0.3)[1],creature->aboveSurfacePoint(frameCount/4,frameCount/2,0.3)[2]);
	mesh->render(*shade);
	//skel->render(ikJoint);
	//glPopMatrix();
    sweep->renderWithDisplayList(*shade,2,.3,2);
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

void playAnimation(){
	double frame=0;
	while(frame<(creature->anim->roots.size()-1)){
		creature->anim->setJoints(skel->getJointArray(),frame,skel->getRoot());
		skel->updateSkin(*mesh);
		display();
		frame+=0.01;
	}
}//need testing
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
        case 'T':
        case 't':
            shade->toggleTextureMap();
            sweep->clearDisplayList();
            break;
        case 'B':
        case 'b':
            shade->toggleBumpMap();
            sweep->clearDisplayList();
            break;
        case 'P':
        case 'p':
            shade->togglePhong();
            sweep->clearDisplayList();
            break;
		case 'g':
			for(int t=0;t<200;t++){
				display();
				frameCount++;
				viewport.orientation = viewport.orientation * rotation3D(vec3(0,1,0.8), -180 * 0.00005*t);
				if(t%2 != 0){
					imgSaver->saveFrame();
				}
			}
			break;
		case 'a': // add a frame to your animation
        case 'A':
            creature->anim->addAsFrame(skel->getJointArray(),skel->joints[skel->getRoot()].posn);
            break;
		case 'z':
			playAnimation();
			break;
	}
}
void myMouseFunc(int button, int state, int x, int y) {
    setupView();
    ikJoint = skel->pickJoint(ikDepth, vec2(x,y));
}

// helper to set joints from the animation file, using the mouse x to select the animation frame
void setJointsByAnimation(int x) {
    if (playanim && anim->orientations.size() > 1) {
        double t = double(anim->orientations.size()-1) * double(x)/double(glutGet(GLUT_WINDOW_WIDTH));
        anim->setJoints(skel->getJointArray(), t,skel->getRoot());
        skel->updateSkin(*mesh);
    }
}

//-------------------------------------------------------------------------------
/// Called whenever the mouse moves while a button is pressed
void myActiveMotionFunc(int x, int y) {
    if (ikJoint != -1 && !playanim) { // if a joint is selected for ik and we're not in animation playback mode, do ik
        vec3 target = skel->getPos(vec2(x,y), ikDepth);
        skel->inverseKinematics(ikJoint, target, ik_mode);
        skel->updateSkin(*mesh);
    } else { // else mouse movements update the view
        // Rotate viewport orientation proportional to mouse motion
        vec2 newMouse = vec2((double)x / glutGet(GLUT_WINDOW_WIDTH),(double)y / glutGet(GLUT_WINDOW_HEIGHT));
        vec2 diff = (newMouse - viewport.mousePos);
        double len = diff.length();
        if (len > .001) {
            vec3 axis = vec3(diff[1]/len, diff[0]/len, 0);
            //viewport.orientation = rotation3D(axis, 180 * len) * viewport.orientation;
			viewport.orientation = viewport.orientation * rotation3D(axis, -180 * len);
        }

        //Record the mouse location for drawing crosshairs
        viewport.mousePos = newMouse;
    }
    

    //Force a redraw of the window.
    glutPostRedisplay();
}


//-------------------------------------------------------------------------------
/// Called whenever the mouse moves without any buttons pressed.
void myPassiveMotionFunc(int x, int y) {
    ikJoint = -1;

    setJointsByAnimation(x);

    //Record the mouse location for drawing crosshairs
    viewport.mousePos = vec2((double)x / glutGet(GLUT_WINDOW_WIDTH),(double)y / glutGet(GLUT_WINDOW_HEIGHT));

    //Force a redraw of the window.
    glutPostRedisplay();
}

//-------------------------------------------------------------------------------
/// Called to update the screen at 30 fps.
void frameTimer(int value){

    frameCount++;
    glutPostRedisplay();
    glutTimerFunc(1000/30, frameTimer, 1);
}

void exitFreeimage(void) {
    FreeImage_DeInitialise();
}


//-------------------------------------------------------------------------------
/// Initialize the environment
int main(int argc,char** argv) {
	//Initialize OpenGL
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);

    FreeImage_Initialise();
    atexit(exitFreeimage);

	//Set up global variables
	viewport.w = 600;
	viewport.h = 600;
	imgSaver = new UCB::ImageSaver("./", "ik");

	if (argc < 2) {
	    cout << "USAGE: sweep sweep.trk" << endl;
	    return -1;
    }

	//Initialize the screen capture class to save BMP captures
	//in the current directory, with the prefix "sweep"
	imgSaver = new UCB::ImageSaver("./", "sweep");

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
	glutMouseFunc(myMouseFunc);
    frameTimer(0);

    // set some lights
    {
       float ambient[4] = { .1f, .1f, .1f, 1.f };
       float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.f };
       float pos[4] = { 0, 0, 1, 0 };
       glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
       glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
       glLightfv(GL_LIGHT0, GL_POSITION, pos);
       glEnable(GL_LIGHT0);
    }
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("No GLSL support\n");
		exit(1);
	}

    shade = new Shader("sweepShader.vert", "sweepShader.frag");
  
    // Create the coaster
    sweep = new Sweep(argv[1]);
	
    if (sweep->bad()) {
        cout << "Sweep file appears to not have a proper sweep in it" << endl;
        return -1;
    }
	mesh = new Mesh();
    mesh->loadFile("tempdragtest.obj");
    // load a matching skeleton
    skel = new Skeleton();
    skel->loadPinocchioFile("skeletonTemp.out");
    mesh->centerAndScale(*skel,3.0);
    // load the correspondence between skeleton and mesh
    skel->initBoneWeights("attachmentTemp.out", *mesh);
    skel->updateSkin(*mesh);
	creature=new Creature();
	creature->initialize(skel,sweep,mesh);
	//And Go!
	glutMainLoop();
}
