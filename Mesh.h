#pragma once

#include <vector>
#include "Group.h"
#include "Vertex.h"
#include "TextureMapping.h"

using namespace std;

class Mesh
{
public:
	vector<Vertex*> allVertex;
	vector<Vertex*> allNormals;
	vector<TextureMapping*> allTextures;
	vector<Group*> groups;
	float posX;
	float posY;
	float posZ;
	int indexObject;

	Mesh()
	{
		posX = 0.0f;
		posY = 0.0f;
		posZ = 0.0f;
	}

	~Mesh()
	{
	}

	void addGroup(Group* g) {
		groups.push_back(g);
	}

	void addVertex(Vertex* v) {
		allVertex.push_back(v);
	}

	void addNormal(Vertex* v) {
		allNormals.push_back(v);
	}

	void addTexture(TextureMapping* m) {
		allTextures.push_back(m);
	}

	void setTexturesIDs(vector<Material*> m) {
		for (int i = 0; i < groups.size(); i++)
		{
			for (int j = 0; j < m.size(); j++)
			{
				if (m[j]->name == groups[i]->material) {
					groups[i]->indexMaterial = m[j]->textureID;
					break;
				}
			}
		}
	}

	void generatePoints() {
		for (int i = 0; i < groups.size(); i++)
		{
			for (int j = 0; j < groups[i]->faces.size(); j++)
			{
				if (groups[i]->faces[j]->refPoints.size() == 4) {
					int numAuxVertex = 0;
					for (int k = 0; k < groups[i]->faces[j]->refPoints.size() - 1; k++)
					{
						Vertex* vx = allVertex[groups[i]->faces[j]->refPoints[k]];
						if (groups[i]->faces[j]->normalPoints.size() > 0) {
							Vertex* vn = allNormals[groups[i]->faces[j]->normalPoints[k]];
							groups[i]->addGLFW_NormalPoint(vn->coords[0], vn->coords[1], vn->coords[2]);
						}

						if (groups[i]->faces[j]->textPoints.size() > 0) {
							TextureMapping* vt = allTextures[groups[i]->faces[j]->textPoints[k]];
							groups[i]->addGLFW_TexturePoint(vt->u, vt->v);
						}

						groups[i]->addGLFW_Point(vx->coords[0] ,vx->coords[1], vx->coords[2]);

						numAuxVertex++;
					}

					while (numAuxVertex < 9) {
						int auxIndex = numAuxVertex % groups[i]->faces[j]->refPoints.size();

						Vertex* vx = allVertex[groups[i]->faces[j]->refPoints[auxIndex]];
						if (groups[i]->faces[j]->normalPoints.size() > 0) {
							Vertex* vn = allNormals[groups[i]->faces[j]->normalPoints[auxIndex]];
							groups[i]->addGLFW_NormalPoint(vn->coords[0], vn->coords[1], vn->coords[2]);
						}

						if (groups[i]->faces[j]->textPoints.size() > 0) {
							TextureMapping* vt = allTextures[groups[i]->faces[j]->textPoints[auxIndex]];
							groups[i]->addGLFW_TexturePoint(vt->u, vt->v);
						}

						groups[i]->addGLFW_Point(vx->coords[0], vx->coords[1], vx->coords[2]);

						numAuxVertex++;
					}
				}
				else {
					for (int k = 0; k < groups[i]->faces[j]->refPoints.size(); k++)
					{
						Vertex* vx = allVertex[groups[i]->faces[j]->refPoints[k]];
						if (groups[i]->faces[j]->normalPoints.size() > 0) {
							Vertex* vn = allNormals[groups[i]->faces[j]->normalPoints[k]];
							groups[i]->addGLFW_NormalPoint(vn->coords[0], vn->coords[1], vn->coords[2]);
						}

						if (groups[i]->faces[j]->textPoints.size() > 0) {
							TextureMapping* vt = allTextures[groups[i]->faces[j]->textPoints[k]];
							groups[i]->addGLFW_TexturePoint(vt->u, vt->v);
						}

						groups[i]->addGLFW_Point(vx->coords[0], vx->coords[1], vx->coords[2]);
						
					}
				}
			}
		}
	}
};
