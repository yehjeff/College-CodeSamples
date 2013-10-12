#include "main.h"
using namespace std;

//****************************************************
// Global Variables
//****************************************************
World * world;
Film * film;
int frameCount = 0;
int buttonFlag;//0 is none, 1 is left button down, 2 is right button down
string imgName = "raycaster";
int defaultDepth = 4;

// use this to multiply colors:
inline vec3 pairwiseMult(const vec3 &a, const vec3 &b) {
	return vec3(a[0]*b[0], a[1]*b[1], a[2]*b[2]);
}

//-------------------------------------------------------------------------------
// Here you raycast a single ray, calculating the color of this ray.
/*void setColor(Ray & r,double t, vec3 n, MaterialInfo m,vec3 & color){
	vector<Light>::iterator lightIter;
	vec3 L;
	vec3 V;
	vec3 R;
	vec3 cSpec;
	for(lightIter=world->getLightsBeginIterator(LIGHT_DIRECTIONAL);lightIter!=world->getLightsEndIterator(LIGHT_DIRECTIONAL);lightIter++){
		double t1; vec3 n1; MaterialInfo m1;
		Ray r1(r.getPos(t),r.getPos(t)-vec4(lightIter->getDirection()),0.001);
		if(!world->intersect(r1,t1,n1,m1)){
			L=(-lightIter->getDirection()).normalize();
			R=n+n-L-2*((n-L)*n)*n;
			V=vec3(-r.direction(),VW).normalize();
			cSpec=m.k[MAT_KSM]*pairwiseMult(m.color,lightIter->getLightInfo().color)+(1-m.k[MAT_KSM])*lightIter->getLightInfo().color;
			color += m.k[MAT_KD]*max(0.0,(n*L))*pairwiseMult(m.color,lightIter->getLightInfo().color);
			color +=m.k[MAT_KS]*pow(max(0.0,R*V),m.k[MAT_KSP])*cSpec;
		}
	}
	for(lightIter=world->getLightsBeginIterator(LIGHT_POINT);lightIter!=world->getLightsEndIterator(LIGHT_POINT);lightIter++){
		double t1; vec3 n1; MaterialInfo m1;
		Ray r1(r.getPos(t),vec4(lightIter->getPosition(),1.0),0.001);
		if((!world->intersect(r1,t1,n1,m1))||t1>=1){
			L=(lightIter->getPosition()-vec3(r.start()+t*r.direction(),VW)).normalize();
			R=n+n-L-2*((n-L)*n)*n;
			V=vec3(-r.direction(),VW).normalize();
			double d=(lightIter->getPosition()-vec3(r.start()+t*r.direction(),VW)).length();
			double d0=lightIter->getLightInfo().deadDistance;
			cSpec=m.k[MAT_KSM]*pairwiseMult(m.color,lightIter->getLightInfo().color)+(1-m.k[MAT_KSM])*lightIter->getLightInfo().color;
			color += m.k[MAT_KD]*max(0.0,(n*L))*pairwiseMult(m.color,lightIter->getLightInfo().color)/pow((d0+d),lightIter->getLightInfo().falloff);
			color +=m.k[MAT_KS]*pow(max(0.0,R*V),m.k[MAT_KSP])*cSpec/pow((d0+d),lightIter->getLightInfo().falloff);
		}
	}
	for(lightIter=world->getLightsBeginIterator(LIGHT_SPOT);lightIter!=world->getLightsEndIterator(LIGHT_SPOT);lightIter++){
		double t1; vec3 n1; MaterialInfo m1;
		Ray r1(r.getPos(t),vec4(lightIter->getPosition(),1.0),0.001);
		if((!world->intersect(r1,t1,n1,m1))||t1>=1){
			L=(lightIter->getPosition()-vec3(r.start()+t*r.direction(),VW)).normalize();
			R=n+n-L-2*((n-L)*n)*n;
			V=vec3(-r.direction(),VW).normalize();
			double d=(lightIter->getPosition()-vec3(r.start()+t*r.direction(),VW)).length();
			double d0=lightIter->getLightInfo().deadDistance;
			cSpec=m.k[MAT_KSM]*pairwiseMult(m.color,lightIter->getLightInfo().color)+(1-m.k[MAT_KSM])*lightIter->getLightInfo().color;
			color += m.k[MAT_KD]*max(0.0,(n*L))*pairwiseMult(m.color,lightIter->getLightInfo().color)*pow(max(0.0,(0+lightIter->getDirection()).normalize()*(-L)),lightIter->getLightInfo().angularFalloff)/pow((d0+d),lightIter->getLightInfo().falloff);
			color +=m.k[MAT_KS]*pow(max(0.0,R*V),m.k[MAT_KSP])*cSpec*pow(max(0.0,(-L)*(0+lightIter->getDirection()).normalize()),lightIter->getLightInfo().angularFalloff)/pow((d0+d),lightIter->getLightInfo().falloff);
		}
	}
}
vec3 raycast(Ray & ray) {
	vec3 retColor(0,0,0);
	double t; vec3 n; MaterialInfo m;
	if (world->intersect(ray, t, n, m)) {
		retColor += pairwiseMult(m.color, world->getAmbientLight()) * m.k[MAT_KA];
		setColor(ray,t,n,m,retColor);
	}

    return retColor;
}
*/
vec3 raycast(Ray & ray, int depth) {
	vec3 retColor(0,0,0);
	vec3 d = vec3(ray.direction().normalize(),VW);
	vector<Light>::iterator myIterator;
  
	double t; vec3 n; MaterialInfo m;
	if (world->intersect(ray, t, n, m)) {
		
		retColor += pairwiseMult(m.color, world->getAmbientLight()) * m.k[MAT_KA];
		vec4 hitPoint = (ray.start() + (t * ray.direction()));
		for(myIterator = world->getLightsBeginIterator(LIGHT_DIRECTIONAL); myIterator != world->getLightsEndIterator(LIGHT_DIRECTIONAL); myIterator++) {
			double tempDoub; 
			if (!world->intersect(Ray(hitPoint, hitPoint + vec4(-(myIterator->getDirection()), 1.0), 0.000001), tempDoub, vec3(), MaterialInfo())) {
				vec3 L = ((myIterator->getDirection()) * -1).normalize();
				vec3 R = -L + (2 * (L * n) * n); //(n + (n - L) - (2 * n * (((n - L) * n)/(n.length()))));
				vec3 V = (vec3(ray.direction(), 3) * -1).normalize();
				vec3 Cdiff = m.color;
				vec3 Clight = myIterator->getLightInfo().color;
				retColor += (m.k[MAT_KD] * max((n * L), 0.0) * pairwiseMult(Cdiff, Clight));
				retColor +=	(m.k[MAT_KS] * pow(max((R * V), 0.0), m.k[MAT_KSP]) * ((m.k[MAT_KSM] * pairwiseMult(Cdiff, Clight)) + ((1 - m.k[MAT_KSM]) * Clight)));
			}
		}
		
		for(myIterator = world->getLightsBeginIterator(LIGHT_POINT); myIterator != world->getLightsEndIterator(LIGHT_POINT); myIterator++) {
			double tempDoub; 
			if ((!world->intersect(Ray(hitPoint, vec4(myIterator->getPosition(), 1.0), 0.000001), tempDoub, vec3(), MaterialInfo())) || (tempDoub > 1)) {
				vec3 L = (myIterator->getPosition() - (vec3(hitPoint, 3))).normalize();
				vec3 R = -L + (2 * (L * n) * n); //(n + (n - L) - (2 * n * (((n - L) * n)/(n.length()))));
				vec3 V = (vec3(ray.direction(), 3) * -1).normalize();
				vec3 Cdiff = m.color;
				vec3 Clight = myIterator->getLightInfo().color;
				vec3 dist = (myIterator->getPosition() - (vec3(hitPoint, 3)));
				vec3 color;
				color = ((m.k[MAT_KD] * max((n * L), 0.0) * pairwiseMult(Cdiff, Clight)) +
						(m.k[MAT_KS] * pow(max((R * V), 0.0), m.k[MAT_KSP]) * ((m.k[MAT_KSM] * pairwiseMult(Cdiff, Clight)) + ((1 - m.k[MAT_KSM]) * Clight))));
				color = (color/pow((myIterator->getLightInfo().deadDistance + dist.length()), (myIterator->getLightInfo().falloff)));
				//THE ABOVE LINE ACCOUNTS FOR FALLOFF
				retColor += color;
			}
		}
	
		for(myIterator = world->getLightsBeginIterator(LIGHT_SPOT); myIterator != world->getLightsEndIterator(LIGHT_SPOT); myIterator++) {
			double tempDoub; 
			if ((!world->intersect(Ray(hitPoint, vec4(myIterator->getPosition(), 1.0), 0.000001), tempDoub, vec3(), MaterialInfo())) || (tempDoub > 1)) {
				vec3 L = ((myIterator->getPosition() - (vec3(hitPoint, 3)))).normalize();
				vec3 R = -L + (2 * (L * n) * n); //(n + (n - L) - (2 * n * (((n - L) * n)/(n.length()))));
				vec3 V = (vec3(ray.direction(), 3) * -1).normalize();
				vec3 D = myIterator->getDirection();
				vec3 Cdiff = m.color;
				vec3 Clight = myIterator->getLightInfo().color;
				vec3 dist = (myIterator->getPosition() - (vec3(hitPoint, 3)));
				vec3 color;
				color = ((m.k[MAT_KD] * max((n * L), 0.0) * pairwiseMult(Cdiff, Clight)) +
						(m.k[MAT_KS] * pow(max((R * V), 0.0), m.k[MAT_KSP]) * ((m.k[MAT_KSM] * pairwiseMult(Cdiff, Clight)) + ((1 - m.k[MAT_KSM]) * Clight))));
				retColor += (color * pow((D * -L), myIterator->getLightInfo().angularFalloff) /pow((myIterator->getLightInfo().deadDistance + dist.length()), (myIterator->getLightInfo().falloff)));
			}
		}
		
		if (depth <= defaultDepth) {
			vec4 V = (ray.direction() * -1).normalize();
			vec4 norm(n, 0);
			vec4 R = -V + (2 * (V * norm) * norm);
			Ray bounce(hitPoint, hitPoint + R, 0.000001);
			vec3 S = (m.k[MAT_KSM] * m.color) + ((1.0 - m.k[MAT_KSM]) * vec3(1.0, 1.0, 1.0));
			retColor += (1 - m.k[MAT_KT]) * (m.k[MAT_KS] * pairwiseMult(S, raycast(bounce, depth + 1)));
			//(1 - KT)*that such that for kt = 0 it woulf still be the same.
		}
		/*
		if (m.k[MAT_KT] != 0){ //does this indicate dielectric?
			vec4 V = (ray.direction() * -1).normalize();
			vec4 D = ray.direction().normalize();
			vec4 norm(n, 0);
			vec4 Ref = -V + (2 * (V * norm) * norm);
			vec4 T; int C; vec3 colour; int check; check = 0;
			int kr; int kg; int kb;
			if (D * norm < 0) {
				refract(D, norm, 1.0, T, m.k[MAT_KTN]); //WTF IS LITTLE N?
				C = -D * norm;
				kr = 1; kg = 1; kb = 1;
			} else {
				kr = 1; kg = 1; kb = 1; //eh? is a = 0?
				if (refract(D, -norm, 1/1.0, T, m.k[MAT_KTN])) {
					C = T * norm;
				} else {
					vec3 K(kr, kg, kb);
					colour = K * raycast(Ray(hitPoint, hitPoint+Ref, 0.000001), 0); //depth = ???
					check = 1;
				}
			}
			if (check == 0) {
				int R;
				R = pow(1 - C, 5.0);
				vec3 K(kr, kg, kb);
				colour = K * (R*raycast(Ray(hitPoint, hitPoint + R, 0.000001), 0) + 
							 ((1 - R) * raycast(Ray(hitPoint, hitPoint + T, 0.000001), 0))); //two depths here to consider too
			}
			retColor += colour;
		}*/	
		
	}
    return retColor;
}

