#include "permissions.h"


  int state = 0;
  bool ended = false;
  int setMode = 0;
  int perms[3] = {0, 0, 0};
  
  int usermode = 0; //0:all , 1:user , 2:group , 3:others
  int operation = 2; // 0:- , 1:+ , 2:=
  int oldperms = 0;
  int newperms = 0;


int getPermissions(const char *path){

  struct stat ret;
    
    if (stat(path, &ret) == -1) {
        return -1;
    }
  
    mode_t bits = ret.st_mode % 32768;

    return bits;

  }

  
  int changePermissions(int oldPerms){
    int temp =0;
    if (usermode == 0)
    {
      if(perms[0])
        temp |= (S_IREAD | S_IRGRP |S_IROTH ) ;
      if(perms[1])
       temp |= (S_IWRITE | S_IWGRP | S_IWOTH);
      if(perms[2])
        temp |= (S_IEXEC | S_IXGRP | S_IXOTH);
    }
    else if (usermode == 1)
    {
      if(perms[0])
        temp |= S_IREAD;
      if(perms[1])
       temp |= S_IWRITE;
      if(perms[2])
        temp |= S_IEXEC;
      
    }
    else if (usermode == 2)
    {
      if(perms[0])
        temp |= S_IRGRP;
      if(perms[1])
       temp |= S_IWGRP;
      if(perms[2])
        temp |= S_IXGRP;
    }

     else if (usermode == 3)
    {
      if(perms[0])
        temp |= S_IROTH;
      if(perms[1])
       temp |= S_IWOTH;
      if(perms[2])
        temp |= S_IXOTH;  
     
    }

    if(operation == 2)
      return temp;
    if(operation == 0)
      return oldPerms ^ temp;
    if(operation == 1)
      return oldPerms | temp;
    
   
    return 0;
  }

  void mode_state_machine(char *argv) {
  int cnt = 0;

  while(!ended){
    switch (state)
    {
    case 0:
      switch (argv[cnt])
      {
        case 'u':
          usermode = 1;
          cnt++;
          break;
        case 'a':
         usermode = 0;
          cnt++;
          break;
        case 'g':
        usermode = 2;
          cnt++;
          break;
        case 'o':
         usermode = 3;
          cnt++;
          break;
        default:
         usermode = 0;
          break;
    }
    state = 1;
    
   
    break;

    case 1 :
      switch (argv[cnt])
      {
      case '-':
        operation = 0;
        break;
      case '+':
        operation = 1;
        break;
      case '=':
        operation = 2;
        break;
      default:
        printf("Bad args\n");
        break;
      }
      
      cnt++;
      state = 2;
      
      case 2 :
        for (int i = cnt; i < strlen(argv) ;i++ ){
          if (argv[i] == 'r')
            perms[0] = 1;
          if (argv[i] == 'w')
            perms[1] = 1;
          if (argv[i] == 'x')
             perms[2] = 1;
        }
        ended = true;
        break;
    default:
      break;
      
    }
  }


}
