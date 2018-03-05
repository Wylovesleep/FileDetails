/*filedetails.h文件*/
#ifndef FILEDETAILS_H
#define FILEDETAILS_H
#include "stdafx.h"
#include <iostream>
#include <string>
#include <io.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include "md5file.h"
#include "base_64.h"
using namespace std;

#define GAMEFOLDER "game\\"
#define IMAGEFOLDER "image\\"

typedef struct
{
	int mode;    //标志位：GameFiles为1，ImageFiles为2，configFile为0，其他为-1。
	string id;
	string path;
	string md5;
	_fsize_t size;
}FileDetails;

static int str_to_hex(string string, unsigned char *arrMd5, int len);
string Md5ToBase64(string SrcMd5);
string GetSourcePath(void);
void DealFilePath(string &srcpath);
string DealFileId(string srcpath, string path, int mode);
void GetFileName(string path, vector<FileDetails> &filedetails);
void OutputFileName(string SourcePath);
void GetFileDetails(string srcpath, vector<FileDetails> &filedetails);
void WriteFileDetails(const string srcpath, vector<FileDetails> vctfiledetails);

#endif
