#ifndef HELPERS_H
#define HELPERS_H

#include "global.h"


void print_error(char *);
void print_success(char *, char *, bool);

std::vector<std::string> *read_line();
int identify_cmd(std::string);
char *str2char(std::string);

#endif