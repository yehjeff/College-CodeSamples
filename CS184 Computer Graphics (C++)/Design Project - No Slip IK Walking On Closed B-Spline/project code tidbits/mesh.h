/*
 * mesh.h
 *
 * A simple mesh class
 *
 *  Created on: March 10, 2011
 *      Author: jima
 */

#ifndef MESH_H_
#define MESH_H_

#include "skeleton.h"
#include "shaders.h"

#include <string>
#include <sstream>

using namespace std;

// winged edge class
struct Edge {
	int f[2]; // face index for both sides
	int v[2]; // vertex index (to vert at 'end' of half edge)
	
    double sharpness;
	int temp; // to be used by algorithms as you please; cc subdiv uses it to store indices in the next mesh temporarily

	Edge(int f0, int v0, int v1, double sharpness = 0) : sharpness(sharpness)
    {
        f[0] = f0; f[1] = -1;
        v[0] = v0; v[1] = v1;
    }
};

// mesh face
struct Face {
	// array of edge indices
	vector<int> edges;

    // vertex, texture and normal indices, in order 
    // -- vi is technically redundant but a bit easier than getting it from the edges.
    vector<int> vi, ti, ni;

    Face() { vi.reserve(3); } //CHECKTHISIFBUGGY //what does this do and do i need to change it?

    void addVertex(vector<int> info, int offset=0) {
        info.resize(3, -1-offset);
        vi.push_back(info[0]+offset);
        ti.push_back(info[1]+offset);
        ni.push_back(info[2]+offset);
    }
};

// describes attachment of vertex to a joint
struct BoneWeight {
    int joint; // joint whose frame we attach to
    double weight; // weight with which we attach
    vec3 jointLocalPos; // the position of the vertex in joint-local space

    BoneWeight(int joint, double weight, vec3 jointLocalPos) 
        : joint(joint), weight(weight), jointLocalPos(jointLocalPos) {}
};

// mesh vertex
struct Vert {
	vector<int> edges; // ring of edges eminating from vertex

	vec3 p;
    vector<BoneWeight> weights;
    int temp;

	Vert(vec3 v) : p(v) {}


};

class Skeleton;

class Mesh {
	vector<Vert> verts;
	vector<Face> faces;
    vector<Edge> edges;

	vector<vec2> uvs;
    vector<vec3> normals;

	// just uses a fixed texture for the whole mesh for simplicity
    // (instead of loading materials from an mtl file properly)
    GLuint tex, heighttex, normaltex;

    // skeleton has vertex access to do skinning
    friend class Skeleton; 

public:

	void clear() {
		verts.clear(); faces.clear(); edges.clear();
	}

    // centers the mesh and scales it to the given scale.  Also scales the skeleton similarly.
	void centerAndScale(Skeleton &skel, double scale = 1.0);
	

    // face normal
    vec3 getNormal(int f);

    // an inefficient render routine
	void render(Shader &shade);
    
	// just naively sums all nearby triangle normals; no area weighting
    void computeVertexNormals();

    // adds a face to the mesh, setting up the edge connectivity
	void connectFace(Face &face);

	bool loadFile(string file);
	bool loadTextures(string texture, string bumpmap);

	// helpers for subdivision
	vec3 getCentroid(int f);
	vec3 getCCEdgeStencil(int ei);
	vec3 getCCVertexStencil(int vi);

	// put a CC subdivided version of this mesh into 'output' mesh
	void subdivide(Mesh &output);

    // --- some helpers to access the mesh:
    // get the ii'th vert of the f'th face
	inline Vert &v(int f, int ii) { return verts[faces[f].vi[ii]]; }
    // get the ii'th vert of the f'th face (by index)
	inline int vind(int f, int ii) { return faces[f].vi[ii]; }
    // get the edge connecting vert i to vert j
    inline int edge(int i, int j) {
        for (int ii = 0; ii < (int)verts[i].edges.size(); ++ii) {
            int ei = verts[i].edges[ii];
            if (edges[ei].v[0] == j || edges[ei].v[1] == j)
                return ei;
        }
        return -1;
    }
};



#endif

