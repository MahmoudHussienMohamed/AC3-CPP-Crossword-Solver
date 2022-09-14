#include "crossword.h"
#include <iostream>
#include <algorithm>
using std::sort;
string uppered(string str){
    for(auto &chr : str)
        chr -= 32 * (chr >= 'a' && chr <= 'z');
    return str;
}
void crossword::set_domain(STRINGS &words){
    domain.resize(words.size());
    for(size_t i = 0; i < words.size(); i++)
        domain[i] = uppered(words[i]);    
}
inline void crossword::update(size_t i, size_t j, DIRECTION dir, size_t len){
    variables.push_back({(dir == DOWN? CELL(i, j) : CELL(j, i)), dir, len + 1});
}
void crossword::extract_VARIABLES(STRINGS _board, DIRECTION dir){
    size_t start{SIZE_MAX}, len{};
    for(size_t i = 0; i < _board.size(); i++, start = SIZE_MAX, len = 0){
        string &row = _board[i];
        for(size_t j = 0; j < row.length(); j++){
            if(row[j] == empty_cell){
                if(start != SIZE_MAX) len++;
                else start = j;
            } 
            else if(start != SIZE_MAX){
                if(len > 0) update(start, i, dir, len);
                start = SIZE_MAX, len = 0;
            }
        }
        if(len > 0) update(start, i, dir, len);
    }
}
void crossword::VARIABLE_tokenize(){
    // // Horizontal
    extract_VARIABLES(board, ACROSS);
    // Vertical (Rotate it then do horizontal stuff)
    STRINGS rotated_board(board[0].length(), string(board.size(), ' '));
    for(size_t i = 0; i < board.size(); i++)
        for(size_t j = 0; j < board[i].length(); j++)
            rotated_board[j][i] = board[i][j];
    extract_VARIABLES(rotated_board, DOWN);
}
void crossword::setVARIABLES_domains(){
    for(auto &var : variables)
        for(auto &word : domain)
            if(var.can_fit(word))
                var.domain.push_back(word);
}
bool crossword::revise(VARIABLE &x, VARIABLE &y){
    CELL overlap = x.overlapping(y);
    if(overlap.is_empty()) return false;
    STRINGS newXdomain;
    for(auto &xword : x.domain)
        for(auto &yword : y.domain)
            if(xword[overlap.x] == yword[overlap.y]){
                newXdomain.push_back(xword);
                break;
            }
    bool revised = (newXdomain.size() != x.domain.size());
    x.domain = newXdomain;
    return revised;
}
VARIABLES_ITERATORS crossword::neighbors(VARIABLE &x){
    VARIABLES_ITERATORS Xneighbors;
    for(auto var = variables.begin(); var != variables.end(); var++)
        if(x.is_overlapped_with(*var)) 
            Xneighbors.push_back(var);
    return Xneighbors;
}
bool crossword::AC3(){
    vector<VARIABLES_ITERATORS> overlapped_vars;
    for(auto lvar = variables.begin(); lvar != variables.end(); lvar++)
        for(auto rvar = lvar + 1; rvar != variables.end(); rvar++)
            if(lvar->is_overlapped_with(*rvar))
                overlapped_vars.push_back({lvar, rvar});
        
    VARIABLES::iterator x, y;
    while(!overlapped_vars.size()){
        x = overlapped_vars.back()[0];
        y = overlapped_vars.back()[1];
        overlapped_vars.pop_back();
        if(revise(*x, *y)){
            if(!x->domain.size()) return false;
            auto xneighbors = neighbors(*x);
            for(auto neighbor : xneighbors)
                overlapped_vars.push_back({neighbor, x});
        }   
    }
    return true;
}
bool crossword::is_complete(){
    for(auto var = variables.begin(); var != variables.end(); var++)
        if(var->isnt_assigned()) return false;
    return true;
}
bool crossword::consistent_for_assigned_vars(){
    VARIABLES assigned_vars;
    for(auto var = variables.begin(); var != variables.end(); var++)
        if(!var->isnt_assigned()) assigned_vars.push_back(*var);

    for(auto lvar = assigned_vars.begin(); lvar != assigned_vars.end(); lvar++){
        for(auto rvar = lvar + 1; rvar != assigned_vars.end(); rvar++)
            if(rvar->has_same_word(*lvar) || !lvar->in_agreement(*rvar)) 
                return false;        
    }
    return true;
}
VARIABLE& crossword::select_unassigned_VARIABLE(){
    vector<size_t> unassigend_vars;
    for(size_t i = 0; i < variables.size(); i++)
        if(variables[i].isnt_assigned())
            unassigend_vars.push_back(i);
    size_t selected_i = unassigend_vars[0];
    for(auto i : unassigend_vars){
        if((variables[selected_i].domain.size() > variables[i].domain.size())
        || (variables[selected_i].domain.size() == variables[i].domain.size() 
            && neighbors(variables[selected_i]).size() < neighbors(variables[i]).size()))
            selected_i = i;
    }
    auto &selected = variables[selected_i];
    return selected;
}
bool in_agreement(string &word, STRINGS &domain, CELL &overlap){
    for(auto &dword : domain)
        if(word[overlap.x] == dword[overlap.y]) 
            return true;
    return false;
}
STRINGS crossword::order_domain_values(VARIABLE &var){
    auto var_neighbors = neighbors(var);
    VARIABLES_ITERATORS unassigned_neighbors;
    for(auto &neighbor : var_neighbors)
        if(neighbor->isnt_assigned())
            unassigned_neighbors.push_back(neighbor);
    var_neighbors.clear();
    auto eliminated_values_number = [&](string word){
        size_t cnt{};
        CELL overlap;
        for(auto &neighbor : unassigned_neighbors){
            overlap = var.overlapping(*neighbor);
            cnt += !in_agreement(word, neighbor->domain, overlap);
        }
        return cnt;
    };
    auto compare_fun = [&](string x, string y){ 
        return eliminated_values_number(x) < eliminated_values_number(y); 
    };
    auto domain = var.domain;
    sort(domain.begin(), domain.end(), compare_fun);
    return domain;
}
bool crossword::backtrack(){
    if(is_complete()) return true;
    auto &var = select_unassigned_VARIABLE();
    auto domain = order_domain_values(var);
    for(auto &word : domain){
        var.setword(word);
        if(consistent_for_assigned_vars()){
            bool result = backtrack();
            if(result) return true;
        }
        else var.resetword();
    }
    return false;
}
void crossword::solve(){
    VARIABLE_tokenize();
    setVARIABLES_domains();
    if(AC3() && backtrack())
        for(auto &var : variables)
            var.apply(board);
    else
        std::cout << "Crossword couldn't be solved with the given words! Try another set of words.\n";
}
