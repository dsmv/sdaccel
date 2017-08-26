
#ifndef __EXCEPTION_INFO_H__
#define __EXCEPTION_INFO_H__

#include <string>

typedef  struct _except_info_t {
    std::string info;
} except_info_t;

except_info_t except_info(const char *fmt, ...);

#endif //__EXCEPTION_INFO_H__

