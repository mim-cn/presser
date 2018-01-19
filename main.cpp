/*************************************************************************
  > File Name: main.cpp
  > Author: skybosi
  > Mail: skybosi_ok@163.com 
  > Created Time: 2017年10月26日 星期四 17时30分24秒
 ************************************************************************/

#include<iostream>
#include"Stream.h"
using namespace std;
using namespace mimer;
#define BUFFER_SIZE 1024
#define REVERSE     0
#define getbit(BYTE, S, E) 

inline int I(unsigned char ch){
	int size = 0;
	while(ch >>= 1){
		size++;
	}
	return size;
}

inline int A(char* buffer, char* left_char, unsigned char* left_size, char cur_char, unsigned char cur_size)
{
	// (*left_size <= cur_size) ? tmp_char >> (cur_size - *left_size) : tmp_char << (*left_size - cur_size);
	if(*left_size + cur_size >= 8){
		unsigned char tmp_char = cur_char;
		if(cur_size == 8){
			tmp_char >>= (*left_size);
			*left_size = 0;
			*left_char = 0;
			*buffer = tmp_char;
		}else{
			tmp_char >>= (cur_size - (*left_size));
			*left_size = cur_size - (*left_size);
			*left_char = cur_char & (1 << ((*left_size) - 1));
			*buffer = (*left_char) | tmp_char;
		}
		return 1;
	}else{
		*left_size = cur_size + (*left_size);
		*left_char |= (cur_char << (8 - (*left_size)));
	}
	return 0;
}


void calcer(char* buf, size_t size)
{
	//	std::cout << size << std::endl;
	size_t i = 0;
	char diff = 0;
	unsigned char pref = 0;
	unsigned char next = 0;
	char*  newbuf = (char*)malloc(size*2);
	memset(newbuf, 0, size*2);
	char left_char = 0;
	unsigned char left_size = 0;
	unsigned char d = 0;
	size_t offset = 0;
	while(i < size - 1){
		pref = buf[i];
		next = buf[i+1];
		diff = next - pref;
		printf("%d\n",abs(diff));
		if (diff < 0){
			// k
			diff = REVERSE - diff;
			offset += A(&newbuf[offset], &left_char, &left_size, diff, 8);
			// d
			d = (1 << (256 - pref)) | pref;
			offset += A(&newbuf[offset], &left_char, &left_size, d, I(256 - pref));
		}else{
			// k
			offset += A(&newbuf[offset], &left_char, &left_size, diff, 8);
			// d
			d = pref;
			offset += A(&newbuf[offset], &left_char, &left_size, d, I(256 - pref));
		}
		i += 2;
	}
}


int main(int argc,char** argv)
{
	if (argc < 2){
		printf("%s filename\n",argv[0]);
		exit(0);
	}
	Stream file(argv[1], "rb+");
	char buff[BUFFER_SIZE] = {0};
	while(!file.Eof()){
		size_t size = file.Read(buff, 1, 1024);
		if(!size){
			break;
		}
		calcer(buff, size);
		//std::cout << buff << std::endl;
	}
	return 0;
}
