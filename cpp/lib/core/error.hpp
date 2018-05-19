#ifndef LIB_CORE_ERROR_HPP
#define LIB_CORE_ERROR_HPP

#include <exception>

namespace qmeta {
namespace lib {
namespace core {

struct NotInGitRepoError : public std::exception {
    char const* what() const noexcept;
};

} // cose namespace core
} // close namespace lib
} // close namespace qmeta

#endif
