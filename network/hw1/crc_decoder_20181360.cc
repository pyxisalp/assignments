#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dataword_size, generator_size, codeword_size;
char generator[200];

int get_rest(unsigned char codeword[]) {
	for (int i = codeword_size - 1; i >= generator_size - 1; i--) {
		if (codeword[i] & 1)
			for (int j = generator_size - 1; j >= 0; j--)
				codeword[i - j] ^= generator[generator_size - j - 1];
	}

	for (int i = 0; i < codeword_size; i++)
		if (codeword[i]) return 1;
	return 0;
}

int main(int argc, char *argv[]) {
	FILE *fp1, *fp2, *fp3;
	int buf_size, padding_size;
	int mul = 1, num = 1, cnt = 0, codeword_num = 0, error_num = 0;
	unsigned char *buf;

	if (argc != 6) {
		printf("usage: ./crc_decoder input_file output_file result_file generator dataword_size\n");
		exit(1);
	}
	
	generator_size = strlen(argv[4]);
	strcpy(generator, argv[4]);
	for (int i = 0; i < generator_size; i++)
		generator[i] -= '0';
	for (int i = 0; i < generator_size / 2; i++) {
		int temp = generator[i];
		generator[i] = generator[generator_size - i - 1];
		generator[generator_size - i - 1] = temp;
	}

	dataword_size = atoi(argv[5]);
	codeword_size = dataword_size + generator_size - 1;

	if (!(fp1 = fopen(argv[1], "rb"))) {
		printf("input file open error.\n");
		exit(1);
	}
	if (!(fp2 = fopen(argv[2], "w"))) {
		printf("output file open error.\n");
		exit(1);
	}
	if (!(fp3 = fopen(argv[3], "w"))) {
		printf("result file open error.\n");
		exit(1);
	}
	if ((dataword_size != 4) && (dataword_size != 8)) {
		printf("dataword size must be 4 or 8.\n");
		exit(1);
	}

	fseek(fp1, 0, SEEK_END);
	buf_size = ftell(fp1);
	fseek(fp1, 0, SEEK_SET);

	buf = (unsigned char*)malloc(sizeof(unsigned char) * buf_size);
	fread(buf, buf_size, 1, fp1);
	padding_size = buf[0];
	
	int buf_idx = 1, bit_idx = 7 - padding_size;
	while (buf_idx < buf_size) {
		if (dataword_size == 4) {
			unsigned char front = 0, back = 0, front_codeword[204] = {0,}, back_codeword[204] = {0,};
			char letter;

			for (int i = codeword_size - 1; i >= 0; i--) {
				front_codeword[i] = ((buf[buf_idx] >> bit_idx) & 1);
				bit_idx--;
				if (bit_idx < 0) {bit_idx = 7; buf_idx++;}
			}
			for (int i = codeword_size - 1; i >= 0; i--) {
				back_codeword[i] += ((buf[buf_idx] >> bit_idx) & 1);
				bit_idx--;
				if (bit_idx < 0) {bit_idx = 7; buf_idx++;}
			}
			
			mul = 1;
			for (int i = 0; i < dataword_size; i++) {
				front += front_codeword[i + generator_size - 1] * mul;
				back += back_codeword[i + generator_size - 1] * mul;
				mul *= 2;
			}

			if (get_rest(front_codeword)) error_num++;
			if (get_rest(back_codeword)) error_num++;
			codeword_num += 2;
			
			letter = front * 16 + back;
			fputc(letter, fp2);
		}
		else {
			unsigned char temp = 0, codeword[208] = {0,};
			char letter;

			for (int i = codeword_size - 1; i >= 0; i--) {
				codeword[i] = ((buf[buf_idx] >> bit_idx) & 1);
				bit_idx--;
				if (bit_idx < 0) {bit_idx = 7; buf_idx++;}
			}
			

			mul = 1;
			for (int i = 0; i < dataword_size; i++) {
				temp += codeword[i + generator_size - 1] * mul;
				mul *= 2;
			}

			if (get_rest(codeword)) error_num++;
			codeword_num += 1;

			letter = temp;
			fputc(temp, fp2);
		}
	}
	
	fprintf(fp3, "%d %d\n", codeword_num, error_num);
	fclose(fp1); fclose(fp2); fclose(fp3);

	return 0;
}
