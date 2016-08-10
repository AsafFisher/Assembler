#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "legality.h"
#define FIRST_LABEL  1
#define LABEL_space 2
#define MAX_LABEL 30
#define LINE_MAX 80
enum{NO,YES};
enum{mov,cmp,add,sub,lea,not,clr,inc,dec,jmp,bne,red,prn,jsr,rts,stop,r0,r1,r2,r3,r4,r5,r6,r7};
char* actionsANDregisters[]={"mov","cmp","add","sub","lea","not","clr","inc","dec","jmp","bne","red","prn","jsr","rts","stop",
"r0","r1","r2","r3","r4","r5","r6","r7"};

int start_in_label ;
int has_cmd;
int cmd_type;

int is_legal_line(char *buf,int line);
int is_cmd_or_reg(char*temp);
int check_cmd(char*buf);
int check_label(char*buf);
int isCommentOrEmpty(char *buf);
int good_label(char*buf,int line);
int search_point_or_cmd(char * buf,int line);
int is_legal_directive(char*buf,int line);
int is_legal_data(char*buf,int line);
int is_legal_string(char*buf,int line);
int is_legal_ent_ext(char*buf,int line);
int is_legal_cmd(char*buf,int line);
int good_label2(char*buf);
int  two_oper(char*buf,int line);
int  one_oper(char*buf,int line);
int direct_addresing(char*buf);
int immediate_addresing(char*buf);
int dinamic_direct_addresing(char*buf);
int register_addresing(char*buf);

/*int main()
{
 char line1[80]="K: .data 22   ",line5[80]="MAIN: mov K      ",line4[80]="END: stop  ",line3[80]="STR: .string \"abcdef\"",line2[80]="LENGTH: .data 6,-9,15";
   if(is_legal_line(line1,1))
	printf("ok\n"); 
if(	is_legal_line(line2,2))
	printf("ok\n"); 
if(	is_legal_line(line3,3)) 
	printf("ok\n"); 
if(	is_legal_line(line4,4))
	printf("ok\n"); 

if(	is_legal_line(line5,5))
	printf("ok\n");  
printf("%s\n%s\n%s\n%s\n%s\n",line1,line2,line3,line4,line5);
	return 1;
}
*/
int is_legal_line(char *buf,int line)/*----------------check if the line is legal*/
{
 	has_cmd=cmd_type=start_in_label=NO; /*initialization of the global variables*/
 	if(isCommentOrEmpty(buf))/*    Comment Or Empty line */
 		return 1;
 	if(isalpha(buf[0]))      /*    letter in the first character - it can be label*/
 	{
 		if(!good_label(buf,line))/*  check if has use in register or command in label */
 			return 0;
 		else if (check_label(buf)==FIRST_LABEL)
 			start_in_label=YES;
 		else if	(check_label(buf)!=LABEL_space)
 		{
 			printf("error - label  must finish with : , line -%d\n",line);
 			return 0;
 		}	
 	}
 	if (!search_point_or_cmd(buf,line))/*if no label or there is legal label,we search point or coomand*/
 		return 0;
 	return 1;	
}

int good_label2(char*buf)/*----------------check if has use in register or command in label for operand part*/
{
	int i=0 ,j;
	char temp[LINE_MAX];/*temp for isolate the register word*/
 	strcpy(temp,buf);
    while(isalnum(buf[i]))
	 	i++;
	 if(i>MAX_LABEL) 
	 	return 0;
	 temp[i]='\0';
	j=(is_cmd_or_reg(temp));
 	if (j<r7)
 		return 0;
 	return 1;
}

int good_label(char*buf,int line)/*----------------check if has use in register or command in label for first part*/
{
	int i=0 ,j;
	char temp[LINE_MAX];
 	strcpy(temp,buf);
    while(isalnum(buf[i]))
	 	i++;
	 if(i>MAX_LABEL) 
	 	{
	 		printf("error-the maximum length of label it 30 line-%d\n",line);
	 		return 0;
	 	}
	 temp[i]='\0';
	j=(is_cmd_or_reg(temp));
 	if(j<=stop)
 		{
 			if(!isspace(buf[i]))
 			{
 				printf("error- worng using in command word %s,need space after command,line-%d\n",temp,line);
 				return 0;
 			}
 			has_cmd=YES;
 			cmd_type=j;	
 		}	
 	else if(j<=r7)
 	{
 		printf("error- worng using in register word %s,line-%d\n",temp,line);
 		return 0;
 	}			
	return 1;
}

