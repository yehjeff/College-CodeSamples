#include "skeleton.h"
#include <fstream>
#include <string>
#include <sstream>


void Animation::setJoints(vector<Joint> &joints, double frame, int rootIndex) {
    int startFrame=(int)frame;
	double t=frame-startFrame;
	vector<quat> startOrients=orientations[startFrame];
	vector<quat> endOrients=orientations[startFrame+1];
	cout<<startOrients.size()<<endl;
	cout<<joints.size()<<endl;
	cout<<"frame is"<<frame<<endl;
	for(int i=0;i<startOrients.size();i++){
		joints[i].orient=(startOrients[i].getNearest(endOrients[i])).nlerp(endOrients[i],t);
		//joints[i].updateJointPosnAndFrame();
	}
	joints[rootIndex].posn=(1-t)*roots[startFrame]+t*roots[startFrame+1];
}


// note: if you only do one ik implementation, you can ignore the "method" argument here.
void Skeleton::inverseKinematics(int j, vec3 pos, int method) {
    // get chain of joints going back to root
    vector<Joint*> chain = getChain(j);
    // note you can use this function after changing joint orientations in the chain to find the new joint positions & frames:
    //    void updateChainFrames(vector<Joint*> &chain);
    
    double stepSize=-0.1;
	double error=0.01;
	double distance=(chain[0]->posn-pos).length();
	double oldDistance=distance;
	vector<vector<double>> jacobianT(4.0*(chain.size()-1),vector<double>(3.0));
	vector<double> paramDisplacement(4*(chain.size()-1));
	vector<double> dxyz(3);
	int iterations=0;
	while(distance>error&&iterations<500){
		calcJacobian(&jacobianT,chain,pos);
		dxyz[0]=stepSize;
		dxyz[1]=stepSize;
		dxyz[2]=stepSize;
		vecMatMult(&jacobianT,&dxyz,&paramDisplacement);
		applyDisplacement(chain,&paramDisplacement);
		updateChainFrames(chain);
		distance=(chain[0]->posn-pos).length();
		if(distance>oldDistance){
			stepSize=stepSize/2;
		}
		else{
			oldDistance=distance;
		}
		//cout<<"nooo"<<endl;
		iterations++;
	}
}

void Skeleton::calcJacobian(vector<vector<double>> *jt,vector<Joint*> chain,vec3 target){
	double d=0.001;
	double dDX;
	double dDY;
	double dDZ;
	double DX=fabs((chain[0]->posn-target)[VX]);
	double DY=fabs((chain[0]->posn-target)[VY]);
	double DZ=fabs((chain[0]->posn-target)[VZ]);
	for(int i=0;i<chain.size()-1;i++){
		quat originalQuat=chain[i]->orient;
		for(int j=0;j<4;j++){
			chain[i]->orient[j]=chain[i]->orient[j]+d;
			chain[i]->orient.normalize();
			updateChainFrames(chain);
			dDX=fabs((chain[0]->posn-target)[VX])-DX;
			dDY=fabs((chain[0]->posn-target)[VY])-DY;
			dDZ=fabs((chain[0]->posn-target)[VZ])-DZ;
			(*jt)[i*4+j][0]=dDX/d;
			(*jt)[i*4+j][1]=dDY/d;
			(*jt)[i*4+j][2]=dDZ/d;
			chain[i]->orient=originalQuat;
		}
	}
}
void Skeleton::vecMatMult(vector<vector<double>> *jacobian, vector<double> *dxyz, vector<double> *paramDisp) {
    for(int i = 0; i < paramDisp->size(); i++){
		(*paramDisp)[i] = ((*dxyz)[0])*((*jacobian)[i][0]) + ((*dxyz)[1])*((*jacobian)[i][1]) + ((*dxyz)[2])*((*jacobian)[i][2]);
    }
}

