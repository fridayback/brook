#include "String_Tool.h"
#include <string.h> 
#include <algorithm>

#ifdef USE_MFC

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#endif


namespace DealStringFun
{
	//	这个拆分会破坏原有数据，不保留空字段
	size_t split_string_withAllempty(char* pchBuff, char chSplit, 
		std::vector<char*>& vecResult)
	{
		while (pchBuff)
		{
            if(*pchBuff != ' ')
            {
                vecResult.push_back(pchBuff);
            }
			
			//	第一个层级的循环
			char* pchRight = strchr(pchBuff, chSplit);
			if (pchRight)
			{
				*pchRight = 0;
				pchRight ++;
				while(*pchRight == ' ')
				{
					*pchRight = 0;
					pchRight ++;
				}
			}

			pchBuff = pchRight;
		}
		return vecResult.size();
	}

    //	这个拆分会破坏原有数据，保留空字段
    size_t split_string_withempty(char* pchBuff, char chSplit, 
        std::vector<char*>& vecResult)
    {
        while (pchBuff)
        {
            vecResult.push_back(pchBuff);
            //	第一个层级的循环
            char* pchRight = strchr(pchBuff, chSplit);
            if (pchRight)
            {
                *pchRight = 0;
                pchRight ++;
            }

            pchBuff = pchRight;
        }
        return vecResult.size();
    }

	int split_string(std::string str,std::vector<std::string>& strs,std::string splitstr)
	{
		strs.clear();
		const char *d = splitstr.c_str();
		char *p = NULL;
 		char *ptr = new char[str.length()+1];
 		memset(ptr,0,str.length()+1);
		memcpy(ptr, str.c_str(), str.length());

		char *tmp = NULL;
		
#ifdef _WIN32
		p = strtok_s(ptr,d, &tmp);
#else
		p = strtok_r(ptr, d, &tmpstr);
#endif
		
		while(p)
		{
			std::string tmpstr = "";
			tmpstr.append(p);
			strs.push_back(tmpstr);
#ifdef _WIN32
			p = strtok_s(NULL, d, &tmp);
#else
			p = strtok_r(NULL, d, &tmpstr);
#endif
		}
		delete [] ptr;
		return strs.size();
	}
    
}//namespace DealStringFun

