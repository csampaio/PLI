#include "headers/Interpreter.h"
#include "headers/BranchBound.h"
#include "headers/Exception.h"

using namespace std;

int main() {

	BranchBound *bb = NULL;
	Interpreter *interpreter = NULL;

	try {

        //interpreter = new Interpreter("input.txt");
        interpreter = new Interpreter("input");

        bb = new BranchBound(interpreter->getProblem(), interpreter->getMode());

		if (bb->hasSolution()) {
			cout << "Valor otimizado: " << bb->getOptimum() << endl;
			cout << "Solucao: [" << bb->getSolution().transpose() << "]"<<endl;
		} else {
			cout << "Solucao nao encontrada" << endl;
		}

	} catch (Exception *ex) {
		ex->print();
	}

	delete bb;
	delete interpreter;

	return 0;
}

