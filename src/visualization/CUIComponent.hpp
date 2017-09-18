//
// Created by user on 18.09.17.
//

#ifndef FERIENAKADEMIE_CUICOMPONENT_H
#define FERIENAKADEMIE_CUICOMPONENT_H


class CUIComponent
{
	int* position;

	CObjectGeometry geometry;
};

class CObjectGeometry
{
public:
	void* get_image();
};




#endif //FERIENAKADEMIE_CUICOMPONENT_H
