#include <iostream>
#include <time.h>
#include "Stream.h"
#include "presser.h"
using namespace std;
#define BUFFER_SIZE   4096

int main(int argc,char** argv)
{
	if (argc < 3){
		printf("%s in-file out-file\n",argv[0]);
		exit(0);
	}
	int begintime = 0, endtime = 0;
	begintime = clock();  //计时开始
	Stream file(argv[1], "rb+");
	char buff[BUFFER_SIZE] = {0};
	presser press(argv[2], "wb+");
	while(!file.Eof()){
		size_t size = file.Read(buff, 1, 1024);
		if(!size){
			break;
		}
		if (!press.decode(buff, size)) {
			break;
		}
	}
	endtime = clock();  //计时开始
	printf("Running Time:%d ms\n", endtime - begintime);
	return 0;
}
