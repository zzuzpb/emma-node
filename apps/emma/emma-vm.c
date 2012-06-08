#include "emma-vm.h"
#include "emma-resources.h"
#include "emma-rest.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

#define Nop 12
const char operators[Nop][7] = {
	{"&&\0"},
	{"||\0"},
	{"<\0"},
	{">\0"},
	{"==\0"},
	{"+\0"},
	{"-\0"},
	{"/\0"},
	{"%\0"},
	{"*\0"},
	{"!\0"},
	{"this.\0"}
	};
	
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
vm_eval_parse(char* formula, char* res){
	resource_t * r = NULL;
	char* tmp = formula;
	char* tmp2;
	char* tmp3;
	int offset = 0;
	int i=0, dist;
	char var[BUFF_SIZE];
	
	// Find next variable
	while((tmp2 = strstr(tmp, operators[THIS])) != NULL){
	
		// Insert previous elements of the formula
		strncpy(res + offset, tmp, tmp2-tmp);
		offset += tmp2-tmp;
		tmp2 += strlen(operators[THIS]);
		
		// Looking for end of local variable name
		dist = strlen(tmp2);
		i=-1;
		for(i=0; i < Nop; i++){
			tmp3 = strstr(tmp2, operators[i]);
			
			if(tmp3 != NULL) if(dist > tmp3-tmp2) 
				dist = tmp3-tmp2;
			}				
		strncpy(var,tmp2,dist);
		var[dist] = '\0';		
		
		// Replacing local variable name by its value
		r = rest_get_resource(var);
		if(r != NULL){
			tmp3 = rest_get_user_data(r);
			if(tmp3 != NULL){
				strcpy(res + offset, tmp3);
				offset += strlen(tmp3);
				}		
			}
		// If remote variable
		else {
			sprintf(res + offset, "this.%s", var);
			offset += strlen(var) + 5;			
			}
		tmp = tmp2 + strlen(var);
		}
	
	// Insert final elements of the formula
	sprintf(res+offset, "%s", tmp);
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
		case AND    : strcpy(op,operators[AND]);    					break;
		case OR     : strcpy(op,operators[OR]);     					break;
		case INF    : strcpy(op,operators[INF]);   						break;
		case SUP    : strcpy(op,operators[SUP]);    					break;
		case EGU    : strcpy(op,operators[EGU]);    					break;
		case PLUS   : strcpy(op,operators[PLUS]);   					break;
		case MINUS  : strcpy(op,operators[MINUS]);  					break;
		case DIV    : strcpy(op,operators[DIV]);    					break;
		case MODULO : strcpy(op,operators[MODULO]); 					break;
		case TIME   : strcpy(op,operators[TIME]);   					break;
		case COMP   : strcpy(op,operators[COMP]);   					break;
		case THIS   : strcpy(op,operators[THIS]);   					break;
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
