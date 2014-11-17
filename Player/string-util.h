#ifndef _CXM_STRING_UTIL_H_
#define _CXM_STRING_UTIL_H_

#include <string>
#include <vector>

namespace cxm {
namespace util {

class StringUtil {
	public: static void Split(const std::string &str,
		char spliter, std::vector<std::string> &container)
	{
		container.clear();

		size_t offsetStart = 0;
		size_t offsetEnd = 0;
		while (std::string::npos != (offsetEnd = str.find(spliter, offsetStart))) {
			std::string sub = str.substr(offsetStart, offsetEnd);
			if (sub.length() > 0)
				container.push_back(sub);
			offsetStart = offsetEnd + 1;
		}
		std::string sub = str.substr(offsetStart, offsetEnd);
		if (sub.length() > 0)
			container.push_back(sub);
	}
};

}
}

#endif
