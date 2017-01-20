#include "IniFile.h"
#include <string>
#include <algorithm>
#include "String_Tool.h"
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32


#ifdef USE_MFC

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#endif

IniFile::IniFile(const char * pchFileName)
{
    m_bOpened = false;
    m_strFileName = pchFileName;
}

IniFile::~IniFile()
{
    if (m_inputFile.is_open())
    {
        m_inputFile.clear();
        m_inputFile.close();
    }

    for (std::list<Section*>::iterator it = m_listSection.begin();
        it != m_listSection.end(); it ++)
    {
        delete *(it);
    }
    m_listSection.clear();
}

void IniFile::SetFileName(const char * pchFileName)
{
    m_strFileName = pchFileName;
}


char* IniFile::Trim(char* pBuff, int nBuffLen)
{
    //  right;
    while ((*(pBuff + nBuffLen - 1) == ' ' || *(pBuff + nBuffLen - 1) == '\r') && nBuffLen >= 0)
    {
        nBuffLen --;
    }
    //  left
    while (*pBuff == ' ' || *pBuff == '\r')
    {
        pBuff ++;
        nBuffLen --;
    }
    pBuff[nBuffLen] = 0;

    //�����߿ո���������bug
    return pBuff;
}

void IniFile::ParseData(std::iostream& iStream)
{
    while (!iStream.eof()) 
    {
        char Line[1024];

        iStream.getline(Line, 1022);
        int nReadLen = iStream.gcount();
        if (nReadLen == 0)
        {
            continue;
        }
        Line[nReadLen] = 0;
        char* pLine = Line;
        //����ĳ���,�������ַ�����,�����������������,���Դ˴��ĳ��ַ�����
        nReadLen = (int)strlen(pLine);
        Trim(pLine, nReadLen);
        nReadLen = (int)strlen(pLine);
        if (*pLine == ';')
        {
            //  ��һ����ע��
            Section *pSection;
            //�����ʱ��û��section,���һ����section���ڱ���ǰ�����Ч����
            if(m_listSection.empty())
            {
                pSection = new Section;
                pSection->m_strSectionName = "";
                m_listSection.push_back(pSection);
            }
            else
            {
                pSection = m_listSection.back();
            }
            LoadLine(pSection, pLine);
            continue;
        }
        if (nReadLen < 3)
        {
            //  ��һ��̫���ˣ���Ч
            Section *pSection;
            //�����ʱ��û��section,���һ����section���ڱ���ǰ�����Ч����
            if(m_listSection.empty())
            {
                pSection = new Section;
                pSection->m_strSectionName = "";
                m_listSection.push_back(pSection);
            }
            else
            {
                pSection = m_listSection.back();
            }
            LoadLine(pSection, pLine);
            continue;
        }
        if (*pLine == '[' && *(pLine + nReadLen - 1) == ']')
        {
            //  ���һ�� section
            *(pLine + nReadLen - 1) = 0;
            pLine ++;
            Section *pSection = new Section;
            pSection->m_strSectionName = pLine;
            m_listSection.push_back(pSection);
            //  ���
            continue;
        }
//         if (m_listSection.empty())
//         {
//             LoadLine(pLine);
//             continue;
//         }
        Section *pSection = m_listSection.back();
//         if (pSection == NULL)
//         {
//             LoadLine(pLine);
//             continue;
//         }
        char* pSplit = strchr(pLine, '=');
        if (pSplit == NULL)
        {
            //  ����������Ч
            LoadLine(pSection, pLine);
            continue;
        }
        char* pValue = pSplit + 1;
        *pSplit = 0;
        int nLenTemp = (int)(pSplit - pLine);
        pLine = Trim(pLine, nLenTemp);
        nLenTemp = nReadLen - nLenTemp - 1;
        pValue = Trim(pValue, nLenTemp);
        string_pair* pPair = new string_pair;
        pPair->first = pLine;
        pPair->second = pValue;
        pSection->m_listValue.push_back(pPair);
    }
}

void IniFile::LoadLine(Section *pSection, const char * pLine)
{
    string_pair* pPair = new string_pair;
    pPair->first = "";
    pPair->second = pLine;
    pSection->m_listValue.push_back(pPair);
}

