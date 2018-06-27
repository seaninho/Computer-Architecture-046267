#ifndef LINE_HPP_
#define LINE_HPP_

#include <iostream>

const int UNUSED_LINE = 0;

class Line {
private:
	bool init;				// True if the line has been initialized
	bool dirty;				// True if line is dirty, false otherwise
	unsigned long tag;
	unsigned long addr;

public:
	Line() : init(false), dirty(false), tag(UNUSED_LINE), addr(UNUSED_LINE){}

	/*
	 * isInit - determines whether a set has been initialized or not
	 * 		return true if it is, false otherwise
	 */
	bool isInit();

	/*
	 * setInit - Sets initialization argument for the line
	 * param[in] init - true if the line is being initialized. false otherwise (being removed)
	 */
	void setInit(bool init);

	/*
	 * getLineDirtyBit - Returns line's dirty bit
	 * 		returns true if the line is dirty (the data has been changed), false otherwise
	 */
	bool getLineDirtyBit();

	/*
	 * setLineDirtyBit - Sets line's dirty bit
	 * param[in] dirty - true if the line is dirty (the data has been changed), false otherwise
	 */
	void setLineDirtyBit(bool dirty);

	/*
	 * getLineTag - Returns line's tag
	 *
	 */
	unsigned long getLineTag();

	/*
	 * setLineTag - Sets line's tag
	 * param[in] tag - line's tag (unsigned long int)
	 */
	void setLineTag(unsigned long tag);

	/*
	 * getLineAddr - Returns line's address
	 *
	 */
	unsigned long getLineAddr();

	/*
	 * setLineDirtyBit - Sets line's address
	 * param[in] addr - line's address
	 */
	void setLineAddr(unsigned long addr);

};

#endif /* LINE_HPP_ */
