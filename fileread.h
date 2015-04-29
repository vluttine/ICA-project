#ifndef FILEREAD_H_INCLUDED
#define FILEREAD_H_INCLUDED
#include <vector>
#include <string>

using namespace std;

class FileReader
{
public:
	//Constructor
	FileReader();
	
	//Getter
	vector <string> getFileList() const;
	
	//Destructor
	~FileReader();
	
};

#endif