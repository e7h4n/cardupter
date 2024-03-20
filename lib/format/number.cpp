#include "number.h"
#include <algorithm> 
#include <stdarg.h>

std::string printfToString(const char *format, ...)
{
    // 定义一个足够大的缓冲区来容纳输出
    char buffer[1024];

    // 使用变参函数处理格式化字符串
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 将字符数组转换为字符串并返回
    return std::string(buffer);
}

std::string thousandSeparator(float n, int precision)
{

    std::string ans = "";
    std::string fmt = printfToString("%%.%df", precision);
    std::string num = printfToString(fmt.c_str(), n);

    size_t dot_pos = num.find('.');

    int count = 0;
    for (int i = num.size() - 1; i >= 0; i--)
    {
        if (dot_pos == std::string::npos || i < dot_pos)
        {
            count++;
        }
        ans.push_back(num[i]);

        if (count == 3 && i != 0)
        {
            ans.push_back(',');
            count = 0;
        }
    }

    reverse(ans.begin(), ans.end());
    if (ans.size() % 4 == 0)
    {
        ans.erase(ans.begin());
    }

    return ans;
}