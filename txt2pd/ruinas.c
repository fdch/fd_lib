#include <stdio.h>
#include <ctype.h>

#define	IN 	1
#define	OUT	0
#define TOTDUR 6.43
int lcount = 0;
int gc=0, gnl=0, gnw=0, totwords=0;
float gtime=0;

int main()	{

	FILE *my_file;
	//FILE *output;
	my_file = fopen("ruinas.txt","r");
	//output = fopen("output-1.txt","w");
	int c, nl, nw, nc;
	nw = nc = 0;
	nl = 0;
	float time = 0,tscale=0;
	int state = OUT;
    while ( (c = getc(my_file)) != EOF)
    { //within the text
    	nc++;//there is one new character
    	
		if (c == '\n') {//if you find a paragraph
			nl++;
			if(gnw!=0){
			totwords+=gnw;
			float ratio = (float)gnw/1964;
			time = TOTDUR*ratio;
			tscale = gtime+=time;
			printf("prg:%d,words:%d,ratio:%f,time(min):%f,tscale:%f\n",nl,gnw,ratio,time,tscale);
			}
			gnw=0;//there is one new paragraph
			nw=0;
		}
		
		if (c == ' ' || c == '\n' || c == '\t' || c == '.' || c == ',' || c == ';' || c == ':' || c == '(' || c == ')' || c == '-') {	//if you are not inside a word (in a paragraph, a space or a tab)
			state  = OUT;//then you are not a word
			
		}
		
		else if (state == OUT) {//Otherwise, if you are not a word
			//if (lcount != 0)
			//fprintf(output,"%d\n", lcount);
			
			state = IN;//BECOME A WORD
			
			gnw++;//there is a new word
			
			lcount = 0;
		}
		if  (state == IN) {//If you are inside a word
			++lcount;//for how many characters do you stay a word?
		}
    }
    printf("parraf:%d palabr:%d charac:%d,TOTDUR:%d\n", nl, totwords+gnw, nc,TOTDUR);

	fclose(my_file);
    return 0;
}