int is_cmd_or_reg(char*temp)/*----------check if temp is string of command or register name */
{	 
	int i=0;
	while(i<=r7)
	{
 		if(!strcmp(temp,actionsANDregisters[i]))
 			return i;
 	    i++;
 	}
 	return i;
} 

int check_label(char*buf)/*---------------check the end of lable*/
{
 	int i=0;
 	while(isalnum(buf[i]))
 		i++;
 	if(buf[i]==' ')
 		return LABEL_space;
 	if(buf[i]==':')
 		 return FIRST_LABEL; 
 	return 0;
}
    
int isCommentOrEmpty(char *buf)/*---------------is Comment Or Empty?*/
{
	int i=0;
	int ch=*buf;
	if (ch==';')
		return 1;
	while(isspace(ch)&&ch!='\n'&&ch)
	{
		i++;
		ch=buf[i];
	}
	if(ch=='\n'||ch=='\0')
		return 1;
	return 0;
}

int search_point_or_cmd(char * buf,int line)/* -----------search point(for directive) or command*/
{
	int i=0;
	while(isspace(buf[i]))
		i++;
	if( (buf[i]!='.')&&(!isalpha(buf[i])) )
		{
			printf("error -line can start just with letter or '.' for directive ,line-%d\n",line);
 			return 0;
 		}
	if(start_in_label==YES)/*if line start in label skip on label*/
	{
		while(isalnum(buf[i]))
			i++;
		i++;	
	}
	while(isspace(buf[i]))
		i++;
	if(buf[i]=='.')/*has '.'  -check if is legal directive */
		return is_legal_directive(&buf[i+1],line);
	return is_legal_cmd(&buf[i],line);/*has letter,check if is legal cmd*/
}

int is_legal_directive(char*buf,int line)/*------------check the type of directive*/
{
	int i=0;
	char temp[LINE_MAX];
	strcpy(temp,buf);
	while(isalpha(buf[i]))
		i++;
	temp[i]='\0';
	if(!strcmp(temp,"data"))
			return is_legal_data(&buf[i],line);
	if(!strcmp(temp,"string"))
			return is_legal_string(&buf[i],line);
	if(!strcmp(temp,"entry")||!strcmp(temp,"extern"))
	{
		if(start_in_label==YES)
			printf("warning - entry/extern directive not need a lable before him, line-%d\n",line);
		return is_legal_ent_ext(&buf[i],line);
	}
	printf("error -no such directive name %s, line-%d\n",temp,line);
	return 0;
}

int is_legal_data(char*buf,int line)/*---------------------check if the parameters of data are legals*/
{
	int i=0;
	for(;;)/* loop until the end of the line to get numbers*/
	{
		while(isspace(buf[i]))
			i++;
		if(buf[i]=='+'||buf[i]=='-')
		{	
			if(!isdigit(buf[i+1]))
			{
				printf("error - you need digit after operator ,line%d\n",line);
				return 0;
			}
			i++;
		}
		if(isdigit(buf[i]))
		{	
			while(isdigit(buf[i]))
				i++;
			while(isspace(buf[i]))
				i++;  
			if(buf[i]=='\n'||buf[i]=='\0')
				return 1;
			if(buf[i]==',')
			{
				i++;
				continue;
			}
			printf("error - you need insert ',' after digit,line-%d\n",line);
			return 0;
		}
		printf("error - you need insert digit or opertor ,line-%d\n",line);
		return 0;
	}
}

int is_legal_string(char*buf,int line)/*-------------------check if the  string is legal*/
{
	int i=0;
	while(isspace(buf[i]))
			i++;
	if(buf[i]!='"')
	{
		printf("error -string must begin with - \" ,line-%d\n",line);
		return 0;
	}
	i++;
	while(buf[i]!='"')
		i++;
	i++;
	while(isspace(buf[i]))
			i++;
	if(buf[i]=='\n'||buf[i]=='\0')
		return 1;
	printf("error - you can't insert characters after the string ,line-%d\n",line);
	return 0;
}
			/*-------------------------------------check the lable after extern/entry directive and that has nothing after it*/
