#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <stack>
#include <map>
#include <set>
#include <assert.h>


using namespace std;
  
 
// g++ -std=c++17 main.cpp -o main && ./main


//////////

string expr = "a|~a|b|~b"; // example expression 
string simplified_expr = "a|~a"; // simplified expression

//////////


/* 
since it is prohibited to use c++ "built in functions except for basic math operations" as written in project notes,
which likely refers to next_permutation, set intersection and difference functions specifically
the first part will be about quickly implementing two of these neccessary functions from scratch for use 
*/

void extract_intersection(set<char>& s1, set<char>& s2, vector<char>& v) {
    for(auto& e : s1) {
        if(s2.find(e) != s2.end()) v.push_back(e);
    }
}

void extract_difference(set<char>& s1, set<char>& s2, vector<char>& v) {
    for(auto& e : s1) {
        if(s2.find(e) == s2.end()) v.push_back(e);
    }
}

/* **** core project code **** */

string to_postfix(string infix) {
    string postfix = "";
    map<char, int> priority = {{'~', 2}, {'&', 1}, {'|', 0}};
    stack<char> s;

    for(auto c : infix) {
        
        if(isalpha(c)) {
            postfix += c;
        }else {
            if(s.empty()) {
                s.push(c);
                continue;
            }

            if(c == ')') {
                while(s.top() != '(') {
                    postfix += s.top();
                    s.pop();
                }
                s.pop();

            }else {
                while(c != '(' && !s.empty() && priority[s.top()] > priority[c]) {
                    postfix += s.top();
                    s.pop();
                }
                s.push(c);
            }
            
        }
    }

    while(!s.empty()) {
        postfix += s.top();
        s.pop();
    }

    return postfix;

}


class LogicExpression {
    
    private:
        vector<vector<bool>> permutations;
        vector<bool> outputs;
        vector<char> variables;
        map<char, int> var_map;

        string expression;

        void clear() {

            permutations.clear();
            outputs.clear();
            variables.clear();
            var_map.clear();
        }

        void set_variables(vector<char> vars) {
            variables = vars;

            for(int i=0;i < variables.size();i++) {
                var_map[variables[i]] = i;
            }
        }

        bool bool_evaluateExpr(vector<bool>& state) { 
            stack<bool> s;
            
            for(auto& e : expression) {
                if(isalpha(e)) {
                    s.push(state[var_map[e]]);
                }else {

                    bool a = s.top();s.pop();

                    if(e == '~') {
                        s.push(!a);
                        continue;
                    }

                    bool b = s.top();s.pop();
                    s.push(bool_evaluate(a, b, e));
                }
            }

            return s.top();
        }

        void make_permutations(vector<bool>& state, int i=0) {

            if(i == variables.size()) {
                permutations.push_back(state);
                return;
            }

            state[i] = 0;
            make_permutations(state, i+1);
            state[i] = 1;
            make_permutations(state, i+1);
        }

        void evaluate_permutations() { 
            outputs.clear();
            for(auto& p : permutations) {
                outputs.push_back(bool_evaluateExpr(p));
            }
        }

    public:

        static bool bool_evaluate(bool a, bool b, char op) {
            if(op == '|') return a|b;
            if(op == '&') return a&b;

            throw invalid_argument( "invalid logic operator: " + string(1, op));
        }

        LogicExpression(string expr) {
            set_expression(expr);
            
        }

        void set_expression(string expr, bool total_reset=true) {
            // @param total_reset when false it assumes that no change happened to the expression variables
            expression = to_postfix(expr);
            if(total_reset) {
                clear();
                set<char> vars;
                for(int i=0;i<expr.size();i++) {
                    if(isalpha(expr[i])) vars.insert(expr[i]);
                }

                set_variables(vector(vars.begin(), vars.end()));
                vector<bool> initial_state(variables.size());
                make_permutations(initial_state);
            }
            
            evaluate_permutations();

        }

        void printTruthTable(bool satisfiable_only=false) {
    
            cout << "\n";
            for(auto& var : variables) {
                cout << var << " | ";
            }
            cout << "Expr \n";

            for(int i=0;i< 4*variables.size() + 4;i++) cout << '-';
            cout << endl;

            int i=0;
            for(auto& row : permutations) {
                if(satisfiable_only && !outputs[i]) {
                    i++;
                    continue;   
                }

                for(auto val : row) {
                    cout << val << " | ";
                }
                
                cout << outputs[i] << endl;
                i++;

            }

            cout << "\n";
        }

