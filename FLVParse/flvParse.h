#ifndef _FLV_PARSE_H
#define _FLV_PARSE_H

typedef struct
{
	unsigned char signature[3];
	unsigned char version;
	unsigned char flvInformation;
	unsigned int headerLength;
}_FlvHeader, *p_FlvHeader;


typedef struct
{
	unsigned char tagType;
	unsigned int dataSize;
	unsigned int timeStamp;
	unsigned char timeStampExt;
	unsigned int streamID;
}_TagHeader, *p_TagHeader;


typedef struct
{
	p_TagHeader flvTagHeader;
	unsigned char* flvTagData;
}_Tag, *p_Tag;

#endif // !_FLV_PARSE_H

