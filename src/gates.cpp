#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <set>
#include <map>
#include <valarray>
#include <cmath>
#include <vector>

#include "gates.h"
#include "ui.h"

using namespace std;


vector<string> split(const string str, const string delim)
{
    vector<string> v;

    set<char> d;
    for (char c : delim) d.insert(c);

    string current {};
    for (char c : str)
    {
        if (d.find(c) != d.end())
        {
            v.push_back(current);
            current = "";
        }
        else
        {
            current += c;
        }
    }
    v.push_back(current);

    return v;
}

using Input = string;
struct TermInput
{
    Input name;
    int ypos;
    bool neg;
};
using Term = vector<TermInput>;
using Expr = vector<Term>;

#ifdef CP437
constexpr char VERT    = '\xB3';
constexpr char HORIZ   = '\xC4';
constexpr char RBRANCH = '\xC3';
constexpr char LUP     = '\xD9';
constexpr char LDOWN   = '\xBF';
constexpr char RUP     = '\xC0';
constexpr char RDOWN   = '\xDA';
#else
constexpr char VERT    = '|';
constexpr char HORIZ   = '-';
constexpr char RBRANCH = '}';
constexpr char LUP     = '\'';
constexpr char LDOWN   = '.';
constexpr char RUP     = '`';
constexpr char RDOWN   = ',';
#endif

string repeat(string s, int n)
{
    stringstream ss;
    for (int i = 0; i < n; i++)
    {
        ss << s;
    }

    return ss.str();
}
string repeat(char c, int n)
{
    stringstream ss;
    for (int i = 0; i < n; i++)
    {
        ss << c;
    }

    return ss.str();
}

enum class Direction
{
    RIGHT,
    DOWN
};

void matrixPut(char *matrix, int h, int w, int x, int y, const string s, Direction dir)
{
    if (x >= w || y >= h)
    {
        stringstream ss;
        ss << "matrixPut: point (" << x << ", " << y << ") out of matrix bounds!";
        throw new out_of_range(ss.str());
    }

    char *ins = matrix + (y * w) + x;
    char *end = matrix + (h * w);
    for (char c : s)
    {
        if (ins >= end) break;
        *ins = c;

        if (dir==Direction::RIGHT)
        {
            ins++;
        }
        else
        {
            ins += w;
        }
    }
}

void matrixPrint(Panel *pan, char *matrix, int h, int w)
{
    for (int i = 0; i < (h * w); i++)
    {
        /*
        cout << matrix[i];
        if ((i+1)%w == 0) cout << endl;
        */

        wprintw(pan->buf, "%c", matrix[i]);
        if ((i+1)%w == 0){
            wprintw(pan->buf, "\n");
        }
    }
}

