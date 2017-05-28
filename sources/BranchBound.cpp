#include "../headers/BranchBound.h"
#include <Eigen>
#include <cmath>

using namespace Eigen;

/**
 * Construtor
 *
 * @param Problem *pli problema de Programação Linear Inteira a ser resolvido pelo branch-and-bound.
 * @param int mode Pode ser: MINIMIZE, MAXIMIZE
 */
BranchBound::BranchBound(Problem *pli, int mode) {
    this->mode = mode;
    this->root = new Node();
    this->root->pli = pli;
    this->optimum = 0;
    this->findSolutions(root);
}

/**
 * Busca por todas as soluções
 *
 * @param Node node contém o problema a ser resolvido.
 * @returns void
 */
void BranchBound::findSolutions(Node *node) {
    node->solver = new Simplex(this->mode, node->pli->getObjectiveFunction(), node->pli->getConstraints(), node->pli->getRelations());
    //verifica se o problema possui solução e se ela é melhor que a atual
    if(node->solver->hasSolution() && this->isBetterSolution(node->solver->getOptimum())) {
        //verifica se a solução possui números reais
        //escolher método para escolha do branch
        long long pos = this->findBranch(node->solver->getSolution());
        if(pos != -1) {
            double intPart, temp;
            VectorXd newConstraint;

            //busca a parte fracional do número
            temp = node->solver->getSolution()(pos);
            modf(temp, &intPart);

            node->left = new Node();
            node->left->pli = new Problem(node->pli->getObjectiveFunction(), node->pli->getConstraints(), node->pli->getRelations());
            //cria uma nova restrição baseado no limite inferior
            newConstraint = VectorXd::Zero(node->pli->getConstraints().cols());
            newConstraint(pos) = 1;
            newConstraint(newConstraint.rows()-1) = intPart;
            node->left->pli->addConstraint(newConstraint, 0);
            this->findSolutions(node->left);

            node->right = new Node();
            node->right->pli = new Problem(node->pli->getObjectiveFunction(), node->pli->getConstraints(), node->pli->getRelations());
            //cria uma nova restrição baseado no limite superior
            newConstraint(newConstraint.rows()-1)++;
            node->right->pli->addConstraint(newConstraint, 1);
            this->findSolutions(node->right);
        } else {
            //verifica se a solução econtrada é melhor que a atual
            if(node->solver->getOptimum() > this->optimum) {
                this->foundSolution = true;
                this->optimum = node->solver->getOptimum();
                this->solution = node->solver->getSolution();
            }
        }
    }
}

/**
 * Busca por um número Real para ramificar
 * Foi utilizado a técnica de Variante de Dakin
 *
 * @param VectorXd vectorToSearch vetor ao qual a busca será realizada
 * @returns __int64 Retorna o índice da coluna ou -1 se não achou.
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
 * Verifica se a solução atual é melhor
 *
 * @param double Valor a ser verificado
 * @returns bool true se for melhor e false se não
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
 * Retorna true se a solução foi encontrada.
 * Retorna false caso contrário.
 *
 * @returns boolean
 */
bool BranchBound::hasSolution() {
	return this->foundSolution;
}


/**
 * Retorna o valor ótimo da função objetivo maximizado ou minimizado com valores inteiros
 *
 * @returns double
 */
double BranchBound::getOptimum() {
	return this->optimum;
}

/**
 * Retorna o valor das variáveis para a solução encontrada.
 *
 * @returns VectorXd
 */
VectorXd BranchBound::getSolution() {
	return this->solution;
}
