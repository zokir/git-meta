#include <lib/util/git_util.hpp>
#include <lib/core/error.hpp>

namespace {

dev_t getDevice(boost::filesystem::path const& directory) {
    struct stat statBuffer;
    int status = 0;
    status = stat(directory.string().c_str(), &statBuffer);
    if (status) {
        throw
            std::runtime_error(boost::str(boost::format("Cannot get stat for %1% with error %2% ")
                % directory % std::strerror(errno)));
    }

    return statBuffer.st_dev;
}

// Return parent directory if it is on the same device as specified 'directory'.
boost::filesystem::path getParentDirnameIfSameDev(boost::filesystem::path const& directory) {
    boost::filesystem::path const parentPath = directory.parent_path();

    dev_t parentMount = getDevice(parentPath);
    dev_t directoryMount = getDevice(parentPath);
    if (directoryMount != parentMount) {
        throw "Crossed filesystem boundary while looking for parent directory";
    }

    return parentPath;
}

} // close namespace<anonymous>

boost::filesystem::path
qmeta::lib::util::getContainingGitDir(boost::filesystem::path const& sourceDir) {
    using namespace boost::filesystem;

    if (!exists(sourceDir)) {
        throw boost::format("Directory %1% does not exist.") % sourceDir;
    }

    path const gitDir = sourceDir / ".git";
    if (is_directory(gitDir)) {
        return sourceDir;
    }

    path const parentPath = getParentDirnameIfSameDev(sourceDir);
    if (parentPath.empty() || "/" == parentPath) {
        throw qmeta::lib::core::NotInGitRepoError();
    }

    return getContainingGitDir(parentPath);
}

