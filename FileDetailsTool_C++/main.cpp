#include "stdafx.h"
#include "FileDetails.h"

int main()
{
	vector<FileDetails> filedetails;
	string SourcePath;
	SourcePath = GetSourcePath();
	cout << endl << "正在获取FileDetails..." << endl;
	GetFileDetails(SourcePath, filedetails);
	cout << endl << "正在写入文件..." << endl;
	WriteFileDetails(SourcePath, filedetails);

	system("pause");

	return 0;
}