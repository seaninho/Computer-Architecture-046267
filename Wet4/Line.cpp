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
	return this->addr;
}

void Line::setLineTag(unsigned long addr) {
	this->addr = addr;
}

