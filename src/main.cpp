#include <iostream>
#include <stdlib.h>
#include <string>

#include "../inc/TypeDefs.h"
#include "../inc/TraceHandler.h"
#include "../inc/MemoryHandler.h"

using namespace std;

int main(int argc, char** argv) {
	
	UInt wFrame = atoi(argv[1]);
	UInt hFrame = atoi(argv[2]);
	UInt numOfTiles = atoi(argv[3]);
	UInt idFrame = atoi(argv[4]);
	UInt idRefFrame = atoi(argv[5]);
		
	string traceFileName(argv[6]);
	
	TraceHandler* th = new TraceHandler(numOfTiles, wFrame, hFrame, traceFileName);
	th->parse(idFrame, idRefFrame);
	
	MemoryHandler* mh = new MemoryHandler(numOfTiles, wFrame, hFrame);
	mh->generateAccessMap(th);

	mh->reportAccessMap();
	
	return 0;
}

