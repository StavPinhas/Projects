#ifndef IP_H
#define IP_H

#include "string.h"
#include "generic-field.h"
#include <stddef.h>


class Ip : public GenericField{
    String src_or_dst;
    //highest and lowest numbers in the mask range
    unsigned long highest;
    unsigned long lowest;
    
public:

   /**
     * @brief Initiates an empty ip adress
     */
    Ip();
     /**
     * @brief Initiates an ip adress from other ip adress
     */
    Ip(const Ip &other);
     /**
     * @brief destroys ip
     */
    ~Ip();


    /**
     * @param packet - packet of addresses
     * @brief Returns true iff the requested ip is valid 
     * @return true - if ip is valid, else - false
     */
    bool match(String packet);

    /**
     * @param value - the mask rule
     * @brief Returns true iff the mask rule is valid
     * @return true - if rule is valid, else - false.
     */
    bool set_value(String value);



};


#endif
