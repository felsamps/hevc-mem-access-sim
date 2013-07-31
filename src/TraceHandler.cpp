#include "../inc/TraceHandler.h"

TraceHandler::TraceHandler(UInt numOfTiles, UInt wFrame, UInt hFrame, string traceFileName) {
	UInt wInLCU = (wFrame / 64) + 1;
	UInt hInLCU = (hFrame / 64) + 1;
	
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
	
	/*look for LCU memory accesses regarding the idFrame and ifRefFrame*/
	while(!fp.eof()) {		
		
		fp >> opcode;
		
		switch(opcode) {
			case 'L': /*LCU entry*/
				fp >> xLCU >> yLCU >> currIdTile >> currIdFrame;
				newLCU = new LCUData(xLCU, yLCU, currIdTile, currIdFrame);
				break;
				
			case 'U': /*CU entry*/
				fp >> xCU >> yCU >> idDepth;
				break;
				
			case 'P': /*PU entry*/
				fp >> idPart >> sizePart >> currIdRefFrame;
				break;
				
			case 'F': /*TZ First Search*/
				fp >> xFS >> yFS;
				newEntry = new Entry();
				newEntry->opcode = opcode;
				newEntry->xFS = xFS;
				newEntry->yFS = yFS;
				if(sizePart == 0) { //ONLY 2Nx2N
					newLCU->insert(newEntry);
				}
				break;
				
			case 'C': /*TZ Candidate Search*/
				fp >> xCand >> yCand;
				newEntry = new Entry();
				newEntry->opcode = opcode;
				newEntry->xCand = xCand;
				newEntry->yCand = yCand;
				if(sizePart == 0) { //ONLY 2Nx2N
					newLCU->insert(newEntry);
				}
				break;
				
			case 'R': /*TZ Raster Search*/
				fp >> xLeft >> xRight >> yTop >> yBottom;
				newEntry = new Entry();
				newEntry->opcode = opcode;
				newEntry->xLeft = xLeft;
				newEntry->xRight = xRight;
				newEntry->yTop = yTop;
				newEntry->yBottom = yBottom;
				if(sizePart == 0) { //ONLY 2Nx2N
					newLCU->insert(newEntry);
				}
				break;
				
			case 'E': /*End of LCU*/
				if(idFrame == currIdFrame and idRefFrame == currIdRefFrame) {
					//TODO fix it!!
					if(idDepth == 3) {
						this->videoData[currIdTile][xLCU/64][yLCU/64] = newLCU;
					}
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