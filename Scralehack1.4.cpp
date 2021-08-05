#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <pthread.h>

/* 
// Scarlehack v1.4
// Coded by haru1ca
*/

struct MAPS
{
long int addr;
long int taddr;
struct MAPS *next;
};

struct RESULT
{
long int addr;
struct RESULT *next;
};

#define LEN sizeof(struct MAPS)
typedef struct MAPS *PMAPS; 
typedef struct RESULT *PRES;

typedef int TYPE;
typedef int RANGE;
typedef int COUNT;
typedef long int OFFSET;
typedef long int ADDRESS;
typedef char PACKAGENAME;

enum type
{
DWORD,
FLOAT
};

enum Range
{
CODE_APP
};

PMAPS Res=NULL;
int ResCount=0;
int MemorySearchRange=0;
int getPID(char bm[64]);

int SetSearchRange(int type);
PMAPS readmaps(char *bm,int type);
PMAPS readmaps_code_app(char *bm);

void MemorySearch(char *bm,char *value,int *gs,int TYPE);
PMAPS MemorySearch_DWORD(char *bm,int value,int *gs,PMAPS pMap); 
PMAPS MemorySearch_FLOAT(char *bm,float value,int *gs,PMAPS pMap); 

void MemoryOffset(char *bm,char *value,long int offset,int *gs,int type);
PMAPS MemoryOffset_DWORD(char *bm,int value,long int offset,PMAPS pBuff,int *gs);
PMAPS MemoryOffset_FLOAT(char *bm,float value,long int offset,PMAPS pBuff,int *gs);

void MemoryWrite(char *bm,char *value,long int offset,int type);
int MemoryWrite_DWORD(char *bm,int value,PMAPS pBuff,long int offset);
int MemoryWrite_FLOAT(char *bm,float value,PMAPS pBuff,long int offset);

void *SearchAddress(char *bm,long int addr);
int WriteAddress(char *bm,long int addr,void *value,int type);
void BypassGameSafe(char *bm);
void AntiMTP(char *bm);
void RecMTP(char *bm);
void RecBypassGameSafe(char *bm);
void Print();
void ClearResults();
void ClearMaps(PMAPS pMap);
void RemoveGameLogs();
int killprocess(char *bm);
char GetProcessState(char *bm);
void isOutdated();
PMAPS GetResults();
PACKAGENAME *package="com.tencent.tmgp.sgame";

int main(int argc,char **argv)
{
int count;
RemoveGameLogs();
int ipid = getPID(package);
if (ipid == 0)
{
puts("Cannot get processId!");
killprocess(package);
exit(1);
}
isOutdated();
AntiMTP(package);
BypassGameSafe(package);
SetSearchRange(CODE_APP);
MemorySearch(package,"167772281",&count,FLOAT);
MemoryOffset(package,"-509587454",-16,&count,DWORD);
MemoryOffset(package,"-443285480",-12,&count,DWORD);
MemoryOffset(package,"-509583359",-8,&count,DWORD);
MemoryOffset(package,"-481296384",-4,&count,DWORD);
MemoryWrite(package,"-476028927",-8,DWORD);
RecBypassGameSafe(package);
puts("Scralehack injected successfully!");
RemoveGameLogs();
RecMTP(package);
ClearResults();
exit(1);
}

int getPID(PACKAGENAME *PackageName)
{
DIR *dir=NULL;
struct dirent *ptr=NULL;
FILE *fp=NULL;
char filepath[256];	
char filetext[128];	
dir = opendir("/proc");	
if (NULL != dir)
{
while ((ptr = readdir(dir)) != NULL)
{
if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
continue;
if (ptr->d_type != DT_DIR)
continue;
sprintf(filepath, "/proc/%s/cmdline", ptr->d_name);	
fp = fopen(filepath, "r");
if (NULL != fp)
{
fgets(filetext,sizeof(filetext),fp);
if (strcmp(filetext,PackageName)==0)
{
break;
}
fclose(fp);
}
}
}
if (readdir(dir) == NULL)
{
return 0;
}
closedir(dir);
return atoi(ptr->d_name);
}

