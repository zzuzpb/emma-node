#include "emma-vm.h"
#include "emma-rest.h"

const char op_AND[]     		= {"&&\0"};
const char op_OR[]      		= {"||\0"};
const char op_INF[]     		= {"<\0"};
const char op_SUP[]     		= {">\0"};
const char op_EGU[]     		= {"==\0"};
const char op_PLUS[]    		= {"+\0"};
const char op_MINUS[]   		= {"-\0"};
const char op_DIV[]     		= {"/\0"};
const char op_MODULO[]  		= {"%\0"};
const char op_TIME[]    		= {"*\0"};
const char op_COMP[]    		= {"!\0"};
const char op_THIS[]    		= {"this.\0"};
 
 enum {
 AND,
 OR,
 INF,
 SUP,
 EGU,
 PLUS,
 MINUS,
 DIV,
 MODULO,
 TIME,
 COMP,
 THIS
 };
/***************************************************************/
/* vm_parse(formula) replaces local variable in formula	       */
/***************************************************************/
void
vm_eval_parse(char* formula){
	char* tmp = formula;
	
	printf("coucou\n");
	while((tmp = strstr(tmp, op_THIS)) != NULL){
		tmp += strlen(op_THIS);
		printf("ICI %s\n",tmp);
		}
	}

/***************************************************************/
/* vm_eval(formula) evaluates the value of the algebra formula */
/***************************************************************/
int
vm_eval_extractor(char* str, char opT, char* opd1, char* opd2, int size);
 
int
vm_eval(char* str){
	resource_t * r = NULL;
	char* tmp;
	
	char opd1[strlen(str)];
	char opd2[strlen(str)];
 	int res1;
	int res2;
	char op = -1;

	// Operation extraction
	if      (vm_eval_extractor(str,COMP,opd1, opd2,strlen(str)) == 2)             op = COMP;
	else if (vm_eval_extractor(str,AND,opd1, opd2,strlen(str)) == 2)              op = AND;
	else if (vm_eval_extractor(str,OR,opd1, opd2,strlen(str)) == 2)               op = OR;
	else if (vm_eval_extractor(str,INF,opd1, opd2,strlen(str)) == 2)              op = INF;
	else if (vm_eval_extractor(str,SUP,opd1, opd2,strlen(str)) == 2)              op = SUP;
	else if (vm_eval_extractor(str,PLUS,opd1, opd2,strlen(str)) == 2)             op = PLUS;
	else if (vm_eval_extractor(str,MINUS,opd1, opd2,strlen(str)) == 2)            op = MINUS;
	else if (vm_eval_extractor(str,DIV,opd1, opd2,strlen(str)) == 2)              op = DIV;
	else if (vm_eval_extractor(str,TIME,opd1, opd2,strlen(str)) == 2)             op = TIME;
	else if (vm_eval_extractor(str,MODULO,opd1, opd2,strlen(str)) == 2)           op = MODULO;

	// Local variable replacing
	else if (vm_eval_extractor(str,THIS,opd1, opd2,strlen(str)) == 2){
	 	r = rest_get_resource(opd2);
	 	if(r != NULL){
		 	tmp = rest_get_user_data(r);
			return atoi(tmp);
			}
			return 0;
		}
		
	// Return end value
	else return atoi(str);
 
 	// Computation
	res1 = vm_eval(opd1);
	res2 = vm_eval(opd2);
	
	if (op == AND)                  return (res1 && res2);
	else if (op == OR)              return (res1 || res2);
	else if (op == INF)             return (res1 < res2);
	else if (op == SUP)             return (res1 > res2);
	else if (op == PLUS)            return (res1 + res2);
	else if (op == MINUS)           return (res1 - res2);
	else if (op == DIV)             return (res1 / res2);
	else if (op == TIME)            return (res1 * res2);
	else if (op == MODULO)          return (res1 % res2);
	else if (op == COMP)            return !res2;
 
	return 0;
	}
	
/***************************************************************/
	
int
vm_eval_extractor(char* str, char opT, char* opd1, char* opd2, int size){
	int i=0,j;
	char ok;
	char * temp;
	char op[7];
 
	// Operator selection
	switch(opT){
		case AND    : strcpy(op,op_AND);    					break;
		case OR     : strcpy(op,op_OR);     					break;
		case INF    : strcpy(op,op_INF);   						break;
		case SUP    : strcpy(op,op_SUP);    					break;
		case EGU    : strcpy(op,op_EGU);    					break;
		case PLUS   : strcpy(op,op_PLUS);   					break;
		case MINUS  : strcpy(op,op_MINUS);  					break;
		case DIV    : strcpy(op,op_DIV);    					break;
		case MODULO : strcpy(op,op_MODULO); 					break;
		case TIME   : strcpy(op,op_TIME);   					break;
		case COMP   : strcpy(op,op_COMP);   					break;
		case THIS   : strcpy(op,op_THIS);   					break;
		default : return 0;
		}

	// Operand 2 selection
	temp = strstr(str,op);
	if(temp == NULL)        return 0;
	temp += strlen(op);

	// Operand 1 extraction
	while(i < strlen(str) && i < size){
	  ok = 1;
		for(j=0;j<strlen(op);j++)
			if(str[i+j] != op[j])	ok = 0;
			if(ok == 1) break;
 
			opd1[i] = str[i];
			i++;
		}
	opd1[i] = '\0';

	// Operand 2 extraction
	i=0;
	while(i < strlen(temp) && i < size){
		opd2[i] = temp[i];
		i++;
		}
	opd2[i] = '\0';
 
	return 2;
	}
/***************************************************************/
