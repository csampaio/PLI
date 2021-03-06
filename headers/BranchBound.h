#pragma once

#include <iostream>
#include "Simplex.h"
#include "Problem.h"
#include <Eigen>

#define MINIMIZE 1
#define MAXIMIZE 2

using namespace std;

struct Node {
    Simplex *solver;
	Problem *pli;
	Node *left;
	Node *right;
};

class BranchBound {
    private:
        Node *root;
        int mode;
        bool foundSolution;
        double optimum;
        VectorXd solution;

        long long findBranch(VectorXd vectorToSearch);
        void findSolutions(Node *node);
        bool isBetterSolution(double optimumFound);

    public:
        BranchBound(Problem *pli, int mode);
        bool hasSolution();
        double getOptimum();
        VectorXd getSolution();
};
