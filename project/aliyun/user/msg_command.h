#ifndef _MSG_COMMAND_H_
#define _MSG_COMMAND_H_
char * parse_command(char* msg,const char * type,char* value);
int parse_topic(char* msg,char*bookcaseId,char*bookcellId,char*bookcopyId,char*operations,char*status);
void make_cmd_response(char *buf,int type,char*bookcaseId,char*bookcellId,int code,char* bookId,char*booklsbn,char*RFID);
#endif
