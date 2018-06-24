#ifndef LINE_HPP_
#define LINE_HPP_

class Line {
private:
	bool dirty;				// True if line is dirty, false otherwise
	unsigned int tag;
public:
	Line(bool dirty, unsigned int tag) : dirty(dirty), tag(tag) {}

	bool getLineDirtyBit();
	void setLineDirtyBit(bool dirty);
	unsigned int getLineTag();
	void setLineTag(unsigned int tag);

};



#endif /* LINE_HPP_ */