void Skeleton::applyDisplacement(vector<Joint*> chain, vector<double> *paramDisplacement){
    for(int i = 0; i<chain.size()-1; i++) {
		chain[i]->orient[0.0] += (*paramDisplacement)[(4.0*i)+0.0];
		chain[i]->orient[1.0] += (*paramDisplacement)[(4.0*i)+1.0];
		chain[i]->orient[2.0] += (*paramDisplacement)[(4.0*i)+2.0];
		chain[i]->orient[3.0] += (*paramDisplacement)[(4.0*i)+3.0];
		chain[i]->orient.normalize();
    }
}
void Skeleton::inverseKinematicsLocked(int j, vec3 pos, int method) {
    // get chain of joints going back to root
    vector<Joint*> chain = getChain(j);
    // note you can use this function after changing joint orientations in the chain to find the new joint positions & frames:
    //    void updateChainFrames(vector<Joint*> &chain);
    
    double stepSize=-0.1;
	double error=0.01;
	double distance=(chain[0]->posn-pos).length();
	double oldDistance=distance;
	vector<vector<double>> jacobianT(4.0*(chain.size()-1),vector<double>(3.0));
	vector<double> paramDisplacement(4*(chain.size()-1));
	vector<double> dxyz(3);
	int iterations=0;
	while(distance>error&&iterations<500){
		calcJacobianLocked(&jacobianT,chain,pos);
		dxyz[0]=stepSize;
		dxyz[1]=stepSize;
		dxyz[2]=stepSize;
		vecMatMult(&jacobianT,&dxyz,&paramDisplacement);
		applyDisplacementLocked(chain,&paramDisplacement);
		updateChainFrames(chain);
		distance=(chain[0]->posn-pos).length();
		if(distance>oldDistance){
			stepSize=stepSize/2;
		}
		else{
			oldDistance=distance;
		}
		//cout<<"nooo"<<endl;
		iterations++;
	}
}
void Skeleton::calcJacobianLocked(vector<vector<double>> *jt,vector<Joint*> chain,vec3 target){
	double d=0.001;
	double dDX;
	double dDY;
	double dDZ;
	double DX=fabs((chain[0]->posn-target)[VX]);
	double DY=fabs((chain[0]->posn-target)[VY]);
	double DZ=fabs((chain[0]->posn-target)[VZ]);
	for(int i=0;i<chain.size()-1;i++){
		quat originalQuat=chain[i]->orient;
		if(chain[i]->locked){
			(*jt)[i*4][0]=0;
			(*jt)[i*4][1]=0;
			(*jt)[i*4][2]=0;
			(*jt)[i*4+1][0]=0;
			(*jt)[i*4+1][1]=0;
			(*jt)[i*4+1][2]=0;
			(*jt)[i*4+2][0]=0;
			(*jt)[i*4+2][1]=0;
			(*jt)[i*4+2][2]=0;
			(*jt)[i*4+3][0]=0;
			(*jt)[i*4+3][1]=0;
			(*jt)[i*4+3][2]=0;
		}
		else{
			for(int j=0;j<4;j++){
				chain[i]->orient[j]=chain[i]->orient[j]+d;
				chain[i]->orient.normalize();
				updateChainFrames(chain);
				dDX=fabs((chain[0]->posn-target)[VX])-DX;
				dDY=fabs((chain[0]->posn-target)[VY])-DY;
				dDZ=fabs((chain[0]->posn-target)[VZ])-DZ;
				(*jt)[i*4+j][0]=dDX/d;
				(*jt)[i*4+j][1]=dDY/d;
				(*jt)[i*4+j][2]=dDZ/d;
				chain[i]->orient=originalQuat;
			}
		}
	}
}
void Skeleton::applyDisplacementLocked(vector<Joint*> chain, vector<double> *paramDisplacement){
    for(int i = 0; i<chain.size()-1; i++) {
		if(!chain[i]->locked){			
			chain[i]->orient[0] += (*paramDisplacement)[(4*i)+0];
			chain[i]->orient[1] += (*paramDisplacement)[(4*i)+1];
			chain[i]->orient[2] += (*paramDisplacement)[(4*i)+2];
			chain[i]->orient[3] += (*paramDisplacement)[(4*i)+3];
			chain[i]->orient.normalize();
		}
    }
}
void Skeleton::moveRoot(vec3 newRootPos){
	joints[root].posn=newRootPos;
}
void Skeleton::updateSkin(Mesh &mesh) {
    updateJointPosnAndFrame(root);
    vector<Vert> &meshVerts = mesh.verts;
    for (int i=0;i<meshVerts.size();i++){
		vec3 soFar(0.0,0.0,0.0);
		for(int j=0;j<meshVerts[i].weights.size();j++){
			double weight=meshVerts[i].weights[j].weight;
			Joint currentJoint=joints[meshVerts[i].weights[j].joint];
			vec3 relativePos=meshVerts[i].weights[j].jointLocalPos;
			soFar=soFar+weight*currentJoint.localToWorld(relativePos);
		}
		meshVerts[i].p=soFar;
    }
}

