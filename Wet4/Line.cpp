bool Line::getLineDirtyBit() {
 return dirty;
}

void Line::setLineDirtyBit(bool newDirty) {
 this->dirty = newDirty;
}

unsigned int Line::getLineTag() {
 return tag;
}

void Line::setLineTag(unsigned int newTag) {
	this->tag = newTag;
}

unsigned long int Line::getLine() {
	return this->line;
}

void Line::setLine(unsigned long int line) {
	this->line = line;
}
