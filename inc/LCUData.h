#ifndef LCUDATA_H
#define	LCUDATA_H

#include <list>
#include <iostream>

#include "TypeDefs.h"
#include "Entry.h"


using namespace std;

class LCUData {
private:
    UInt xLCU;
    UInt yLCU;
    UInt idTile;
    UInt idFrame;
    
    list<Entry*> accList;

public:
   LCUData(UInt xLCU, UInt yLCU, UInt idTile, UInt idFrame);
   
   void insert(Entry* e);
   Entry* pop();
   bool empty();

   void printEntries();
};

#endif	/* LCUDATA_H */

