#include "crossword.h"
#include <fstream>
using namespace std;
#define input(msg, var){ cout << msg; cin >> var;}
STRINGS load_words(string file_name){
    ifstream words_file(file_name);
    STRINGS words;
    string buffer;
    while(getline(words_file, buffer))
        words.push_back(buffer);
    words_file.close();
    return words;
}
// template<typename type> void input(string msg, type& var){ cout << msg; cin >> var; }
int main(int argc, char **argv)
{
    string crossword_filename, words_filename;
    size_t height;
    if(argc == 1){
        input("Crossword height = ", height);
        input("Crossword text file: ", crossword_filename);
        input("Words' text file: ", words_filename);
    }
    else if(argc == 4){
        height = stoi(argv[1]);
        crossword_filename = argv[2];
        words_filename = argv[3];
    }
    else{
        cout << "Usage: [crossword.exe] [heigth] [crossword.txt] [words.txt]\n";
        return 0;
    }
    crossword cw('-', '#', height);
    ifstream cwfile(crossword_filename);
    cwfile >> cw;
    cwfile.close();
    auto domain = load_words(words_filename);
    cw.set_domain(domain);
    cw.solve();
    cout << cw;
    return 0;
}