string_pair * IniFile::GetPairItem(const char * pchSection, const char * pchKey)
{
    //��������д�
    if(pchSection == NULL || pchKey == NULL)
        return NULL;
    if(*pchSection == 0 || *pchKey == 0)
        return NULL;

    std::list<Section *>::iterator iSection;
    for(iSection = m_listSection.begin(); iSection != m_listSection.end(); iSection++)
    {
        if((*iSection)->m_strSectionName == pchSection)
            break;
    }

    std::list<string_pair*>::iterator iStringPair;
    if(iSection != m_listSection.end())
    {
        for(iStringPair = (*iSection)->m_listValue.begin(); 
            iStringPair != (*iSection)->m_listValue.end(); iStringPair++)
        {
            if((*iStringPair)->first == pchKey)
                return (*iStringPair);
        }
        return NULL;
    }
    else
    {
        return NULL;
    }
}

string_pair * IniFile::CreatePairItem(const char * pchSection, const char * pchKey)
{
    //����section
    std::list<Section*>::iterator itSection;
    for(itSection = m_listSection.begin(); itSection != m_listSection.end(); itSection++)
    {
        if((*itSection)->m_strSectionName == pchSection)
            break;
    }
    //û�ҵ�section
    if(itSection == m_listSection.end())
    {
        Section * pSection;
        pSection = new Section;
        pSection->m_strSectionName = pchSection;
        m_listSection.push_back(pSection);
        itSection = m_listSection.end();
        itSection--;
    }
    //����pair
    std::list<string_pair*>::iterator itPair;
    for(itPair = (*itSection)->m_listValue.begin(); itPair != (*itSection)->m_listValue.end(); itPair++)
    {
        if((*itPair)->first == pchKey)
            break;
    }
    //û�ҵ�pair
    if(itPair == (*itSection)->m_listValue.end())
    {
        string_pair * pPair;
        pPair = new string_pair;
        pPair->first = pchKey;
        (*itSection)->m_listValue.push_back(pPair);
        return pPair;
    }
    return *itPair;
}

int IniFile::GetInt(const char *pchSection, const char *pchKey, int& nDefaultValue)
{
    string_pair * pPair = GetPairItem(pchSection, pchKey);
    if(pPair != NULL)
    {
        if(pPair->second.length() == 0)
        {
            return -1;
        }
        else
        {
            char ** pEnd = NULL;
            nDefaultValue = strtol(pPair->second.c_str(), pEnd, 10);
            return 0;
        }
    }
    return -1;
}

int IniFile::GetInt64(const char *pchSection, const char *pchKey, INT64& nDefaultValue)
{
	string_pair * pPair = GetPairItem(pchSection, pchKey);
	if(pPair != NULL)
	{
		if(pPair->second.length() == 0)
		{
			return -1;
		}
		else
		{
			char ** pEnd = NULL;
			//nDefaultValue = _strtoi64(pPair->second.c_str(), pEnd, 10);
			nDefaultValue = DealStringFun::string_to_type<INT64>(pPair->second);
			return 0;
		}
	}
	return -1;
}

int IniFile::GetShort(const char* pchSection, const char* pchKey, unsigned short& nDefaultValue)
{
    string_pair * pPair = GetPairItem(pchSection, pchKey);
    if(pPair != NULL)
    {
        if(pPair->second.length() == 0)
        {
            return -1;
        }
        else
        {
            char ** pEnd = NULL;
            nDefaultValue = (unsigned short)strtol(pPair->second.c_str(), pEnd, 10);
            return 0;
        }
    }
    return -1;
}

