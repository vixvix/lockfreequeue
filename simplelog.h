#ifndef __SIMPLELOG_H__
#define __SIMPLELOG_H__

#include <sstream>
#include <stdio.h>

struct SimpleLog
{
    std::ostringstream o;
    std::ostream& Get()
    {
        return o;
    }
    ~SimpleLog()
    {
        fprintf(stderr, "%s\n", o.str().c_str());
    }
};

#define LOG SimpleLog().Get()

#endif
