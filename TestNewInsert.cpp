/*******************************************************************************
File:          TestNewInsert.cpp
Author:        Dandan Lin
Date Created:  04/11/2017

Description:	This program will fill a Priority Queue with entries
from an input file, then search for all entries from a search file.
*******************************************************************************/
#include "BinomialQueue.h"

#include <fstream>
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
	if (argc != 3) {
		cout << "Usage: " << argv[0] << "<input_file_to_create_queuee> <input_file_to_check_search>\n";
		return 0;
	}

	ifstream inFile(argv[1]);
		if(!inFile.good()) {
			cout << "Unable to open file \"" << argv[1] << "\". Please double check. \n";
		return 0;
	}

	ifstream testFile(argv[2]);
		if(!testFile.good()) {
			cout << "Unable to open file \"" << argv[2] << "\". Please double check. \n";
		return 0;
	}

	BinomialQueue<int> priority_queue;
	int input_number, successful_inseart = 0;
	// Test Insert
	while (inFile >> input_number) {
		priority_queue.newInsert(input_number);
		++successful_inseart;
	}

	cout << "Success inserting "<< successful_inseart << " elements into the queue. The minimum element is " << priority_queue.findMin() << endl;

	// Test Search
	int targetNumber;
	while(testFile >> targetNumber){
		if (priority_queue.find_(targetNumber))
			cout << targetNumber << " Found\n";
		else
			cout << targetNumber << " Not Found\n";
	}

	return 0;
}