int SetSearchRange(TYPE type)
{
switch (type)
{
case CODE_APP:
MemorySearchRange=0;
break;
default:
printf("selected a null type\n");
break;
}
return 0;
}

PMAPS readmaps(char* bm,TYPE type)
{
PMAPS pMap=NULL;
switch (type)
{
case CODE_APP:
pMap=readmaps_code_app(bm);
break;
default:
printf("selected a null type\n");
break;
}
if (pMap == NULL)
{
return 0;
}
return pMap;
}

PMAPS readmaps_code_app(PACKAGENAME *bm)
{
PMAPS pHead=NULL;
PMAPS pNew=NULL;
PMAPS pEnd=NULL;
pEnd=pNew=(PMAPS)malloc(LEN);
FILE *fp;
int i = 0,flag=1;
char lj[64], buff[256];
int pid = getPID(bm);
sprintf(lj, "/proc/%d/maps", pid);
fp = fopen(lj, "r");
if (fp == NULL)
{
puts("error while reading");
return NULL;
}
while (!feof(fp))
{
fgets(buff,sizeof(buff),fp);
if (strstr(buff, "r-xp") != NULL && !feof(fp) && strstr(buff,"/data/app"))
{
sscanf(buff, "%lx-%lx", &pNew->addr, &pNew->taddr);
flag=1;
}
else
{
flag=0;
}
if (flag==1)
{
i++;
if (i==1)
{
pNew->next=NULL;
pEnd=pNew;
pHead=pNew;
}
else
{
pNew->next=NULL;
pEnd->next=pNew;
pEnd=pNew;
}
pNew=(PMAPS)malloc(LEN);
}
}
free(pNew);
fclose(fp);
return pHead;
}

void Print()
{
PMAPS temp = Res;
int i;
for (i=0;i<ResCount;i++)
{
printf("addr:0x%lX,taddr:0x%lX\n", temp->addr, temp->taddr);
temp = temp->next;
}
}

void ClearResults()
{
PMAPS pHead=Res;
PMAPS pTemp=pHead;
int i;
for (i=0;i<ResCount;i++)
{
pTemp=pHead;
pHead=pHead->next;
free(pTemp);
}
}

void MemorySearch(PACKAGENAME *bm,char *value,int *gs,TYPE type)
{
PMAPS pHead=NULL;
PMAPS pMap=NULL;
switch (MemorySearchRange)
{
case CODE_APP:
pMap=readmaps(bm,CODE_APP);
break;
default:
printf("selected a null type\n");
break;
}
if (pMap == NULL)
{
puts("map error");
return (void)0;
}
switch (type)
{
case DWORD:
pHead=MemorySearch_DWORD(bm,atoi(value),gs,pMap);
break;
case FLOAT:
pHead=MemorySearch_FLOAT(bm,atof(value),gs,pMap);
break;
default:
printf("selected a null type\n");
break;
}
if (pHead == NULL)
{
puts("search error");
return (void)0;
}
ResCount=*gs;
Res=pHead;
}

PMAPS MemorySearch_DWORD(PACKAGENAME *bm,int value,COUNT *gs,PMAPS pMap)
{
pid_t pid=getPID(bm);
if (pid == 0)
{
puts("can not get pid");
return NULL;
}
*gs=0;
PMAPS pTemp=NULL;
pTemp=pMap;
PMAPS n,e;
e=n=(PMAPS)malloc(LEN);
PMAPS pBuff;
pBuff=n;
int handle;
int iCount=0;
int c;
char lj[64];
int buff[1024]={0};
memset(buff,0,4);
sprintf(lj, "/proc/%d/mem", pid);
handle = open(lj, O_RDWR);
lseek(handle, 0, SEEK_SET);
while (pTemp != NULL)
{
c=(pTemp->taddr-pTemp->addr)/4096;
for (int j=0;j<c;j++)
{
pread64(handle,buff,0x1000,pTemp->addr+j*4096);
for (int i=0;i<1024;i++)
{
if (buff[i]==value)
{
iCount++;
*gs+=1;
ResCount+=1;
n->addr=(pTemp->addr)+(j*4096)+(i*4);
if (iCount==1)
{
n->next=NULL;
e=n;
pBuff=n;
}
else
{
n->next=NULL;
e->next=n;
e=n;
}
n=(PMAPS)malloc(LEN);
}
}
}
pTemp = pTemp->next;
}
free(n);
close(handle);
return pBuff;
}

