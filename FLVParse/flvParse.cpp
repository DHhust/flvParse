#include "flvParse.h"
#include <iostream>
#include <stdlib.h>
#include <string>
using namespace std;

void conv32(unsigned int & toconv) {
	union {
		unsigned int u;
		unsigned char c[4];
	} temp1, temp2;
	temp1.u = toconv;
	temp2.c[0] = temp1.c[3];
	temp2.c[1] = temp1.c[2];
	temp2.c[2] = temp1.c[1];
	temp2.c[3] = temp1.c[0];
	toconv = temp2.u;
}

void conv24(unsigned int & toconv) {
	union {
		unsigned int u;
		unsigned char c[4];
	} temp1, temp2;
	temp1.u = toconv;
	temp2.c[0] = temp1.c[2];
	temp2.c[1] = temp1.c[1];
	temp2.c[2] = temp1.c[0];
	temp2.c[3] = 0;
	toconv = temp2.u;
}

void malloc_init_Tag(p_Tag& tag) {
	tag = p_Tag(malloc(sizeof(_Tag)));
	if (tag == NULL) {
		cout << "malloc Tag failed!" << endl;
		return;
	}
	tag->flvTagData = NULL;
	tag->flvTagHeader = NULL;
}

void free_Tag(p_Tag& tag) {
	if (tag != NULL) {
		if (tag->flvTagData != NULL) {
			free(tag->flvTagData);
			tag->flvTagData = NULL;
		}
			
		if (tag->flvTagHeader != NULL) {
			free(tag->flvTagHeader);
			tag->flvTagHeader = NULL;
		}
		free(tag);
		tag = NULL;
	}
}



int read_flv_header(FILE* p_flvFile, p_FlvHeader& file_header){
	int ret=0;
	// malloc flv header
	file_header = p_FlvHeader(malloc(sizeof(_FlvHeader)));
	if(file_header==NULL){
		cout << "flv header malloc failed!" << endl;
		return -1;
	}
	memset(file_header, 0, sizeof(_FlvHeader));

	// read file type
	if ((ret = fread(file_header->signature, 3, 1, p_flvFile)) <= 0) {
		cout << "read file type failed!" << endl;
		goto error_exit;
	}

	// read version
	if ((ret = fread(&(file_header->version), 1, 1, p_flvFile)) <= 0) {
		cout << "read version failed!" << endl;
		goto error_exit;
	}

	// read flv information
	if ((ret = fread(&(file_header->flvInformation), 1, 1, p_flvFile)) <= 0) {
		cout << "read flv information failed!" << endl;
		goto error_exit;
	}

	// read flv header length
	if ((ret = fread(&(file_header->headerLength), 4, 1, p_flvFile)) <= 0) {
		cout << "read flv header length failed!" << endl;
		goto error_exit;
	}
	conv32(file_header->headerLength);
	return 0;

error_exit:
	free(file_header);
	return ret;
}

int verifyFlvHeader(p_FlvHeader file_header) {
	if (file_header == NULL) {
		cout << "flv header not exit!" << endl;
		return -1;
	}
	if (file_header->signature[0] != 'F' || file_header->signature[1] != 'L' || file_header->signature[2] != 'V') {
		cout << "this is not a flv file!" << endl;
		free(file_header);
		return -2;
	}
	else cout << "this is a flv file:" << endl;
	cout << "FLV version: " << file_header->version + 0 << endl;
	cout << "audio exist£¿ " << (file_header->flvInformation & 0x04) << endl;
	cout << "video exist£¿ " << (file_header->flvInformation & 0x01) << endl;
	cout << "data offset: " << file_header->headerLength << endl;
	return 0;
}

int read_preTagSize(FILE* p_flvFile, unsigned int & preTagSize) {
	int ret = 0;
	if ((ret = fread(&preTagSize, 4, 1, p_flvFile)) <= 0) {
		cout << "read preTagSize failed!" << endl;
		return ret;
	}
	conv32(preTagSize);
	cout << "previous tag size:" << preTagSize << endl;
	return 1;
}

int read_tagHeader(FILE* p_flvFile, p_TagHeader& tagHeader) {
	int ret = 0;
	tagHeader = p_TagHeader(malloc(sizeof(_TagHeader)));
	memset(tagHeader, 0, sizeof(_TagHeader));

	if (tagHeader == NULL) {
		cout << "flv header malloc failed!" << endl;
		return -1;
	}

	if ((ret=fread(&(tagHeader->tagType),1,1, p_flvFile))<=0) {
		cout << "read tagHeader tagType failed!" << endl;
		goto error_exit;
	}

	if ((ret = fread(&(tagHeader->dataSize), 3, 1, p_flvFile)) <= 0) {
		cout << "read tagHeader tagType failed!" << endl;
		goto error_exit;
	}
	conv24(tagHeader->dataSize);

	if ((ret = fread(&(tagHeader->timeStamp), 3, 1, p_flvFile)) <= 0) {
		cout << "read tagHeader timeStamp failed!" << endl;
		goto error_exit;
	}
	conv24(tagHeader->timeStamp);

	if ((ret = fread(&(tagHeader->timeStampExt), 1, 1, p_flvFile)) <= 0) {
		cout << "read tagHeader timeStampExt failed!" << endl;
		goto error_exit;
	}

	if ((ret = fread(&(tagHeader->streamID), 3, 1, p_flvFile)) <= 0) {
		cout << "read tagHeader streamID failed!" << endl;
		goto error_exit;
	}
	conv24(tagHeader->streamID);
	return 1;

error_exit:
	return ret;
}

