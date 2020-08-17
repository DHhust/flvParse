#include "flvParse.h"
#include <stdio.h>
#include <stdlib.h>

int read_flv_header(FILE* p_flvFile, p_flvHeader file_header){
	int ret=0;
	file_header=malloc(sizeof(flvHeader))
	if(file_header==NULL){
		fprintf(stderr,"malloc file header failed\n")
		perror("")
	}
}

int main(int argc, char* argv[]) {
	FILE* p_flvFile=NULL;
	if((p_flvFile = fopen("./sample.flv", "rb"))==NULL){
		perror("flv file open failed!")
		return;
	}
	p_flvHeader file_header=NULL;
	if()
}