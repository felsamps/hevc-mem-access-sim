#ifndef TRACEHANDLER_H
#define	TRACEHANDLER_H

#include <fstream>

#include "LCUData.h"
#include "TypeDefs.h"
#include "Entry.h"


using namespace std;

class TraceHandler {
private:
    
    LCUData**** videoData; 
    string traceFileName;
    
public:
    TraceHandler(UInt numOfTiles, UInt wFrame, UInt hFrame, string traceFileName);
    
    void parse(UInt idFrame, UInt idRefFrame);
    
    LCUData* getLCUData(UInt idTile, UInt xLCU, UInt yLCU);

};

#endif	/* TRACEHANDLER_H */

