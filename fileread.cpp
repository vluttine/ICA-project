#include <ftw.h>
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fileread.h>
#include <algorithm>

using namespace std;
int i=0;
vector <string> temp;

static int callback(const char *fpath, const struct stat *sb, int typeflag) {
		/* if it's a file */
		if (typeflag == FTW_F) {
				/* if the filename matches the filetype, */
				if (fnmatch("*.wav", fpath, FNM_CASEFOLD) == 0) {
					i +=1;
					string str(fpath);
					temp.push_back(str.erase(0,2));	
				}
		}
		return 0;
}

FileReader::FileReader(){}

FileReader::~FileReader(){}

vector <string> FileReader::getFileList() const {	
	ftw("./input/", callback, 1);
	sort(temp.begin(), temp.end());
	return temp;
}