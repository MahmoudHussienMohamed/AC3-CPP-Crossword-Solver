#ifndef __VARIABLES__
#define __VARIABLES__
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
using std::string; 
using std::vector;
using std::to_string;
#define DEBUG(x) (std::cout << #x << " = " << x << '\n')
#define BETWEEN(L, x, R) ((x) >= (L) && (x) < R)
#define abs(x) (int64_t(x) < 0? -int64_t(x) : (x)) 
#define STRINGS vector<string>
#define VARIABLES vector<VARIABLE>
#define VARIABLES_ITERATORS vector<VARIABLES::iterator>
enum DIRECTION{ACROSS, DOWN};
struct CELL{ 
    size_t x, y; 
    CELL(size_t x_ = SIZE_MAX, size_t y_ = SIZE_MAX): x(x_), y(y_){}
    inline bool is_empty(){ return x == SIZE_MAX && y == SIZE_MAX; }
    inline bool operator==(const CELL &c){ return x == c.x && y == c.y; }
    inline string as_string(){ return "(" + to_string(x) + ", " + to_string(y) + ")"; }
};
class VARIABLE
{
    CELL start;
    size_t length;
    DIRECTION dir;
    string word;
public:
    STRINGS domain;
    VARIABLE(){}
    VARIABLE(CELL st, DIRECTION d, size_t len): length(len), dir(d), start(st){}
    void apply(STRINGS &crossword){
        for(size_t i{}, down{(dir == DOWN)}, across{!down}; i < length; i++)
            crossword[start.x +  i * down][start.y + i * across] = word[i];
    }
    inline bool can_fit(const string w){ return this->length == w.length(); }
    inline void setword(string &w){ word = w; }
    inline void resetword(){ word.clear(); }
    inline bool isnt_assigned(){ return !word.length(); }
    inline bool has_same_word(const VARIABLE &var){ return word == var.word; }
    inline string as_string(){ return start.as_string() + ", " +to_string(length) + ", " + (dir == DOWN? "DOWN" : "ACROSS"); }
    inline bool in_agreement(VARIABLE &var){
        CELL overlap = overlapping(var);
        return overlap.is_empty()? true : word[overlap.x] == var.word[overlap.y];
    }
    inline bool is_overlapped_with(const VARIABLE &var){
        return (dir != var.dir) 
        &&     ((dir == DOWN && BETWEEN(this->start.x, var.start.x, this->start.x + this->length) 
                             && BETWEEN(var.start.y, this->start.y, var.start.y + var.length))
            ||  (dir == ACROSS && BETWEEN(this->start.y, var.start.y, this->start.y + this->length)
                               && BETWEEN(var.start.x, this->start.x, var.start.x + var.length)));
    }
    inline CELL overlapping(VARIABLE &var){
        CELL overlap;
        if(is_overlapped_with(var))
            overlap = (dir == DOWN?
                      CELL(var.start.x - start.x, this->start.y - var.start.y)
                    : CELL(var.start.y - this->start.y, this->start.x - var.start.x));
        return overlap;
    }
};
#endif
