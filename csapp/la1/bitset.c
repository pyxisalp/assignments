
/*
 * addNumber(set, x): Add 'x' to the bitset represented in array 'set'
 *   Assume that 0 <= x <= 127 and 'set' is a pointer to 16-byte array
 */

void addNumber(unsigned char* set, int x) {
	int i, j;
	i = x / 8, j = 7 - x % 8;
	set[i] = 1 << j;

	return;
}
