#include "../headers/Exception.h"

/**
 * @desc Construtor
 * @desc Passa apenas uma mensagem de erro
 *
 * @param error_msg
 * @returns Exception
 */
Exception::Exception(string error_msg) {
	this->error_msg = error_msg;
}

/**
 * @desc Construtor
 * @desc Passa uma mensagem de erro e o c�digo do erro
 *
 * @param error_code
 * @param error_msg
 * @returns Exception
 */
Exception::Exception(unsigned long error_code, string error_msg) {
	this->error_msg = error_msg;
	this->error_code = error_code;
}

/**
 * @desc Mostra no console a mensagem do erro.
 */
void Exception::print() {
	cout<< this->error_msg << endl;
}

/**
 * @desc Retorna o c�digo do erro.
 *
 * @returns unsigned long
 */
unsigned long Exception::getErrorCode() {
	return this->error_code;
}

/**
 * @desc Retorna a mensagem da exce��o
 *
 * @returns string
 */
string Exception::getMessage() {
	return this->error_msg;
}
