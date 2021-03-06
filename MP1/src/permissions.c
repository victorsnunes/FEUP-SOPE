#include "permissions.h"

  int state = 0;
  bool ended = false;
  int setMode = 0;
  int perms[3] = {0, 0, 0};
  int usermode = 0; //0:all , 1:user , 2:group , 3:others
  int operation = 2; // 0:- , 1:+ , 2:=
  int oldperms = 0;
  int newperms = 0;


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
    /*
    printf("Operation : %d\n", operation);
    printf("User : %d\n", usermode);
    printf("perms0 : %d\n", perms[0]);
    printf("perms1 : %d\n", perms[1]);
    printf("perms2 : %d\n", perms[2]);
    */
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
/*
    printf("temp : %o\n", temp);
    printf("old : %o\n", oldPerms);
    printf("xor1 : %o\n", (oldPerms & ~temp));
    printf("or : %o\n", oldPerms | temp);
  

    if(operation == 2)
      return temp;
    if(operation == 0)
      return (oldPerms & ~temp);
    if(operation == 1)
      return oldPerms | temp;
*/
    if(operation == 2) {
        if(usermode == 0)
            return temp;
        if (usermode == 1)
            return (oldPerms & ~(S_ISUID | S_IRUSR | S_IWUSR | S_IXUSR)) | (temp & (S_ISUID | S_IRUSR | S_IWUSR | S_IXUSR));
        if (usermode == 2)
            return (oldPerms & ~(S_ISGID | S_IRGRP | S_IWGRP | S_IXGRP)) | (temp & (S_ISGID | S_IRGRP | S_IWGRP | S_IXGRP));
        if (usermode == 3)
            return (oldPerms & ~(S_IROTH | S_IWOTH | S_IXOTH)) | (temp & (S_IROTH | S_IWOTH | S_IXOTH));
    }
    if(operation == 0)
        return (oldPerms & ~temp);

    if(operation == 1)
        return oldPerms | temp;

    return 0;
  }
