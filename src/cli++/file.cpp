#include "file.h"

#include "../../resources/resources.h"

std::string get_file() {
    return std::string(
        reinterpret_cast<char*>(resources_file_txt),
        resources_file_txt_len
    );
}
