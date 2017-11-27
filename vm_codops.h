#define OP_ADD		1 
#define OP_SUB 		2
#define OP_MULT		3
#define OP_DIV		4
#define OP_DIVI		5

#define OP_NEG		7
#define OP_AND		8
#define OP_OR		9
#define OP_NOT		10
#define OP_EQ		11
#define OP_LS		12
#define OP_GT		13

#define OP_INC		14	
#define OP_DEC		15

#define OP_PUSH 	100
#define OP_PUSHR 	101
#define OP_LIBP 	102
#define OP_DUPL 	103
#define OP_CONT 	104
#define OP_MOVE 	105
#define OP_COPY 	106
#define OP_STM	 	107
#define OP_MTS     	108

#define OP_JP		200
#define OP_JF		201
#define OP_JL		202
#define OP_JG		203
#define OP_CALL		204
#define OP_RET		205

#define OP_INPUT	300	
#define OP_OUTPUT	301
#define OP_OUTCHAR	302

#define OP_CALLS	400
#define OP_SAVEBP	401
#define OP_RSTRBP	402
#define OP_HALT		403
