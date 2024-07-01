#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#ifdef _WIN32
#include<io.h>
#define isatty _isatty
#define STDIN "CON:"
#else
#include<unistd.h>
#define STDIN "/dev/tty"
#endif
long long *pointer,copy;
char *code,isCopied=0,exitCode=0;
size_t len=1,lenCode=0,codeLocation=0,pointerLocation=0;
FILE *input;
void discard(char *array,char times,FILE *file){
	while(times--&&(file==stdin&&isatty(0)?array[2]!='\n':!feof(file)))
		array[0]=array[1],array[1]=array[2],array[2]=getc(file);
	return;
}
void appendCode(char command){
	if(lenCode)
		code=realloc(code,lenCode+1);
	else
		code=malloc(1);
	code[lenCode++]=command;
	return;
}
void read0(FILE *file){
	if(lenCode)
		free(code),lenCode=0;
	char command[4];
	for(command[3]=0,discard(command,3,file);file==stdin&&isatty(0)?command[2]!='\n':!feof(file);discard(command,1,file))
		if(!strcmp(command,"moo"))
			appendCode(0),discard(command,2,file);
		else if(!strcmp(command,"mOo"))
			appendCode(1),discard(command,2,file);
		else if(!strcmp(command,"moO"))
			appendCode(2),discard(command,2,file);
		else if(!strcmp(command,"mOO"))
			appendCode(3),discard(command,2,file);
		else if(!strcmp(command,"Moo"))
			appendCode(4),discard(command,2,file);
		else if(!strcmp(command,"MOo"))
			appendCode(5),discard(command,2,file);
		else if(!strcmp(command,"MoO"))
			appendCode(6),discard(command,2,file);
		else if(!strcmp(command,"MOO"))
			appendCode(7),discard(command,2,file);
		else if(!strcmp(command,"OOO"))
			appendCode(8),discard(command,2,file);
		else if(!strcmp(command,"MMM"))
			appendCode(9),discard(command,2,file);
		else if(!strcmp(command,"OOM"))
			appendCode(10),discard(command,2,file);
		else if(!strcmp(command,"oom"))
			appendCode(11),discard(command,2,file);
	return;
}
void append(char front){
	pointer=realloc(pointer,(len+1)*sizeof(long long));
	if(front){
		for(size_t i=len++;i--;pointer[i+1]=pointer[i]);
		pointer[0]=0;
	}else
		pointer[len++]=0;
	return;
}
char switchCase(long long val,char mode){
	switch(val){
		case 0:
			if(codeLocation<2){
				exitCode=1,fprintf(stderr,"\nError at locaton %u: Missing 'MOO'!\n",codeLocation);
				return 0;
			}
			size_t debug=codeLocation;//Save current pointer location for debug purposes
			codeLocation-=2;//Decrease the pointer indicator value
			for(size_t i=1;i;codeLocation--)//Go back until reaching a matching `[`
				if(!codeLocation&&(code[codeLocation]!=7||i>1)){//Halt if there's no matching `[`
					exitCode=1,fprintf(stderr,"\nError at locaton %u: Missing 'MOO'!\n",debug);
					return 0;
				}else if(code[codeLocation]==7)//(Matching `[`)
					i--;
				else if(!code[codeLocation])//(Nested `]`)
					i++;
			break;
		case 1:
			if(!pointerLocation--)//Add a new pointer to the front if needed
				append(1),pointerLocation++;
			break;
		case 2:
			if(++pointerLocation==len)//Add a new pointer to the back if needed
				append(0);
			break;
		case 3:
			if(mode)
				return 0;
			switchCase(pointer[pointerLocation],1);
			break;
		case 4:
			if(pointer[pointerLocation])
				putchar(pointer[pointerLocation]);
			else
				pointer[pointerLocation]=getc(input);
			break;
		case 5:
			pointer[pointerLocation]--;
			break;
		case 6:
			pointer[pointerLocation]++;
			break;
		case 7:
			if(!pointer[pointerLocation]){
				if(codeLocation+2==lenCode){
					exitCode=1,fprintf(stderr,"\nError at locaton %u: Missing 'moo'!\n",codeLocation);
					return 0;
				}
				size_t debug=codeLocation;//Save current pointer location for debug purposes
				codeLocation+=2;//Increase the pointer indicator value
				for(size_t i=1;i;codeLocation++)//Skip part of the code until reaching a matching `]`
					if(codeLocation+1==lenCode&&(code[codeLocation]!=0||i>1)){//Halt if there's no matching `]`
						exitCode=1,fprintf(stderr,"\nError at locaton %u: Missing 'moo'!\n",debug);
						return 0;
					}else if(code[codeLocation]==7)//(Nested `[`)
						i++;
					else if(!code[codeLocation])//(Matching `]`)
						i--;
				codeLocation--;//Decrease the pointer indicator value
			}
			break;
		case 8:
			pointer[pointerLocation]=0;
			break;
		case 9:
			if(isCopied)
				pointer[pointerLocation]=copy;
			else
				copy=pointer[pointerLocation];
			isCopied=!isCopied;
			break;
		case 10:
			printf("%lli\n",pointer[pointerLocation]);
			break;
		case 11:
			fscanf(input,"%lli",&pointer[pointerLocation]);
			break;
		default:
			return 0;
	}
	return 1;
}
void execute(){
	while(codeLocation<lenCode&&switchCase(code[codeLocation],0))//Read a part of code each time
		codeLocation++;
	return;
}
int main(int argc,char** argv){
	setbuf(stdout,0),input=fopen(STDIN,"r");
	if(argc>1){
		FILE *file=fopen(argv[1],"r");//Open the file
		if(file)
			read0(file),fclose(file);
	}
	pointer=malloc(sizeof(long long)),pointer[0]=0;
	if(!lenCode&&isatty(0)){//(shell mode)
		printf("Entered shell-mode!");//Notify
		while(1)
			printf("\n>>> "),read0(stdin),execute(),codeLocation=0;//Prompt
	}else if(!lenCode)
		read0(stdin);
	execute(),free(code),free(pointer),fclose(input);
	return exitCode;
}