PMAPS MemorySearch_FLOAT(PACKAGENAME *bm,float value,COUNT *gs,PMAPS pMap)
{
pid_t pid=getPID(bm);
if (pid == 0)
{
puts("can not get pid");
return NULL;
}
*gs=0;
PMAPS pTemp=NULL;
pTemp=pMap;
PMAPS n,e;
e=n=(PMAPS)malloc(LEN);
PMAPS pBuff;
pBuff=n;
int handle;
int iCount=0;
int c;
char lj[64];
float buff[1024]={0};
sprintf(lj, "/proc/%d/mem", pid);
handle = open(lj, O_RDWR);
lseek(handle, 0, SEEK_SET);
while (pTemp->next != NULL)
{
c=(pTemp->taddr-pTemp->addr)/4096;
for (int j=0;j<c;j+=1)
{
pread64(handle,buff,0x1000,pTemp->addr+(j*4096));
for (int i=0;i<1024;i+=1)
{
if (buff[i]==value)
{
iCount++;
*gs+=1;
ResCount+=1;
n->addr=(pTemp->addr)+(j*4096)+(i*4);
if (iCount==1)
{
n->next=NULL;
e=n;
pBuff=n;
}
else
{
n->next=NULL;
e->next=n;
e=n;
}
n=(PMAPS)malloc(LEN);
}
}
}
pTemp = pTemp->next;
}
free(n);
close(handle);
return pBuff;
}

void MemoryOffset(PACKAGENAME *bm,char *value,OFFSET offset,COUNT *gs,TYPE type)
{
PMAPS pHead=NULL;
switch (type)
{
case DWORD:
pHead=MemoryOffset_DWORD(bm,atoi(value),offset,Res,gs);
break;
case FLOAT:
pHead=MemoryOffset_FLOAT(bm,atof(value),offset,Res,gs);
break;
default:
printf("selected a null type\n");
break;
}
if (pHead == NULL)
{
puts("offset error");
return (void)0;
}
ResCount=*gs;
ClearResults();
Res=pHead;
}

PMAPS MemoryOffset_DWORD(PACKAGENAME *bm,int value,OFFSET offset,PMAPS pBuff,COUNT *gs)
{
pid_t pid=getPID(bm);
if (pid == 0)
{
puts("can not get pid");
return 0;
}
*gs=0;
PMAPS pEnd=NULL;
PMAPS pNew=NULL;
PMAPS pTemp=pBuff;
PMAPS BUFF=NULL;
pEnd=pNew=(PMAPS)malloc(LEN);
BUFF=pNew;
int iCount=0,handle;
char lj[64];
long int all;
int *buf=(int *)malloc(sizeof(int));
int jg;
sprintf(lj,"/proc/%d/mem",pid);
handle=open(lj,O_RDWR);
lseek(handle,0,SEEK_SET);
while (pTemp != NULL)
{
all=pTemp->addr+offset;
pread64(handle,buf,4,all);
jg=*buf;
if (jg == value)
{
iCount++;
*gs+=1;
pNew->addr=pTemp->addr;
if (iCount == 1)
{
pNew->next=NULL;
pEnd=pNew;
BUFF=pNew;
}
else
{
pNew->next=NULL;
pEnd->next=pNew;
pEnd=pNew;
}
pNew=(PMAPS)malloc(LEN);
if (ResCount==1)
{
free(pNew);
close(handle);
return BUFF;
}
}
pTemp=pTemp->next;
}
free(pNew);
close(handle);
return BUFF;
}