int is_legal_ent_ext(char*buf,int line)/*---if there is lable before the directive send warning measage because it unnecessary*/ 
{
 	int i=0;
	while(isspace(buf[i]))
			i++; 
	if(!isalpha(buf[i]))
	{
		printf("error - label must begin with letter ,line-%d\n",line);
		return 0;
	}
	while(isalnum(buf[i]))
		i++;
	if(buf[i]=='\n'||buf[i]=='\0')
		return 1;
	if(!isspace(buf[i]))
		printf("error - label can combined just with letters and digit ,line-%d\n",line);	
	while(isspace(buf[i]))
		i++;
	if(buf[i]=='\n'||buf[i]=='\0')
		return 1;
	printf("error - lable can't be with space,line-%d\n",line);
	return 0;
}	 
	
int is_legal_cmd(char*buf,int line)/*------------------check the command is legal ,and some operands is need*/
{
	int i=0;
	if(!good_label(buf,line))/*take the command name,and check that there is aspace after it*/
		return 0;
 	if(has_cmd==NO)
 	{	
 		printf("error - no such command ,line-%d\n",line);
 		return 0;
 	}
	while(isalpha(buf[i]))/*skip after the command name*/
		i++;
	while(isspace(buf[i]))
		i++;
	if(cmd_type<=lea)					/*if it 2 operands method*/
		return two_oper(&buf[i],line);	/*return if the operands are legal*/
	else if(cmd_type<=jsr)		/*if it 1 operand method*/
		return one_oper(&buf[i],line);/*return if the operand is legal*/
	if(buf[i]=='\n'||buf[i]=='\0')	/*stop or rts command */
		return 1;
	printf("error - you can't insert any character after command %s,line-%d\n",actionsANDregisters[cmd_type],line);
	return 0;	

}

int  two_oper(char*buf,int line)/*-----------------------search operands for two operands methods*/
{	
	int i=0,j=0;
	if(cmd_type==lea)/*the first operand in "lea" can be just direct addresing*/
	{
		if(direct_addresing(buf))
 			i=direct_addresing(buf);
	}
	else/*the first operand for the other two operands methods can be addresing method 0,1,2,3*/
	{
		if(direct_addresing(buf))
 			i=direct_addresing(buf);
		else if(immediate_addresing(buf))
			i=immediate_addresing(buf);
		else if(dinamic_direct_addresing(buf)) 
			i=dinamic_direct_addresing(buf); 
		else if(register_addresing(buf))
			i=register_addresing(buf);
	}		
	if(!i)/*there is no legal addresing method*/
	{
			printf("error - worng parameters in first operand for %s ,line-%d\n",actionsANDregisters[cmd_type],line);
			return 0;
	}
	if(buf[i]!=',')/*we need ',' before the second operand*/
	{
		printf("error -you need ',' after first parameter ,line-%d\n",line);
		return 0;
	}
	i++;
	while(isspace(buf[i]))
			i++;
	if(cmd_type==cmp)/*the second operand in cmp can be addresing method 0,1,2,3*/ 
	{
		if(direct_addresing(&buf[i]))
 			j=direct_addresing(&buf[i]);
		else if(immediate_addresing(&buf[i]))
			j=immediate_addresing(&buf[i]);
		else if(dinamic_direct_addresing(&buf[i])) 
			j=dinamic_direct_addresing(&buf[i]); 
		else if(register_addresing(&buf[i]))
			j=register_addresing(&buf[i]);
	}
	else /*the second operand for the other two operands methods can be addresing method  1,3*/
	{	
		if(direct_addresing(&buf[i]))
 			j=direct_addresing(&buf[i]);
		else if(register_addresing(&buf[i]))
			j=register_addresing(&buf[i]);	
	}
	if(!j)/*there is no legal addresing method*/
	{
		printf("error - worng parameters in second operand for %s ,line-%d\n",actionsANDregisters[cmd_type],line);
		return 0;	
	}			
	i+=j;
	if(buf[i]!='\n'&&buf[i]!='\0')	/*there is any character after 2 operands*/
	{
		printf("error - you can't insert any character after 2 operands of %s  ,line- %d\n",actionsANDregisters[cmd_type],line);
		return 0;
	}
	return 1;
}	

