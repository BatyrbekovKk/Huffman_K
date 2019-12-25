#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <clocale>
struct sym //структуры или записи
{
	unsigned char ch; //наш символ
	float freq;      //вероятность этого символа
	char code[255]; //код этого сивола
	sym *left;     
	sym *right;	  
};

union code //объединение
{
	unsigned char chhh;//переменная содержащая код для записи в сжатый файл
	struct byte
	{
		unsigned b1 : 1;
		unsigned b2 : 1;
		unsigned b3 : 1;
		unsigned b4 : 1;
		unsigned b5 : 1;
		unsigned b6 : 1;
		unsigned b7 : 1;
		unsigned b8 : 1;
	}byte;
};
sym *makeTree(sym *psym[], int k)//рeкурсивная функция создания дерева Хофмана
{
	sym *temp;
	temp = (sym*)malloc(sizeof(sym));
	temp->freq = psym[k - 1]->freq + psym[k - 2]->freq;
	temp->code[0] = 0;
	temp->left = psym[k - 1];
	temp->right = psym[k - 2];

	if (k == 2)
		return temp;
	else //внесение в массив в нужное место элемента дерева Хофмана
	{
		for (int i = 0; i < k; i++)
			if (temp->freq > psym[i]->freq) {
				for (int j = k - 1; j > i; j--)
					psym[j] = psym[j - 1];

				psym[i] = temp;
				break;
			}
	}
	return makeTree(psym, k - 1);
}

