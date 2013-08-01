#include "../inc/LCUData.h"

LCUData::LCUData(UInt xLCU, UInt yLCU, UInt idTile, UInt idFrame) {
	this->xLCU = xLCU;
	this->yLCU = yLCU;
	this->idTile = idTile;
	this->idFrame = idFrame;
	
	this->accList.clear();
}

void LCUData::insert(Entry* e) {
	this->accList.push_back(e);
}

Entry* LCUData::pop() {
	Entry* returnable = this->accList.front();
	this->accList.pop_front();
	return returnable;
}

bool LCUData::empty() {
	return this->accList.empty();
}

void LCUData::printEntries() {
	cout << this->accList.size() << endl;
	/*for(list<Entry*>::iterator it = this->accList.begin(); it != this->accList.end(); it++) {
		Entry* e = (*it);
		cout << e->opcode << endl;
	}*/
}