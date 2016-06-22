#pragma once

#include <stdlib.h>
#include <vector>
#include "Face.h"
#include "Material.h"

using namespace std;

class Group
{
public:
	vector<Face*> faces;
	string name;
	bool visible;
	string material;

	int indexVAO;
	int indexVBO;
	int indexTextureVBO;
	int indexMaterial;

	vector<float> glfw_points;
	vector<float> glfw_normalPoints;
	vector<float> glfw_texturePoints;

	Group()
	{
		visible = true;
	}

	void addGLFW_Point(float px, float py, float pz) {
		glfw_points.push_back(px);
		glfw_points.push_back(py);
		glfw_points.push_back(pz);
	}

	void addGLFW_NormalPoint(float px, float py, float pz) {
		glfw_normalPoints.push_back(px);
		glfw_normalPoints.push_back(py);
		glfw_normalPoints.push_back(pz);
	}

	void addGLFW_TexturePoint(float pu, float pv) {
		glfw_texturePoints.push_back(pu);
		glfw_texturePoints.push_back(pv);
	}

	void AddFace(Face* f) {
		faces.push_back(f);
	}

	void changeVisibility() {
		visible = !visible;
	}

	~Group()
	{
		for (int i = 0; i < faces.size(); i++)
		{
			delete faces[i];
		}
	}

};