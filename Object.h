#pragma once
class Object
{
public:
	float posX;
	float posY;
	float posZ;
	float a;
	float escala;
	int indexElement;
	bool isRotating;

	Object(int index);
	~Object();

private:

};

Object::Object(int index)
{
	indexElement = index;
	posX = 0.0f;
	posY = 0.0f;
	posZ = 0.0f;
	escala = 1.0f;
	a = 0.0f;
	isRotating = false;
}

Object::~Object()
{
}