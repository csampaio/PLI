#include <iostream>
#include <string>
#include "Problem.h"

using namespace std;

class Interpreter {
    private:
        Problem *pli;
        int mode;

    public:
        Interpreter(string fileName);
        Problem getProblem();
        int getMode();
};
