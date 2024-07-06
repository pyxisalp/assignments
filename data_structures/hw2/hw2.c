#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 20

struct node {
	int data;
	struct node *prev;
	struct node *next;
};
typedef struct node *listPointer;

void insert(listPointer* list, int data) {
	listPointer temp = (listPointer)malloc(sizeof(struct node));
	listPointer curr = *list;

	temp->data = data;

	if (!curr) {
		temp->prev = temp;
		temp->next = temp;
		*list = temp;
	}
	else {
		while (1) {
			if (curr->data == data)
				return;

			if (curr->data > data) {
				temp->prev = curr->prev;
				temp->next = curr;
				curr->prev->next = temp;
				curr->prev = temp;
				if ((*list)->data > data)
					*list = temp;
				return;
			}
			
			curr = curr->next;
			if (curr == *list) {
				temp->prev = curr->prev;
				temp->next = curr;
				curr->prev->next = temp;
				curr->prev = temp;
				break;
			}
		}
	}
}

void delete(listPointer* list, int data) {
	listPointer temp, curr = *list;
	
	if (!curr) return;
	else {
		while (1) {
			if (curr->data == data) {
				temp = curr;
				temp->prev->next = temp->next;
				temp->next->prev = temp->prev;

				if (curr->prev->data == curr->next->data) {
					*list = NULL;
					free(temp);
				}
				else {
					free(temp);
					while ((*list)->prev->data < (*list)->data)
						*list = (*list)->prev;
				}
				return;
			}
			if (curr->data > data)
				return;

			curr = curr->next;
			if (curr == *list)
				break;
		}
	}
}

void print_forward(listPointer list) {
	listPointer curr;
	FILE *outfile;

	outfile = fopen("hw2_result.txt", "a");
	if(list) {
		curr = list;
		while(1) {
			fprintf(outfile, "%d ", curr->data);
			printf("%d ", curr->data);
			curr = curr->next;
			if(curr == list) break;
		}
	}
	fprintf(outfile, "\n");
	printf("\n");
	fclose(outfile);
}

void print_reverse(listPointer list) {
	listPointer curr;
	FILE *outfile;

	outfile = fopen("hw2_result.txt", "a");
	if(list) {
		curr = list->prev;

		while(curr != list) {
			fprintf(outfile, "%d ", curr->data);
			printf("%d ", curr->data);
			curr = curr->prev;
		}
		fprintf(outfile, "%d ", curr->data);
		printf("%d ", curr->data);
	}
	fprintf(outfile, "\n");
	printf("\n");
	fclose(outfile);
}

int main(int argc, char* argv[]) {
	FILE *fp;
	int num;
	char buf[MAX_SIZE], *ptr;
	listPointer first = NULL;

	if (argc != 2) {
		printf("usage: ./hw2 input_filename\n");
		return 1;
	}

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("The input file does not exist.\n");
		return 2;
	}

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		ptr = strtok(buf, " \n");

		if (!strcmp(ptr, "INSERT")) {
			ptr = strtok(NULL, "\n");
			num = atoi(ptr);
			insert(&first, num);
		}
		else if (!strcmp(ptr, "DELETE")) {
			ptr = strtok(NULL, "\n");
			num = atoi(ptr);
			delete(&first, num);
		}
		else if (!strcmp(ptr, "ASCEND"))
			print_forward(first);
		else if (!strcmp(ptr, "DESCEND"))
			print_reverse(first);
	}

	return 0;
}
