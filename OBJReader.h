#include <vector>
#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "MTLReader.h"
#include "Material.h"
#include "Object.h"

using namespace std;

static string const VERTEX = "v";
static string const TEXTURE_MAP = "vt";
static string const FACE = "f";
static string const NORMAL = "vn";
static string const GROUP = "g";
static string const USEMTL = "usemtl";
static string const MATERIAL = "mtllib";
static int const SCALE = 1;

class OBJReader
{
public:
	OBJReader() {}
	~OBJReader() {}

	void readObj(char* path, Mesh* m, vector<Material*> &mat) {
		Group* g = new Group();

		ifstream OBJ_FILE(path);
		string MTL_FILE_NAME = "";

		if (!OBJ_FILE) {
			cerr << "Arquivo OBJ n�o encontrado." << endl;
			return;
		}

		string line;
		char word[60];

		for (int lineNumber = 0; getline(OBJ_FILE, line); lineNumber++) {

			if (line.empty() || line[0] == '#') continue;

			stringstream ss(line);

			for (int wordNumber = 1; ss >> word; wordNumber++) {

				if (word == VERTEX) {

					Vertex* v = new Vertex();
					int i = 0;
					while (ss >> word) {
						float n = atof(word);
						v->coords[i++] = n;
					}

					m->addVertex(v);
				}
				else if (word == NORMAL) {
					Vertex* v = new Vertex();
					int i = 0;
					while (ss >> word) {
						float n = atof(word);
						v->coords[i++] = n;
					}

					m->addNormal(v);
				}
				else if (word == TEXTURE_MAP) {
					TextureMapping* tm = new TextureMapping();
					
					ss >> word;
					float u = atof(word);
					ss >> word;
					float v = atof(word);

					tm->u = u;
					tm->v = v;

					m->addTexture(tm);
				}
				else if (word == FACE) {
					Face* f = new Face();
					while (ss >> word) {
						vector<string> a = split(word, '/');
						for (int i = 0; i < a.size(); i++) {

							if (a[i].empty()) continue;

							if (i == 0) {
								f->addRefPoint(atoi(a[i].c_str()) - 1);
							}
							else if (i == 1) {
								f->addTextPoint(atoi(a[i].c_str()) - 1);
							}
							else if (i == 2) {
								f->addNormalPoint(atoi(a[i].c_str()) - 1);
							}
						}
					}
					int s = m->groups.size();
					m->groups[s - 1]->AddFace(f);
				}
				else if (word == GROUP) {
					ss >> word;
					g = new Group();
					g->name = word;
					m->addGroup(g);
				}
				else if (word == USEMTL) {
					ss >> word;
					m->groups[m->groups.size() - 1]->material = word;
				}
				else if (word == MATERIAL) {

					ss >> word;

					MTL_FILE_NAME = word;
					//MTL_FILE_NAME = word;

				}
				else
				{
					continue;
				}
			}
		}

		cout << "Leitura OBJ finalizado.";

		MTLReader* mtlReader = new MTLReader();
		mtlReader->read(MTL_FILE_NAME, mat);
	}

	void readElements(char* path, vector<Object*> &elements) 
	{
		ifstream OBJ_FILE(path);

		string line;
		char word[60];

		for (int lineNumber = 0; getline(OBJ_FILE, line); lineNumber++)
		{
			stringstream ss(line);

			Object* m = new Object(-1);
			for (int i = 0; ss >> word; i++) 
			{
				switch (i)
				{
				case 0:
					m->indexElement = atoi(word);
					break;
				case 1:
					m->posX = atof(word);
					break;
				case 2:
					m->posY = atof(word);
					break;
				case 3:
					m->posZ = atof(word);
					break;
				case 4:
					m->escala = atof(word);
					break;
				case 5:
					m->a = atof(word);
					break;
				}
			}
			elements.push_back(m);
		}
	}

	void saveElements(char* path, vector<Object*> &elements) 
	{
		ofstream myfile(path, ios_base::in | ios_base::out | ios_base::trunc);
		if (myfile.is_open())
		{
			for (int i = 0; i < elements.size(); i++)
			{
				myfile << elements[i]->indexElement;
				myfile << " ";
				myfile << elements[i]->posX;
				myfile << " ";
				myfile << elements[i]->posY;
				myfile << " ";
				myfile << elements[i]->posZ;
				myfile << " ";
				myfile << elements[i]->escala;
				myfile << " ";
				myfile << elements[i]->a;
				myfile << "\n";
			}
			myfile.close();
		}
		else cout << "Unable to open file";
	}

	vector<string> &split(const string &s, char delim, vector<string> &elems) {
		stringstream ss(s);
		string item;
		while (getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}


	vector<string> split(const string &s, char delim) {
		vector<string> elems;
		split(s, delim, elems);
		return elems;
	}
};