PMAPS MemoryOffset_FLOAT(PACKAGENAME *bm,float value,OFFSET offset,PMAPS pBuff,COUNT *gs)
{
pid_t pid=getPID(bm);
if (pid == 0)
{
puts("can not get pid");
return 0;
}
*gs=0;
PMAPS pEnd=NULL;
PMAPS pNew=NULL;
PMAPS pTemp=pBuff;
PMAPS BUFF=NULL;
pEnd=pNew=(PMAPS)malloc(LEN);
BUFF=pNew;
int iCount=0,handle;
char lj[64];
long int all;
float *buf=(float *)malloc(sizeof(float));
float jg;
sprintf(lj,"/proc/%d/mem",pid);
handle=open(lj,O_RDWR);
lseek(handle,0,SEEK_SET);
while (pTemp != NULL)
{
all=pTemp->addr+offset;
pread64(handle,buf,4,all);
jg=*buf;
if (jg == value)
{
iCount++;
*gs+=1;
pNew->addr=pTemp->addr;
if (iCount == 1)
{
pNew->next=NULL;
pEnd=pNew;
BUFF=pNew;
}
else
{
pNew->next=NULL;
pEnd->next=pNew;
pEnd=pNew;
}
pNew=(PMAPS)malloc(LEN);
if (ResCount==1)
{
free(pNew);
close(handle);
return BUFF;
}
}
pTemp=pTemp->next;
}
free(pNew);
close(handle);
return BUFF;
}

void MemoryWrite(PACKAGENAME *bm,char *value,OFFSET offset,TYPE type)
{
switch (type)
{
case DWORD:
MemoryWrite_DWORD(bm,atoi(value),Res,offset);
break;
case FLOAT:
MemoryWrite_FLOAT(bm,atof(value),Res,offset);
break;
default:
printf("selected a null type\n");
break;
}
}

int MemoryWrite_DWORD(PACKAGENAME *bm,int value,PMAPS pBuff,OFFSET offset)
{
pid_t pid=getPID(bm);
if (pid == 0)
{
puts("can not get pid");
return 0;
}
PMAPS pTemp=NULL;
char lj[64];
int handle;
pTemp=pBuff;
sprintf(lj,"/proc/%d/mem",pid);
handle=open(lj,O_RDWR);
lseek(handle,0,SEEK_SET);
int i;
for (i=0;i<ResCount;i++)
{
pwrite64(handle,&value,4,pTemp->addr+offset);
if (pTemp->next != NULL)
pTemp=pTemp->next;
}
close(handle);
return 0;
}

int MemoryWrite_FLOAT(PACKAGENAME *bm,float value,PMAPS pBuff,OFFSET offset)
{
pid_t pid=getPID(bm);
if (pid == 0)
{
puts("can not get pid");
return 0;
}
PMAPS pTemp=NULL;
char lj[64];
int handle;
pTemp=pBuff;
sprintf(lj,"/proc/%d/mem",pid);
handle=open(lj,O_RDWR);
lseek(handle,0,SEEK_SET);
int i;
for (i=0;i<ResCount;i++)
{
pwrite64(handle,&value,4,pTemp->addr+offset);
if (pTemp->next != NULL)
pTemp=pTemp->next;
}
close(handle);
return 0;
}

void *SearchAddress(PACKAGENAME *bm,ADDRESS addr)
{
pid_t pid=getPID(bm);
if (pid == 0)
{
puts("can not get pid");
return 0;
}
char lj[64];
int handle;
void *buf=malloc(8);
sprintf(lj,"/proc/%d/mem",pid);
handle=open(lj,O_RDWR);
lseek(handle,0,SEEK_SET);
pread64(handle,buf,8,addr);
close(handle);
return buf;
}

int WriteAddress(PACKAGENAME *bm,ADDRESS addr,void *value,TYPE type)
{
pid_t pid=getPID(bm);
if (pid == 0)
{
puts("can not get pid");
return 0;
}
char lj[64];
int handle;
sprintf(lj,"/proc/%d/mem",pid);
handle=open(lj,O_RDWR);
lseek(handle,0,SEEK_SET);
switch (type)
{
case DWORD:
pwrite64(handle,(int*)value,4,addr);
break;
case FLOAT:
pwrite64(handle,(float*)value,4,addr);
break;
default:
printf("selected a null type\n");
break;
}
close(handle);
return 0;
}

int killprocess(PACKAGENAME *bm)
{
int pid=getPID(bm);
if (pid == 0)
{
return -1;
}
char ml[32];
sprintf(ml,"kill %d",pid);
system(ml);
return 0;
}

