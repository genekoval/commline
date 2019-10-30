#include "file.h"

#include <resources.h>

std::string get_file() {
    return std::string(
        reinterpret_cast<char*>(resources_file_txt),
        resources_file_txt_len
    );
}
