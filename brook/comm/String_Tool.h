#ifndef _TSM_POST_ADDRESS_STRING_TOOL_H_
#define _TSM_POST_ADDRESS_STRING_TOOL_H_

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdarg.h>


namespace DealStringFun
{
	inline int hex2int(const std::string value_str)
	{
		int a = 0;
		sscanf_s(value_str.c_str(),"%x",&a);
		return a;
	}

	inline std::string int2hex(const int n)
	{
		char hex[17] = "";
		sprintf_s(hex,"%016x",n);
		return std::string(hex);
	}

    template<typename Type>
    inline Type string_to_type(const std::string& value)
    {
        Type val_temp;
        std::stringstream ss;
        ss << value;
        ss >> val_temp;

        return val_temp;
    };


    template<typename Type>
    inline Type string_to_type(const char* value)
    {
        Type val_temp;
        std::string strtemp(value);
        std::stringstream ss;
        ss << strtemp;
        ss >> val_temp;

        return val_temp;
    };


    template<typename Type>
	inline std::string type_to_string(const Type& value, size_t nWide = 0, char chFill = ' ')
    {
        std::string strValue;
		std::stringstream ss;
		ss << std::setw(nWide);
		ss << std::setfill(chFill);
        ss << value;
        ss >> strValue;

        return strValue;
    };

	//	这个拆分会破坏原有数据，不保留空字段
	size_t split_string_withAllempty(char* pchBuff, char chSplit, 
		std::vector<char*>& vecResult);
    //	这个拆分会破坏原有数据，保留空字段
    size_t split_string_withempty(char* pchBuff, char chSplit, 
        std::vector<char*>& vecResult);

	int split_string(std::string str,std::vector<std::string>& strs,std::string splitstr);
 
} // namespace DealStringFun


#endif // !defined(_TSM_POST_ADDRESS_STRING_TOOL_H_)

