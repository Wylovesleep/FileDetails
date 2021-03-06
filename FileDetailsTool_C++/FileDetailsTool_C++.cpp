// FileDetailsTool_C++.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <io.h>
#include <vector>
#include <fstream>
#include "md5file.h"
#include "base_64.h"

using namespace std;


typedef struct
{
	int mode;    //标志位：GameFiles为1，ImageFiles为2，versionFile为0，其他为-1。
	string id;
	string path;
	string md5;
	_fsize_t size;
}FileDetails;

static int str_to_hex(const char *string, unsigned char *cbuf, int len)
{
	BYTE high, low;
	int idx, ii = 0;
	for (idx = 0; idx<len; idx += 2)
	{
		high = string[idx];
		low = string[idx + 1];

		if (high >= '0' && high <= '9')
			high = high - '0';
		else if (high >= 'A' && high <= 'F')
			high = high - 'A' + 10;
		else if (high >= 'a' && high <= 'f')
			high = high - 'a' + 10;
		else
			return -1;

		if (low >= '0' && low <= '9')
			low = low - '0';
		else if (low >= 'A' && low <= 'F')
			low = low - 'A' + 10;
		else if (low >= 'a' && low <= 'f')
			low = low - 'a' + 10;
		else
			return -1;

		cbuf[ii++] = high << 4 | low;
	}
	return 0;
}


string Md5ToBase64(string SrcMd5)
{
	Base64 *base = new Base64();
	int len = SrcMd5.length();
	unsigned char *arrMd5 = new unsigned char[len];
	str_to_hex(SrcMd5.c_str(),arrMd5,len);
//	const unsigned char *arrMd5 = reinterpret_cast<const unsigned char*>(SrcMd5.c_str());
	string encode = base->Encode(arrMd5, SrcMd5.length());
	delete[] arrMd5;
	return encode;
}

string GetSourcePath(void)
{
	string SourcePath;
	cout << "请输入游戏路径,例如：E:\\HTC_GAME\\95a472f1-0a8b-495b-aa2f-6df838c16788" << endl;
	getline(cin,SourcePath);
//	cout << "输入的路径为："<<SourcePath << endl;                   //打印信息

	return SourcePath;
}

void DealFilePath(string &srcpath)
{
	int pos = 0;
//	int i = 0;

	while (1)
	{
		pos = srcpath.find("\\", pos);
		if (pos == -1)
		{
			break;
		}
//		cout << i++;
		srcpath.insert(pos, "\\");
		pos += 2;
	}
}

/*
传入文件path，获取文件id
mode：GameFiles为1，ImageFiles为2，versionFile为0，其他为-1.
*/
string DealFileId(string path, int mode)       
{
	int pos = 0;
	while (1)
	{
		pos = path.find("\\\\", pos);
		if (pos == -1)
		{
			break;
		}
		path.replace(pos, 2, "/");
		pos++;
	}	
	if (mode == 1)
	{
		path = path.erase(0, path.find("/game/") + 6);		
	}
	else if (mode == 2)
	{
		path = path.erase(0, path.find("/thumbnail/") + 11);
	}
	else if (mode == 0)
	{
		path = path.erase(0, path.find("/version") + 1);
	}
	else
	{
		//-1暂无处理
	}

	return path;
}

/*
获取传入的path路径下所有文件的路径（包含文件名）及size，分别存入传入的容器files和vctsize中，会忽略文件名为FileDetails的文件。
*/
void GetFileName(string path, vector<string>& files, vector<_fsize_t>& vctsize, vector<string>& vctmd5)
{

	//文件句柄  
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	static string tpath = path;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
			if ((fileinfo.attrib == _A_SUBDIR))
			{
				//文件名不等于"."&&文件名不等于"..
				//.表示当前目录
				//..表示当前目录的父目录
				//判断时，两者都要忽略，不然就无限递归跳不出去了！
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)   
				{
//					cout << "文件夹" << endl;
					GetFileName(p.assign(path).append("\\").append(fileinfo.name), files, vctsize, vctmd5);
				}
			}
			else
			{
				string temp("FileDetails");
				p.assign(path).append("\\").append(fileinfo.name);
				
				cout << p << endl;
				if (p.find(temp) == -1)     //忽略FileDetails文件
				{
					string tmpmd5;
					tmpmd5 = getFileMD5(p);  //生成MD5；
					tmpmd5 = Md5ToBase64(tmpmd5); //md5转base64
//					cout << p << "  md5:" << tmpmd5 << endl;
					vctmd5.push_back(tmpmd5); //push base64

					DealFilePath(p);               //单斜线改成双斜线
//					cout << tpath << endl;
//					p = p.erase(0, tpath.find_last_of('\\', tpath.size() - 1) + 1);   //删除上级路径
					p = p.erase(0, p.find((tpath.erase(0, tpath.find_last_of('\\', tpath.size() - 1)+1))));  //删除上级路径
					files.push_back(p);
					vctsize.push_back(fileinfo.size);
				}
			}
		}
		while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

