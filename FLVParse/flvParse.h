#ifndef _FLV_PARSE_H
#define _FLV_PARSE_H

typedef struct
{
	unsigned char signature[3];
	unsigned char version;
	unsigned char flvInformation;
	unsigned int headerLength;
}flvHeader, *p_flvHeader;


typedef struct
{
	unsigned char tagType;

	unsigned char timeStamp[3];
	unsigned char timeStampExt;
	unsigned char streamID[3];
}tagHeader, *p_tagHeader;



#endif // !_FLV_PARSE_H

