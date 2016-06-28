#include<iostream>//输入输出流
#include <cstdlib>//生成随机数，malloc函数
#include<cmath>//数学公式包括根号
#include<cstdio>//cin.get字符数组初始化
#include<iomanip>//控制小数位数
#include<time.h>//时间种子生成随机数
#include<fstream>//文件输入输出
#include <string>//字符串类
#include<vector>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
using namespace std;

class infoofaline
{
public:
	char instr[80];
	int PCaddr;
	int type;//0=R 1=I 2=Br 3=J
	char op[6];
	int rs;
	int rt;
	int rd;
	int shamt;
	int imm_addr;//16位立即数数值
	char tag[20];//lable
	char target[20];//beq的lable
	char immchar[20];//16位立即数的char
	char mc[33];
	char error[33];

	infoofaline();//构造函数
	void init(char*,int addrs);
	void reload(char line[80],int linenum);//重载下一行指令
	void opload();
	int typeload();
	void regload();
		void immload(int immlocat);
		int trans_regchar_to_regint(char* regchar);
	void dataload();
		void addzero(char* T,int targetlenth,int lengthnow);
	void targetload();
};

void infoofaline::targetload()
{
	if(type!=3){return;}
	bool judge;
	int j;//ins内字符指针
	int tlocate = 0;
	for(int i=0;instr[i];i++)
	{
		if(instr[i]=='j' && !strcmp(op,"j")){j=i+2;break;}
		else if(instr[i]=='j' && !strcmp(op,"jal")){j=i+4;break;}
	}
	//judgeset
	judge = ((instr[j] >= 48 && instr[j] <= 57) || 
			(instr[j] >= 65 && instr[j] <= 90) || 
			(instr[j] >= 97 && instr[j] <= 122) || 
			 instr[j] == 95);
	while(!judge && instr[j]!='\0')
	{
	j++;
	judge = ((instr[j] >= 48 && instr[j] <= 57) || 
			(instr[j] >= 65 && instr[j] <= 90) || 
			(instr[j] >= 97 && instr[j] <= 122) || 
			 instr[j] == 95);
	}
	while(judge)
	{
	target[tlocate] = instr[j];
	j++;tlocate++;
	judge = ((instr[j] >= 48 && instr[j] <= 57) || 
			(instr[j] >= 65 && instr[j] <= 90) || 
			(instr[j] >= 97 && instr[j] <= 122) || 
			 instr[j] == 95);
	}
	return;
}


void infoofaline::init(char* i,int addrs)
{
	memset(op,0,6);
	memset(target,0,20);
	memset(immchar,0,20);
	memset(mc,0,33);
	memset(error,0,33);
	memset(tag,0,20);
	rs = rd = rt = type = shamt = imm_addr = 0;
	strcpy_s(instr,i);
	PCaddr = 4*addrs;
	return;
}

//构造函数
infoofaline::infoofaline()
{
	memset(instr,0,80*sizeof(char));
	PCaddr = 0;
	type = 0;
	memset(op,0,6*sizeof(char));
	rs = 0;
	rt = 0;
	rd = 0;
	shamt = 0;
	imm_addr = 0;
	memset(tag,0,20*sizeof(char));
	memset(target,0,20*sizeof(char));
	memset(immchar,0,20*sizeof(char));
	memset(mc,0,33*sizeof(char));
	memset(error,0,33*sizeof(char));



}

