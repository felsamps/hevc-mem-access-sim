#ifndef ENTRY_H
#define	ENTRY_H

class Entry {
public: 
    char opcode;
    UInt xFS, yFS;
    UInt xCand, yCand;
    UInt xLeft, xRight, yTop, yBottom;
    
    Entry() {};
};


#endif	/* ENTRY_H */

