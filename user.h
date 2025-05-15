#ifndef USER_H
#define USER_H

#define MAXSIZE 1024
#define MAX_USERNAME 30


typedef struct User
{
    char username[MAX_USERNAME];
    char buf[MAXSIZE];
    int username_is_sent;
    char *read_start;

} User_T;


#endif