//完成机器码部分 除j/jal的后26位 Br的target
void infoofaline::dataload()
{
	char Top[7],Trs[6],Trd[6],Trt[6],Tshamt[6],Tfunct[7],Timm[17],Timm32[33];
	char numbit[5];
	int Numbit = 0;
	memset(Top,0,7);memset(Trs,0,6);memset(Trd,0,6);
	memset(Trt,0,6);memset(Tshamt,0,6);memset(Tfunct,0,7);
	memset(Timm,0,17);memset(Timm32,0,33);memset(numbit,0,5);

	if(type==0)//R
	{
		_itoa_s(rs,Trs,2);
		_itoa_s(rt,Trt,2);
		_itoa_s(rd,Trd,2);
		_itoa_s(shamt,Tshamt,2);
		strcpy_s(Top,"000000");
		if(!strcmp(op,"jr")){strcpy_s(Tfunct,"001000");}
		else if(!strcmp(op,"jalr")){strcpy_s(Tfunct,"001001");}
		else if(!strcmp(op,"add")){strcpy_s(Tfunct,"100000");}
		else if(!strcmp(op,"addu")){strcpy_s(Tfunct,"100001");}

		else if(!strcmp(op,"sub")){strcpy_s(Tfunct,"100010");}
		else if(!strcmp(op,"subu")){strcpy_s(Tfunct,"100011");}
		else if(!strcmp(op,"and")){strcpy_s(Tfunct,"100100");}
		else if(!strcmp(op,"or")){strcpy_s(Tfunct,"100101");}

		else if(!strcmp(op,"xor")){strcpy_s(Tfunct,"100110");}
		else if(!strcmp(op,"nor")){strcpy_s(Tfunct,"100111");}
		else if(!strcmp(op,"slt")){strcpy_s(Tfunct,"101010");}
		else if(!strcmp(op,"sll")){strcpy_s(Tfunct,"000000");}

		else if(!strcmp(op,"srl")){strcpy_s(Tfunct,"000010");}
		else if(!strcmp(op,"sra")){strcpy_s(Tfunct,"000011");}
		else if(!strcmp(op,"nop")){strcpy_s(Tfunct,"000000");}
		else {strcpy_s(error, "ins error");}
		if(!strlen(error))
		{
			strcat_s(mc,Top);//
			addzero(Trs,5,strlen(Trs));strcat_s(mc,Trs);
			addzero(Trt,5,strlen(Trt));strcat_s(mc,Trt);
			addzero(Trd,5,strlen(Trd));strcat_s(mc,Trd);
			addzero(Tshamt,5,strlen(Tshamt));strcat_s(mc,Tshamt);
			strcat_s(mc,Tfunct);
			return;	
		}
		else{strcpy_s(mc,error);return;}
	}

	else if (type==1)//I
	{
		_itoa_s(rs,Trs,2);
		_itoa_s(rt,Trt,2);
		//set immchar[20]
		if(immchar[0]=='0' && immchar[1]=='x')//16进制处理
		{
			int n = 0;
			char temp[17];
			memset(temp,0,17);
			for(n=2;immchar[n]=='0' && immchar[n]!='\0';n++){}
			for(;immchar[n]!='\0';n++)
			{
				if(immchar[n]<=70 && immchar[n]>=65){Numbit = immchar[n]-55;}//小写
				else if(immchar[n]<=57 && immchar[n]>=48){Numbit = immchar[n]-48;}//数
				else if(immchar[n]<=102 && immchar[n]>=97){Numbit = immchar[n]-87;}//大写
				else {strcpy_s(error,"num error");}
				_itoa_s(Numbit,numbit,2);
				addzero(numbit,4,strlen(numbit));
				strcat_s(temp,numbit);
			}
			strcpy_s(immchar,temp);
			addzero(immchar,16,strlen(immchar));
		}
		else//10进制处理
		{
			if(imm_addr<0)
			{
				_itoa_s(imm_addr,Timm32,2);
				for(int w=16;w<32;w++)
				{immchar[w-16] = Timm32[w];	}
				immchar[16] = '\0';
			}
			else
			{
				_itoa_s(imm_addr,immchar,2);
				addzero(immchar,16,strlen(immchar));
			}
		}
		//set immchar over
			if(!strcmp(op, "lw")) strcpy_s(Top, "100011");
			else if(!strcmp(op, "sw"))    {strcpy_s(Top, "101011");}
	 		else if(!strcmp(op, "lui"))   {strcpy_s(Top, "001111");}
	 		else if(!strcmp(op, "addi"))  {strcpy_s(Top, "001000");}
	 		else if(!strcmp(op, "addiu")) {strcpy_s(Top, "001001");}
	 		else if(!strcmp(op, "ori"))   {strcpy_s(Top, "001101");}
	 		else if(!strcmp(op, "andi"))  {strcpy_s(Top, "001100");}
	 		else if(!strcmp(op, "slti"))  {strcpy_s(Top, "001010");}
	 		else if(!strcmp(op, "sltiu")) {strcpy_s(Top, "001011");}
			else {strcpy_s(error, "ins error");}
			strcat_s(mc, Top);
			addzero(Trs,5,strlen(Trs)); strcat_s(mc, Trs);
			addzero(Trt,5,strlen(Trt)); strcat_s(mc, Trt);
			strcat_s(mc,immchar);
	}


	else if (type==2)//Br
	{
		_itoa_s(rs, Trs, 2);
		_itoa_s(rt, Trt, 2);
		if(!strcmp(op, "beq")) {strcpy_s(Top, "000100");}
		else if(!strcmp(op, "bne"))  {strcpy_s(Top, "000101");}
		else if(!strcmp(op, "blez")) {strcpy_s(Top, "000110");}
		else if(!strcmp(op, "bgtz")) {strcpy_s(Top, "000111");}
		else if(!strcmp(op, "bgez")) {strcpy_s(Top, "000001");}
		else {strcpy_s(error, "ins error");}
	 	strcat_s(mc, Top);
	 	addzero(Trs, 5, strlen(Trs)); strcat_s(mc, Trs);
	 	addzero(Trt, 5, strlen(Trt)); strcat_s(mc, Trt);
	}

	else if (type==3)//J
	{
		if(!strcmp(op, "j")) {strcpy_s(Top, "000010");}
		else if(!strcmp(op, "jal"))   {strcpy_s(Top, "000011");}
		else {strcpy_s(error, "ins error");}
		strcat_s(mc,Top);
	}
	else{strcpy_s(error,"type error"); return;}//error
}

