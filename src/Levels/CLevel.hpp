#ifndef CLEVEL_H_
#define CLEVEL_H_

#include <string>
#include "../CPipelinePacket.hpp"

using namespace std;

namespace Level
{
    class CLevel : public CPipelinePacket
    {
    public:
        enum CellType
        {
            OBSTACLE, BC_BOUNCE_BACK, BC_NO_SLIP, BC_INFLOW, BC_OUTFLOW, EMPTY
        };

    private:
        int width;
        int height;
        string name;

        CellType *array;

    public:
        CLevel(int width, int height, string name) : width(width), height(height), name(name)
        {
            CPipelinePacket::setPacketTypeInfoName(typeid(*this).name());
            array = new CellType[width * height]();
        }

        ~CLevel()
        {
            delete [] array;
        }

        inline CellType getValue(int x, int y)
        {
            return array[x + y*width];
        }

        inline void setValue(int x, int y, CellType type)
        {
            array[x + y*width] = type;
        }

        void *getPayloadRaw()
        {
            return this;
        }
    };
}

#endif