//-------------------------------------------------------------------------------
/// The display function
void display() {

	//Clear Buffers
    glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);					// indicate we are specifying camera transformations
	glLoadIdentity();							// make sure transformation is "zero'd"

	// Calls the raycast method on each pixel
	// sampled using the Viewport and Ray classes.
	// and stores the result using the Film class
	film->clear();
	Viewport &view = world->getViewport();
    view.resetSampler();
	vec2 point; Ray ray;
    while(view.getSample(point, ray)) {
		ray.transform(view.getViewToWorld());
        vec3 c = raycast(ray,1);
        film->expose(c, point);
    }
	film->show();

	//Now that we've drawn on the buffer, swap the drawing buffer and the displaying buffer.
	glutSwapBuffers();
}


//-------------------------------------------------------------------------------
/// \brief	Called when the screen gets resized.
/// This gives you the opportunity to set up all the relevant transforms.
///
void reshape(int w, int h) {
	//Set up the viewport to ignore any size change.
	Viewport &view = world->getViewport();
	glViewport(0,0,view.getW(),view.getH());

	//Set up the PROJECTION transformationstack to be a simple orthographic [-1, +1] projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, view.getW(), 0, view.getH(), 1, -1);

	//Set the MODELVIEW transformation stack to the identity.
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//-------------------------------------------------------------------------------
/// Called to handle keyboard events.
void myKeyboardFunc(unsigned char key, int x, int y) {
	switch (key) {
        case 's':
            film->saveFrame("./",imgName); // image saving has been moved to the film class
			cout<<imgName<<endl;
            break;
		case 27:			// Escape key
			exit(0);
			break;
	}
}
vec2 inverseViewportTransform(vec2 screenCoords) {
    //Create a vec2 on the local stack. See algebra3.h
    vec2 viewCoords(0.0,0.0);
	Viewport viewport =world->getViewport();
    viewCoords[0] = ((float)screenCoords[0] - viewport.getH()/2)/((float)(viewport.getW()/2));
    viewCoords[1] = ((float)screenCoords[1] - viewport.getH()/2)/((float)(viewport.getW()/2));
    //Flip the values to get the correct position relative to our coordinate axis.
    viewCoords[1] = -viewCoords[1];

    //C++ will copy the whole vec2 to the calling function.
    return viewCoords;
}

void myMouseFunc(int button, int state, int x, int y) {
    vec2 screenCoords((double) x, (double) y);
    vec2 viewCoords = inverseViewportTransform(screenCoords);
	if ( button==GLUT_LEFT_BUTTON && state==GLUT_DOWN ) {
		buttonFlag=1;
		//world->getLightsBeginIterator(LIGHT_SPOT)->setColor(vec3(14,14,14));
		//glutPostRedisplay();
	}
	if( button==GLUT_LEFT_BUTTON && state==GLUT_UP ) {
		buttonFlag=0;
		//world->getLightsBeginIterator(LIGHT_SPOT)->setColor(vec3(0,0,0));
		//glutPostRedisplay();
	}
	if( button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN ) {
		buttonFlag=2;
	}
	if( button==GLUT_RIGHT_BUTTON && state==GLUT_UP ) {
		buttonFlag=0;
	}
}

void myMotionFunc(int x, int y) {
    vec2 mousePos = inverseViewportTransform(vec2((double)x,(double)y));
	double s=(mousePos[0]+1.0)/2;
	double t=(mousePos[1]+1.0)/2;
	if(buttonFlag==2){
		(*(world->getSpheres()))[0].setMetalness(s);
		(*(world->getSpheres()))[0].setSmoothness(t);
		glutPostRedisplay();
	}
}




//-------------------------------------------------------------------------------
/// Initialize the environment
int main(int argc,char** argv) {

	//Initialize OpenGL
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);

	// Load the world
	if(argc>0){
		world = new World(argv[1]);
	}
	else{
		world = new World();
	}
	if(argc>1){
		imgName=argv[2];
	}
	//Allocate film for rendering
	film = new Film(IMAGE_WIDTH, IMAGE_HEIGHT);
	//Create OpenGL Window
	glutInitWindowSize(IMAGE_WIDTH,IMAGE_HEIGHT);
	glutInitWindowPosition(0,0);
	glutCreateWindow("CS184 Raycaster");

	//Register event handlers with OpenGL.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(myKeyboardFunc);
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(myMotionFunc);
	glutPassiveMotionFunc(myMotionFunc);
	
	//And Go!
	glutMainLoop();
}