//补零到指定长度
void infoofaline::addzero(char* T,int targetlenth,int lengthnow)
{
	int addlength = targetlenth - lengthnow;
	char *temp = new char[targetlenth+1];
	memset(temp,0,targetlenth+1);
	memset(temp,'0',addlength);
	strcat(temp,T);
	strcpy(T,temp);
	delete temp;
	return;
}



//immlocat为在instr中立即数开始的顺位
void infoofaline::immload(int immlocat)
{
	int i = immlocat;
	int minus = 0;
	while((instr[i]<48 || instr[i]>57) && (instr[i] != '-') && (instr[i] != '\0')  )
	{i ++;}

	if(instr[i] == '-') {minus = 1; i++;}

	else if(instr[i] == '0' && (instr[i+1] == 'X' || instr[i+1] == 'x'))
	{
		strcpy_s(immchar, "0x");
		for(int j = i+2; 
			(instr[j] >= 48 && instr[j] <= 57) || //16进制数
			(instr[j] >= 65 && instr[j] <= 70) || 
			(instr[j] >= 97 && instr[j] <= 102); j ++)
		{immchar[j-i] = instr[j]; immchar[j-i+1]='\0';}
		return;
	}

	while(instr[i] >= 48 && instr[i] <= 57) //十进制数
	{
		imm_addr = 10*imm_addr + (instr[i]-48);
		i++;
	}

	if(minus) 
	{imm_addr = -imm_addr;}

	return;
}