/*void Skeleton::updateSkin(Mesh &mesh) {
    updateJointPosnAndFrame(root);
    
    vector<Vert> &meshVerts = mesh.verts;
    for (vector<Vert>::iterator it = meshVerts.begin(); it != meshVerts.end(); ++it) {
            //IMPLEMENT_ME(__FILE__,__LINE__);
    //each vert has a list of boneweights
    //each bone weight specifies a joint and where the vert(of the mesh) is in relation to that joint
    //transform this position from jointspace -> worldspace
    //take weighted average of world positions of verts and set that result to the vert's p.
    //[(weight*position) + (weight2*position2) + ...] / (sum total weights)
		int totalweight = 0;
		vec3 soFar(0.0,0.0,0.0);
		vector<BoneWeight> &bWeights = it->weights;
		for (vector<BoneWeight>::iterator bones = bWeights.begin(); bones != bWeights.end(); ++bones) {
			totalweight += bones->weight;
			//sofar += weight* joint's localtoworld transform applied to vertex's pos based on local joint pos
			soFar += (bones->weight) * joints[bones->joint].localToWorld(bones->jointLocalPos);
		}
		it->p = soFar / totalweight;
        }
    }*/





void Skeleton::saveRestPose() {
    restPose.resize(joints.size());
    for (size_t i = 0; i < joints.size(); i++) {
        restPose[i] = joints[i].orient;
    }
}
void Skeleton::resetPose() {
    for (size_t i = 0; i < joints.size(); i++) {
        joints[i].orient = restPose[i];
    }
    updateJoints();
}

// Load a skeleton file
void Skeleton::loadPinocchioFile(const char *skelfile) {
    clear();
    ifstream f(skelfile);

    string line;
	while (getline(f,line)) {
		if (line.empty())
			continue;
		stringstream ss(line);
		int id, parent;
        vec3 posn;
        if (ss >> id >> posn[0] >> posn[1] >> posn[2] >> parent) {
            assert(id == int(joints.size()));
            joints.push_back(Joint(parent, posn));
            if (parent == -1)
                root = id;
        }
    }

    // init lengths and children array
    for (size_t i = 0; i < joints.size(); i++) {
        int parent = joints[i].parent;
        if (parent > -1) {
            joints[i].length = (joints[i].posn - joints[parent].posn).length();
            joints[parent].children.push_back(int(i));
        }
    }

    updateOrientationAndFrameFromPosn(root);

    saveRestPose();
}
// Load a bone weight file (attaching the skeleton to a mesh)
void Skeleton::initBoneWeights(const char *skinfile, Mesh &mesh, double threshWeight) {
    ifstream f(skinfile);

    vector<Vert> &meshVerts = mesh.verts;
    updateJointPosnAndFrame(root);

    // expect one line per vertex
    string line;
    size_t vertind = 0;
	while (getline(f,line)) {
		if (line.empty())
			continue;
		stringstream ss(line);
        
        vector<BoneWeight> &weights = meshVerts[vertind].weights; 

        int id = 0;
        double weight;
        double totalWeight = 0;
        while (ss >> weight) {
            if (id == root) id++;
            if (weight > threshWeight) {
                totalWeight += weight;
                weights.push_back(BoneWeight(id, weight, joints[id].worldToLocal(meshVerts[vertind].p)));
            }
            id++;
        }
        
        assert(totalWeight > 0); // double check you didn't throw away too much ...

        // renormalize weights so they sum to 1 after discarding all small weights.
        for (vector<BoneWeight>::iterator it = weights.begin(); it != weights.end(); ++it) {
            it->weight /= totalWeight;
        }

        vertind++;
    }
}

// offset and scale joint positions; useful when adjusting mesh scale/offset
void Skeleton::offsetAndScale(vec3 offset, double scale) {
    for (vector<Joint>::iterator it = joints.begin(); it != joints.end(); ++it) {
        it->posn += offset;
        it->posn *= scale;
        it->length *= scale;
    }
}

