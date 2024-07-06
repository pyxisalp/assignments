#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

int nodeNum = 0, usedCharNum = 0, headerLength = 11, paddingLength, bufLength;
int lengthArr[128], freqArr[128] = {0, };
string codeword = "";
string codewordArr[128];

typedef struct _node{
	char symbol;
	int freq;
	struct _node *left;
	struct _node *right;
} NODE;
NODE *heap[129], *u, *v, *w;

void insertion(NODE *node) {
	int i;

	if (nodeNum == 127) {
		printf("heap full\n");
		exit(1);
	}

	for (i = ++nodeNum; (i != 1) && (node->freq < heap[i / 2]->freq); heap[i] = heap[i / 2], i /= 2);
	heap[i] = node;
}

NODE* deletion(void) {
	int parent, child;
	NODE *item, *temp;

	if (!nodeNum) {
		printf("heap empty\n");
		exit(1);
	}

	item = heap[1];
	temp = heap[nodeNum--];
	for (parent = 1, child = 2; child <= nodeNum; heap[parent] = heap[child], parent = child, child *= 2) {
		if ((child < nodeNum) && (heap[child]->freq > heap[child + 1]->freq)) child++;
		if (temp->freq <= heap[child]->freq) break;
	}
	heap[parent] = temp;

	return item;
}

void HuffmanCode(NODE *node, string codeword) {
	if (node) {
		codeword += '0';
		HuffmanCode(node->left, codeword);
		if (!(node->left) && !(node->right)) {
			lengthArr[node->symbol] = codeword.length();
			headerLength += (14 + codeword.length() * (1 + freqArr[node->symbol]));
			codewordArr[node->symbol] = codeword;	
		}
		codeword += '1';
		HuffmanCode(node->right, codeword);
	}
}