        vector<char> get_variables() {
            return variables;
        }

        pair<vector<vector<bool>>, vector<bool>> getTruthTable() {
            return make_pair(permutations, outputs);
        }

        string get_expression() {
            return expression;
        }

        bool is_tautology() {

            bool ands = 1;
            for(auto o : outputs) {
                ands &= o;
                if(!ands) break;
            }

            return ands;
        }

        bool is_satisfiable() {
            bool ors = 0;
            for(auto o : outputs) {
                ors |= o;
                if(ors) break;
            }

            return ors;
        }

        bool is_otherVariablesRedundant(vector<char> vars) {
            // returns true if all the variables other than those in @param vars are redundant
            set<char> s_set(variables.begin(), variables.end()), s_subset(vars.begin(), vars.end());
            assert(includes(s_set.begin(), s_set.end(), s_subset.begin(), s_subset.end()));

            vector<char> ordered_vars;

            extract_difference(s_set, s_subset, ordered_vars);

            ordered_vars.insert(ordered_vars.end(), vars.begin(), vars.end());
            set_variables(ordered_vars);
            evaluate_permutations();

            bool redundant = true;
            long long boundary = 1 << vars.size(); // 2^n
            for(long long i=0;i<outputs.size();i++) {
                
                redundant = outputs[i] == outputs[i%boundary];
                if(!redundant) return false;
            }

            return true;
        }

        bool operator==(LogicExpression& expr) {
            set<char> expr1_variables = set(variables.begin(), variables.end());
            set<char> expr2_variables = set(expr.variables.begin(), expr.variables.end());
            vector<char> var_common, var1, var2;

            extract_intersection(expr1_variables, expr2_variables, var_common);
            
            if(var_common.empty()) {
                return is_tautology() && expr.is_tautology() || !is_satisfiable() && !expr.is_satisfiable();
            }

            // verify if the values of the unshared variables is redundant
            if(expr1_variables.size() != var_common.size()) {
                if(!is_otherVariablesRedundant(var_common)) return false;
            }
            if(expr2_variables.size() != var_common.size()) {
                if(!expr.is_otherVariablesRedundant(var_common)) return false;
            }

            long long boundary = 1 << var_common.size(); // 2^n
            for(long long i=0; i < boundary;i++) {
                if(outputs[i] != expr.outputs[i]) return false;
            }

            return true;
        }

        friend ostream& operator<<(ostream& os, LogicExpression const& e) {
            return os << e.expression;
        }
};



LogicExpression simplified(simplified_expr), expression(expr);

void step1() {

    cout << "\nlogic Expression: " << expr << endl;
    expression.printTruthTable();
    

    cout << "\nsimplified logic Expression: " << simplified_expr << endl;
    simplified.printTruthTable();

    if(expression == simplified) cout << "* the two expressions are equivalent. \n\n";
    else cout << "* the two expressions are NOT equivalent.\n\n";
    
}

int main() {

    step1();

    // step 2
    cout << "Expression: " << simplified_expr;
    if(simplified.is_satisfiable()) {
        cout << "\n\n* is satisfiable at values: \n";
        simplified.printTruthTable(true);
        
    }else {
        cout << "\n\n* the expression is unsatisfiable \n";
    }

    bool satisfaiable = expression.is_satisfiable(), tautology = expression.is_tautology();

    cout << "\nExpression: " << expr;
    if(satisfaiable) {
        cout << "\n\n* is satisfiable at values: \n";
        expression.printTruthTable(true);
        
    }else {
        cout << "\n\n* the expression is unsatisfiable \n";
    }

    if(tautology) {
        cout << "* the expression is tautology.\n";
    }

    if(tautology || !satisfaiable) {
        map<char, char> gate_invert = {{'&', '|'}, {'|', '&'}};
        
        for(int i=0;i<expr.size();i++) {
            if(expr[i] == '|' || expr[i] == '&') {
                
                char temp = expr[i];
                expr[i] = gate_invert[expr[i]];
                expression.set_expression(expr, false);

                if(expression.is_satisfiable()) {
                    cout << "\nexpression becomes satisfiable by changing\n\n";
                    swap(expr[i], temp);
                    cout << "from: " << expr << endl;
                    swap(expr[i], temp);
                    cout << "to:   " << expr << endl << "      ";
                    
                    for(int j=0;j<i;j++) {
                        cout << ' ';
                    }
                    cout << "^\n";

                    step1();
                    return 0;
                }
                
            }
        }

    }
    return 0;
}