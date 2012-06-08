#include "list.h"
#include "rest.h"
#include "memb.h"

#define AGENT_MAX						3

#define AGENT_NAME_SIZE 		20
#define AGENT_PRE_SIZE 			20
#define AGENT_POST_SIZE 		20
#define AGENT_TARGET_SIZE 	BUFF_SIZE

struct agent_t {
  struct agent_t *next; 
	char name[AGENT_NAME_SIZE];
	char pre[AGENT_PRE_SIZE];
	char post[AGENT_POST_SIZE];
	char target[AGENT_TARGET_SIZE];
};

typedef struct agent_t agent_t;

void 
agent_init(void);

void 
agent_run(agent_t* ag);

void
 agent_run_all();

int 
agent_new(char * name, char * pre, char* url, char * post);

struct agent_t* 
agent_get(char * name);

struct agent_t* 
agent_get_all(void);

int 
agent_delete(char * name);

