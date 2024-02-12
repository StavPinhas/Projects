#ifndef PORT_H
#define PORT_H

#include "string.h"
#include "generic-field.h"
#include <stddef.h>

class Port : public GenericField{
    String src_or_dst;
    //highest and lowest numbers in the mask range
    int highest;
    int lowest;

public:

    /**
     * @brief Initiates an empty port
     */
    Port();
    /**
     * @brief Initiates port from other port
     */
    Port(const Port &other);

    /**
     * @brief destroys port
     */
    ~Port();

    /**
     * @param packet - packet of addresses
     * @brief Returns true iff the requested port is valid 
     * @return true - if port is valid, else - false
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
