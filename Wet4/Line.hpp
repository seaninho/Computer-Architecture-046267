#ifndef LINE_HPP_
#define LINE_HPP_

const int UNUSED_LINE = -1;

class Line {
private:
	bool dirty;				// True if line is dirty, false otherwise
	int tag;
	unsigned long int line;
public:
	Line() : dirty(false), tag(UNUSED_LINE), line(0) {}
	Line(bool dirty, int tag, unsigned long int line) : dirty(dirty), tag(tag),
			line(line) {}

	bool getLineDirtyBit();
	void setLineDirtyBit(bool newDirty);
	unsigned int getLineTag();
	void setLineTag(unsigned int newTag);

};



#endif /* LINE_HPP_ */
