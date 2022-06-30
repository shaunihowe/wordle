#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

enum res_t {GREEN,YELLOW,GREY};

typedef struct {
	char word[6];
	int score;
	int active;
} word_t;
int words;
int numactive;
word_t wordlist[15000];

int loadwords(char *filename)
{
	FILE *wordfile;
	char input[2048];

	if ((wordfile = fopen(filename, "r")) == NULL)
		fclose(wordfile);

	while (fgets(input, 2048, wordfile))
	{
		sscanf(input, "%s", wordlist[words].word);
		wordlist[words].score = 0;
		wordlist[words].active = 1;
		words++;
	};
	numactive = words;

	fclose(wordfile);
	return words;
}

void resetwords()
{
	int word;
	for (word = 0; word < words; word++)
	{
		wordlist[word].active = 1;
		wordlist[word].score = 0;
	}
	numactive = words;
}

void addletter(int letter, char alpha, enum res_t response)
{
	int word, letter2, inword;
	numactive = 0;
	for (word = 0; word < words; word++)
	{
		if (response == GREEN)
		{
			if (wordlist[word].word[letter] != alpha)
				wordlist[word].active = 0;
		}
		else if (response == YELLOW)
		{
			if (wordlist[word].word[letter] == alpha)
				wordlist[word].active = 0;
			else
			{
				inword = 0;
				for (letter2 = 0; letter2 < 5; letter2++)
					if (wordlist[word].word[letter2] == alpha)
						inword = 1;
				if (inword == 0)
					wordlist[word].active = 0;
			}
		}
		else if (response == GREY)
		{
			if (wordlist[word].word[letter] == alpha)
				wordlist[word].active = 0;
			else
			{
				inword = 0;
				for (letter2 = 0; letter2 < 5; letter2++)
					if (wordlist[word].word[letter2] == alpha)
						inword = 1;
				if (inword == 1)
					wordlist[word].active = 0;
			}
		}
		if (wordlist[word].active == 1)
			numactive++;
	}
	return;
}

int recalcscores()
{
	int letter, alpha, word, bestword, bestscore;
	int letterscore[5][256];
	int wordscore[256];

	for (letter = 0; letter < 5; letter++)
		for (alpha = 0; alpha < 256; alpha++)
			letterscore[letter][alpha] = 0;

	for (word = 0; word < words; word++)
		if (wordlist[word].active == 1)
			for (letter = 0; letter < 5; letter++)
				letterscore[letter][wordlist[word].word[letter]]++;

	bestword = 0;
	bestscore = 0;
	for (word = 0; word < words; word++)
	{
		for (alpha = 0; alpha < 256; alpha++)
			wordscore[alpha] = 0;

		for (letter = 0; letter < 5; letter++)
			if (letterscore[letter][wordlist[word].word[letter]] > wordscore[wordlist[word].word[letter]])
				wordscore[wordlist[word].word[letter]] = letterscore[letter][wordlist[word].word[letter]];

		wordlist[word].score = 0;
		for (alpha = 0; alpha < 256; alpha++)
			wordlist[word].score+= wordscore[alpha];
		if (wordlist[word].active == 0)
			wordlist[word].score = 0;
		if (wordlist[word].score > bestscore)
		{
			bestword = word;
			bestscore = wordlist[word].score;
		}
	}

	return bestword;
}

int simulate(char *startword, char *findword)
{
	int guess = 0, done = 0;
	int bestword, bestsco = 0;
	int letter, letter2, inword;
	char *word;
	resetwords();
	word = startword;
	printf("words  guess  score\n-----  -----  -----\n");
	do
	{
		printf("%5i  ",numactive);
		guess++;
		if (!strcmp(word, findword))
			done = 1;
		for (letter = 0; letter < 5; letter++)
		{
			if (word[letter] == findword[letter])
			{
				addletter(letter, word[letter], GREEN);
				printf("\033[30;42m");
			}
			else
			{
				inword = 0;
				for (letter2 = 0; letter2 < 5; letter2++)
					if (word[letter] == findword[letter2])
						inword = 1;
				if (inword == 1)
				{
					addletter(letter, word[letter], YELLOW);
					printf("\033[30;43m");
				}
				else
				{
					addletter(letter, word[letter], GREY);
					printf("\33[0;0m");
				}
			}
			printf("%c", word[letter]);
		}
		printf("\33[0;0m%7i\n",bestsco);
		bestword = recalcscores();
		bestsco = wordlist[bestword].score;
		word = wordlist[bestword].word;
	} while (done == 0);
	return guess;
}

int main(int argc, char *argv[])
{
	int word;
	srand(time(NULL));
	loadwords("words.txt");
	resetwords();
	recalcscores();
	if (argc == 2)
	{
		do {word = rand() % words;}while (wordlist[word].score < 9000);
		return simulate(wordlist[word].word,argv[1]);
	}
	if (argc == 3)
		return simulate(argv[1],argv[2]);
	printf("Usage:\n%s <startingguess> <findword>\n",argv[0]);
	printf("---- Or ----\n");
	printf("%s <findword>\n",argv[0]);
	return 0;
}

