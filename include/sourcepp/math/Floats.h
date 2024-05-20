#pragma once

//TODO: could this be a short union?
struct FP16
{
    unsigned short int mantissa : 10;
    unsigned short int exponent : 5;
    unsigned short int sign : 1;
};