int IniFile::GetBool(const char *pchSection, const char *pchKey, int& bDefaultValue)
{
    string_pair * pPair = GetPairItem(pchSection, pchKey);
    std::string sValue;

    if(pPair != NULL)
    {
        sValue = pPair->second;
        transform(sValue.begin(), sValue.end(), sValue.begin(), tolower);
        
        if(sValue == "ture" || sValue == "1" || sValue == "t")
        {
            bDefaultValue = 1;
            return 0;
        }
        else if(sValue == "false" || sValue == "0" || sValue == "f")
        {
            bDefaultValue = 0;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    return -1;
}

int IniFile::GetString(const char* pchSection, 
					   const char* pchKey, 
					   std::string& strDefaultValue,
					   bool bAdd)
{
    string_pair * pPair = GetPairItem(pchSection, pchKey);
    if(pPair != NULL)
    {
        if(pPair->second.length() == 0)
        {
            return -1;
        }
        else
        {
            strDefaultValue = pPair->second;
			if(bAdd && !strDefaultValue.empty())
			{
				if(strDefaultValue[strDefaultValue.length() - 1] != '/')
				{
					strDefaultValue.append(1,'/');
				}
			}
            return 0;
        }
    }
    return -1;
}

int IniFile::SetBool(const char *pchSection, const char *pchKey, int bValue)
{
    string_pair * pPair = GetPairItem(pchSection, pchKey);
    if(pPair == NULL)
    {
        pPair = CreatePairItem(pchSection, pchKey);
    }

    if(bValue)
    {
        pPair->second = "1";
    }
    else
    {
        pPair->second = "0";
    }
    m_bIsWrited = true;
    return 0;
}

int IniFile::SetInt(const char *pchSection, const char *pchKey, int nValue)
{
    string_pair * pPair = GetPairItem(pchSection, pchKey);
    if(pPair == NULL)
    {
        pPair = CreatePairItem(pchSection, pchKey);
    }

    char szValue[255];
#pragma warning(push)
#pragma warning(disable : 4996)
    sprintf(szValue, "%d", nValue);
#pragma warning(pop)
    pPair->second = szValue;
    m_bIsWrited = true;
    return 0;
}

int IniFile::SetShort(const char* pchSection, const char* pchKey, unsigned short nDefaultValue)
{
    string_pair * pPair = GetPairItem(pchSection, pchKey);
    if(pPair == NULL)
    {
        pPair = CreatePairItem(pchSection, pchKey);
    }

    char szValue[255];
#pragma warning(push)
#pragma warning(disable : 4996)
    sprintf(szValue, "%d", nDefaultValue);
#pragma warning(pop)
    pPair->second = szValue;
    m_bIsWrited = true;
    return 0;
}

int IniFile::SetString(const char* pchSection, const char* pchKey, const char* psValue)
{
    string_pair * pPair = GetPairItem(pchSection, pchKey);
    if(pPair == NULL)
    {
        pPair = CreatePairItem(pchSection, pchKey);
    }

    pPair->second = psValue;
    m_bIsWrited = true;
    return 0;
}

int IniFile::WriteStream(std::iostream& oStream)
{
    std::list<Section*>::iterator itSection;
    std::list<string_pair*>::iterator itPair;
    for(itSection = m_listSection.begin(); itSection != m_listSection.end(); itSection++)
    {
        //������ǣ�α��Section�����
        if((*itSection)->m_strSectionName.length())
        {
            oStream<<"[" << (*itSection)->m_strSectionName << "]" <<std::endl;
        }
        for(itPair = (*itSection)->m_listValue.begin(); itPair != (*itSection)->m_listValue.end(); itPair++)
        {
            //������pair
            if((*itPair)->first.length())
            {
                oStream<< (*itPair)->first << " = " << (*itPair)->second << std::endl;
            }
            //ע�ͣ����С�������������pair
            else
            {
                oStream<< (*itPair)->second << std::endl;
            }
        }
    }
    return 0;
}

int IniFile::Open()
{
    int nRes = -1;
    if (m_bOpened)
    {
    }
    else
    {
        setlocale(LC_ALL,"");
        m_inputFile.open(m_strFileName.c_str(), std::ios::in);
        m_bOpened = m_inputFile.is_open();
        if (m_bOpened)
        {
            nRes = 0;
            ParseData(m_inputFile);
        }
        else
        {
            nRes = -2;
        }
        m_inputFile.clear();
        m_inputFile.close();
    }

    return nRes;
}

int IniFile::ReadStream(std::iostream& iStream)
{
    ParseData(iStream);
    return 0;
}

int IniFile::WriteIniFile()
{
    if(!m_bIsWrited)
    {
        return 0;
    }
    std::fstream outputFile;
    outputFile.open(m_strFileName.c_str(), std::ios::out);
    if(!outputFile.is_open())
    {
        return -1;
    }

    WriteStream(outputFile);

    outputFile.close();
    return 0;
}

//
//
//int IniFile::getPchar(const char *p_szSection, const char *p_szKey, char *p_szDefaultValue){
//    char szBuff[1024];
//    char szKey[32] ;
//    char szDefaultValue[64] ;
//    char szCurrentSection[32];
//    string strLine;
//    int iLen = 0, bInSelfSection = 0 ;
//    char * pStrValue = NULL ;
//
//    memset(m_szSection, 0, sizeof(m_szSection) ) ;
//    strncpy(m_szSection, p_szSection, sizeof(m_szSection)-1);
//
//    memset(szKey, 0, sizeof(szKey));
//    strncpy(szKey, p_szKey, sizeof(szKey) - 1);
//
//    memset(szDefaultValue, 0, sizeof(szDefaultValue));
//    strncpy(szDefaultValue, p_szDefaultValue, sizeof(szDefaultValue)-1);
//
//    inFile.seekg(0, ios_base::beg); //���ļ�ָ��ָ���ʼdλ��
//    while(!inFile.eof()){
//        getline(inFile, strLine);
//
//        memset(szBuff, 0, sizeof(szBuff));
//        snprintf(szBuff, sizeof(szBuff)-1, "%s", strLine.c_str());
//        trim(szBuff);
//
//        if ( (szBuff[0] == '#') || (strlen(szBuff) < 3) ) { //ȡ����ע�ͺ��������ݵĽ���,
//            continue;
//        }
//
//        iLen = strlen(szBuff);
//        if (szBuff[iLen-1] == 0x0D) //��ÿһ�����õĻ��з�������ȥ��
//            szBuff[iLen-1] = 0x0;
//
//        iLen = strlen(szBuff);
//        if (szBuff[0] == '[' && szBuff[iLen-1] == ']') { //�ж��Ƿ�ΪSection  [MOSERVER]
//            if (bInSelfSection)
//                bInSelfSection = 0;
//
//            szBuff[iLen-1] = 0x0;
//            memset(szCurrentSection, 0, sizeof(szCurrentSection));
//            strcpy(szCurrentSection, szBuff + 1); //ȡ��section��
//
//            if (strcasecmp(m_szSection, szCurrentSection) == 0) {  //������Լ���Ҫ��section�����ñ��
//                bInSelfSection = 1;
//                continue;
//            }
//        }
//
//        if (!bInSelfSection)  //���û�ж�����Ҫ��section�������
//            continue;
//
//        if (pStrValue == NULL){
//            pStrValue = strchr(szBuff, '=');  //����'='��λ��,û���ҵ������һ��
//        }
//
//        if (pStrValue == NULL){
//            continue;
//        }
//        *pStrValue = 0; //��'='��Ϊ'0'���ָ�value��key
//        pStrValue++;
//        if (*pStrValue == 0){//���û�ж���value���������һ��
//            continue;
//        }
//        if (bInSelfSection) {
//            if(strcasecmp(szKey,szBuff)==0){
//                strncpy(p_szDefaultValue, pStrValue, 64 - 1);
//                return 0;
//            }
//        }
//        pStrValue = NULL;
//    }
//    return -1;
//}
//
//
//int IniFile::getString(string p_strSection, string p_strKey, string& p_strDefaultValue){
//    inFile.seekg(0, ios_base::beg);
//    char szDefaultValue[64];
//    memset(szDefaultValue, 0, sizeof(szDefaultValue));
//    getPchar(p_strSection.c_str(), p_strKey.c_str(), szDefaultValue);
//    getPchar("MOSERVER", "TIMEOUT", szDefaultValue);
//    //printf("getchar: %s\n", szDefaultValue);//
//    p_strDefaultValue = szDefaultValue;
//    return 0;
//} 


std::string app_path()
{
#ifdef _WIN32
	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	std::string _exeName = szFilePath;
	_exeName = _exeName.substr(0, _exeName.rfind("\\"));
	_exeName = _exeName + "\\";
	return _exeName;
#else
	std::string _exeName = "/proc/self/exe";
	size_t linksize = 256;
	char   exeName[256] = { 0 };
	if (readlink(_exeName.c_str(), exeName, linksize) != -1)
	{
		_exeName = exeName;
	}
	_exeName = _exeName.substr(0, _exeName.rfind("/"));
	_exeName = _exeName + "/";

	return _exeName;
#endif
}