void makeCodes(sym *root)//Рекурсивная функция кодирования
{
	if (root->left){
		strcpy(root->left->code, root->code); //копируем В root->left->code "строку" root->code
		strcat(root->left->code, "0"); //объединяем root->left->code с 0
		makeCodes(root->left);
	}
	if (root->right){
		strcpy(root->right->code, root->code);
		strcat(root->right->code, "1");
		makeCodes(root->right);
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");
	FILE *fpin1, *fpout, *fpout1,*fpout2,
		*fpin2, *fpdec,	*fpalltext, *fpzip;

	fpin1 = fopen("text1.txt", "r"); //открываем файл для чтения текста для кодирования
	fpin2 = fopen("text2.txt", "r"); //открываем файл для чтения текста для кодирования

	fpout1 = fopen("coder1.txt", "w");//открываем файл для записи бинарного кода
	fpout2 = fopen("coder2.txt", "w");//открываем файл для записи бинарного кода
	fpdec = fopen("tabl.txt", "w"); //создает файл для записи таблицы
	fpalltext = fopen("alltext.txt", "w"); //создаем файл с общим декодированным тестом
	fpzip = fopen("zip.dat", "wb");

	int fsize1 = 0, fsize2 = 0;//счётчик количества символов из 0 и 1 в промежуточном файле teemp
	int ts1 = 0, ts2 = 0;//размер хвоста файла (то, что не кратно 8 в промежуточном файле)
	int mes[8];//массив 0 и 1
	int chh;  // в эту переменную читается информация из файла 
	int k = 0; //счётчик количества различных букв, уникальных символов
	int kk = 0;
	int kolvo[256] = { 0 };//инициализирем массив повторений уникальных символов
	sym simbols[256] = { 0 }; //инициализируем массив записей 
	sym *psym[256]; //инициализируем массив указателей на записи
	sym *symbols = (sym*)malloc(k * sizeof(sym));//создание динамического массива структур simbols
	float summir = 0;//сумма частот встречаемости
	char j = 0;//вспомогательная переменная
	int j1 = 0;
	int i = 0;

	sym **psum = (sym**)malloc(k * sizeof(sym*));//создание динамического массива указателей на simbols
	//составляем общий текст из двух файлов
	while ((chh = fgetc(fpin1)) != EOF)
		fprintf(fpalltext, "%c", chh);
	
	while ((chh = fgetc(fpin2)) != EOF)
		fprintf(fpalltext, "%c", chh);

	fclose(fpalltext);
	fpalltext = fopen("alltext.txt", "r");

	//Начинаем побайтно читать файл и составлять таблицу встречаемости
	while ((chh = fgetc(fpalltext)) != EOF) {
		for (int j = 0; j < 256; j++) {
			if (chh == simbols[j].ch) {
				kolvo[j]++;
				kk++;
				break;
			}
			if (simbols[j].ch == 0) {
				simbols[j].ch = (unsigned char)chh;
				kolvo[j] = 1;
				k++; kk++;
				break;
			}
		}
	}
	fclose(fpalltext);
	for (i = 0; i < k; i++) {
		simbols[i].freq = (float)kolvo[i] / kk; // Рассчёт частоты встречаемости
		psym[i] = &simbols[i];					//в массив указателей заносим адреса записей
	}

	//Сортировка по убыванию
	sym tempp;
	for (i = 1; i < k; i++)
		for (int j = 0; j < k - 1; j++)
			if (simbols[j].freq < simbols[j + 1].freq) {
				tempp = simbols[j];
				simbols[j] = simbols[j + 1];
				simbols[j + 1] = tempp;
			}

	/*----СОЗДАНИЕ ДЕРЕВА ХАФМАНА----*/
	sym *root = makeTree(psym, k);
	makeCodes(root);//вызов функции получения кода

	/*ПРОВЕРКА - ВЫВОД НА ЭКРАН*/
	for (int i = 0; i < k; i++) {
		summir += simbols[i].freq;
		printf("%3d) Char='%c'  Freq= %f  Code= %s\n", i, simbols[i].ch, simbols[i].freq, simbols[i].code);
	}
	printf("\n all char = %d     Summir=%f\n", kk, summir);
	

	/*----ЗАПИСЬ ТАБЛИЦЫ----*/
	for (i = 0; i < k; i++){
		fprintf(fpdec, " %c = %s ", simbols[i].ch, simbols[i].code);
	}
	fclose(fpdec);

	/*----ЗАПИСЬ КОДЕРА1----*/
	//в цикле читаем исходный файл, и записываем уже закодированный текст
	rewind(fpin1);
	while ((chh = fgetc(fpin1)) != EOF) {
		for (i = 0; i < k; i++) {
			if (chh == simbols[i].ch)
				fputs(simbols[i].code, fpout1);
		}
	}
	fclose(fpout1);

	/*----ЗАПИСЬ КОДЕРА2----*/
	rewind(fpin2);
	while ((chh = fgetc(fpin2)) != EOF) {
		for (i = 0; i < k; i++) {
			if (chh == simbols[i].ch)
				fputs(simbols[i].code, fpout2);
		}
	}
	fclose(fpout2);


	/*----ДЕКОДИРОВАНИЕ КОДЕРА1----*/
	fpout1 = fopen("coder1.txt", "r");
	FILE *f1;
	f1 = fopen("result1.txt", "w");
	char buf[20] = { '\0' };
	rewind(fpout1);
	i = 0;
	while ((buf[i] = fgetc(fpout1)) != EOF) {
		i++;
		for (int ii = 0; ii < k; ii++) {
			if (strcmp(buf, simbols[ii].code) == 0) {
				fprintf(f1, "%c", simbols[ii].ch);
				for (int ii = 0; ii < 20; ii++) {
					buf[ii] = '\0';
				}
				i = 0;
			}
		}
	}
	fclose(f1);
	/*----ДЕКОДИРОВАНИЕ КОДЕРА2----*/
	fpout2 = fopen("coder2.txt", "r");
	FILE *f2;
	f2 = fopen("result2.txt", "w");
	for (int ii = 0; ii < 20; ii++) {
		buf[ii] = '\0';
	}
	rewind(fpout2);
	i = 0;
	while ((buf[i] = fgetc(fpout2)) != EOF) {
		i++;
		for (int ii = 0; ii < k; ii++) {
			if (strcmp(buf, simbols[ii].code) == 0) {
				fprintf(f2, "%c", simbols[ii].ch);
				for (int ii = 0; ii < 20; ii++) {
					buf[ii] = '\0';
				}
				i = 0;
			}
		}
	}
	fclose(f2);

	/*----РАССЧЕТЫ И АРХИВАЦИЯ----*/
	rewind(fpout1);
	//Считаем размер файла(количество символов в нём)
	while ((chh = fgetc(fpout1)) != EOF)
	fsize1++;
	ts1 = fsize1 % 8;//находим остаток, количество символов не кратных 8 (хвост)

	rewind(fpout2);
	while ((chh = fgetc(fpout2)) != EOF)
		fsize2++;
	ts2 = fsize2 % 8;//находим остаток, количество символов не кратных 8 (хвост)

	//формируем заголовок сжатого файла через поля байтов
	fwrite("compresing!!!", sizeof(char), 13, fpzip);//условная подпись
	fwrite(&k, sizeof(unsigned int), 1, fpzip);//количество уникальных символов
	fwrite(&ts1, sizeof(unsigned int), 1, fpzip);//величина хвоста1
	fwrite(&ts2, sizeof(unsigned int), 1, fpzip);//величина хвоста2
	
	//Записываем в сжатый файл таблицу встречаемости
	for (i = 0; i < k; i++) {
		fwrite(&simbols[i].ch, sizeof(unsigned char), 1, fpzip);
		fwrite(&simbols[i].code, sizeof(int), 3, fpzip);
	}
	

	rewind(fpout1);
	rewind(fpout2);
	union code code1;//инициализируем переменную code1
	//Читаем бинарный файл, занося последовательно каждые 8 элементов в массив для последующей побитовой обработки в объединении union
	j = 0;
	fprintf(fpzip, "\n");
	for (i = 0; i < (fsize1 - ts1); i++) {
		mes[j] = fgetc(fpout1);
		if (j == 7)
		{
			code1.byte.b1 = mes[0] - '0';
			code1.byte.b2 = mes[1] - '0';
			code1.byte.b3 = mes[2] - '0';
			code1.byte.b4 = mes[3] - '0';
			code1.byte.b5 = mes[4] - '0';
			code1.byte.b6 = mes[5] - '0';
			code1.byte.b7 = mes[6] - '0';
			code1.byte.b8 = mes[7] - '0';
			fputc(code1.chhh, fpzip);
			j = 0;
		}
		j++;
	}
	//Записываем хвост
	j = 0;
	for (i = 0; i <= ts1; i++) {
		mes[j] = fgetc(fpout1);
		if (j == ts1)
		{
			code1.byte.b1 = mes[0] - '0';
			code1.byte.b2 = mes[1] - '0';
			code1.byte.b3 = mes[2] - '0';
			code1.byte.b4 = mes[3] - '0';
			code1.byte.b5 = mes[4] - '0';
			code1.byte.b6 = mes[5] - '0';
			code1.byte.b7 = mes[6] - '0';
			code1.byte.b8 = mes[7] - '0';
			fputc(code1.chhh, fpzip);
			fputc(' ', fpzip);
		}
		j++;
	}

	j = 0;
	for (i = 0; i < fsize2 - ts2; i++) {
		mes[j] = fgetc(fpout2);
		if (j == 7)
		{
			code1.byte.b1 = mes[0] - '0';
			code1.byte.b2 = mes[1] - '0';
			code1.byte.b3 = mes[2] - '0';
			code1.byte.b4 = mes[3] - '0';
			code1.byte.b5 = mes[4] - '0';
			code1.byte.b6 = mes[5] - '0';
			code1.byte.b7 = mes[6] - '0';
			code1.byte.b8 = mes[7] - '0';
			fputc(code1.chhh, fpzip);
			j = 0;
		}
		j++;
	}
	//Записываем хвост
	j = 0;
	for (i = 0; i <= ts2; i++) {
		mes[j] = fgetc(fpout2);
		if (j == ts2)
		{
			code1.byte.b1 = mes[0] - '0';
			code1.byte.b2 = mes[1] - '0';
			code1.byte.b3 = mes[2] - '0';
			code1.byte.b4 = mes[3] - '0';
			code1.byte.b5 = mes[4] - '0';
			code1.byte.b6 = mes[5] - '0';
			code1.byte.b7 = mes[6] - '0';
			code1.byte.b8 = mes[7] - '0';
			fputc(code1.chhh, fpzip);
			fputc(' ', fpzip);
		}
		j++;
	}
	fcloseall();
	return 0;
}