int parseTagHeader(p_TagHeader tagHeader) {
	switch (tagHeader->tagType)
	{
	case 0x08:
		cout << "this is a audio tag:" << endl;
		break;
	case 0x09:
		cout << "this is a video tag:" << endl;
		break;
	case 0x12:
		cout << "this is a script tag:" << endl;
		break;
	default:
		cout << "tag type error!" << endl;
		return -1;
	}
	cout << "tag data size is: " << tagHeader->dataSize << endl;
	cout << "tag times stamp is: " << tagHeader->timeStamp << endl;
	cout << "tag times stamp extended  is: " << tagHeader->timeStampExt+0 << endl;
	cout << "tag stream ID  is: " << tagHeader->streamID << endl;
	return 1;
}

int read_tagData(FILE* p_flvFile, p_Tag tag) {
	int ret = 0;
	tag->flvTagData = (unsigned char *)(malloc(tag->flvTagHeader->dataSize+1));
	if (tag->flvTagData == NULL) {
		cout << "flvTagData malloc failed!" << endl;
		return -1;
	}
	if ((ret = fread(tag->flvTagData, tag->flvTagHeader->dataSize, 1, p_flvFile))<=0) {
		cout << "read flvTagData failed!" << endl;
		return ret;
	}
	return 1;
}

void audio_tagData(unsigned char* tagData) {
	unsigned char audioInf = tagData[0];
	unsigned char audioFormat, soundRate, soundSize, soundType;
	audioFormat = (audioInf & 0xf0) >> 4;
	soundRate = (audioInf & 0x0c) >> 2;
	soundSize = (audioInf & 0x02) >> 1;
	soundType = audioInf & 0x01;
	switch (audioFormat)
	{
	case 2:
		cout << "audio format: MP3" << endl;
		break;
	case 10:
		cout << "audio format: AAC" << endl;
		break;
	default:
		break;
	}

	switch (soundRate)
	{
	case 0:
		cout << "audio rate: "<<5.5<<"-khz" << endl;
		break;
	case 1:
		cout << "audio rate: " << 11 << "-khz" << endl;
		break;
	case 2:
		cout << "audio rate: " << 22 << "-khz" << endl;
		break;
	case 3:
		cout << "audio rate: " << 44 << "-khz" << endl;
		break;
	default:
		break;
	}

	switch (soundSize)
	{
	case 0:
		cout << "audio size: snd8bit" << endl;
		break;
	case 1:
		cout << "audio size: snd16bit" << endl;
		break;
	default:
		break;
	}

	switch (soundType)
	{
	case 0:
		cout << "audio type: sndMono" << endl;
		break;
	case 1:
		cout << "audio type: sndStereo" << endl;
		break;
	default:
		break;
	}
}

void video_tagData(unsigned char* tagData) {
	unsigned char videoInf = tagData[0];
	unsigned char frameType, codecID;
	frameType = (videoInf & 0xf0) >> 4;
	codecID = (videoInf & 0x0f);
	
	switch (frameType)
	{
	case 1:
		cout << "video frame type: keyframe" << endl;
		break;
	case 2:
		cout << "video frame type: inter frame" << endl;
		break;
	default:
		break;
	}

	switch (codecID)
	{
	case 1:
		cout << "video codecID: JPEG" << endl;
		break;
	case 7:
		cout << "video codecID: AVC" << endl;
		break;
	default:
		break;
	}

}

int script_tagData(unsigned char* tagData) {
	/*
	string name;
	//first AMF
	if (tagData==NULL||tagData[0] != 0x02) {
		cout << "script tag AMF error." << endl;
		return -1;
	}
	for (int i = 3; i < 10; i++)
		name += tagData[i + 3];
	if (name != "onMetaData") {
		cout << "script tag AMF error." << endl;
		return -1;
	}
	*/

}


int read_flv_body(FILE* p_flvFile, unsigned int& preTagSize, p_Tag tag) {
	int ret = 0;
	if ((ret = read_preTagSize(p_flvFile, preTagSize)) <= 0)
		return ret;
	if ((ret = read_tagHeader(p_flvFile, tag->flvTagHeader)) <= 0)
		return ret;
	if ((ret = parseTagHeader(tag->flvTagHeader)) <= 0)
		return ret;
	if ((ret = read_tagData(p_flvFile, tag)) <= 0)
		return ret;
	
	switch (tag->flvTagHeader->tagType)
	{
	case 0x08:
		audio_tagData(tag->flvTagData);
		break;
	case 0x09:
		video_tagData(tag->flvTagData);
		break;
	case 0x12:
		script_tagData(tag->flvTagData);
		break;
	default:
		cout << "tag type error!" << endl;
		return -1;
	}
	return 1;

}

int main(int argc, char* argv[]) {
	FILE* p_flvFile=NULL;
	if((p_flvFile = fopen("out.flv", "rb"))==NULL){
		cout << "flv file open failed!" << endl;
		return -1;
	}

	// flv header section
	p_FlvHeader file_header=NULL;
	if (read_flv_header(p_flvFile, file_header) < 0) {
		fclose(p_flvFile);
		cout << "read_flv_header execute error" << endl;
		return -2;
	}
	if (verifyFlvHeader(file_header) < 0) {
		cout << "verifyFlvHeader execute error" << endl;
		return -3;
	}
	free(file_header);

	// flv body section
	unsigned int preTagSize = 0;
	p_Tag tag = NULL;
	while (1) {
		free_Tag(tag);
		malloc_init_Tag(tag);
		if (read_flv_body(p_flvFile, preTagSize, tag) <= 0)
			break;
	}
	free_Tag(tag);
	fclose(p_flvFile);
	
	return 0;


}