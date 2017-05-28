#pragma once

#include <Eigen>

using namespace Eigen;

#define SIMPLEX_MINIMIZE 1
#define SIMPLEX_MAXIMIZE 2

#define FIRST_PHASE 1
#define SECOND_PHASE 2

class Simplex {
    private:
        MatrixXd tableau;
        bool foundSolution;
        double optimum;
        VectorXd solution;
        long long numberOfVariables;

        bool simplexSolver(long long  variableNum, int mode, int phase);
        long long findPivot_min(long long column, int phase);
        long long getPivotRow(long long column);
        void removeRow(long long rowToRemove);
        void removeColumn(long long colToRemove);

    protected:

    public:
        Simplex(int mode, const VectorXd &objectiveFunction, const MatrixXd &constraints, const VectorXd &relations);
        bool hasSolution();
        double getOptimum();
        VectorXd getSolution();
};