/*
获取路径及大小并打印出来。
*/
void OutputFileName(string SourcePath)
{
	int filenum = 0;
	vector<string> Filepath;  //
	vector<_fsize_t> vctsize;
	vector<string>::iterator iter;
	vector<_fsize_t>::iterator iter2;
	
//	GetFileName(SourcePath, Filepath, vctsize);

	for (iter = Filepath.begin(); iter != Filepath.end(); iter++)
	{
		cout <<*iter<<endl;
		filenum++;
	}
	for (iter2 = vctsize.begin(); iter2 != vctsize.end(); iter2++)
	{
		cout << *iter2 << endl;
	}
	cout << "总共" << filenum <<"个文件"<< endl;
}

void GetFileDetails(string srcpath, vector<FileDetails> &filedetails)
{
	vector<string> filepath;
	vector<_fsize_t> filesize;
	vector<string> filemd5;
	vector<string>::iterator iterpath;
	FileDetails stufiledetails;
	int i = 0;

	GetFileName(srcpath, filepath, filesize, filemd5);

	
	for (iterpath = filepath.begin(); iterpath != filepath.end(); iterpath++)
	{
//		cout << *iterpath << endl;
		stufiledetails.path = *iterpath;
//		cout << *iterpath << endl;;
		if ((*iterpath).find("\\\\game\\\\") != (*iterpath).npos)
		{
			stufiledetails.mode = 1;
		}
		else if ((*iterpath).find("\\\\thumbnail\\\\") != (*iterpath).npos)
		{
			stufiledetails.mode = 2;
		}
		else if ((*iterpath).find("\\\\version.") != (*iterpath).npos)
		{
			stufiledetails.mode = 0;
		}
		else
		{
			stufiledetails.mode = -1;
		}
		filedetails.push_back(stufiledetails);	
	}
	

	for (i = 0; i < filepath.size(); i++)   //warning
	{
		filedetails[i].size = filesize[i];
		filedetails[i].id = DealFileId(filedetails[i].path, filedetails[i].mode);	
		
	}

	for (i = 0; i < filepath.size(); i++)   //warning
	{
		filedetails[i].md5 = filemd5[i];
		cout << "{\"id\":\"" << filedetails[i].id << "\",\"path\":\"" << filedetails[i].path << "\",\"md5\":\"" << filedetails[i].md5 << "\",\"size\":\"" << filedetails[i].size << "}," <<filedetails[i].mode<< endl;
	}
	cout << "总共" << i << "个文件" << endl;
}

void WriteFileDetails(const string srcpath, vector<FileDetails> vctfiledetails)
{
	string filename = srcpath + "\\FileDetails.txt";
	ofstream filedetails;
	string folder1("\"GameFiles\"");
	string folder2("\"ImageFiles\"");
	string folder0("\"versionFile\"");
	string id("\"id\"\:\"");   //   "id":"
	string path("\",\"path\"\:\"");    // ","path":"
	string md5("\",\"md5\"\:\"");           // ","md5":"
	string size("\",\"size\"\:\"");        //","size":

	filedetails.open(filename,ios::trunc);  //ios::trunc打开后之前清空文件
	if (filedetails.is_open())
	{
		cout << "open success" << endl;
		filedetails << "\{";      //写入第一个总的大括号

		//写game文件
		filedetails << folder1 <<"[";
		for (int i = 0; i < vctfiledetails.size(); i++)
		{
			if (vctfiledetails[i].mode == 1)
			{
				filedetails << "{" << id << vctfiledetails[i].id << path << vctfiledetails[i].path << md5 << vctfiledetails[i].md5 << size << vctfiledetails[i].size << "},";
				filedetails << endl;

			}
			else
			{
				continue;
			}
		}
		filedetails << "],";

		//写thumbnail文件
		filedetails << folder2 << "[";
		for (int i = 0; i < vctfiledetails.size(); i++)
		{
			if (vctfiledetails[i].mode == 2)
			{
				filedetails << "{" << id << vctfiledetails[i].id << path << vctfiledetails[i].path << md5 << vctfiledetails[i].md5 << size << vctfiledetails[i].size << "},";
				filedetails << endl;
			}
			else
			{
				continue;
			}
		}
		filedetails << "],";

		//写version文件
		filedetails << folder0 ;
		for (int i = 0; i < vctfiledetails.size(); i++)
		{
			if (vctfiledetails[i].mode == 0)
			{
				filedetails << "{" << id << vctfiledetails[i].id << path << vctfiledetails[i].path << md5 << vctfiledetails[i].md5 << size << vctfiledetails[i].size << "},";
				filedetails << endl;
			}
			else
			{
				continue;
			}
		}

		filedetails << "\}";      //写入最后一个总大括号
	}
	else
	{
		cout << "open file error!" << endl;
	}

	filedetails.close();

}

int main()
{
	vector<FileDetails> filedetails;
	string SourcePath;
	SourcePath = GetSourcePath();
	GetFileDetails(SourcePath, filedetails);
	WriteFileDetails(SourcePath, filedetails);

	system("pause");

    return 0;
}

