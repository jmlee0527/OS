#include "types.h"
#include "user.h"
#include "param.h"
#include "stat.h"


#define MAX_PATH_LEN 50
#define MAX_CMD_ARGS 3
#define MAX_STACK_PAGES 100000

int check_cmd(char* array, char* cmd) {      //문자열 확인 함수
    for (int i = 0; i < strlen(cmd); i++) {
        if (array[i] != cmd[i]) {
            return -1;
            exit();
        }
    }
    return 1;
}

int main(int argc, char* argv[]) {
  char cmd_str[MAX_PATH_LEN];

  while (1) {
    printf(1, "input cmd >>>");
    gets(cmd_str, MAX_PATH_LEN);    //명령줄의 입력을 받음
    cmd_str[strlen(cmd_str)-1] = '\n';

    char* list_cmd = "list";
    char* kill_cmd = "kill";
    char* execute_cmd = "execute";
    char* memlim_cmd = "memlim";
    char* exit_cmd = "exit";

    if (check_cmd(cmd_str, list_cmd) == 1){     //list 명령어
      print_pinfo();      //프로세스 정보를 출력하는 함수
      continue;
    }

    else if(check_cmd(cmd_str,kill_cmd) ==1){   //kill 명령어
      char *pid = (char*)malloc(sizeof(char)*100);
      int n_pid = 0;
      int result;
			for(int i=5;i<strlen(cmd_str);i++){
				pid[n_pid] = cmd_str[i];
          n_pid++;
			}
			pid[n_pid] = '\0';
			int kill_pid = atoi(pid);
			result = kill(kill_pid);
      if (result == -1){
        printf(1,"kill fail!\n");
      }
      else{
			printf(1,"kill success!\n");
      }
			continue;
    }
    
    else if(check_cmd(cmd_str,execute_cmd) ==1){
      char *argv[10];
			char path[1024];
      int index = 8;
			int stacksize = 0;
      int pindex = 0;

      while (cmd_str[index] != ' ')
      {
      path[pindex] = cmd_str[index];
      pindex += 1;
      index += 1;
      }
      printf(1,"path :[%c]", path[0]);
      path[pindex] = '\0';
      argv[0] = path;
      argv[1] = 0;

      index += 1;
      while(cmd_str[index] != '\n') 
      {
        stacksize = stacksize * 10;
        stacksize += cmd_str[index] - 48;
        index++;
      }
      printf(1,"[%d],[%d]", index, stacksize);
      int fpid = 0;
      if(cmd_str[index] == '\n')
      {
          fpid = fork();
          if(fpid == 0)
          {
              int result = 0;
              if(argv[0] == 0) printf(1, "input cmd again >>> ");
              else
              {
                  result = exec2(argv[0], argv, stacksize);
                  if(result == -1) printf(1, "Execute Fail\n");
                  else printf(1, "input cmd : ");
              }
          }
          else if(fpid > 0) wait();
          else printf(1, "Fail\n");
      }
      else printf(1,"input cmd again >>> \n");
    }
    else if(check_cmd(cmd_str,memlim_cmd)==1){
      int pid = 0;
      int index = 7;
      int limit = 0;
      
      while(cmd_str[index]!= ' '){
        pid = pid*10;
        pid += cmd_str[index] - 48;
        index++;
      }
      index += 1;
      
      while(cmd_str[index] != '\n')
      { 
        limit = limit*10;
        limit += cmd_str[index] -48;
        index++;
      }
      if(cmd_str[index] == '\n')
      {
        printf(1,"setmemory start\n");
        int result = setmemorylimit(pid,limit);
        if(result  == -1){
          printf(1,"setmemorylimit fail!\n");
        }
        else{
          printf(1,"setmemorylimit success!\n");
        }
      }
    }
    else if(check_cmd(cmd_str,exit_cmd) ==1){
      exit();
    }
}
}

