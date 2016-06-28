#include "cplier.h"
using namespace std;
void main()
{
	//打开文件
	fstream fc,fchead;
	fc.open("MIPScode.txt",ios::in);
	fchead.open("MIPScode.txt",ios::in);
	if(!fc.is_open() || !fchead.is_open())
	{cout<<"fail to open file."<<endl;exit(1);}
	fstream mc;
	mc.open("Machinecode.txt",ios::out);
	if(!mc.is_open())
	{cout<<"fail to open file."<<endl;exit(1);}

	vector<infoofaline> info;
	infoofaline singleinfo;
	int errorjudge = 0;

	char instruction[81];
	bool tagjudge = 0; //tagjudge=1表示有tag wordjudge=1表示有指令
	bool wordjudge = 0;
	bool judge = 0;
	int label = 80;//注释位置
	int i=0;
	int countline = 0;
	char Tag[20];

	memset(Tag,0,20);memset(instruction,0,81);
	bool inherit = 0;
	while(!fc.eof())//检测文件尾
	{
		fc.getline(instruction,80);
		//label judge
		tagjudge = 0;
		for(i=0;instruction[i] != '\0';i++)
		{
			if(instruction[i] == ':' && label > i){tagjudge=1;}
			if(instruction[i] == '#') {instruction[i] = instruction[i+1] = '\0'; label = i; }
		}
		//word judge
		wordjudge = 0;
		for(i=0;instruction[i] != '\0';i++)

		{
			if(instruction[i] == '\t'){instruction[i]=' ';}
			if(((instruction[i]>47 && instruction[i] <58))||//数字大小写字母
			(instruction[i] >64 && instruction[i] <91) || 
			(instruction[i] >96 && instruction[i] <123)){wordjudge = 1;}
			if(instruction[i] == ':' && label > i){wordjudge=0;}
		}

		if(!wordjudge && !tagjudge){continue;}
		if(tagjudge)
		{
			int j=0;
			for(i=0;instruction[i] != '\0'; i++) 
			{
				judge = (instruction[i] >= 48 && instruction[i] <= 57) || 
						(instruction[i] >= 65 && instruction[i] <= 90) || 
						(instruction[i] >= 97 && instruction[i] <= 122) || 
						instruction[i] == 95;
				if(judge)
				{
					Tag[j] = instruction[i];
					j++;
					instruction[i] = 127; //清零 统一label word同一行时使用
				}
				if(instruction[i] == ':') { break;}
			}
			Tag[j]='\0';
			if(!wordjudge){inherit=1;}
		}

		if(wordjudge && tagjudge)
		{
			char *temp = strtok(instruction,":");
			temp = strtok(NULL,":");
			strcpy_s(instruction,temp);

			singleinfo.init(instruction,countline);
			strcpy_s(singleinfo.tag,Tag);
			singleinfo.opload();
			singleinfo.typeload();
			singleinfo.regload();
			singleinfo.dataload();
			singleinfo.targetload();
			info.push_back(singleinfo);
			countline++;
		}
		else if(wordjudge)
		{
			singleinfo.init(instruction,countline);
			if(inherit){strcpy_s(singleinfo.tag,Tag);inherit = 0;}
			singleinfo.opload();
			singleinfo.typeload();
			singleinfo.regload();
			singleinfo.dataload();
			singleinfo.targetload();
			info.push_back(singleinfo);
			countline++;
		}
		
		
	}

	//vector info建立完毕
	//calcu target //待看


	
	char branchTarget[17]; memset(branchTarget, 0, 17);
	char targchar[33]; memset(targchar, 0, 33);
	int aim;
	bool match = 0;
	int infolocat = 0;
	int j = 0;
	for(infolocat = 0; infolocat <int(info.size()); infolocat ++) 
	{ 
		match = 0;
		if(info[infolocat].type == 2) //Br
		{ 
			for(j = 0; j < int(info.size()); j ++) 
			{
				if(!strcmp(info[infolocat].target, info[j].tag)) 
				{
					match = 1;
					aim = info[j].PCaddr - (info[infolocat].PCaddr+4);
					_itoa_s(aim, targchar, 2);
					if(aim >= 0) {info[j].addzero(targchar, 32, strlen(targchar));}
					int q;
					for(q = 0; q < 16; q ++) 
					{targchar[q] = targchar[q+14];}
					for(q = 16; q < 33; q++) 
					{targchar[q] = '\0';}

					strcat(info[infolocat].mc, targchar);
				}
			}
			if(match == 0) strcpy(info[infolocat].error, "targ error");
		}

		else if(info[infolocat].type == 3) //J
		{ 
			for(j = 0; j < int(info.size()); j ++) 
			{
				if(!strcmp(info[infolocat].target, info[j].tag)) 
				{
					match = 1;
					_itoa_s(info[j].PCaddr, targchar, 2);
					info[j].addzero(targchar, 32, strlen(targchar));
					int w;
					for(w = 0; w < 26; w ++) 
					{targchar[w] = targchar[w+4];}
					for(w = 26; w <33; w ++) 
					{targchar[w] = '\0';}

					strcat(info[infolocat].mc, targchar);
				}
			}
			if(match == 0) strcpy(info[infolocat].error, "targ error");
		}
	}


	


	
	
	


	//outfile
	for(i=0;i<int(info.size());i++)
	{
		if(!strlen(info[i].error))
		{
			if(!strlen(info[i].tag))//no tag
			{
				mc<<"ROMDATA["<<info[i].PCaddr/4<<"] <= 32'b";
				for(int q=0;q<32;q++)
				{
				mc<<info[i].mc[q];
				if(q==5||q==10||q==15||q==20||q==25){mc<<"_";}
				}
				mc<<"; //"<<info[i].instr<<endl;
			}
			else
			{
				mc<<"ROMDATA["<<info[i].PCaddr/4<<"] <= 32'b";
				for(int q=0;q<32;q++)
				{
				mc<<info[i].mc[q];
				if(q==5||q==10||q==15||q==20||q==25){mc<<"_";}
				}
				mc<<"; //"<<info[i].tag<<": "<<info[i].instr<<endl;
			}
		}
		else
		{
			errorjudge++;
			mc<<info[i].error<<endl;
		}
		
	}
	if(!errorjudge){mc<<"//no error"<<endl;}
	if(errorjudge){mc<<"num of error:"<<errorjudge<<endl;}
	//close
	fc.close();
	fchead.close();
	mc.close();
	return;
}