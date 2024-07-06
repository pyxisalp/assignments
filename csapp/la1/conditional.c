#include "conditional.h"

/*
 * conditional(x, y, z): same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 */
int conditional(int x, int y, int z) {
	int rtn = !(!x);
	rtn = ~rtn + 1;
	rtn = (rtn & y) | (~rtn & z);

	return rtn;
}