char GetProcessState(PACKAGENAME *bm)
{
int pid=getPID(bm);
if (pid == 0)
{
return 0;
}
FILE *fp;
char lj[64];
char buff[64];
char zt;
char zt1[16];
sprintf(lj,"/proc/%d/status",pid);
fp = fopen(lj,"r");
if (fp == NULL)
{
return 0;
}
while (!feof(fp))
{
fgets(buff,sizeof(buff),fp);
if (strstr(buff,"State"))
{
sscanf(buff,"State: %c",&zt);
break;
}
}
fclose(fp);
return zt;
}


void BypassGameSafe(char *bm)
{
char ml[80];
sprintf(ml,"chmod 444 /data/data/%s/files",bm);
system(ml);
}

void AntiMTP(char *bm)
{
char ml[80];
sprintf(ml,"chmod 7462 /data/data/%s/lib/libtersafe.so",bm);
system(ml);
}

void RemoveGameLogs()
{
remove("/sdcard/Android/data/com.tencent.tmgp.sgame/cache");
remove("/sdcard/Android/data/com.tencent.tmgp.sgame/files/tbslog");
remove("/sdcard/Android/data/com.tencent.tmgp.sgame/files/ca-bundle.pem");
remove("/sdcard/Android/data/com.tencent.tmgp.sgame/files/cacheFile.txt");
remove("/sdcard/Android/data/com.tencent.tmgp.sgame/files/login-identifier.txt");
remove("/sdcard/Android/data/com.tencent.tmgp.sgame/files/vmpcloudconfig.json");
remove("/sdcard/Android/data/com.tencent.tmgp.sgame/files/ProgramBinaryCache");
remove("/data/data/com.tencent.tmgp.sgame/cache");
remove("/data/data/com.tencent.tmgp.sgame/code_cache");
remove("/data/data/com.tencent.tmgp.sgame/app_appcache");
remove("/data/data/com.tencent.tmgp.sgame/app_bugly");
remove("/data/data/com.tencent.tmgp.sgame/app_crashrecord");
remove("/data/data/com.tencent.tmgp.sgame/app_databases");
remove("/data/data/com.tencent.tmgp.sgame/app_geolocation");
remove("/data/data/com.tencent.tmgp.sgame/app_tbs");
remove("/data/data/com.tencent.tmgp.sgame/app_textures");
remove("/data/data/com.tencent.tmgp.sgame/app_webview");
remove("/data/data/com.tencent.tmgp.sgame/app_webview_imsdk_inner_webview");
remove("third_party.java_src.error_prone.project.annotations.Google_internal");
remove("/data/data/com.tencent.tmgp.sgame/files/tss_tmp");
remove("/data/data/com.tencent.tmgp.sgame/files/GCloud");
}

void RecMTP(char *bm)
{
char ml[80];
sprintf(ml,"chmod 771 /data/data/%s/lib/libtersafe.so",bm);
system(ml);
}

void RecBypassGameSafe(char *bm)
{
char ml[80];
sprintf(ml,"chmod 771 /data/data/%s/files",bm);
system(ml);
}

PMAPS GetResults()
{
if (Res == NULL)
{
return NULL;
}
else
{
return Res;
}
}

void isOutdated()
{
char url[128];
sprintf(url, "curl -s %s", "http://www.iyuji.cn/iyuji/s/RjdXcFhmU0FmemNzRFY4WUJ2b0MrQT09/1624803964200624");
FILE *fp = popen(url,"r");
char *str;
while (feof(fp) == 0)
{
char buf[512];
fgets(buf, 512, fp);
char bti[256];
sscanf(buf, " <title>%[^<]</title>", bti);
if (strstr(buf, "[开关]"))
{
str = strstr(buf, "[开关]") + 8;
pclose(fp);
break;
}
}
if (strstr(str, "开"))
{
puts("Connected to the server successfully.");
}
else if (strstr(str, "关"))
{
puts("this version is disabled or outdated.");
killprocess(package);
exit(0);
}
else
{
puts("cannot connect to the server.");
killprocess(package);
exit(0);
}
}
