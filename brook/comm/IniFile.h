/************************************************************************
文件名: IniFile.h
作者: 张旭
日期: 2009-01-14
说明: 用于操作 INI 文件的类   
 ' 用于注释整行，不处理部分注释的情形
************************************************************************/

#ifndef _INI_FILE_H_
#define _INI_FILE_H_

#include <string>
#include <fstream>
#include <list>
#include "Config.h"

typedef std::pair<std::string, std::string>  string_pair;

class IniFile 
{
    struct Section
    {
        std::list<string_pair*> m_listValue;
        ~Section()
        {            
            for (std::list<string_pair*>::iterator it = m_listValue.begin();
                it != m_listValue.end(); it ++)
            {
                delete *(it);
            }
            m_listValue.clear();
        }
        std::string m_strSectionName;
    };

public:
    IniFile(){ m_bOpened= false; m_bIsWrited = false;}

    IniFile(const char * pchFileName);
    ~IniFile();

public:
    void SetFileName(const char * pchFileName);
    int Open();
    int GetInt(const char* pchSection, const char* pchKey, int& nDefaultValue);
	int GetInt64(const char* pchSection, const char* pchKey, INT64& nDefaultValue);
	int GetShort (const char* pchSection, const char* pchKey, unsigned short& nDefaultValue);
    int GetBool  (const char* pchSection, const char* pchKey, int& bDefaultValue);
    int GetString(const char* pchSection, const char* pchKey, std::string& strDefaultValue,bool bAdd = false);

    int SetInt   (const char* pchSection, const char* pchKey, int nValue);
	int SetShort (const char* pchSection, const char* pchKey, unsigned short nDefaultValue);
    int SetBool  (const char* pchSection, const char* pchKey, int bValue);
    int SetString(const char* pchSection, const char* pchKey, const char* psValue);

    int ReadStream(std::iostream& iStream);
    int WriteStream(std::iostream& oStream);
    int WriteIniFile();

private:
    void ParseData(std::iostream& iStream);
    char * Trim(char* pBuff, int nBuffLen);
    string_pair * GetPairItem(const char * pchSection, const char * pchKey);
    string_pair * CreatePairItem(const char * pchSection, const char * pchKey);
    inline void LoadLine(Section *pSection, const char * pLine);

private:
    std::fstream m_inputFile;
    std::string m_strFileName;
    std::list<Section*> m_listSection;
    bool m_bOpened;
    bool m_bIsWrited;
};

std::string app_path();
#endif  //_INI_FILE_H_
