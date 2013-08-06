#include <set>

#include "../inc/MemoryHandler.h"

int TZ_FIRST_SEARCH[65][2] = { {0,-1}, {-1,0}, {1,0}, {0,1}, {0,-2}, {-1,-1}, {1,-1}, {-2,0}, {2,0}, {-1,1}, {1,1}, {0,2}, {0,-4}, {-2,-2}, {2,-2}, {-4,0}, {4,0}, {-2,2}, {2,2}, {0,4}, {0,-8}, {-4,-4}, {4,-4}, {-8,0}, {8,0}, {-4,4}, {4,4}, {0,8}, {0,-16}, {-16,0}, {16,0}, {0,16}, {-4,-12}, {4,-12}, {-4,12}, {4,12}, {-8,-8}, {8,-8}, {-8,8}, {8,8}, {-12,-4}, {12,-4}, {-12,4}, {12,4}, {32,0}, {0,32}, {-8,-24}, {8,-24}, {-8,24}, {8,24}, {-16,-16}, {16,-16}, {-16,16}, {16,16}, {-24,-8}, {24,-8}, {-24,8}, {24,8}, {64,0}, {0,64}, {-16,48}, {16,48}, {32,32}, {48,-16}, {48,16} };
int NUM_OF_FIRST_SEARCH_CANDIDATES = 65;

MemoryHandler::MemoryHandler(UInt numOfTiles, UInt wFrame, UInt hFrame) {
	this->wInLCU = (wFrame % TARGET_CU_SIZE == 0) ? wFrame / TARGET_CU_SIZE : wFrame / TARGET_CU_SIZE + 1;
	this->hInLCU = (hFrame % TARGET_CU_SIZE == 0) ? hFrame / TARGET_CU_SIZE : hFrame / TARGET_CU_SIZE + 1;
	
	this->wFrame = wFrame;
	this->hFrame = hFrame;
	this->numOfTiles = numOfTiles;
	
	this->refFramePerspective = new LLInt**[numOfTiles];
	this->accessed = new UInt**[numOfTiles];
	for (int t = 0; t < numOfTiles; t++) {
		this->refFramePerspective[t] = new LLInt*[wFrame];
		this->accessed[t] = new UInt*[wFrame];
		for (int x = 0; x < wFrame; x++) {
			this->refFramePerspective[t][x] = new LLInt[hFrame];
			this->accessed[t][x] = new UInt[hFrame];
			for (int y = 0; y < hFrame; y++) {
				this->refFramePerspective[t][x][y] = 0;
				this->accessed[t][x][y] = 0;
			}
		}
	}
	
	this->currFramePerspective = new LLInt*[this->wInLCU];
	for (int x = 0; x < this->wInLCU; x++) {
		this->currFramePerspective[x] = new LLInt[this->hInLCU];
		for (int y = 0; y < hInLCU; y++) {
			this->currFramePerspective[x] = new LLInt[hFrame];
		}
	}
	
	this->overlapping = new LLInt*[wFrame];
	for (int x = 0; x < wFrame; x++) {
		this->overlapping[x] = new LLInt[hFrame];
		for (int y = 0; y < hFrame; y++) {
			this->overlapping[x][y] = 0;
		}
	}
				
	for (int t = 0; t < numOfTiles; t++) {
		stringstream ss;
		ss << t;
		
		string name = "tile_" + ss.str() + ".mat";
		this->outputFilesName.push_back(name);
	}
}

void MemoryHandler::generateAccessMap(TraceHandler* th) {
	/*for each tile*/
	for (int t = 0; t < numOfTiles; t++) {
		/*for each LCU*/
		for (int x = 0; x < wFrame/TARGET_CU_SIZE; x++) {
			for (int y = 0; y < hFrame/TARGET_CU_SIZE; y++) {
				cout << x << " " << y << " " << t << endl;
				LCUData* lcu = th->getLCUData(t, x, y);
				if(lcu != NULL) {
					this->xAccessMemory(lcu, t, x, y);
				}
			}			
		}
	}
}

