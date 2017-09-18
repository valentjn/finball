//
// Created by user on 18.09.17.
//

#ifndef FERIENAKADEMIE_CWORLDOBJECT_H
#define FERIENAKADEMIE_CWORLDOBJECT_H


class CWorldObject
{
	void* image;

	int* position;
	int* direction;
	int velocity;
	CObjectGeometry geometry;
};

class CObjectGeometry
{
public:
	void* get_image();
};

#endif //FERIENAKADEMIE_CWORLDOBJECT_H
