#include "main.h"

using namespace std;

//****************************************************
// Global Variables
//****************************************************
Scene *scene;
UCB::ImageSaver * imgSaver;
int colour = 0;
int frameCount = 0;
mat4 tempMat;
/* The recursive render function.  
 Note: This is the only function you need to edit for this assignment! 
 (but feel free to edit anything you'd like to.) */
BoundingBox RenderInstance(SceneInstance *n, vec3 color, int lod)
{
	n->computeTransform(tempMat,frameCount);
	const double tmp[16]={tempMat[0][0],tempMat[1][0],tempMat[2][0],tempMat[3][0],tempMat[0][1],tempMat[1][1],tempMat[2][1],tempMat[3][1],tempMat[0][2],tempMat[1][2],tempMat[2][2],tempMat[3][2],tempMat[0][3],tempMat[1][3],tempMat[2][3],tempMat[3][3]};
	n->computeColor(color,frameCount);
	n->computeLOD(lod,frameCount);
	glPushMatrix();
	glMultMatrixd(tmp);
	BoundingBox bounds;
	mat4 localMat=tempMat;
	//scene->PrintScene();
	//cout << "pos1" << n->getName() << endl;
	colour++;
	for(int i=0;i<(n->getChild()->getChildCount());i++){
		//colour++;
		BoundingBox tempBounds=RenderInstance(n->getChild()->getChild(i),color,lod);
		bounds.expand(tempBounds);
	}
	if(n->getChild()->getChildCount()==0){
		glColor3d(color[0],color[1],color[2]);
		switch (lod) {
		case 0:
			break;
		case 1:
			n->getChild()->getPolygon()->getBoundingBox().draw();
			break;
		case 2:
			n->getChild()->getPolygon()->draw(GL_LINE_LOOP);
			break;
		case 3:
			n->getChild()->getPolygon()->draw(GL_POLYGON);
			break;
		}
	}
	colour--;
	switch (colour) {
		case 0:
			glColor3d(1.0,0,0);
			break;
		case 1:
			glColor3d(1.0,1.0,0);
			break;
		case 2:
			glColor3d(0.0,1.0,0);
			break;
		case 3:
			glColor3d(0.0,1.0,1.0);
			break;
		case 4:
			glColor3d(0.0,0,1.0);
			break;
		case 5:
			glColor3d(1.0,0,1.0);
			break;
	}
	if(n->getChild()->getChildCount()==0){
		n->getChild()->getPolygon()->getBoundingBox().draw();
	}
	else{
		bounds.draw();
	}
	glPopMatrix();

	BoundingBox bb;
	if(n->getChild()->getChildCount()==0){
		bb=n->getChild()->getPolygon()->getBoundingBox();
	}
	else{
		bb=bounds;
	}
	vec2 max;
	vec2 min;
	bb.getMax(max);
	bb.getMin(min);
	vec4 topRight(max[0],max[1],0,1);
	vec4 bottomLeft(min[0],min[1],0,1);
	vec4 topLeft(min[0],max[1],0,1);
	vec4 bottomRight(max[0],min[1],0,1);
	topRight=localMat*topRight;
	bottomLeft=localMat*bottomLeft;
	topLeft=localMat*topLeft;
	bottomRight=localMat*bottomRight;
	vec2 vert1(topRight[0],topRight[1]);
	vec2 vert2(bottomRight[0],bottomRight[1]);
	vec2 vert3(bottomLeft[0],bottomLeft[1]);
	vec2 vert4(topLeft[0],topLeft[1]);
	BoundingBox bb2;
	bb2.expand(vert1);
	bb2.expand(vert2);
	bb2.expand(vert3);
	bb2.expand(vert4);
	return bb2;
		
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
    glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"

    RenderInstance(scene->getRoot(), vec3(1,1,1), 3); // render at solid LOD

	//Now that we've drawn on the buffer, swap the drawing buffer and the displaying buffer.
	glutSwapBuffers();

}


//-------------------------------------------------------------------------------
/// \brief	Called when the screen gets resized.
/// This gives you the opportunity to set up all the relevant transforms.
///
void reshape(int w, int h) {
	//Set up the viewport to ignore any size change.
	glViewport(0,0,w,h);

	//Set up the PROJECTION transformationstack to be a simple orthographic [-1, +1] projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    if (w < h)
	    glOrtho(-20, 20, -h*20/float(w), h*20/float(w), 1, -1);	// resize type = stretch
    else
        glOrtho(-w*20/float(h), w*20/float(h), -20, 20, 1, -1);	// resize type = stretch

	//Set the MODELVIEW transformation stack to the identity.
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
		case 97:
			int formerFC=frameCount;
			for(int t=0;t<200;t = t + 2){
				glClear(GL_COLOR_BUFFER_BIT);
				glMatrixMode(GL_MODELVIEW);					
				glLoadIdentity();							
				frameCount=t;
				RenderInstance(scene->getRoot(), vec3(1,1,1), 3);
				imgSaver->saveFrame(600, 600);
				}
			frameCount=formerFC;
			break;
	}
}

//-------------------------------------------------------------------------------
/// Called to update the screen at 30 fps.
void frameTimer(int value){

    frameCount++;
    glutPostRedisplay();
    glutTimerFunc(1000/30, frameTimer, 1);
}


//-------------------------------------------------------------------------------
/// Initialize the environment
int main(int argc,char** argv) {
	//Initialize OpenGL
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);

	//Here we load a scene from a scene file.
	if (argc < 2) {
	    cout << "USAGE: show scene.scd" << endl;
	    exit(1);
	}
    scene = new Scene(argv[1]);
	//scene = new Scene("scene.scd");

	//Initialize the screen capture class to save BMP captures
	//in the current directory, with the prefix "output"
	imgSaver = new UCB::ImageSaver("./", "output");

	//Create OpenGL Window
	glutInitWindowSize(600,600);
	glutInitWindowPosition(0,0);
	glutCreateWindow("CS184 Framework");

	//Register event handlers with OpenGL.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(myKeyboardFunc);

    frameTimer(0);

	//And Go!
	glutMainLoop();
}