//寄存器load,j,jal,nop无寄存器不作处理
void infoofaline::regload()
{
	int regamount = 0;//表征寄存器顺序
	int locat = 0;
	int regnum = 0;//寄存器序号
	char regchar[5];
	bool judge = 0;
	int reglocat = 0;

	for(int i=0;instr[i]!='\0';i++)
	{
	if(instr[i]=='$')
	{
		regamount++;
		reglocat = 0;
		regchar[0]='\0';
		for(locat = i+1; (instr[locat]>47 && instr[locat]<58) || 
				(instr[locat]>96 && instr[locat]<123); locat++) 
			{
			regchar[reglocat] = instr[locat];
			reglocat ++;
			}		
		regchar[reglocat] = '\0';
		
		
		if(type==0)//R
		{
			if(!strcmp(op,"nop"))
			{
			for(int t=0;t<32;t++)
			{mc[t]='0';}
			}
			else if(!strcmp(op,"jr"))
			{
				rd = 0; rt = 0; shamt = 0;
				if(regamount==1){rs = trans_regchar_to_regint(regchar);}
			}

			else if (!strcmp(op,"jalr"))
			{
				rt = 0; shamt = 0;
				if(regamount==1){rd = trans_regchar_to_regint(regchar);}
				if(regamount==2){rs = trans_regchar_to_regint(regchar);}
			}

			else if(!strcmp(op,"add")||!strcmp(op,"addu")||
					!strcmp(op,"sub")||!strcmp(op,"subu")||
					!strcmp(op,"and")||!strcmp(op,"or")||
					!strcmp(op,"xor")||!strcmp(op,"nor")||
					!strcmp(op,"slt"))
			{
				shamt = 0;
				if(regamount==1){rd = trans_regchar_to_regint(regchar);}
				if(regamount==2){rs = trans_regchar_to_regint(regchar);}
				if(regamount==3){rt = trans_regchar_to_regint(regchar);}
			}

			else if(!strcmp(op,"sll")||!strcmp(op,"srl")||
					!strcmp(op,"sra"))
			{
				rs = 0;
				if(regamount==1){rd = trans_regchar_to_regint(regchar);}
				if(regamount==2)//默认移位的量为十进制
				{
					rt = trans_regchar_to_regint(regchar);
					int tempi = i+3;
					while((instr[tempi]<48||instr[tempi]>57) && instr[tempi]!='\0')
					{tempi++;}
					while((instr[tempi]>=48 && instr[tempi]<=57))
					{
					shamt = 10*shamt + (int(instr[tempi])-48);
					tempi++;
					}
				}
			}
		}


		else if (type==1)//I
		{
		if(!strcmp(op,"lw")||!strcmp(op,"sw"))
		{
			if(regamount==1)
			{
				rt = trans_regchar_to_regint(regchar);
				if(!strcmp(regchar,"0"))
				{immload(i+2);}
				else if(!strcmp(regchar,"zero"))
				{immload(i+5);}
				else {immload(i+3);}
			}
			if(regamount==2)
			{rs = trans_regchar_to_regint(regchar);}
		}

		else if(!strcmp(op,"lui"))
		{
		rs = 0;
		if(regamount==1)
		{rt = trans_regchar_to_regint(regchar);immload(i+3);	}
		}

		else if(!strcmp(op,"addi")||!strcmp(op,"addiu")||
				!strcmp(op,"ori")||!strcmp(op,"andi")||
				!strcmp(op,"slti")||!strcmp(op,"sltiu"))
		{
			if(regamount==1){rt = trans_regchar_to_regint(regchar);}
			if(regamount==2){rs = trans_regchar_to_regint(regchar); immload(i+3);}
		}

		
		}
		else if(type==2)//Br
		{
			if(!strcmp(op,"beq")||!strcmp(op,"bne"))//rs rt exist
			{
				if(regamount==1){rs = trans_regchar_to_regint(regchar);}
				if(regamount==2)
				{
					rt = trans_regchar_to_regint(regchar);
					//lable work
					int tempj = 0;
					int lablelocat = 0;
					if(!strcmp(regchar,"zero")){tempj = i+5;}
					else if(!strcmp(regchar,"0")){tempj = i+2;}
					else {tempj = i+3;}
					judge = ( instr[tempj]==95 ||//下划线
							(instr[tempj]>64 && instr[tempj]<91) ||//数字和大小写字母
							(instr[tempj]>96 && instr[tempj]<123) ||
							(instr[tempj]>47 && instr[tempj]<58) );
					while( (!judge) && instr[tempj]!='\0' )//judge=1 或到尾则跳出 跳过那些不是lable的东西
					{
						tempj++;
						judge = ( instr[tempj]==95 ||//下划线
							(instr[tempj]>64 && instr[tempj]<91) ||//数字和大小写字母
							(instr[tempj]>96 && instr[tempj]<123) ||
							(instr[tempj]>47 && instr[tempj]<58) );
					}
					while(judge)
					{
					target[lablelocat]=instr[tempj];
					lablelocat++; tempj++;
					judge = ( instr[tempj]==95 ||//下划线
							(instr[tempj]>64 && instr[tempj]<91) ||//数字和大小写字母
							(instr[tempj]>96 && instr[tempj]<123) ||
							(instr[tempj]>47 && instr[tempj]<58) );
					}
				}
			}

			else if(!strcmp(op,"blez")||!strcmp(op,"bgtz")||//only rs
					!strcmp(op,"bgez"))
			{
				rt = 0;
				if(regamount==1)
				{
					rs = trans_regchar_to_regint(regchar);
					//lable work
					int tempj = 0;
					int lablelocat = 0;
					if(!strcmp(regchar,"zero")){tempj = i+5;}
					else if(!strcmp(regchar,"0")){tempj = i+2;}
					else {tempj = i+3;}
					judge = ( instr[tempj]==95 ||//下划线
							(instr[tempj]>64 && instr[tempj]<91) ||//数字和大小写字母
							(instr[tempj]>96 && instr[tempj]<123) ||
							(instr[tempj]>47 && instr[tempj]<58) );
					while( (!judge) && instr[tempj]!='\0' )//judge=1 或到尾则跳出 跳过那些不是lable的东西
					{
						tempj++;
						judge = ( instr[tempj]==95 ||//下划线
							(instr[tempj]>64 && instr[tempj]<91) ||//数字和大小写字母
							(instr[tempj]>96 && instr[tempj]<123) ||
							(instr[tempj]>47 && instr[tempj]<58) );
					}
					while(judge)
					{
					target[lablelocat]=instr[tempj];
					lablelocat++; tempj++;
					judge = ( instr[tempj]==95 ||//下划线
							(instr[tempj]>64 && instr[tempj]<91) ||//数字和大小写字母
							(instr[tempj]>96 && instr[tempj]<123) ||
							(instr[tempj]>47 && instr[tempj]<58) );
					}
				}	
			}
	
		}
	}
	}
}


