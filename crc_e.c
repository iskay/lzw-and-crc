/*CRC Encoder - Ian Kay
/*Output file is specified by first cmd line argument, or leave blank for standard out.
/*Input word is read as text and converted to a binary number.
/*Division is done by a combination of XOR and shifting (more details below).
/*Output word is converted from binary back to text.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INFO_BITS	16		/*number of info bits*/
#define CRC_BITS    5		/*number of CRC bits (must be one less than the order of G(x))*/
#define G_OF_X      0x2B    /*G(x) expressed in binary, x^5 + x^3 + x + 1 => 0010 1011*/
#define GEN_POLY    G_OF_X << (INFO_BITS - (CRC_BITS + 1))      /*the division algorithm requires the divisor to be in a form
																/*where its MSB is aligned with the MSB of the dividend*/

int getInputCode(FILE *input, unsigned int *code);
unsigned int calculateCRC(unsigned int msg, int msg_len, unsigned int gen_poly);
void printOutputCode(FILE *out, unsigned int word, int word_len);

int main(int argc, char *argv[])
{
    unsigned int info;			/*holds the input word*/
    unsigned int crc;			/*holds the crc after it has been calculated*/
    unsigned int out_code;		/*holds combined input+crc code*/
    FILE *out_file;

    /*open output file*/
	if(argc == 1) out_file = stdout;
	else out_file = fopen(argv[1], "w");

    if(out_file==NULL) {
        printf("Cannot create output file.\n");
        exit(1);
    }
    fprintf(stderr, "Working...\n");
    
	/*main loop*/
	while(1)
    {
        /*read next input word, stop in case of end of file or bad input*/
        if( !getInputCode(stdin, &info)) break;
      
        /*calculate the CRC*/
        crc = calculateCRC(info, INFO_BITS, GEN_POLY);

        /*combine the info + the crc to make the output code*/
        out_code = info << CRC_BITS;
        out_code = out_code | crc;

        /*output result*/
        printOutputCode(out_file, out_code, INFO_BITS + CRC_BITS);
    }

    fprintf(stderr, "Done.\n");
    fclose(out_file);
    return 0;
}

/*reads word from text file and converts it to binary. Stores result in location *code.
/*returns 0 if end of file has been reached or error has occured, otherwise returns 1.*/
int getInputCode(FILE *input, unsigned int *code)
{
    int i;
    char in_buf[INFO_BITS+2];		/*needs space to hold a newline char plus a string terminator char*/
    unsigned int tmp_code=0;

    fgets(in_buf, INFO_BITS+2, input);
	if(feof(input) || in_buf[0] == '\n') return 0;
    for(i=0; i<INFO_BITS; i++)
    {
		/*create a binary number corresponding to the text string by setting the
		/*appropriate bits one at a time, starting with MSB*/
        if(in_buf[i] == '1') tmp_code = tmp_code | (1<<(INFO_BITS-1 - i));
        else if(in_buf[i] == '0') {/*do nothing*/}
        else {
            fprintf(stderr, "Invalid input, exiting...\n");
            return 0;
        }
    }
	/*store the result*/
    *code = tmp_code;
    return 1;
}

/*calculates the CRC of msg and returns the result as an int.*/
unsigned int calculateCRC(unsigned int msg, int msg_len, unsigned int gen_poly)
{
    unsigned int rem;					/*holds the remainder of each division step.
										/*when all steps have been completed, the final remainder will be left*/
    int msb = 1 << (msg_len - 1);		/*will be used to test whether the MSB of rem is 1 or 0 at each step*/

    rem = msg;
	int i;
    for(i=msg_len; i>0; i--)	/*division loop*/
    {
        if(rem & msb) rem = (rem ^ gen_poly);	/*check if the MSB of rem is a 1, if so, do mod-2 addition (XOR) with divisor*/
		rem = rem << 1;							/*bring down next bit of msg into place to be divided.*/
    }
	/*right now, the MSB of the remainder is aligned with the MSB of the msg.
	/*to finish, the remainder must be shifted down to the lowest-order bit locations*/
    rem = rem >> (INFO_BITS - CRC_BITS);
    return rem;
}

/*converts a binary number to text and writes result to file.*/
void printOutputCode(FILE *out, unsigned int word, int word_len)
{
    unsigned int tmp;
	
	int i;
    for(i=word_len-1; i>=0; i--)
    {
        tmp = word;
        tmp = 1 & (tmp>>i);			/*isolate each bit one at a time, starting with MSB*/
        if(tmp) fputc('1', out);
        else fputc('0', out);
    }  
    fputc('\n', out);
}