int  one_oper(char*buf,int line)/*--------------------------------------search operand for one operand methods*/
{
	int i=0;
	if(cmd_type==prn)/*the operand in prn can be addresing method 0,1,2,3*/ 
	{
		if(direct_addresing(buf))
 			i=direct_addresing(buf);
		else if(immediate_addresing(buf))
			i=immediate_addresing(buf);
		else if(dinamic_direct_addresing(buf)) 
			i=dinamic_direct_addresing(buf); 
		else if(register_addresing(buf))
			i=register_addresing(buf);
	}
	else/*the operand for the other one operand methods can be addresing method  1,3*/
	{
		if(direct_addresing(buf))
 			i=direct_addresing(buf);
		else if(register_addresing(buf))
			i=register_addresing(buf);
	}	
	if(!i)/*there is no legal addresing method*/
	{
			printf("error - worng parameters for operand for %s command,line-%d\n",actionsANDregisters[cmd_type],line);
			return 0;
	}
	if(buf[i]!='\n'&&buf[i]!='\0')	/*there is any character after the operand*/
	{
		printf("error - you can't insert any characters after the operand of %s ,line- %d\n",actionsANDregisters[cmd_type],line);
		return 0;
	}
	return 1;

}
			/*-----------------------------------check if there is direct addresing(addresing method 1)if there is-return the*/
int direct_addresing(char*buf)/*---------------------------------index of the next character after it*/
{
	int i=0;
	if(!good_label2(buf))/*the lable is not register or command*/
		return 0;
	if(isalpha(buf[i]))/*stat with letter*/
	{
		while(isalnum(buf[i]))
			i++;
		if(buf[i]=='[')/*it not dinamic direct*/ 
			return 0;
		while(isspace(buf[i])) 
			i++;
		return i;
	}
	return 0;
}
								
		/*------------------------------------check if there is immediate addresing(addresing method 0)if there is-return the*/ 
int immediate_addresing(char*buf)/*-------------------------------------index of the next character after it*/
{
	int i=0;
	if(buf[i]=='#')
	{
		i++;
		if(buf[i]=='+'||buf[i]=='-')/*optional opertor*/
			i++;
		if(!isdigit(buf[i]))
			return 0;
		while(isdigit(buf[i]))
			i++;
		while(isspace(buf[i]))
			i++;
		return i;
	}
	return 0;
}
             /*--------------------------check if there is dinamic direct addresing(addresing method 2)if there is-return the*/ 
int dinamic_direct_addresing(char*buf) /*-----------------------------index of the next character after it*/ 			
{	
	int i=0;
	if(!good_label2(buf))/*the lable is not register or command*/
		return 0;
	if(isalpha(buf[i]))/*stat with letter*/
	{
		while(isalnum(buf[i]))
			i++;
		if(buf[i]!='[')/*search pattern:lable[number-number]*/ 
			return 0;
		i++;
		if(!isdigit(buf[i]))
			return 0;
		while(isdigit(buf[i]))
			i++;
		if(buf[i]!='-')
			return 0;
		i++;
		if(!isdigit(buf[i]))
			return 0;
		while(isdigit(buf[i]))
			i++;
		if(buf[i]!=']')
			return 0;	
		i++;
		while(isspace(buf[i]))
			i++;
		return i;	
	}
	return 0;
}
				/*----------------check if there is legal register(addresing method 3)if there is-return the index of the next*/
int register_addresing(char*buf)  /*--------------------------------------- character after the register*/
{
	int i=0,j=0;
	char temp[LINE_MAX];/*isolate the register word*/
 	strcpy(temp,buf);
    while(isalnum(buf[i]))
	 	i++;
	temp[i]='\0';
	j=(is_cmd_or_reg(temp));
	if(j<r0||j>r7)/*if is illegal register*/
		return 0;
	while(isspace(buf[i]))
			i++;
		return i;	
}
			
			







 