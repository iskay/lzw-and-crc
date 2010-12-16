/*LZW compression -- Ian Kay, Oct 6, 2008*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CODE_LEN 12
#define TABLE_SIZE (1<<CODE_LEN)	/*2^12 = 4096*/
#define NUM_CODE TABLE_SIZE-1		/*number of available codes*/

int Search_Dict(unsigned int search_for, int next_code);
int dictionary[TABLE_SIZE];

int main()
{
	unsigned int c;				/*character read from input*/
	unsigned int prefix=0;		/*prefix part of code*/
	unsigned int string=0;		/*holds <prefix><c>*/
	int next_code=256;			/*next available code*/
	int index;
	char input_file[20];
	FILE *input, *output;

	int i;
	for(i=0; i<next_code; i++) dictionary[i] = i;
	printf("Input file: ");
    scanf("%s",input_file);
	input = fopen(input_file, "rb");
	output = fopen("output.txt", "w");

	if(input==NULL || output==NULL)
	{
		printf("Cannot open files.\n");
		exit(1);
	}

	/*read first character*/
	prefix = fgetc(input);

	/*for each remaining input character*/
	while(1)
	{
		c = fgetc(input);
		if(c==EOF) break;

		/*string = <prefix><c>*/
		string = prefix << 8;
		string = string | c;
		
		index = Search_Dict(string, next_code);
		if(index != -1)
		{
			/*already in dictionary*/
			prefix = index; 
		}
		else
		{
			/*not yet in dictionary*/
			if(next_code <= NUM_CODE)
			{
				dictionary[next_code] = string;
				next_code++;
			}
			fprintf(output, "%d\n", prefix);
			prefix = c;
		}
	}

	/*write last code*/
	fprintf(output, "%d\n", prefix);

	fclose(input);
	fclose(output);

	return 0;
}


int Search_Dict(unsigned int search_for, int next_code)
{
	int i;
	for(i=0; i<next_code; i++)
	{
		/*already in dictionary, return its index*/
		if(dictionary[i] == search_for) return i;	
	}

	/*not yet in dictionary*/
	return -1;
}