void MemoryHandler::xAccessBlock(UInt x, UInt y, UInt sizeCU) {
	for (int xx = 0; xx < sizeCU; xx++) {
		for (int yy = 0; yy < sizeCU; yy++) {
			pair<UInt, UInt> p(x+xx,y+yy);
			refAcc.insert(p);
		}
	}

}

void MemoryHandler::xUpdateMemory(UInt idTile, UInt xLCU, UInt yLCU) {
	for(set<pair<UInt,UInt> >::iterator it=this->refAcc.begin(); it!= this->refAcc.end(); it++) {
		UInt x = (*it).first;
		UInt y = (*it).second;
		
		if((x >=0 and x < this->wFrame) and (y >= 0 and y < this->hFrame)) {
			this->refFramePerspective[idTile][x][y] += 1;
			this->currFramePerspective[xLCU][yLCU] += 1;
		}
	}
}

void MemoryHandler::xAccessMemory(LCUData* lcu, UInt idTile, UInt xLCU, UInt yLCU) {
	
	UInt x, y;
	
	this->refAcc.clear();	
	
	while(!lcu->empty()) {
		Entry* e = lcu->pop();
		switch(e->opcode) {
			case 'C':
				x = e->xCand;
				y = e->yCand;
				xAccessBlock(x, y, TARGET_CU_SIZE);
				xUpdateMemory(idTile, xLCU, yLCU);
				this->refAcc.clear();
				break;
				
			case 'F':
				for (int c = 0; c < NUM_OF_FIRST_SEARCH_CANDIDATES; c++) {
					x = TZ_FIRST_SEARCH[c][0] + e->xFS;
					y = TZ_FIRST_SEARCH[c][1] + e->yFS;
					xAccessBlock(x, y, TARGET_CU_SIZE);
					xUpdateMemory(idTile, xLCU, yLCU);
					this->refAcc.clear();
				}
				break;
			case 'R':
				for (x = e->xLeft; x < e->xRight; x+=RASTER_STEP) {
					for (y = e->yTop; y < e->yBottom; y+=RASTER_STEP) {
						xAccessBlock(x, y, TARGET_CU_SIZE);
						xUpdateMemory(idTile, xLCU, yLCU);
						this->refAcc.clear();
					}
				}

		}
	}
}

void MemoryHandler::reportAccessMap() {
	/*Reference frame report*/	
	
	for (int t = 0; t < this->numOfTiles; t++) {
		fstream fp;
		fp.open(this->outputFilesName[t].c_str(), fstream::out);
		for (int y = 0; y < this->hFrame; y++) {
			for (int x = 0; x < this->wFrame; x++) {
				fp << this->refFramePerspective[t][x][y] << " ";
				this->accessed[t][x][y] = (this->refFramePerspective[t][x][y] > 0) ? 1 : 0;
			}	
			fp << endl;
		}
		fp.close();
	}
	
	/*Current frame report*/
	fstream fp;
	fp.open("curr_frame.mat", fstream::out);
	for (int y = 0; y < this->hInLCU; y+= 64/TARGET_CU_SIZE) {
		for (int x = 0; x < this->wInLCU; x+= 64/TARGET_CU_SIZE) {
			fp << this->currFramePerspective[x][y] << " ";
		}	
		fp << endl;
	}
	fp.close();
	
	/*Overlapping report*/	
	UInt overlSize = 0;
	fp.open("overlapping.mat", fstream::out);
	for (int y = 0; y < this->hFrame; y++) {
		for (int x = 0; x < this->wFrame; x++) {
			for (int t = 0; t < this->numOfTiles; t++) {
				this->overlapping[x][y] += this->accessed[t][x][y];								
			}
			fp << this->overlapping[x][y] << " ";
			overlSize += (this->overlapping[x][y] > 1) ? 1 : 0;
		}	
		fp << endl;
	}
	fp.close();
	
	/*Statitics Report*/
	
	cout << "Overl.[bytes]:\t" << overlSize << endl;
	cout << "Overl.[%]:\t" << overlSize/(double)(this->wFrame*this->hFrame) << endl;
	
}