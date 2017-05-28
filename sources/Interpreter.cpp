#include "../headers/Interpreter.h"
#include "../headers/Exception.h"
#include <Eigen>
#include <algorithm>
#include <cctype>
#include <unordered_map>


using namespace Eigen;

/**
 * Construtor
 *
 * @param string fileName nome do arquivo a ser lido a entrada
 * @returns Interpreter
 */
 Interpreter::Interpreter(const string fileName) {
    long long lineNumber = 1;
    string line;

    //abre o arquivo
    this->in.open(fileName.c_str());

	if(!this->in.is_open()) {
        throw(new Exception("Interpreter: N�o foi poss�vel abrir o arquivo de entrada!"));
	}

	//l� a primeira linha
	getline(this->in,line);

	//interpreta a primeira linha que cont�m a fun��o objetivo e modo
    this->getObjectiveAndMode(line);

    //l� o restante das linhas contendo as restri��es
	while(getline(in,line)) {
        //realoca a matriz de restri��o e vetor de rela��o para receber a nova restri��o
        this->constraints.conservativeResize(lineNumber,this->objectiveFunction.rows()+1);
        this->constraints.row(this->constraints.rows()-1) = VectorXd::Zero(this->objectiveFunction.rows()+1);
        this->relations.conservativeResize(lineNumber);
        //interpreta a linha e busca a restri��o
        this->getConstraint(line);
        lineNumber++;
	}

	this->pli = new Problem(this->objectiveFunction, this->constraints, this->relations);

	this->in.close();

 }

 /**
 * M�todo para encontrar fun��o objetivo e modo na string passada como par�metro
 *
 * @param string line a primeira linha do arquivo contendo a fun��o objetiva e o modo
 * @returns void
 */
void Interpreter::getObjectiveAndMode(string line) {
    string temp;
    long long counter, i, variable;
    unordered_map<long long, long long> variables;
    char op = '+';
    bool waitDigit = true;
    bool waitX = true;
    bool waitOperator = false;
    bool waitPos = false;

    //remove espa�os
	line.erase(std::remove(line.begin(),line.end(),' '),line.end());

	temp = line.substr(0,3);

    //transforma para maiusculo
	transform(temp.begin(), temp.end(), temp.begin(), ::toupper);

	if(temp == "MAX") {
        this->mode = 2;
	} else if(temp == "MIN") {
        this->mode = 1;
	} else {
	    throw(new Exception("Interpreter: N�o encontrado modo (Maximizar ou Minimizar) da fun��o objetivo!"));
	}

    //come�a da posi��o 6, pois se assume as 5 primeiras sendo MINZ= ou MAXZ=
	counter = 5;
	variable = 1;

	if(line[counter]=='-') {
        op = '-';
        counter++;
        variable = -variable;
	}

	for (i=counter;i<line.length();i++){
	    temp = line.substr(counter,i-counter+1);
        if(waitDigit && isdigit(line[i]) && !isdigit(line[i+1])) {
            variable = atoll(temp.c_str());
            counter=i+1;
            waitDigit = false;
            continue;
        } else if(waitX && toupper(line[i])=='X') {
            counter=i+1;
            waitX = false;
            waitDigit = false;
            waitPos = true;
            continue;
        } else if(i+1==line.length() || (waitPos && isdigit(line[i]) && !isdigit(line[i+1]))) {
            if(op == '-') {
               variable = -variable;
            }
            variables[atoll(temp.c_str())] = variable;
            variable = 1;
            counter=i+1;
            waitOperator = true;
            waitPos=false;
            continue;
        } else if(waitOperator && (line[i]=='+' || line[i]=='-')) {
            op = line[i];
            counter=i+1;
            waitOperator = false;
            waitDigit=true;
            waitX = true;
        }
	}

	this->objectiveFunction.resize(variables.size());

	for(i=0; i<this->objectiveFunction.rows(); i++) {
        this->objectiveFunction(i) = variables[i+1];
	}

}

 /**
 * M�todo para buscar uma restri��o da PLI a partir da string passada
 *
 * @param string line linha do arquivo contendo a restri��o
 * @returns void
 */
void Interpreter::getConstraint(string line) {
    string temp;
    long long counter, i, variable;
    unordered_map<long long, long long> variables;
    char op = '+';
    int relation;
    bool waitDigit = true;
    bool waitX = true;
    bool waitOperator = false;
    bool waitPos = false;

    //remove espa�os
	line.erase(std::remove(line.begin(),line.end(),' '),line.end());

	counter = 0;
	variable = 1;

	if(line[counter]=='-') {
        op = '-';
        counter++;
        variable = -variable;
	}

	for (i=counter;i<line.length();i++){
	    temp = line.substr(counter,i-counter+1);
        if(waitDigit && isdigit(line[i]) && !isdigit(line[i+1])) {
            variable = atoll(temp.c_str());
            counter=i+1;
            waitDigit = false;
            continue;
        } else if(waitX && toupper(line[i])=='X') {
            counter=i+1;
            waitX = false;
            waitDigit = false;
            waitPos = true;
            continue;
        } else if(waitPos && isdigit(line[i]) && !isdigit(line[i+1])) {
            if(op == '-') {
               variable = -variable;
            }
            variables[atoll(temp.c_str())] = variable;
            variable = 1;
            counter=i+1;
            waitOperator = true;
            waitPos=false;
            continue;
        } else if(waitOperator && (line[i]=='+' || line[i]=='-')) {
            op = line[i];
            counter=i+1;
            waitOperator = false;
            waitDigit=true;
            waitX = true;
            continue;
        } else if(line[i]=='>' || line[i]=='=' || line[i]=='<') {
            switch(line[i]) {
                case '>':
                    relation = 1;
                    break;
                case '=':
                    relation = 2;
                    break;
                case '<':
                    relation = 0;
                    break;
            }
            if(line[i+1] == '=')
                i++;
            if(line[i+2] == '-') {
                op = '-';
                i++;
            } else {
                op = '+';
            }
            counter=i+1;
            waitDigit=false;
            waitPos=false;
            continue;
        } else if(i+1==line.length()) {
            variable = atoll(temp.c_str());
            if(op == '-') {
               variable = -variable;
            }
            variables[this->objectiveFunction.rows()+1] = variable;
        }
	}

	for(i=0; i<this->constraints.cols(); i++) {
        if(variables.find(i+1) != variables.end()) {
            this->constraints(this->constraints.rows()-1, i) = variables[i+1];
        }
	}

	this->relations(this->relations.rows()-1) = relation;

}

/**
 * M�todo para retornar o problema encontrado
 *
 * @returns Problem
 */
Problem* Interpreter::getProblem() {
    return this->pli;
}

/**
 * M�todo para retornar o modo (Maximiza��o ou Minimiza��o)
 *
 * @returns int
 */
int Interpreter::getMode() {
    return this->mode;
}
