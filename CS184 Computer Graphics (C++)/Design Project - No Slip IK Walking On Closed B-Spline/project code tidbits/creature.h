#include "skeleton.h"
#include "mesh.h"
#include "sweep.h"
#include "main.h"
class Creature
{
public:
	void stepForward(){
		/*
		anim->clear();
		anim->addAsFrame(skel->getJointArray(),skel->joints[skel->getRoot()].posn);
		move center to (u+stepSize/2,v+stepSize/2);
		IK pair1 legs to their old positions;
		IK pair2 legs in midair;
		save frame;
		move center to(u+stepSize,v+stepSize);
		IK pair1 legs to their old positions;
		IK pair2 legs to new positions;
		save frame;
		update all leg positions;
		move center to (u+stepSize*3/2,v+stepSize*3/2);
		IK pair2 legs to their old positions;
		IK pair1 legs in midair;
		save frame;
		move center to (u+stepSize*2,v+stepSize*2);
		IK pair2 legs to their old positions;
		IK pair1 legs to new positions;
		save frame;
		update all leg positions;
		u=u+stepSize*2;
		v=v+stepSize*2;
		*/
	}
	void initialize(Skeleton *sk, Sweep *sw, Mesh *m){
		skel=sk;
		mesh=m;
		sweep=sw;
		sweep->generateSurfaceCoords(surface);
		anim=new Animation();
		u=0;
		v=50;
		w=1.6;

		magnitudeU=0.48;
		magnitudeV=0.47;
		angle=0;
		stepSize=1;
				
		neck=3;
		FRshoulder=13;
		FLshoulder=16;
		back=1;
		BRhip=5;
		BLhip=9;
		FRleg=15;
		FLleg=18;
		BRleg=8;
		BLleg=12;

		skel->lockJoint(1);
		skel->lockJoint(2);
		skel->lockJoint(16);
		skel->lockJoint(13);
		skel->lockJoint(9);
		skel->lockJoint(5);
		skel->lockJoint(10);
		skel->lockJoint(6);
		cout<<surface.size()<<endl;
		cout<<surface[0].size()<<endl;
		double avgULength=0.0;
		double avgVLength=0.0;
		for(int i=0;i<surface.size()-1;i++){
			for(int j=0;j<surface[0].size()-1;j++){
				avgULength+=(surface[i][j]-surface[i+1][j]).length();
				avgVLength+=(surface[i][j]-surface[i][j+1]).length();
			}
		}
		cout<<"uMag is "<<avgULength/((surface.size()-1)*(surface[0].size()-1))<<endl;
		cout<<"vMag is "<<avgVLength/((surface.size()-1)*(surface[0].size()-1))<<endl;
		skel->moveRoot(aboveSurfacePoint(u,v,w));
		skel->updateSkin(*mesh);
		/*skel->inverseKinematicsLocked(FRleg,surfacePoint(u+1.5,v+4),0);
		skel->inverseKinematicsLocked(FLleg,surfacePoint(u+1.5,v-4),0);
		skel->inverseKinematics(2,aboveSurfacePoint(u-5,v,w),0);
		skel->unlockJoint(5);
		skel->unlockJoint(9);
		skel->inverseKinematicsLocked(BRhip,aboveSurfacePoint(u-1.8,v+1.3,w),0);
		skel->inverseKinematicsLocked(BLhip,aboveSurfacePoint(u-1.8,v-1.3,w),0);*/
		cout<<"orig    "<<skel->joints[FRleg].posn<<endl;
		skel->inverseKinematics(FRleg,surfacePoint(u+1*0.641309/magnitudeU,v+1*1.3834662/magnitudeV),0);
		cout<<"desired "<<surfacePoint(u+1*0.641309/magnitudeU,v+1*1.3834662/magnitudeV)<<endl;
		//skel->inverseKinematics(BRleg,surfacePoint(u+2,v+2),0);
		//skel->inverseKinematics(BLleg,surfacePoint(u+2,v-2),0);
		//skel->inverseKinematics(BRleg,surfacePoint(u+2,v+2),0);
		//skel->inverseKinematics(BLleg,surfacePoint(u+2,v-2),0);
		//skel->inverseKinematics(FRshoulder,aboveSurfacePoint(u-0.017169,v+0.394,w-0.113914),0);
		//skel->inverseKinematics(FLshoulder,aboveSurfacePoint(u-0.135491,v-0.383375,w-0.113914),0);
		skel->updateSkin(*mesh);
		cout<<"final   "<<skel->joints[FRleg].posn<<endl;
		legFRpos=vec3(0,0,0);//placeholder
		legFLpos=vec3(0,0,0);//placeholder
		legBRpos=vec3(0,0,0);//placeholder
		legBLpos=vec3(0,0,0);//placeholder
	}

	vec3 getSurfaceNormal(double u, double v){
		return ((surfacePoint(u+1,v)-surfacePoint(u,v))^(surfacePoint(u,v-1)-surfacePoint(u,v))).normalize();
	}

	vec3 surfacePoint(double u, double v){
		int ui=(int)u;
		int vi=(int)v;
		double udiff=u-ui;
		double vdiff=v-vi;
		ui=ui%surface.size();
		vi=vi%surface[0].size();
		vec3 immed1=(1-vdiff)*surface[ui][vi]+vdiff*surface[ui][(vi+1)%surface[0].size()];
		vec3 immed2=(1-vdiff)*surface[(ui+1)%surface.size()][vi]+vdiff*surface[(ui+1)%surface.size()][(vi+1)%surface[0].size()];
		return (1-udiff)*immed1+udiff*immed2;
	}
	
	vec3 aboveSurfacePoint(double u, double v,double height){
		return surfacePoint(u,v)+height*getSurfaceNormal(u,v);
	}

	vector<vector<vec3>> surface;
	Animation *anim;
private:
	Skeleton *skel;
	Mesh *mesh;
	Sweep *sweep;	
	double u;
	double v;
	double w;
	double magnitudeU;
	double magnitudeV;
	double angle;
	double stepSize;

	int neck;
	int FRshoulder;
	int FLshoulder;
	int back;
	int BRhip;
	int BLhip;
	int FRleg;
	int FLleg;
	int BRleg;
	int BLleg;

	vec3 legFRpos;
	vec3 legFLpos;
	vec3 legBRpos;
	vec3 legBLpos;
};
	