#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char ttime[9] = { '\0', };
static char btime[9] = { '\0', };
static int newtime = 0;
static char syscall[1024] = { '\0' };
static unsigned maxfd = 0;
static unsigned fdn = 0;
static char fdc[10];
static unsigned fdrcnt[1024] = { 0, };
static unsigned fdwcnt[1024] = { 0, };
static unsigned fdiop[1024] = { 0, };
static unsigned fdiops=0;
static unsigned bytesn = 0;
static char bytess[10];

void lineprint() {
 int n = 0;
 printf("%s %d : ", ttime,fdiops);
 for (n = 0; n <= maxfd; n++) {
  //printf("%d %d %d ", fdrcnt[n], fdwcnt[n], fdiop[n]);
  printf("%d-%d ", fdrcnt[n], fdwcnt[n]);
 }

 printf("\n");
 fdiops =0;
 for (n = 0; n <= maxfd; n++) {
  fdrcnt[n] = 0;
  fdwcnt[n] = 0;
  fdiop[n] = 0;
 }
}

int main(){
 char c='\0';
 //char line[1024] = { '\0', };
 char *line=NULL;
 size_t len=0;
 ssize_t rr;
 int i = 0, j=0, k=0, n=0, m = 0;
 //freopen("strace.out", "r", stdin);
 //freopen("s.out", "r", stdin);
 while ((rr=getline(&line,&len,stdin)) != -1) {

  /*new line read */
  //i = 0;
  //while (((c = getc(stdin)) != '\n') && (c != EOF)) {
  //while (((c = getc(stdin)) != '\n')) {
  // *(line + i) = c; i++;
  //}
  //*(line + i) = '\0';
  //printf("[%s]\n", line);

  /*시간 필드 분리*/
  i = 0; j = 0;
  while ((line[i] != ' ') && (line[i] != '\0')) {
   *(ttime + j) = *(line + i); i++; j++;
  }

  *(ttime + j) = '\0';
  if (j < 8) continue;
  if ((ttime[0] >= '3') || (ttime[0] < '0')) continue;
  //printf("%s \n", ttime);

  /*기존 시간과 동일한지 여부 확인*/
  newtime = 0;
  for (j = 7; j != 0; j--) {
   if (btime[j] != ttime[j]) {
    newtime = 1;
    strcpy(btime, ttime);
    break;
   }
  }

  if (newtime == 1) lineprint(); /* 새로운 1초가 시간되었으면 그 동안 통계정보를 출력*/

  /*syscall 분리*/
  j = 0;
  while ((line[i] != '(') && (line[i] != '\0')) {
   if (*(line + i) != ' ') {
    *(syscall + j) = *(line + i); j++;
   } i++;
  }
  *(syscall + j) = '\0';

  //printf("%s \n", syscall);
  //if (!strcmp(syscall,"set_tid_address")) { printf("[maxfd:%d]\n",maxfd);}

  /**************open*******************/
  if (!strcmp(syscall,"open")) {
   while ((line[i] != '=') && (line[i] != '\0')) i++;
   i++;
   j = 0;
   while (line[i] != '\0') {
    fdc[j] = line[i]; i++; j++;
   }
   fdc[j] = '\0';
   if (j == 0) fdn = 0;
   else fdn = atoi(fdc);/*fd 번호 */
   if (newtime == 1) fdiop[fdn] = 1; else fdiop[fdn] += 1;
   if (maxfd <= fdn) maxfd = fdn;
  }

  /**************close*******************/
  if (!strcmp(syscall, "close")) {
  }

  /**************read*******************/

  if ((!strcmp(syscall, "read")) ||(!strcmp(syscall, "write"))){ /* ead system call*/
   i++; j = 0;
   while ((line[i] != ',') && (line[i] != '\0')) {
    fdc[j] = line[i];i++; j++;
   }
   fdc[j] = '\0';
   fdn = atoi(fdc);/*fd 번호 */
   while ((line[i] != '=') && (line[i] != '\0')) i++;
   i++;
   j = 0;
   while (line[i] != '\0') {bytess[j] = line[i]; i++; j++; }
   bytess[j] = '\0';
   bytesn = atoi(bytess);/*read bytes 값*/
   if (maxfd <= fdn) maxfd = fdn;
   if (newtime == 1){
               if (!strcmp(syscall, "read")) fdrcnt[fdn] = bytesn; else fdwcnt[fdn] = bytesn;
               fdiop[fdn] = 1;
               fdiops=1;
            }  else{
               if (!strcmp(syscall, "read")) fdrcnt[fdn] += bytesn; else fdwcnt[fdn] += bytesn;
               fdiop[fdn]+= 1;
               fdiops += 1;
            }
  }
 }
 if (newtime == 0) lineprint();
 fclose(stdin);
 return 1;

}

