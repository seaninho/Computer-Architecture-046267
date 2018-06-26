#ifndef LINE_HPP_
#define LINE_HPP_

const int UNUSED_LINE = -1;

class Line {
private:
	bool dirty;				// True if line is dirty, false otherwise
	unsigned long tag;
	unsigned long addr;

public:
//	Line() : dirty(false), tag(UNUSED_LINE){}
	Line(bool dirty, int tag, unsigned long addr) : dirty(dirty), tag(tag), addr(addr){}

	bool getLineDirtyBit();
	void setLineDirtyBit(bool dirty);
	unsigned long getLineTag();
	void setLineTag(unsigned long tag);
	unsigned long getLineAddr();
	void setLineAddr(unsigned long addr);

};

#endif /* LINE_HPP_ */