int main(int argc, char *argv[]) {
	FILE *fp1, *fp2;
	char buf, *bufArr, name[30];
	int i, j, bufIdx, bitIdx, tmp;
	NODE *curr, *newNode;

	if (strcmp(argv[1], "-c") != 0 && strcmp(argv[1], "-d") != 0) {
		printf("option error\n");
		exit(1);
	}
	
	if (!(fp1 = fopen(argv[2], "rb"))) {
		printf("input error\n");
		exit(1);
	}

	if (strcmp(argv[1], "-c") == 0) {
		sprintf(name, "%s.zz", argv[2]);

		if (!(fp2 = fopen(name, "wb"))) {
			printf("output error(compression)\n");
			exit(1);
		}

		while (fread(&buf, 1, 1, fp1) != 0) freqArr[buf]++;
		for (i = 0; i < 128; i++) {
			if (freqArr[i]) {
				usedCharNum++;
				newNode = (NODE*)malloc(sizeof(struct _node));
				
				if (!newNode) {
					printf("memory error\n");
					exit(1);
				}

				newNode->symbol = i;
				newNode->freq = freqArr[i];
				newNode->left = NULL;
				newNode->right = NULL;
				insertion(newNode);
			}
		}
		for (i = 0; i < usedCharNum - 1; i++) {
			u = deletion();
			v = deletion();
			w = (NODE*)malloc(sizeof(struct _node));

			if (!w) {
				printf("memory error\n");
				exit(1);
			}

			w->left = u;
			w->right = v;
			w->freq = u->freq + v->freq;
			insertion(w);			
		}
		w = deletion();
		HuffmanCode(w, codeword);
		if (headerLength % 8) paddingLength = 8 - (headerLength % 8);
		else paddingLength = 0;
		bufLength = (headerLength + paddingLength) / 8;
		bufArr = (char*)malloc(sizeof(char) * bufLength);
		memset(bufArr, 0, bufLength);
		bufIdx = 0;
		bitIdx = 7;
		for (i = 2; i >= 0; i--) {
			bufArr[bufIdx] += (((paddingLength >> i) & 1) << bitIdx);
			bitIdx--;
		}
		for (i = 7; i >= 0; i--) {
			bufArr[bufIdx] += (((usedCharNum >> i) & 1) << bitIdx);
			bitIdx--;
			if (bitIdx < 0) {
				bufIdx++;
				bitIdx = 7;
			}
		}
		for (j = 0; j < 128; j++) {
			if (!freqArr[j]) continue;
			tmp = lengthArr[j];
			for (i = 6; i >= 0; i--) {
				bufArr[bufIdx] += (((j >> i) & 1) << bitIdx);
				bitIdx--;
				if (bitIdx < 0) {
					bufIdx++;
					bitIdx = 7;
				}
			}
			for (i = 6; i >= 0; i--) {
				bufArr[bufIdx] += (((tmp >> i) & 1) << bitIdx);
				bitIdx--;
				if (bitIdx < 0) {
					bufIdx++;
					bitIdx = 7;
				}
			}
			codeword = codewordArr[j];
			for (i = 0; i < tmp; i++) {
				bufArr[bufIdx] += ((codeword[i] - '0') << bitIdx);
				bitIdx--;
				if (bitIdx < 0) {
					bufIdx++;
					bitIdx = 7;
				}
			}
		}
		fseek(fp1, 0, SEEK_SET);
		while (fread(&buf, 1, 1, fp1) != 0) {
			tmp = lengthArr[buf];
			codeword = codewordArr[buf];
			for (i = 0; i < tmp; i++) {
				bufArr[bufIdx] += ((codeword[i] - '0') << bitIdx);
				bitIdx--;
				if (bitIdx < 0) {
					bufIdx++;
					bitIdx = 7;
				}
			}
		}
		fwrite(bufArr, bufLength, 1, fp2);
	}
	else {
		sprintf(name, "%s.yy", argv[2]);

		if (!(fp2 = fopen(name, "w"))) {
			printf("output error(decompression)\n");
			exit(1);
		}

		fseek(fp1, 0, SEEK_END);
		bufLength = ftell(fp1);
		fseek(fp1, 0, SEEK_SET);
		bufArr = (char*)malloc(sizeof(char) * bufLength);
		fread(bufArr, bufLength, 1, fp1);
		bufIdx = 0;
		bitIdx = 7;
		paddingLength = 0;
		w = (NODE*)malloc(sizeof(struct _node));
		w->left = NULL;
		w->right = NULL;

		if (!w) {
			printf("memory error\n");
			exit(1);
		}

		for (i = 0; i < 3; i++) {
			paddingLength += (((bufArr[bufIdx] >> bitIdx) & 1) << (2 - i));
			bitIdx--;
		}
		for (i = 0; i < 8; i++) {
			usedCharNum += (((bufArr[bufIdx] >> bitIdx) & 1) << (7 - i));
			bitIdx--;
			if (bitIdx < 0) {
				bufIdx++;
				bitIdx = 7;
			}
		}
		for (j = 0; j < usedCharNum; j++) {
			buf = 0;
			tmp = 0;
			codeword = "";
			for (i = 0; i < 7; i++) {
				buf += (((bufArr[bufIdx] >> bitIdx) & 1) << (6 - i));
				bitIdx--;
				if (bitIdx < 0) {
					bufIdx++;
					bitIdx = 7;
				}
			}
                        for (i = 0; i < 7; i++) {
                                tmp += (((bufArr[bufIdx] >> bitIdx) & 1) << (6 - i));
                                bitIdx--;
                                if (bitIdx < 0) {
                                        bufIdx++;
                                        bitIdx = 7;
                                }
                        }
			curr = w;
			for (i = 0; i < tmp; i++) {
				if ((bufArr[bufIdx] >> bitIdx) & 1) {
					codeword += '1';
					if (!(curr->right)) {
						newNode = (NODE*)malloc(sizeof(struct _node));

						if (!newNode) {
							printf("memory error\n");
							exit(1);
						}

						newNode->left = NULL;
						newNode->right = NULL;
						curr->right = newNode;
						curr = newNode;
					}
					else curr = curr->right;
				}
				else {
					if (!(curr->left)) {
						codeword += '0';
        	                                newNode = (NODE*)malloc(sizeof(struct _node));

                	                        if (!newNode) {
                        	                        printf("memory error\n");
                                	                exit(1);
                                       		}

	                                        newNode->left = NULL;
        	                                newNode->right = NULL;
                	                        curr->left = newNode;
						curr = newNode;
					}
					else curr = curr->left;
				}
				bitIdx--;
				if (bitIdx < 0) {
					bufIdx++;
					bitIdx = 7;
				}
			}
			curr->symbol = buf;
			codewordArr[buf] = codeword;
			curr = w;
		}
		curr = w;
		while (bufIdx <= bufLength - 1) {
			if (bufIdx == bufLength - 1 && bitIdx < paddingLength - 1) break;
			if (!(curr->left) && !(curr->right)) {
				fprintf(fp2, "%c", curr->symbol);
				curr = w;
			}
			if ((bufArr[bufIdx] >> bitIdx) & 1) curr = curr->right;	
			else curr = curr->left;
			bitIdx--;
			if (bitIdx < 0) {
				bufIdx++;
				bitIdx = 7;
			}
		}
		if (!(curr->left) && !(curr->right)) fprintf(fp2, "%c", curr->symbol);
	}
	fclose(fp1);
	fclose(fp2);

	return 0;
}
