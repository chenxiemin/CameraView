#ifndef _CXM_UTIL_PATH_HELPER_H_
#define _CXM_UTIL_PATH_HELPER_H_

#include <string>
#ifdef _WIN32
#include <direct.h>
#endif
#include <sys/stat.h>
#include <io.h>

namespace cxm {
namespace util {

class PathHelper
{
	public: static bool CreateDirectory(std::string path)
	{
#ifdef _WIN32
		return _mkdir(path.c_str()) == 0;
#endif
	}

	public: static bool IsExist(std::string path)
	{
#ifdef _WIN32
		return -1 != _access(path.c_str(), 0);
#endif
	}
};

}
}
#endif
