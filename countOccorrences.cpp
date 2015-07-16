#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

typedef enum {
	File = 100,
	Directory,
	Database,
	Network,
	Registry,
	CallExec,
	SystemInfo,
	Encrypt,
	Login,
	Account
} OperateType;

typedef struct {
	OperateType op;		// operate type
	unsigned int n;		// times
	std::string kw;		// keyword
} Keys;

typedef std::vector<Keys> KeysList;
// make a keys
Keys MakeKeys(std::string fileName, std::string keywords) {
	std::stringstream ss;
	ss << fileName;
	unsigned int nFileName;
	ss >> nFileName;
	Keys one;
	one.kw = keywords;
	one.n = 0;
	one.op = OperateType(nFileName);
	return one;
}

// read keywords from files, initialize structure Keys with keyword and 0
int InitKeys(std::string fileName, const std::string splitString, KeysList& keysList) {

	std::fstream fsRead;
	fsRead.open(fileName.c_str(), std::fstream::in);
	if (!fsRead) {
		return 10;
	}
	
	keysList.clear();

	// read all contents of file once
	std::stringstream ssBuffer;
	ssBuffer << fsRead.rdbuf();
	std::string fileContent(ssBuffer.str());

	fsRead.close();

	std::vector<std::string> vFunctions;
	vFunctions.clear();
	boost::algorithm::split(vFunctions, fileContent, boost::algorithm::is_any_of(splitString.c_str()));

	std::vector<std::string>::iterator it;
	for (it = vFunctions.begin(); it != vFunctions.end(); ++it) {
		keysList.push_back(MakeKeys(fileName, *it));
	}

	return 0;
}

// count occorrences of word in content
size_t CountTimes(std::string word, std::string content) {
	boost::iterator_range<std::string::iterator> iter_rg;
	size_t n = 0;
	while(1) {
		iter_rg = boost::ifind_first(content, word);
		std::string str(iter_rg.end(), content.end());
		if (iter_rg.empty()) break;
		content = str;
		++n;
	}
	return n;
}

// remove comments of php file, bRmSharp = true
std::string RemoveCTypeComments(const std::string& fileName, const bool bRmSharp /* = false */)
{
	typedef enum 
	{
		LITERAL,  	// ÆÕÍ¨×Ö·û
		SINGLE,		// µ¥ÐÐ×¢ÊÍ
		MULTIPLE,	// ¶àÐÐ×¢ÊÍ
		STRING		// ×Ö·û´®ÖÐµÄ×¢ÊÍ·û
	} CommentMode;

	CommentMode mode = LITERAL;

	char cLast = 0, cCurrent;

	std::stringstream ss;
	std::string strRet;

	std::fstream infile;
	infile.open(fileName.c_str(), std::ios::in);
	if (!infile)
	{
		return strRet;
	}


	while ((cCurrent = infile.get()) != EOF) 
	{
		switch (mode) 
		{
		case SINGLE: 
			{
				if (cLast != '\\' && (cCurrent == '\n' || cCurrent == '\r')) 
				{
					ss << cCurrent;
					cCurrent = 0;
					mode = LITERAL;
				}
			} break;

		case MULTIPLE: 
			{
				if (cLast == '*' && cCurrent == '/') 
				{
					cCurrent = 0;
					mode = LITERAL;
				}
			} break;

		case STRING: 
			{
				if (cLast == '\\') 
				{
					ss << cLast;
					ss << cCurrent;
				} 
				else if (cCurrent != '\\') 
				{
					ss << cCurrent;
					if (cCurrent == '"') 
					{
						mode = LITERAL;
					}
				}
			} break;

		default: 
			{
				if (cLast == '/') 
				{
					if (cCurrent == '/') 
					{
						mode = SINGLE;
					} 
					else if (cCurrent == '*') 
					{
						mode = MULTIPLE;
					} 
					else 
					{
						ss << cLast;
						ss << cCurrent;
					}
				} 
				else if (bRmSharp && cCurrent == '#')
				{
					mode = SINGLE;
				}
				else if (cCurrent != '/') 
				{
					ss << cCurrent;
					if (cCurrent == '"') 
					{
						mode = STRING;
					}
				}
			} break;
		}
		cLast = cCurrent;
	}

	strRet = ss.str();
	return strRet;
}

// check each keyword in KeysList, count exists in file
int CountOccurences(KeysList kList, std::string sFileContent) {
	if (sFileContent.empty()) {
		return 11;
	}

	KeysList::iterator it;
	for (it = kList.begin(); it != kList.end(); ++it) {
		size_t n = CountTimes((*it).kw, sFileContent);
		(*it).n = n;
	}
	return 0;
}

int main(int argc, char **argv) {

	getchar();
	return 0;
}