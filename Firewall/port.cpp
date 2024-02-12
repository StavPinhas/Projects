
#include <iostream>
#include "generic-field.h"
#include "string.h"
#include "port.h"


const int PORT_MIN = 0;
const int PORT_MAX = 65535;
const int PORT_SIZE = 2;
const int FIRST_PART = 0;
const int SECOND_PART = 1;
const int SPLIT_PARTS = 2;
const int NUM_FIELDS = 8;


//constructors and destructor
Port::Port() : src_or_dst(""), highest(0), lowest(0) {};

Port::Port(const Port &other) : src_or_dst(other.src_or_dst),
      highest(other.highest), lowest(other.lowest) {};  
    
Port::~Port(){};


bool Port::set_value(String value){
    //check if value is empty - hence not valid
    if(value.equals("\0")){
        return false;
    }
    String *tmp_split;
    size_t tmp_size;

    //split "src" or "dst" from ip and rule
    (value.trim()).split("=", &tmp_split, &tmp_size);
    if(tmp_split == NULL){
        return false;
    }
    if(tmp_size != SPLIT_PARTS){
        delete[] tmp_split;
        return false;
    }
    src_or_dst = tmp_split[FIRST_PART];
    String *port_rule;
    size_t size;
    //checking port valid range
    (tmp_split[SECOND_PART].trim()).split("-", &port_rule, &size);
    if(port_rule == NULL){
        return false;
    }
    if(size != SPLIT_PARTS){
        delete[] port_rule;
        return false;
    }
    delete[] tmp_split;

    //save the ports range as integers and check their validity
    int port_arr[PORT_SIZE];
    for (int i=0; i<PORT_SIZE; i++){
        port_arr[i] = port_rule[i].to_integer();
        if(port_arr[i]<PORT_MIN || port_arr[i]>PORT_MAX){
            delete[] port_rule;
            return false;       
        }
    }
    delete[] port_rule;

    //check that the range is from a lower port to a higher one
    lowest = port_arr[FIRST_PART];
    highest = port_arr[SECOND_PART];

    if(lowest > highest){
        return false;
    }
    return true;
}

bool Port::match(String packet){
    //check if packet is empty - hence not valid
    if(packet.equals("\0")){
        return false;
    }

    String *split_arr;
    size_t fields_num;
    String port_address;

    //split the packet and get the port address
    (packet.trim()).split(",= ", &split_arr, &fields_num);
    if(split_arr == NULL){
        return false;
    }
    if(fields_num != NUM_FIELDS){
        delete[] split_arr;
        return false;
    }

    //searching the right type
    for(size_t i=0; i<fields_num-1; i++){
        split_arr[i].trim();
        if(split_arr[i].equals(src_or_dst)){
            //saving the port address (will be the next field after src_or_dst)
            port_address = split_arr[i+1];   
        }
    }
    delete[] split_arr;

    //save the port address as an integer and check validity
    int curr_port = port_address.to_integer();
    if(curr_port < PORT_MIN || curr_port > PORT_MAX){
        return false;       
    }

    //returns true only if the port is within the mask range
    return ((curr_port <= highest) && (curr_port >= lowest));
}
