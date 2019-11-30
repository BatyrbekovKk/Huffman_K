#include "pch.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <locale.h>
#define N_ASCII 256


struct SYM {
	unsigned char ch;//код символа
//	unsigned int c;//
	float freq;//частота встречаемости
//	char syms[256];
	char code[255];//резуультат
	struct SYM *left; //левая ветка 0
	struct SYM *right; // правая ветка 1	

};


struct SYM syms[256] = { 0 };// хранениеписей
struct SYM *psyms[256] = { 0 };
struct SYM code[255] = { 0 };
int buf[8] = { 0 };
//struct SYM freq = 0;
//unsigned char ch = 0;
int count = 0;// счетчик символов
int count_used = 0;//используемые
int count_bin = 0;//бинарного файла 
int num_bytes = 0;//целые байты
int tail_size = 0;//хвост полу/байта
struct SYM *left = NULL;
struct SYM *right = NULL;
FILE *fp;
FILE *fp_101;
FILE *fp_out;



struct SYM* buildTree(struct SYM*psyms[], int n);
void makeStruct(struct SYM *syms);
//void freqToStruct(FILE *fp, struct SYM *syms);
void makeCodes(struct SYM *root);
void sortByFreq(struct SYM *syms);
int printSort(struct SYM *syms, int count);
//int countBinary();
void packInBytes(FILE *fp, FILE *fp_out, int num_bytes, int tail_size);
unsigned char pack(unsigned char buf[]);

union CODE {
	unsigned char ch;// содерж код для записи в ввыходной ф-л 

	struct
	{
		unsigned short b1 : 1;
		unsigned short b2 : 1;
		unsigned short b3 : 1;
		unsigned short b4 : 1;
		unsigned short b5 : 1;
		unsigned short b6 : 1;
		unsigned short b7 : 1;
		unsigned short b8 : 1;

	}byte;
};


struct SYM* buildTree(struct SYM*psyms[], int N)
{

	struct SYM *temp = (struct SYM*)malloc(sizeof(struct SYM));
	temp->freq = psyms[N - 2]->freq + psyms[N - 1]->freq;
	temp->left = psyms[N - 2];
	temp->right = psyms[N - 1];
	temp->code[0] = 0;
	if (N == 2)
		return temp;

	else// элемент псум( частота двух эл) 
	{
		int i = 0;


		while (i < N)
		{
			++i;
			if (temp->freq > psyms[i]->freq)
			{
				for (int j = N - 1; j > 1; j--)
				{
					psyms[j] = psyms[j - 1];
					//
				}
				psyms[i] = temp;
				break;
			}

		}
	}
	//	free();
	return buildTree(psyms, N - 1);
}

void makeCodes(struct SYM *root)
{
	if (root->left)
	{
		strcpy(root->left->code, root->code);//копи род код
		strcat(root->left->code, "0");
		makeCodes(root->left);

	}
	if (root->right)
	{
		strcpy(root->right->code, "1");
		makeCodes(root->right);
		makeCodes(root->left);
	}
}


void makeStruct(struct SYM *syms)
{

	for (int i = 0; i < N_ASCII; i++)
	{
		syms[i].ch = i;
		syms[i].freq = 0;
		syms[i].code[0] = 0;
		syms[i].right = NULL;
		syms[i].left = NULL;
	}
}
/*
void freqToStruct(FILE *fp, struct SYM *syms)// заполн
{
	char c=0;//
	int c_num=0;
	while ((c_num = fgetc(fp)) != EOF)
	{
		//fscanf(fp, "%c", &c);
			c_num = (unsigned char)c;
			//++syms[(unsigned char)c].freq;
			++syms[c_num].freq;
			++count;

	}
	printf("count:%d\n", count);
	//for (int i = 0; i < N_ASCII; i++)
	//	syms[i].freq = syms[i].freq / count;

	for (int j = 0; j < N_ASCII; j++)
	{
		syms[j].freq = syms[j].freq / count;
	}


}
*/
void sortByFreq(struct SYM *syms)
{
	int i, j;
	struct SYM temp;

	for (i = 0; i < N_ASCII - 1; i++)
	{
		for (j = 0; j < (N_ASCII - 1 - i); j++)
		{
			if (syms[j].freq < syms[j + 1].freq)
			{
				temp = syms[j];
				syms[j] = syms[j + 1];
				syms[j + 1] = temp;
			}
		}
	}
}

int printSort(struct SYM *syms, int count)
{
	int i = 0;
	printf("Sorted:\n");
	for (i = 0; i < N_ASCII; i++)
	{
		if (syms[i].freq > 0)
		{
			printf("%0.f  %0.2f%%     '%c' %c\n", syms[i].freq, (100 * syms[i].freq) / count, syms[i].ch, syms[i].code);
			count_used++;
		}
	}
	printf("count_used:%d\n", count_used);
	return count_used;
}





