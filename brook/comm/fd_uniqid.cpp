#include "fd_uniqid.h"
#include <time.h>
#include "platform.h"
// #include <sstream>
// #include <iomanip>
#ifdef _WIN32
#include <winsock2.h>
#include <Iphlpapi.h>
#include <stdio.h>

#pragma comment(lib,"Iphlpapi.lib")
#else
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif


#ifdef _MSC_VER
typedef __int64 uniqid_t;
#else
typedef int64_t uniqid_t;
#endif

//static uniqid_t MACPART = 0;
static bool inited = false;
static char MACPART[6] = { 0 };
bool get_part_mac(char* buf);
#ifdef _WIN32

bool get_part_mac(char* buf) //获取本机MAC址
{
	
	if (!inited)  
	{
		PIP_ADAPTER_INFO pInfo = NULL;
		int temp = 0;
		ULONG ulSize = 0;
		temp = GetAdaptersInfo(pInfo, &ulSize);//第一处调用，获取缓冲区大小
		pInfo = (PIP_ADAPTER_INFO)malloc(ulSize);
		temp = GetAdaptersInfo(pInfo, &ulSize);
		if (pInfo)
		{
			memcpy(MACPART, pInfo->Address, sizeof(MACPART));
		}
		else
		{
			return false;
		}
	}
	inited = true;
	
	memcpy(buf,MACPART, sizeof(MACPART));
	
	return true ;
}
#else

bool get_part_mac(char* buf) //获取本机MAC址
{
	
	if (!inited)
	{
		struct ifreq tmp;
		int sock_mac;
		char mac_addr[30];
		sock_mac = socket(AF_INET, SOCK_STREAM, 0);
		if (sock_mac == -1)
		{
			return false;
		}
		memset(&tmp, 0, sizeof(tmp));
		strncpy(tmp.ifr_name, "eth0", sizeof(tmp.ifr_name) - 1);
		if ((ioctl(sock_mac, SIOCGIFHWADDR, &tmp)) < 0)
		{
			return false;
		}
		memcpy(MACPART, tmp.ifr_hwaddr.sa_data, sizeof(MACPART));
		close(sock_mac);
	}
	inited = true;
	memcpy(buf, MACPART, sizeof(MACPART));

	return true;
}
#endif

#ifdef _MSC_VER
typedef __int64 uniqid_t;
#else
typedef int64_t uniqid_t;
#endif

static const char FDCONVERT[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#%^&*()_+|-=[]{};':,./<>?~";
static const uniqid_t FDCARRY[] = { 90,8100,729000,65610000 ,5904900000 ,531441000000 ,47829690000000 ,4304672100000000 ,387420489000000000 };
std::string gen_uniqid(int anyBinary)
{
#if 0
	if (anyBinary > sizeof(FDCONVERT) / sizeof(char)) return "";
	timeval t;
	gettimeofday(&t, 0);
	char uuid[16] = { 0 };
	get_part_mac(&uuid[10]) ;
	uniqid_t milsec = t.tv_sec * 1000LL + t.tv_usec / 1000LL;
	int usec = t.tv_sec % 1000;
	memcpy(uuid, &milsec, 8);
	srand(usec);
	int randcnt = (rand() % 32)<<10;
	usec += randcnt;
	unsigned short timeadd = usec % 65536;
	memcpy(&uuid[9], &timeadd, 2);

	std::string ret = "";

	uniqid_t rem = *(uniqid_t*)uuid;
	char szTmp[2] = {0};
	while (rem != 0) {
		int nMod = rem % anyBinary;
		szTmp[0] = FDCONVERT[nMod];
		ret.append(szTmp);

		rem = rem / anyBinary;
	}
	rem = *(uniqid_t*)&uuid[8];

	while (rem != 0) {
		int nMod = rem % anyBinary;
		szTmp[0] = FDCONVERT[nMod];
		ret.append(szTmp);

		rem = rem / anyBinary;
	}
	return ret;
#endif 

	char buffer[64] = { 0 };
#ifdef _WIN32
	GUID guid;

	if (CoCreateGuid(&guid))
	{
		return "";
	}
// 	std::stringstream ss("");
// 	ss<< std::hex << std::setw(6) << std::setfill('0') << guid.Data1
// 		<<
	_snprintf_s(buffer, sizeof(buffer),
		"%08X%04X%04x%02X%02X%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
#else
#error not implement
#endif
	return buffer;
}
