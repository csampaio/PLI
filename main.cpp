#include "headers/Interpreter.h"
#include "headers/BranchBound.h"
#include "headers/Exception.h"

using namespace std;

int main() {

	BranchBound *bb = NULL;
	Interpreter *interpreter = NULL;

	try {

        interpreter = new Interpreter("input.txt");

        bb = new BranchBound(interpreter->getProblem(), interpreter->getMode());

		if (bb->hasSolution()) {
			cout << "O valor otimizado e: " << bb->getOptimum() << endl;
			cout << "A solucao e: " << bb->getSolution().transpose() << endl;
		} else {
			cout << "PLI sem solucao." << endl;
		}

	} catch (Exception *ex) {
		ex->print();
	}

	delete bb;
	delete interpreter;

	return 0;
}

