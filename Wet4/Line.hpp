#ifndef LINE_HPP_
#define LINE_HPP_


const int UNUSED_LINE = 0;

class Line {
private:
	bool init;
	bool dirty;				// True if line is dirty, false otherwise
	unsigned long tag;
	unsigned long addr;

public:
	Line() : init(false), dirty(false), tag(UNUSED_LINE), addr(UNUSED_LINE){}

	bool isInit();
	void setInit(bool init);
	bool getLineDirtyBit();
	void setLineDirtyBit(bool dirty);
	unsigned long getLineTag();
	void setLineTag(unsigned long tag);
	unsigned long getLineAddr();
	void setLineAddr(unsigned long addr);

};

#endif /* LINE_HPP_ */
