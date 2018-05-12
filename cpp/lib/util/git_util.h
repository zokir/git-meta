#ifndef LIB_UTIL_GIT_UTIL_H
#define LIB_UTIL_GIT_UTIL_H

#include <string>

namespace qmeta {
namespace lib {
namespace util {

std::string getContainingGitDir(std::string const& sourceDir) {
}

/**
 * If the directory identified by the specified `dir` contains a ".git"
 * directory, return it.  Otherwise, return the first parent directory of `dir`
 * containing a `.git` directory.  If no such directory exists, return `None`.
 *
 * @private
 * @param {String} dir
 * @return {String}
 */
function getContainingGitDir(dir) {
    const gitDir = path.join(dir, ".git");
    if (fs.existsSync(gitDir) && fs.statSync(gitDir).isDirectory()) {
        return dir;                                                   // RETURN
    }

    const base = path.dirname(dir);

    if ("" === base || "/" === base) {
        return null;                                                  // RETURN
    }

    return getContainingGitDir(base);
}

} // cose namespace util
} // close namespace lib
} // close namespace qmeta

#endif
