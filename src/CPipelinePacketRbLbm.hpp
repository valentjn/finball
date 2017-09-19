#include<tuple>
#include "CPipelinePacket.hpp"
#include "Array2D.hpp"

class PipelinePacketRbLbm : public CPipelinePacket {
    enum type {
        INFLOW,
        OUTFLOW,
        WALL,
        BODY,
    };
    Array2D<std::tuple<type, float, float>> dataArray;
};
