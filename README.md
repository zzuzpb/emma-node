emma-node
=========
E.M.M.A for Environment Monitoring & Management Agents is an architecture for the Internet Of Things based on current IETF specifications and Contiki OS for proposing a new way to decentralize basic interactions between nodes. An interaction is defined by a transmitter, a target and a payload. Consequently, Emma is composed by a COAP server and a COAP client. Both are cached by the notion of Active Web Resources to exchange interactions.

Active Web Resource (AWR) :
* Data are encapsulated inside web resources, when a given resource is edited, its value can be interesting by others nodes. In this sense, interactions are defined like a 4-tuples called interaction agents : agent = {name; condition ; targets ; function}
This agents are hosted on nodes, when "condition" is true according to local AWRs, the payload "function" is sent to "targets" which are others AWR on current or remote nodes. Payloads use JSON represention to format data.

Example : A sensor brightness checks when its value is inferior to 100, in this case the sensor asks the light to increase its current value.
sensor/brightness  (brightness<100) -> light/value (value + 1)

Deployment :
This agents are added from web services on the uri : /agents
For example :
POST coap://node1/agents 
Payload : [agent1 brightness<100, coap://light/value, value+1]

An agent can contains other agents to allow online and self-deploying.
Example:
POST coap://node1/agents 
Payload : [agent2, true, coap://node1/agents, [agent1, brightness<100, coap://light/value, value+1]]

Administration :
Emma has a shell interface to locally manage agents and resources through a set of commands :
eget  : Get the value of a local or remote resource
epost : Post a new resource or agent
eput  : Change the value of a given resource
edelete : Delete a given resource or agent
eeval   : Return the evaluation of a given algebraic function of local resource
eparse  : Replace local resource name by their value on a given algebraic function




