#ifndef LIB_UTIL_GIT_UTIL_H
#define LIB_UTIL_GIT_UTIL_H

#include <string>

#include <boost/filesystem.hpp>

namespace qmeta {
namespace lib {
namespace util {

boost::filesystem::path getContainingGitDir(boost::filesystem::path const& sourceDir) {
    using namespace boost::filesystem;
    path const gitDir = sourceDir / ".git";
    if (is_directory(gitDir)) {
        return sourceDir;
    }

    path const parentPath = sourceDir.parent_path();
    if (parentPath.empty() || "/" == parentPath) {
        throw "Not in git dir"; // create NotInGitRoot exception type
    }

    return getContainingGitDir(parentPath);
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
 /*
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
*/

} // cose namespace util
} // close namespace lib
} // close namespace qmeta

#endif
