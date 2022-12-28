#ifndef FILE_IO_ERROR
#define FILE_IO_ERROR

#include <stdexcept>

class FileIOError : public std::runtime_error {
public:
    explicit FileIOError(const char *description)
            : std::runtime_error(description) {
    }
};

#endif //FILE_IO_ERROR