// update joint positions and frames using joint orientations
void Skeleton::updateJointPosnAndFrame(int joint, quat acc) {
    Joint &j = joints[joint];
    acc = acc * j.orient;
    j.l2w = acc;

    if (j.parent != -1) {
        Joint &p = joints[j.parent];
        vec3 bone(0,j.length,0);
        bone = acc.rotate(bone);
        vec3 oldposn = j.posn;
        j.posn = p.posn + bone;
    }
    
    for (vector<int>::iterator it = j.children.begin(); it != j.children.end(); ++it) {
        int ch = *it;
        updateJointPosnAndFrame(ch, acc);
    }
}
// update joint orientations and frames using joint positions
void Skeleton::updateOrientationAndFrameFromPosn(int joint, quat acc) {
    Joint &j = joints[joint];

    if (j.parent != -1) {
        Joint &p = joints[j.parent]; // parent has orientation and frame set
        vec3 boneDir = p.worldToLocal(j.posn);
        j.orient = quat::getRotation(vec3(0,1,0), boneDir);
        acc = acc * j.orient;
    }
    j.l2w = acc;

    for (vector<int>::iterator it = j.children.begin(); it != j.children.end(); ++it) {
        int ch = *it;
        updateOrientationAndFrameFromPosn(ch, acc);
    }
}

void Skeleton::render(int highlightChainToJoint) {
    updateJointPosnAndFrame(root);

    glDisable(GL_LIGHTING); // just draw plain colored lines
    glDisable(GL_DEPTH_TEST); // make it show through the mesh

    if (highlightChainToJoint) {
        glColor3d(1,0,0);
        glLineWidth(10);
        glBegin(GL_LINE_STRIP);
        vector<Joint*> chain = getChain(highlightChainToJoint);
        for (size_t i = 0; i < chain.size(); i++) {
            glVertex3dv(&chain[i]->posn[0]);
        }
        glEnd();
    }
    
    glColor3d(1,1,0);
    glLineWidth(3);
    glBegin(GL_LINES);
    for (size_t i = 0; i < joints.size(); i++) {
        vec3 posn = joints[i].posn;
        int parent = joints[i].parent;
        if (parent > -1) {
            vec3 pposn = joints[parent].posn;
            glVertex3dv(&posn[0]);
            glVertex3dv(&pposn[0]);
        }
    }
    glEnd();

    glPointSize(10);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < joints.size(); i++) {
        glVertex3dv(&joints[i].posn[0]);
    }
    glEnd();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

// choose a joint within the selection radius of the mouse
int Skeleton::pickJoint(double &depth, vec2 mouse, double selectionRadius) {
    double modelview[16], projection[16];
    int viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    mouse[1] = viewport[3] - mouse[1];

    int bestJoint = -1;
    double bestDist = selectionRadius*selectionRadius;
    for (size_t i = 0; i < joints.size(); ++i) {
        vec3 p = joints[i].posn;
        vec2 s;
        double sz;
        gluProject(p[0], p[1], p[2], 
            modelview, projection, viewport, 
            &s[0], &s[1], &sz);
        vec2 diff = mouse - s;
        
        if (diff.length2() <= bestDist) {
            bestDist = diff.length2();
            bestJoint = int(i);
            depth = sz;
        }
    }

    return bestJoint;
}
// project a screen pos into the world at the given depth 
vec3 Skeleton::getPos(vec2 mouse, double depth) {
    double modelview[16], projection[16];
    int viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    mouse[1] = viewport[3] - mouse[1];

    double x,y,z;
    gluUnProject(mouse[0], mouse[1], depth, 
                 modelview, projection, viewport,
                 &x, &y, &z);
    return vec3(x,y,z);
}

// get chain of joints from j to root, useful for ik
vector<Joint*> Skeleton::getChain(int j) {
    vector<Joint*> chain;
    int nextj = j;
    while (nextj != -1) {
        chain.push_back(&joints[nextj]);
        nextj = joints[nextj].parent;
    }
    return chain;
}
// update just the positions and frames in the given chain
void Skeleton::updateChainFrames(vector<Joint*> &chain) {
    quat acc;
    // root is at end of chain; go from root to front
    for (int i = int(chain.size())-1; i >= 0; i--) {
        Joint &j = *chain[i];
        acc = acc * j.orient;
        j.l2w = acc;
        if (j.parent != -1) {
            Joint &p = joints[j.parent];
            vec3 bone(0,j.length,0);
            bone = acc.rotate(bone);
            j.posn = p.posn + bone;
        }
    }
}
