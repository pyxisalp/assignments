#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int dataword_size, generator_size, codeword_size;
char generator[200];

void get_codeword(unsigned char dataword, unsigned char codeword[]) {
	unsigned char dataword_0[208] = {0,};
	
	for (int i = 0; i < dataword_size; i++)
		dataword_0[i + generator_size - 1] = ((dataword >> i) & 1);
	for (int i = 0; i < generator_size - 1; i++)
		dataword_0[i] = 0;

	for (int i = codeword_size - 1; i >= generator_size - 1; i--) {
		if (dataword_0[i] & 1) {
			for (int j = generator_size - 1; j >= 0; j--)
				dataword_0[i - j] ^= generator[generator_size - 1 - j];
		}
	}
	

	for (int i = 0; i < dataword_size; i++)
		codeword[i + generator_size - 1] = ((dataword >> i) & 1);
	for (int i = 0; i < generator_size - 1; i++)
		codeword[i] = dataword_0[i];

	return;
} 

int main(int argc, char *argv[]) {
	FILE *fp1, *fp2;
	int file_size, buf_size, padding_size, total_codeword_size;
	int mul = 1;
	char letter;
	unsigned char *buf;

	if (argc != 5) {
		printf("usage: ./crc_encoder input_file output_file generator dataword_size\n");
	exit(1);
	}
	
	generator_size = strlen(argv[3]);
	strcpy(generator, argv[3]);
	for (int i = 0; i < generator_size; i++)
		generator[i] -=  '0';
	for (int i = 0; i < generator_size / 2; i++) {
		int temp = generator[i];
		generator[i] = generator[generator_size - i - 1];
		generator[generator_size - i - 1] = temp;
	}
	
	dataword_size = atoi(argv[4]);
	codeword_size = dataword_size + generator_size - 1;

	if (!(fp1 = fopen(argv[1], "rb"))) {
		printf("input file open error.\n");
		exit(1);
	}
	if (!(fp2 = fopen(argv[2], "wb"))) {
		printf("output file open error.\n");
		exit(1);
	}
	if ((dataword_size != 4) && (dataword_size != 8)) {
		printf("dataword size must be 4 or 8.\n");
		exit(1);
	}
	
	fseek(fp1, 0, SEEK_END);
	file_size = ftell(fp1);
	fseek(fp1, 0, SEEK_SET);

	total_codeword_size = (file_size * 8 / dataword_size) * codeword_size;
	if (total_codeword_size % 8 == 0) {
		padding_size = 0;
		buf_size = 1 + total_codeword_size / 8;
	}
	else {
		padding_size = 8 - (total_codeword_size % 8);
		buf_size = 2 + total_codeword_size / 8;
	}

	buf = (unsigned char*)malloc(sizeof(unsigned char) * buf_size);
	memset(buf, 0, buf_size);
	buf[0] = padding_size;

	int buf_idx = 1, bit_idx = 7 - padding_size;
	while(fread(&letter, 1, 1, fp1) != 0) {
		if (dataword_size == 4) {
			unsigned char front, back, front_codeword[204], back_codeword[204];

			front = letter / 16;
			back = letter % 16;

			get_codeword(front, front_codeword);
			get_codeword(back, back_codeword);
			
			for (int i = codeword_size - 1; i >= 0; i--) {
				buf[buf_idx] += ((front_codeword[i] & 1) << bit_idx);
				bit_idx--;
				if (bit_idx < 0) {bit_idx = 7; buf_idx++;}
			}
			for (int i = codeword_size - 1; i >= 0; i--) {
				buf[buf_idx] += ((back_codeword[i] & 1) << bit_idx);
				bit_idx--;
				if (bit_idx < 0) {bit_idx = 7; buf_idx++;}
			}
			
		}
		else {
			unsigned char codeword[208];

			get_codeword(letter, codeword);

			for (int i = codeword_size - 1; i >= 0; i--) {
				buf[buf_idx] += ((codeword[i] & 1) << bit_idx);
				bit_idx--;
				if (bit_idx < 0) {bit_idx = 7; buf_idx++;}
			}
		}
	}

	fwrite(buf, buf_size, 1, fp2);
	fclose(fp1); fclose(fp2);

	return 0;
}
