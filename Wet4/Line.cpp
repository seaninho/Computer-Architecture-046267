#include "Line.hpp"

bool Line::isInit() {
	return this->init;
}

void Line::setInit(bool init) {
	this->init = init;
}

bool Line::getLineDirtyBit() {
	return this->dirty;
}

void Line::setLineDirtyBit(bool dirty) {
	this->dirty = dirty;
}

unsigned long Line::getLineTag() {
	return this->tag;
}

void Line::setLineTag(unsigned long tag) {
	this->tag = tag;
}

unsigned long Line::getLineAddr() {
//	std::cout << "Line.cpp : line 28 - getLineAddr " << std::endl;
	return this->addr;
}

void Line::setLineAddr(unsigned long addr) {
	this->addr = addr;
}

