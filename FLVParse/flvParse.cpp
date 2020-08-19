#include "flvParse.h"
#include <iostream>
#include <stdlib.h>
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

int read_flv_header(FILE* p_flvFile, p_flvHeader& file_header){
	int ret=0;
	// malloc flv header
	file_header = p_flvHeader(malloc(sizeof(flvHeader)));
	if(file_header==NULL){
		cout << "flv header read failed!" << endl;
		return -1;
	}
	memset(file_header, 0, sizeof(flvHeader));

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

int verifyFlvHeader(p_flvHeader file_header) {
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

int main(int argc, char* argv[]) {
	FILE* p_flvFile=NULL;
	if((p_flvFile = fopen("out.flv", "rb"))==NULL){
		cout << "flv file open failed!" << endl;
		return -1;
	}

	// flv header section
	p_flvHeader file_header=NULL;
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


	fclose(p_flvFile);
	
	return 0;


}