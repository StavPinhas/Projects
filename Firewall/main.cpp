
#include <iostream>
#include <cstdlib>
#include "string.h"
#include "ip.h"
#include "port.h"
#include "input.h"

const int SUCCESS = 0;
const int FAIL = 1;
const int FIRST_PART = 0;
const int SECOND_PART = 1;
const int SPLIT_PARTS = 2;


int main(int argc, char **argv){
    //check that the arguments are valid
    if (check_args(argc, argv)){
        return FAIL;
    }

    String str(argv[1]);
    size_t size;
    String *split_str;

    //split the type from the ip/port address and rule
    str.trim();
    str.split("=", &split_str, &size);
     if(split_str == NULL){
        return FAIL;
    }
    if(size != SPLIT_PARTS){
        delete[] split_str;
        return FAIL;
    }
    String type(split_str[FIRST_PART]);

    //check if ip or port and parse accordingly
    if (type.equals("src-ip") || type.equals("dst-ip")){
        GenericField *ip_address = new Ip();
        ip_address->set_value(str);
        parse_input(*ip_address);
        delete ip_address;

    } else if (type.equals("src-port") || type.equals("dst-port")){
        GenericField *port_address = new Port();
        port_address->set_value(str);
        parse_input(*port_address);
        delete port_address; 

    } else{
        //not a valid type
        delete[] split_str;
        return FAIL;
    }

    delete[] split_str;
    return SUCCESS;
}


