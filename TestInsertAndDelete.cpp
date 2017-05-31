/*******************************************************************************
File:          TestInsertAndDelete.cpp
Author:        Dandan Lin
Date Created:  04/11/2017

Description:  This program will fill a Mutable Priority Queue with entries
				  from an input file, then delete all entries from a delete file.
*******************************************************************************/
#include "BinomialQueue.h"

#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** argv) {
	if (argc != 3) {
		cout << "Usage: " << argv[0] << " <input_file_to_create_queue> <input_file_to_check_deletion>" << endl;
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
	int input_number, target_number, successful_inseart = 0;

	// Test Insert
	while (inFile >> input_number) {
		// Insert the input_numbers to the priority_queue
		priority_queue.insert(input_number);
		++successful_inseart;
	}
	// Check Insertion
	cout << "Success inserting "<< successful_inseart << " elements into the queue. The minimum element is " << priority_queue.findMin() << endl;

	// Testing Delete
	while (testFile >> target_number) {
		if (priority_queue.remove(target_number)) {
			cout << target_number << " Deletion successful – New minimum is " << priority_queue.findMin() << endl;
		} else {
			cout << target_number << " Serious problem with deletion – New minimum is " << priority_queue.findMin() << endl;
      }
	}



	return 0;
}
