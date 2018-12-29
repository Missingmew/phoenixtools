struct GMDHeader {
	u8 signature[4];//always GMD(null)
	u8 unk1[4];//always 02030100?
	u8 pad1[12];//always zero
	u32 sectionCnt1,sectionCnt2;//no idea why it's stored twice
	u32 labelChunkSize,scriptChunkSize;
	u32 nameLen;
	u8 name[nameLen];
	sectionEntry sectionTbl[sectionCnt1];
}

struct ptrEntry {
	u32 offset;//relative to pointee block start
	u32 unk1;//usually zero
}

struct sectionEntry {
	u32 sectionId;
	ptrEntry damnedPointer;//encrypted presumed pointer to section data
	ptrEntry labelPointer;
}