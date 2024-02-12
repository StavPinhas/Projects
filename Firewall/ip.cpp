
#include <iostream>
#include "string.h"
#include "ip.h"


const long ONES_VECTOR = 0xffffffff;
const int IP_FIELDS = 4;
const int MIN_BITS = 0;
const int MAX_BITS = 32;
const int BYTE_MIN = 0;
const int BYTE_MAX = 255;
const int FIRST_PART = 0;
const int SECOND_PART = 1;
const int SPLIT_PARTS = 2;
const int FIELD_BIT_SIZE = 8;
const int LEFT_BYTE_LOCATION = 3;


Ip::Ip() : src_or_dst(""), highest(0), lowest(0) {};

Ip::Ip(const Ip &other) : src_or_dst(other.src_or_dst),
    highest(other.highest), lowest(other.lowest) {};    
    
Ip::~Ip(){};

bool Ip::set_value(String value){
    //check if value is empty - hence not valid
    if(value.equals("\0")){
        return false;
    }
    String *tmp_split;
    size_t tmp_size;
    //split "src" or "dst" from ip and rule
    (value.trim()).split("=",&tmp_split,&tmp_size);
    if(tmp_split == NULL){
        return false;
    }
    if(tmp_size != SPLIT_PARTS){
        delete[] tmp_split;
        return false;
    }
    src_or_dst = tmp_split[FIRST_PART];
    String *ip_rule;
    size_t size;

    //split ip and rule
    (tmp_split[SECOND_PART].trim()).split("./",&ip_rule,&size);
    if(ip_rule == NULL){
        return false;
    }
    //check for valid ip and rule
    if(size != (IP_FIELDS + 1)){
        delete[] ip_rule;
        return false;
    }
    delete[] tmp_split;
    //save the rule as an integer and check if it is in valid range
    int rule = ip_rule[IP_FIELDS].to_integer();
    if (rule < MIN_BITS || rule > MAX_BITS){
        delete[] ip_rule;
        return false;
    }

    //save the ip adresses as integers and check their validity
    int ip_arr[IP_FIELDS];
    long unsigned curr_ip = 0;

    for (int i=0; i<IP_FIELDS; i++){
        ip_arr[i] = ip_rule[i].to_integer();
        if(ip_arr[i] < BYTE_MIN || ip_arr[i] > BYTE_MAX){
            delete[] ip_rule;
            return false;       
        }
        curr_ip =curr_ip | ip_arr[i]<<(8*(3-i));
    }
    delete[] ip_rule;

    int mask = ONES_VECTOR<<(MAX_BITS-rule);
    highest = curr_ip | (~mask); /*all the don't cares are 1's*/
    lowest = curr_ip & mask; /*all the don't cares are 0's*/
    return true;
}

bool Ip::match(String packet){
    //check if packet is empty - hence not valid
    if(packet.equals("\0")){
        return false;
    }
    String *split_arr;
    size_t size;
    String ip_adress;

    //split the packet and get the right ip adress
    (packet.trim()).split(",= ",&split_arr,&size);

    for(size_t i=0; i<size; i+=2){
        split_arr[i].trim();
        if(split_arr[i].equals(src_or_dst)){
            ip_adress = split_arr[i+1];    
        }
    }
    delete[] split_arr;

    String *ip_str;
    size_t ip_size;
    //split ip bytes
    (ip_adress.trim()).split(".",&ip_str,&ip_size);
    //check for valid ip
    if(ip_size!=IP_FIELDS){
        delete[] ip_str;
        return false;
    }

    //save the ip adress as integers and check their validity
    int ip_arr[IP_FIELDS];
    //will hold bit-representation of the ip address
    long unsigned curr_ip = 0;

    for (int i=0; i<IP_FIELDS; i++){
        ip_arr[i] = ip_str[i].to_integer();
        if(ip_arr[i]<BYTE_MIN || ip_arr[i]>BYTE_MAX){
            delete[] ip_str;
            return false;       
        }
        //moving fields' bits according to structure.
        curr_ip = 
        curr_ip | ip_arr[i]<<(FIELD_BIT_SIZE*(LEFT_BYTE_LOCATION - i));
    }
    delete[] ip_str;

    //returns true only if the ip is within the mask range
    return ((curr_ip<=highest) && (curr_ip>=lowest));
}
