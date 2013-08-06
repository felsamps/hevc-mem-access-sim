#include <set>

#include "../inc/TraceHandler.h"

TraceHandler::TraceHandler(UInt numOfTiles, UInt wFrame, UInt hFrame, string traceFileName) {
	UInt wInLCU = (wFrame % TARGET_CU_SIZE == 0) ? wFrame / TARGET_CU_SIZE : wFrame / TARGET_CU_SIZE + 1;
	UInt hInLCU = (hFrame % TARGET_CU_SIZE == 0) ? hFrame / TARGET_CU_SIZE : hFrame / TARGET_CU_SIZE + 1;
	
	this->traceFileName = traceFileName;
	
	this->videoData = new LCUData***[numOfTiles];
	for (int t = 0; t < numOfTiles; t++) {
		this->videoData[t] = new LCUData**[wInLCU];
		for (int x = 0; x < wInLCU; x++) {
			this->videoData[t][x] = new LCUData*[hInLCU];
		}
	}
			
}

void TraceHandler::parse(UInt idFrame, UInt idRefFrame) {
	//TODO implement it
	/*open trace file*/
	fstream fp;
	fp.open(this->traceFileName.c_str(), fstream::in);
	
	char opcode;
	UInt xLCU, yLCU, currIdTile, currIdFrame;
	UInt xCU, yCU, idDepth;
	UInt idPart, sizePart, currIdRefFrame;
	UInt xFS, yFS;
	UInt xCand, yCand;
	UInt xLeft, xRight, yTop, yBottom;
	
	LCUData* newLCU;
	Entry* newEntry;
	set<UInt> refs;
	
	
	/*look for LCU memory accesses regarding the idFrame and ifRefFrame*/
	while(!fp.eof()) {		
		
		fp >> opcode;
		
		switch(opcode) {
			case 'L': /*LCU entry*/
				fp >> xLCU >> yLCU >> currIdTile >> currIdFrame;
				newLCU = new LCUData(xLCU, yLCU, currIdTile, currIdFrame);
				refs.clear();
				
				break;
				
			case 'U': /*CU entry*/
				fp >> xCU >> yCU >> idDepth;
				break;
				
			case 'P': /*PU entry*/
				fp >> idPart >> sizePart >> currIdRefFrame;
				refs.insert(currIdRefFrame);
				break;
				
			case 'F': /*TZ First Search*/
				fp >> xFS >> yFS;
				newEntry = new Entry();
				newEntry->opcode = opcode;
				newEntry->xFS = xCU + xFS;
				newEntry->yFS = yCU + yFS;
				if(sizePart == TARGET_PU_SIZE and idDepth == TARGET_DEPTH and idRefFrame == currIdRefFrame) {
					newLCU->insert(newEntry);
				}
				break;
				
			case 'C': /*TZ Candidate Search*/
				fp >> xCand >> yCand;
				newEntry = new Entry();
				newEntry->opcode = opcode;
				newEntry->xCand = xCU + xCand;
				newEntry->yCand = yCU + yCand;
				if(sizePart == TARGET_PU_SIZE and idDepth == TARGET_DEPTH and idRefFrame == currIdRefFrame) {
					newLCU->insert(newEntry);
				}
				break;
				
			case 'R': /*TZ Raster Search*/
				fp >> xLeft >> xRight >> yTop >> yBottom;
				newEntry = new Entry();
				newEntry->opcode = opcode;
				newEntry->xLeft = xCU + xLeft;
				newEntry->xRight = xCU + xRight;
				newEntry->yTop = yCU + yTop;
				newEntry->yBottom = yCU + yBottom;
				if(sizePart == TARGET_PU_SIZE and idDepth == TARGET_DEPTH and idRefFrame == currIdRefFrame) {
					newLCU->insert(newEntry);
				}
				break;
				
			case 'E': /*End of LCU*/
				if(idFrame == currIdFrame and refs.find(idRefFrame) != refs.end()) {
					//TODO fix it!!
					this->videoData[currIdTile][xLCU/TARGET_CU_SIZE][yLCU/TARGET_CU_SIZE] = newLCU;
					
				}
				break;
		}
	}
	
	/*close trace file*/
	fp.close();
}

LCUData* TraceHandler::getLCUData(UInt idTile, UInt xLCU, UInt yLCU) {
	return this->videoData[idTile][xLCU][yLCU];
}