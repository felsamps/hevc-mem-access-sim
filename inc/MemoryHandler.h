#ifndef MEMORYHANDLER_H
#define	MEMORYHANDLER_H

#include <sstream>
#include <set>
#include <fstream>
#include <vector>

#include "TraceHandler.h"


using namespace std;

class MemoryHandler {
private:
    UInt wFrame, hFrame, numOfTiles, wInLCU, hInLCU;    
    LLInt*** refFramePerspective;
    UInt*** accessed;
    LLInt** currFramePerspective;
    LLInt** overlapping;
    vector<string> outputFilesName;
    set<pair<UInt,UInt> > refAcc;

    void xAccessMemory(LCUData* lcu, UInt idTile, UInt xLCU, UInt yLCU);
    void xAccessBlock(UInt x, UInt y, UInt sizeCU);
    void xUpdateMemory(UInt idTile, UInt xLCU, UInt yLCU);
public:
    MemoryHandler(UInt numOfTiles, UInt wFrame, UInt hFrame);
    
    void generateAccessMap(TraceHandler* th);
    void reportAccessMap();
    

};

#endif	/* MEMORYHANDLER_H */

