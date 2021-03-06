#include "libgcl.h"

int calc(int op, int value1, int value2)
{

	switch (op) {
	case 0:
		return 0;
	case 0x01:
		return -value1;
	case 0x02:
		return value1 == 0;
	case 0x03:
		return !value1;
	case 0x04:
		return value1 + value2;
	case 0x05:
		return value1 - value2;
	case 0x06:
		return value1 * value2;
	case 0x07:
		return value2 / value1;
	case 0x08:
		return ((value2 / value1) * value1) - value2;
	case 0x09:
		return value2 << value1;
	case 0x0A:
		return value2 >> value1;
	case 0x0B:
		return (value1 == value2);
	case 0x0C: {
		value1 = value2 ^ value1;
		int temp = (value1 >> 0x1F); //srawi
		value1 = temp ^ value1;
		value1 = temp - value1;
		value1--;
		value1 = value1 >> 31;
		return value1; }
	case 0x0D:
		return value2 < value1;
	case 0x0E:
		return value2 <= value1;
	case 0x0F:
		return value2 > value1;
	case 0x10:
		return value2 >= value1;
	case 0x11:
		return value1 | value2;
	case 0x12:
		return value1 & value2;
	case 0x13:
		return value1 ^ value2;
	case 0x14:
		value1 = value1 | value2;
		return value1 ? 1 : 0;
	case 0x15:
		if (value2 = 0) return 0; 
		return value1 != 0;
	}

}

typedef struct
{
	int value;
	char *ptr; 
} EXPR_STACK;


int GCL_Expr(char *data)
{
	unsigned int type;
	char *p = data;
	EXPR_STACK *sp;
	EXPR_STACK expr_stack[8];

	sp = expr_stack;

	for (;;) 
	{
		int val;
		type = *p;

		if ((type & 0xE0) != 0xA0) 
		{
			sp->ptr = p;
			type &= 0xF0;
			p = GCL_GetNextValue(p, (int*)&type, &val);

			if (type == 0x80) 
			{
				GCL_ExecBlockBody((char *)&val, 0, 0);
				sp->value = gcl_work.status;
			} else {
				sp->value = val;
			}
			sp++;
			continue;
		}

		unsigned int op = type & 0x1F;
		sp--;

		if (!op)
			return sp->value;

		val = sp->value;
		sp--;

		if (op == 0x16) 
		{
			int no = *sp->ptr;
			if ((no & 0xF0) == 0x90) 
			{
				GCL_SetLocalArgs((no & 0x0F), val);
			}  else {
				GCL_SetVar(sp->ptr, val);
			}
			sp->value = val;
		} else {
			sp->value = calc(op, val, sp->value);
			sp->ptr = 0;
		}
		p++;
	}
}