#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
bool LoadDB(char *fName, vector<string> &db, string &firstLine) {
    ifstream input(fName);
    if (input) {
        cout << "Reading data from " + string(fName) + " ..\n";
        getline(input, firstLine);
        while (!input.eof()) {
            string line;
            getline(input, line);
            if (line[line.length() - 1] == '\r') line.erase(line.length() - 1);
            if (line != "") db.push_back(line);
        }
        cout << "Done reading data!\n";
        cout << "Data Before Storing: " << db.size() << endl;
        input.close();
        return true;
    }
    else
        return false;
}
bool RandomStore(string firstLine, vector<string> &db, char *fileName, int size) {
    ofstream output(fileName, ios::trunc);
    if (output) {
        cout << "Mixing and Storing data to " + string(fileName) + " ..\n";
        output << firstLine + "\n";
        int          count = 0;
        stringstream out;
        while (count <= db.size() - 1) {
            int r_index = rand() % db.size();  // from 0 - (size -1)
            if (db[r_index] == "") continue;
            out << db[r_index] + "\n";
            db[r_index] = "";
            count++;
        }
        output.write(out.str().c_str(), out.str().length());
        cout << "Done mixing and storing!\n";
        cout << "Data After Storing: " << count << endl;
        if (size != count) { cout << "Some line is missing :(\n"; }
        else
            cout << "File's good, enjoy :)\n";
        output.close();
        return true;
    }
    else
        return false;
}
int main(int narg, char **argv) {
    srand(time(NULL));
    vector<string> db;
    string         first;
    if (LoadDB(argv[1], db, first) && RandomStore(first, db, argv[2], db.size())) {
        cout << "Operation completed!\n";
    }
    else
        cout << "Operation failed!\n";
    db.erase(db.begin(), db.end());
    return 0;
}