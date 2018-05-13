#ifndef LIB_UTIL_GIT_UTIL_H
#define LIB_UTIL_GIT_UTIL_H

#include <sys/stat.h>
#include <string>
#include <cerrno>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

namespace qmeta {
namespace lib {
namespace util {

// If the directory identified by the specified `sourceDir` contains a ".git"
// directory, return it.  Otherwise, return the first parent directory of `sourceDir`
// containing a `.git` directory.  If no such directory exists, throw 'NotInGitRepoError`.
boost::filesystem::path getContainingGitDir(boost::filesystem::path const& sourceDir);

} // cose namespace util
} // close namespace lib
} // close namespace qmeta

#endif