//由op得type，需要有op
/*
R型		type = 0: jr jalr add addu sub subu and or xor nor
				  slt sll srl sra nop(num=15)
I型		type = 1: lw sw lui addi addiu ori andi slti sltiu(num=9)
Br型	type = 2: beq bne blez bgtz bgez(num=5)
J型		type = 3: j jal(num=2)
*/
int infoofaline::typeload()
{
	if(!strcmp(op,"j") || !strcmp(op, "jal")) {type = 3; return 3;}

	else if (
		!strcmp(op, "beq") || !strcmp(op, "bne") || 
		!strcmp(op, "blez") || !strcmp(op, "bgtz") || 
		!strcmp(op, "bgez")) {type = 2;return 2;}

	else if(
		!strcmp(op, "lw") || !strcmp(op, "sw") || 
		!strcmp(op, "lui") || !strcmp(op, "addi") || 
		!strcmp(op, "addiu") || !strcmp(op, "ori") || 
		!strcmp(op, "andi") || !strcmp(op, "slti") || 
		!strcmp(op, "sltiu")) {type = 1;return 1;}

	else if(
		!strcmp(op, "jr") || !strcmp(op, "jalr") || 
		!strcmp(op, "add") || !strcmp(op, "addu") || 
		!strcmp(op, "sub") || !strcmp(op, "subu") || 
		!strcmp(op, "and") || !strcmp(op, "or") || 
		!strcmp(op, "xor") || !strcmp(op, "nor") || 
		!strcmp(op, "slt") || !strcmp(op, "sll") || 
		!strcmp(op, "srl") || !strcmp(op, "sra") || 
		!strcmp(op, "nop")){type = 0; return 0;}

	else  {return -1;}
}


//通过指令得到op 该指令不能有lable
void infoofaline::opload()
{
	int locat = 0;
	bool judge;
	for(int i =0;instr[i]!='\0';i++)
	{
		judge = (instr[i]>64 && instr[i] <91) || (instr[i]>96 && instr[i] <123);
		if(judge)
		{
		op[locat] = instr[i];
		locat++;
		}
		if((op[0]!='\0') && (judge==0)){break;}
	}

	op[locat]='\0';
}


//载入下一行指令
void infoofaline:: reload(char line[80],int linenum)
{
	strcpy_s(instr,line);
	PCaddr = 4*linenum;
	type = 0;
	memset(op,0,6*sizeof(char));
	rs = 0;
	rt = 0;
	rd = 0;
	shamt = 0;
	imm_addr = 0;
	memset(tag,0,20*sizeof(char));
	memset(target,0,20*sizeof(char));


}


//regload函数中讲char型reg转换为int型所调用函数
int infoofaline:: trans_regchar_to_regint(char* regchar)
{
	return (!strcmp(regchar, "0") || !strcmp(regchar, "zero"))?0:
		(!strcmp(regchar, "at"))?1:
		(!strcmp(regchar, "v0"))?2:
		(!strcmp(regchar, "v1"))?3:
		(!strcmp(regchar, "a0"))?4:
		(!strcmp(regchar, "a1"))?5:
		(!strcmp(regchar, "a2"))?6:
		(!strcmp(regchar, "a3"))?7:
		(!strcmp(regchar, "t0"))?8:
		(!strcmp(regchar, "t1"))?9:
		(!strcmp(regchar, "t2"))?10:
		(!strcmp(regchar, "t3"))?11:
		(!strcmp(regchar, "t4"))?12:
		(!strcmp(regchar, "t5"))?13:
		(!strcmp(regchar, "t6"))?14:
		(!strcmp(regchar, "t7"))?15:
		(!strcmp(regchar, "s0"))?16:
		(!strcmp(regchar, "s1"))?17:
		(!strcmp(regchar, "s2"))?18:
		(!strcmp(regchar, "s3"))?19:
		(!strcmp(regchar, "s4"))?20:
		(!strcmp(regchar, "s5"))?21:
		(!strcmp(regchar, "s6"))?22:
		(!strcmp(regchar, "s7"))?23:
		(!strcmp(regchar, "t8"))?24:
		(!strcmp(regchar, "t9"))?25:
		(!strcmp(regchar, "k0"))?26:
		(!strcmp(regchar, "k1"))?27:
		(!strcmp(regchar, "gp"))?28:
		(!strcmp(regchar, "sp"))?29:
		(!strcmp(regchar, "fp"))?30:
		(!strcmp(regchar, "ra"))?31:-1;
}