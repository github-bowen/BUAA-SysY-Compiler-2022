#ifndef PARSE_END_ERROR
#define PARSE_END_ERROR

#include <exception>

class ParseEndError : public std::exception {
public:
    explicit ParseEndError() : std::exception() {
    }
};


#endif //PARSE_END_ERROR
