/**
 * Various utility functions for the rest of the system.
 * 
 * (C) 2018 - Jimmy Lipham (http://86pixels.com)
 * This file is subject to the GPLv2 software license. Please see LICENSE file for more details.
 */

 #include <stdint.h>

/* A utility function to reverse a string  */
void reverse(char str[], int length)
{
    int i;
    int tmp=0;
 
    for(i=0; i<length/2; i++)
    {
        tmp=str[i];
        str[i]=str[length-1-i];
        str[length-1-i]=tmp;
    }
}
 
// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
    int i = 0;
    int isNegative = 0;
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = 1;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        num = num/base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    return str;
}

/**
 * Convert a 64 bit decimal to hex
 */
char* dec_to_hex64(uint64_t d, char* str) {
    unsigned int n;
    int c;
    int i = 0;
    for (c = 60; c >= 0; c -= 4) {
        n = (d >> c) & 0xF;
        n += n > 9 ? 0x37 : 0x30;
        str[i++] = n;
    }
    return str;
}

void str_clear(char str[], int len) {
    int i;
    for (i = 0; i < len; i++) {
        str[i] = '\0';
    }
}