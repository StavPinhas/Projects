
#include <cstring>
#include <iostream>
#include <stddef.h>
#include "string.h"


const char WHITE_SPACE = ' ';
const int EMPTY_STRING = 1;

using namespace std;

//default constructor
String::String() :
    data(new char[EMPTY_STRING]), length(0) {
    data[EMPTY_STRING-1]='\0';
}

//copy constructors
String::String(const String &str) :
    data(new char[str.length + 1]) ,length(str.length) { 
        strcpy(data, str.data);
    }


String::String(const char *str) :
     data(new char[strlen(str) + 1]) , length(strlen(str)){
     strcpy(data, str);
 }

//destructors
String::~String(){
    delete[] data;
}
     
      


String& String::operator=(const String &rhs) {
    if(this != &rhs) {
        delete[] data;
        length = rhs.length;
        data = new char[length + 1];
        strcpy(data, rhs.data);
    }
    return *this;
}

String& String::operator=(const char *str){
    if(data != str){
        delete[] data;
        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
    }
    return *this;
}


bool String::equals(const String &rhs) const {
    if(!strcmp(rhs.data, data)){
        return true;
    }
    return false;
}



bool String::equals(const char *rhs) const {
    if(!strcmp(rhs, data)){
        return true;
    }
    return false;
}




void String::split(const char *delimiters, String **output,
                   size_t *size) const {
    char *substring;
    const size_t tmp_size = length + 1;
    char tmp1[tmp_size];
    size_t num_substrings = 0, added_strings = 0;
    strcpy(tmp1, data);
    substring = strtok(tmp1, delimiters);
    // Counting the number of substrings in order to allocate memory
    while (substring != NULL) {
        num_substrings++;
        substring = strtok(NULL, delimiters);
    }
    (*size) = num_substrings;
    if (output == NULL) {
        return;
    }
    (*output) = new String[num_substrings];
    strcpy(tmp1, data);
    //inserting the substrings to output
    substring = strtok(tmp1, delimiters);
    while (substring != NULL) {
        (*output)[added_strings] = String(substring);
        added_strings++;
        substring = strtok(NULL, delimiters);
    }
}

int String::to_integer() const{
    return atoi(data);
}


String String::trim() const{
    int begin = 0, end = length-1;
    //if the string is emtpy
    if(!strcmp(data,"")){
        String out;
        return out;
    }
    //ignoring spaces from beginning and end
    while(data[begin] == WHITE_SPACE){
        begin++;
    }
    while(data[end] == WHITE_SPACE){
        end--;
    }
    //all data is spaces - returning empty string
    if(end < begin){
        String out;
        return out;
    }
    int trimmed_length = (end - begin) + 1;
    //allocating memory also for '/0' at the end
    char* trimmed_data = new char[trimmed_length + 1];
    strncpy(trimmed_data, data + begin, trimmed_length);
    trimmed_data[trimmed_length] = '\0';
    String trimmed_str(trimmed_data);
    delete[] trimmed_data;
    return trimmed_str;
}

