#ifndef __CROSSWORD__
#define __CROSSWORD__
#include "VARIABLES.h"
#include <windows.h>
using std::istream; 
using std::ostream;
using std::max;
class crossword{
    char empty_cell, border_cell;
    VARIABLES variables;
    STRINGS board;
    STRINGS domain;
    public:
    crossword(char ecell, char bcell, size_t height): 
    empty_cell(ecell), border_cell(bcell){
        board.resize(height);
    }
    friend istream &operator>>(istream &in, crossword &cw){
        size_t max_width{};
        for(auto &row : cw.board){
            getline(in, row);
            max_width = max(max_width, row.length());
        }
        for(auto &row : cw.board)
            row += string(max_width - row.length(), cw.border_cell);
        return in;
    }
    friend ostream &operator <<(ostream &out, const crossword &cw){
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        // ┌ : -38, ┘ : -39, ┐ : -65, └ : -64, ┴ : -63, ┬ : -62, ┼ : -59, ─ : -60, ├ : -61, ┤ : -76, │ : -77
        SetConsoleTextAttribute(console, 8);
        out << '\n' << format(-38, -62, -65, cw.board[0].size()) << '\n';
        string tmp = format(-61, -59, -76, cw.board[0].size());
        for(size_t i = 0; i < cw.board.size(); i++){
            for(auto &cell : cw.board[i]){
                out << char(-77) << ' ';
                if(cell != cw.border_cell)
                    SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                out << cell << ' ';
                SetConsoleTextAttribute(console, 8);
            }
            out << char(-77) << '\n';
            if(i != cw.board.size() - 1) out << tmp << '\n';
        }
        out << format(-64, -63, -39, cw.board[0].size()) << '\n';
        SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return out;
    }
    void set_domain(STRINGS&);
    void solve();
    private:
    static string format(char first, char sep, char last, size_t len){
        string formated(4 * len + 1, -60);
        formated.front() = first;
        formated.back() = last;
        for(size_t i = 4; i < formated.size() - 4; i += 4) formated[i] = sep;
        return formated;
    }
    bool AC3();
    bool backtrack();
    bool is_complete();
    void VARIABLE_tokenize();
    void setVARIABLES_domains();
    bool revise(VARIABLE&, VARIABLE&);
    bool consistent_for_assigned_vars();
    VARIABLE &select_unassigned_VARIABLE();
    STRINGS order_domain_values(VARIABLE &);
    VARIABLES_ITERATORS neighbors(VARIABLE&);
    void extract_VARIABLES(STRINGS, DIRECTION);
    size_t eliminated_values_number(VARIABLE &var);
    inline void update(size_t, size_t, DIRECTION, size_t);
};
#endif