#include "../headers/BranchBound.h"
#include <Eigen>
#include <cmath>
#include <limits>

using namespace Eigen;

/**
 * @desc Construtor
 *
 * @param Problem *pli problema de Programa��o Linear Inteira a ser resolvido pelo branch-and-bound.
 * @param int mode Pode ser: MINIMIZE, MAXIMIZE
 */
BranchBound::BranchBound(Problem *pli, int mode) {
    this->mode = mode;
    this->root = new Node();
    this->root->pli = pli;
    this->foundSolution = false;
    if(mode == MAXIMIZE) {
        this->optimum = numeric_limits<double>::min();
    } else {
        this->optimum = numeric_limits<double>::max();
    }
    this->findSolutions(root);
}

/**
 * @desc Busca por todas as solu��es
 *
 * @param Node node cont�m o problema a ser resolvido.
 * @returns void
 */
void BranchBound::findSolutions(Node *node) {
    node->solver = new Simplex(this->mode, node->pli->getObjectiveFunction(), node->pli->getConstraints(), node->pli->getRelations());
    //verifica se o problema possui solu��o e se ela � melhor que a atual
    if(node->solver->hasSolution() && this->isBetterSolution(node->solver->getOptimum())) {
        //verifica se a solu��o possui n�meros reais
        //escolher m�todo para escolha do branch
        long long pos = this->findBranch(node->solver->getSolution());
        if(pos != -1) {
            double intPart, temp;
            VectorXd newConstraint;
            //busca a parte fracional do n�mero
            temp = node->solver->getSolution()(pos);
            modf(temp, &intPart);

            node->left = new Node();
            node->left->pli = new Problem(node->pli->getObjectiveFunction(), node->pli->getConstraints(), node->pli->getRelations());
            //cria uma nova restri��o baseado no limite inferior
            newConstraint = VectorXd::Zero(node->pli->getConstraints().cols());
            newConstraint(pos) = 1;
            newConstraint(newConstraint.rows()-1) = intPart;
            node->left->pli->addConstraint(newConstraint, 0);
            this->findSolutions(node->left);

            node->right = new Node();
            node->right->pli = new Problem(node->pli->getObjectiveFunction(), node->pli->getConstraints(), node->pli->getRelations());
            //cria uma nova restri��o baseado no limite superior
            newConstraint(newConstraint.rows()-1)++;
            node->right->pli->addConstraint(newConstraint, 1);
            this->findSolutions(node->right);
        } else {
            this->foundSolution = true;
            this->optimum = node->solver->getOptimum();
            this->solution = node->solver->getSolution();
        }
    }
}

/**
 * @desc Busca por um n�mero Real para ramificar
 * @desc Foi utilizado a t�cnica de Variante de Dakin
 *
 * @param VectorXd vectorToSearch vetor ao qual a busca ser� realizada
 * @returns __int64 Retorna o �ndice da coluna ou -1 se n�o achou.
 */
long long BranchBound::findBranch(VectorXd vectorToSearch) {
    double intPart, floatPart = 0;
    int temp = -1;
    for (long long i = 0; i < vectorToSearch.rows(); i++) {
        if(modf(vectorToSearch(i), &intPart) > floatPart) {
            floatPart = modf(vectorToSearch(i), &intPart);
            temp = i;
        }
    }
    return temp;
}

/**
 * @desc Verifica se a solu��o atual � melhor
 *
 * @param double Valor a ser verificado
 * @returns bool true se for melhor e false se n�o
 */
bool BranchBound::isBetterSolution(double optimumFound) {
    if(this->mode == MAXIMIZE && optimumFound>this->optimum) {
        return true;
    }
    if(this->mode == MINIMIZE && optimumFound<this->optimum){
        return true;
    }
    return false;
}

/**
 * @desc Retorna true se a solu��o foi encontrada.
 * @desc Retorna false caso contr�rio.
 *
 * @returns boolean
 */
bool BranchBound::hasSolution() {
	return this->foundSolution;
}


/**
 * @desc Retorna o valor �timo da fun��o objetivo maximizado ou minimizado com valores inteiros
 *
 * @returns double
 */
double BranchBound::getOptimum() {
	return this->optimum;
}

/**
 * @desc Retorna o valor das vari�veis para a solu��o encontrada.
 *
 * @returns VectorXd
 */
VectorXd BranchBound::getSolution() {
	return this->solution;
}