void printCircuit(Panel *pan, char *str)
{
    menu_CircuitWindow(pan); 

    string expression(str);

    expression.erase(remove(expression.begin(), expression.end(), ' '), expression.end());

    Expr expr {};
    auto inputs = set<Input>();
    auto termInputs = multimap<Input, TermInput>();

    int inputYPos = 0;

    auto e = split(expression, "+");
    for (string term : e)
    {
        Term currTerm {};

        auto t = split(term, "*");
        for (string in : t)
        {
            bool neg = false;
            if (in.back() == '\'')
            {
                in.pop_back();
                neg = true;
            }

            Input currInput {in};
            inputs.insert(currInput);

            TermInput ti {currInput, inputYPos, neg};
            currTerm.push_back(ti);

            termInputs.insert(pair<Input, TermInput>{in, ti});
            inputYPos++;
        }

        expr.push_back(currTerm);
        inputYPos++;
    }


    int height = inputs.size() + termInputs.size() + expr.size() - 1;

    int width = 0;

    width += max_element(inputs.begin(), inputs.end(), [](Input a, Input b){ return a.length() < b.length(); })->length() + 2;
    int inputWidth = width;
    width += inputs.size()*2;

    int notAndStart = width;

    width += 7;
    width += expr.size()+1;

    int orStart = width;

    width += 4;

    width += 4;

    char matrix[height * width];
    for (int i = 0; i < height*width; i++) matrix[i] = ' ';
    
    int  baseY = inputs.size();

    {
        int x = width - 4;
        int y = baseY + ((height-baseY) / 2);
        matrixPut(matrix, height, width, x, y, repeat(HORIZ, 4), Direction::RIGHT);
    }

    {
        int x = orStart;
        int y = baseY + (height-baseY)/2 - expr.size()/2;

        if (expr.size() == 1)
        {
            matrixPut(matrix, height, width, x, y, repeat(HORIZ, 4), Direction::RIGHT);
        }
        else {
            matrixPut(matrix, height, width, x  , y, repeat(HORIZ, expr.size()), Direction::DOWN);
            matrixPut(matrix, height, width, x+1, y, repeat(VERT , expr.size()), Direction::DOWN);
            matrixPut(matrix, height, width, x+2, y, repeat('O'  , expr.size()), Direction::DOWN);
            matrixPut(matrix, height, width, x+3, y, repeat('R'  , expr.size()), Direction::DOWN);
        }
    }

    {
        int x = notAndStart;
        int y = baseY;
        for (Term t : expr)
        {
            string layer[7];
            for (TermInput ti : t)
            {
                layer[0] += HORIZ;
                layer[1] += (ti.neg ? '>' : HORIZ);
                layer[2] += (ti.neg ? 'o' : HORIZ);
                layer[3] += HORIZ;
                layer[4] += (t.size() > 1 ? VERT : HORIZ);
                layer[5] += (t.size() > 1 ? '&' : HORIZ);
                layer[6] += (t.size() > 1 ? '&' : HORIZ);
            }

            for (int i = 0; i < 7; i++)
            {
                matrixPut(matrix, height, width, x+i, y, layer[i], Direction::DOWN);
            }

            y += t.size() + 1;
        }
    }

    {
        int x = 0;
        int y = 0;

        for (Input i : inputs)
        {
            string str = i + ": ";
            str += repeat(HORIZ, inputWidth - str.length());

            matrixPut(matrix, height, width, x, y, str, Direction::RIGHT);
            
            y++;
        }
    }

    {
        int x = inputWidth;
        int num = 0;
        int offset = inputs.size() * 2 - 1;

        for (Input in : inputs)
        {
            int y = 0;
            int len = inputs.size() * 2 - 1;
            auto it = inputs.find(in);
            while (it != inputs.begin())
            {
                it--;
                y++;
                len -= 2;
            }
            matrixPut(matrix, height, width, x, y, repeat(HORIZ, len) + LDOWN, Direction::RIGHT);
        }

        for (Input in : inputs)
        {
            auto yBreakRange = termInputs.equal_range(in);
            valarray<int> yBreaks(termInputs.count(in));
            int idx = 0;
            for (auto ti = yBreakRange.first; ti != yBreakRange.second; ti++)
            {
                yBreaks[idx++] = baseY + ti->second.ypos;
            }

            for (int y : yBreaks)
            {
                matrixPut(matrix, height, width, x+offset, y, repeat(HORIZ, inputs.size()*2 - offset), Direction::RIGHT);
            }

            int y = num + 1;
            int maxBreak = yBreaks.max();
            string branch = repeat(VERT, maxBreak - y + 1);

            for (int i : yBreaks)
            {
                branch[i-y] = RBRANCH;
            }
            branch.back() = RUP;

            matrixPut(matrix, height, width, x+offset, y, branch, Direction::DOWN);

            offset -= 2;
            num++;
        }
    }
    
    {
        int x = notAndStart + 7;
        int outY = baseY + (height-baseY)/2 - expr.size()/2;
        int breakx = expr.size()/2;

        for (Term t : expr)
        {
            int inY = baseY + (t.front().ypos + t.back().ypos) / 2;
            
            matrixPut(matrix, height, width, x, inY, repeat(HORIZ, abs(breakx)+1), Direction::RIGHT);
            
            matrixPut(matrix, height, width, x+abs(breakx)+1, outY, repeat(HORIZ, expr.size()-abs(breakx)), Direction::RIGHT);
            
            if (inY < outY)
            {
                string branch = LDOWN + repeat(VERT, outY-inY-1) + RUP;
                matrixPut(matrix, height, width, x+abs(breakx)+1, inY, branch, Direction::DOWN);
            }
            else if (inY > outY)
            {
                string branch = RDOWN + repeat(VERT, inY-outY-1) + LUP;
                matrixPut(matrix, height, width, x+abs(breakx)+1, outY, branch, Direction::DOWN);
            }
            
            outY++;
            breakx--;
        }
    }

    //cout << endl;
    wprintw(pan->buf, "\n");
    matrixPrint(pan, matrix, height, width);
    //cout << endl;
    wprintw(pan->buf, "\n");
}
