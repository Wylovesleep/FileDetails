#include "stdafx.h"
#include "FileDetails.h"

int main()
{
	vector<FileDetails> filedetails;
	string SourcePath;
	SourcePath = GetSourcePath();
	cout << endl << "���ڻ�ȡFileDetails..." << endl;
	GetFileDetails(SourcePath, filedetails);
	cout << endl << "����д���ļ�..." << endl;
	WriteFileDetails(SourcePath, filedetails);

	system("pause");

	return 0;
}