unsigned char pack(unsigned char buf[])
{
	union CODE code;
	code.byte.b1 = buf[0] - '0';
	code.byte.b1 = buf[1] - '0';
	code.byte.b1 = buf[2] - '0';
	code.byte.b1 = buf[3] - '0';
	code.byte.b1 = buf[4] - '0';
	code.byte.b1 = buf[5] - '0';
	code.byte.b1 = buf[6] - '0';
	code.byte.b1 = buf[7] - '0';
	return code.ch;
};

void packInBytes(FILE *fp_101, FILE *fp_out, int num_bytes, int tail_size)
{

	union CODE code;

	int j = 0;

	for (int i = 0; i < count_bin - tail_size; i++)
	{
		buf[j] = fgetc(fp_101);
		if (j == 7)
		{
			code.byte.b1 = buf[0] - '0';
			code.byte.b2 = buf[1] - '0';
			code.byte.b3 = buf[2] - '0';
			code.byte.b4 = buf[3] - '0';
			code.byte.b5 = buf[4] - '0';
			code.byte.b6 = buf[5] - '0';
			code.byte.b7 = buf[6] - '0';
			code.byte.b8 = buf[7] - '0';
			fputc(code.ch, fp_out);
			j = 0;
		}
		j++;
	}

	j = 0;
	for (int i = 0; i <= tail_size; i++)
	{
		buf[j] = fgetc(fp_101);
		if (j == tail_size)
		{
			code.byte.b1 = buf[0] - '0';
			code.byte.b2 = buf[1] - '0';
			code.byte.b3 = buf[2] - '0';
			code.byte.b4 = buf[3] - '0';
			code.byte.b5 = buf[4] - '0';
			code.byte.b6 = buf[5] - '0';
			code.byte.b7 = buf[6] - '0';
			code.byte.b8 = buf[7] - '0';
			fputc(code.ch, fp_out);
		}
		j++;
	}
}



int main()
{
	setlocale(LC_ALL, "Rus");
	FILE *fp = fopen("tale.txt", "rt");//открытие исх ф-ла
	FILE *fp_101 = fopen("bin.txt", "wt");//для записи бин
	FILE *fp_out = fopen("out.txt", "wt");//для записи сжатого файла

	if (fp == NULL || fp_101 == NULL || fp_out == NULL)
	{
		perror("fopen");
		return 1;
	}

	int c = 0;//

	makeStruct(syms);

	while ((c = fgetc(fp)) != EOF)
	{
		++syms[(unsigned char)c].freq;
		++count;
	}
	printf("count:%d\n", count);



	for (int i = 0; i < 256; i++)
	{
		psyms[i] = &syms[i];
	}
	//вывод частоты
	for (int v = 0; v < N_ASCII; v++)
		printf("freq:%0.2f%% - '%c'\n", 100 * syms[v].freq / count, syms[v].ch);


	sortByFreq(syms);
	printSort(syms, count);

	for (int j = 0; j < N_ASCII; j++)//инициализация массива указателей адресами записей
	{
		psyms[j] = &syms[j];
	}

	struct SYM *root = buildTree(psyms, count_used);//созд дерева
	makeCodes(root);//кодир
	rewind(fp);//указ на начало
	//чтение исх ф-ла запись кода в бин ф-л
	while ((c = fgetc(fp)) != EOF)
	{
		for (int i = 0; i < count_used; i++)
		{
			if (c == syms[i].ch)
			{
				fputs(syms[i].code, fp_101);
				//printf("%c\n", syms[i].code);
				break;
			}
		}
	}

	fclose(fp_101);//
	fp_101 = fopen("bin.txt", "rt");//открыв бин ф-л 

	//rewind(fp_101);


	while ((c = fgetc(fp_101)) != EOF)
	{
		count_bin++;
	}
	printf("count_bin: %d\n", count_bin);

	num_bytes = count_bin / 8;//кол-во цел байты 
	tail_size = count_bin % 8;//хвост

	printf("num_bytes:%d\n", num_bytes);
	printf("tail_size:%d\n", tail_size);
	//запись в заг
	fwrite("Compression done!", sizeof(char), 17, fp_out);
	fwrite(&count_used, sizeof(int), 1, fp_out);//использ символы
	fwrite(&tail_size, sizeof(int), 1, fp_out);//вол-во симв в хвосте
	//записываем в файл таблицу встречаемости
	for (int j = 0; j < count_used; j++)
	{
		fwrite(&syms[j].ch, sizeof(struct SYM), 1, fp_out);
		fwrite(&syms[j].freq, sizeof(struct SYM), 1, fp_out);

	}
	rewind(fp_101);//указ в начало

	//читаем бинарный файл, занося последовательно по 8 элементов в массив для дальнейшей упаковки
	packInBytes(fp_101, fp_out, num_bytes, tail_size);

	fclose(fp);
	fclose(fp_101);
	fclose(fp_out);
	return 0;
}