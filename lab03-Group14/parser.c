#include "parser.h"

//Command to trim whitespace and ASCII control characters from buffer
//[Input] char* inputbuffer - input string to trim
//[Input] size_t bufferlen - size of input and output string buffers
//[Output] char* outputbuffer - output string after trimming 
//[Return] size_t - size of output string after trimming
size_t trimstring(char* outputbuffer, const char* inputbuffer, size_t bufferlen)
{   
    memcpy(outputbuffer, inputbuffer, bufferlen*sizeof(char));

    for(size_t ii = strlen(outputbuffer)-1; ii >=0; ii--){
        if(outputbuffer[ii] < '!') //In ASCII '!' is the first printable (non-control) character
        {
            outputbuffer[ii] = 0;
        }else{
            break;
        }    
    }

    return strlen(outputbuffer);
}

//Command to trim the input command to just be the first word
//[Input] char* inputbuffer - input string to trim
//[Input] size_t bufferlen - size of input and output string buffers
//[Output] char* outputbuffer - output string after trimming 
//[Return] size_t - size of output string after trimming
size_t firstword(char* outputbuffer, const char* inputbuffer, size_t bufferlen) {
    size_t i = 0;
    while (i < bufferlen && inputbuffer[i] != '\0' && (inputbuffer[i] == ' ' || inputbuffer[i] == '\t')) {
        i++; // Skip leading whitespace
    }

    size_t j = 0;
    while (i < bufferlen && inputbuffer[i] != '\0' && inputbuffer[i] != ' ' && inputbuffer[i] != '\t') {
        outputbuffer[j++] = inputbuffer[i++];
    }

    outputbuffer[j] = '\0';

    return j;
}


//Command to test that string only contains valid ascii characters (non-control and not extended)
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] bool - true if no invalid ASCII characters present
bool isvalidascii(const char* inputbuffer, size_t bufferlen)
{
    size_t testlen = bufferlen;
    size_t stringlength = strlen(inputbuffer);
    if (stringlength < bufferlen) {
        testlen = stringlength;
    }

    bool isValid = true;
    for (size_t ii = 0; ii < testlen; ii++) {
        char currentChar = inputbuffer[ii];
        if (!(currentChar >= 32 && currentChar <= 126)) {
            isValid = false;
            break;
        }
    }

    return isValid;
}


//Command to find location of pipe character in input string
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] int - location in the string of the pipe character, or -1 pipe character not found
int findpipe(const char* inputbuffer, size_t bufferlen) {
    for (size_t i = 0; i < bufferlen; i++) {
        if (inputbuffer[i] == '|') {
            return i;
        }
    }
    return -1; // Pipe character not found
}

//Command to test whether the input string ends with "&" and
//thus represents a command that should be run in background
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] bool - true if string ends with "&"
bool runinbackground(const char* inputbuffer, size_t bufferlen) {
    if (bufferlen > 0 && inputbuffer[bufferlen - 1] == '&') {
        return true;
    }
    return false;
}