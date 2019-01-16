#include "stdafx.h"
#include "CCert.h"


#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include <intrin.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include <ctime>
#include <chrono>

//#include "crypt/modes.h"
//#include "crypt/aes.h"
//#include "crypt/filters.h"

#include "crypt/osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include "crypt/cryptlib.h"
using CryptoPP::Exception;

#include "crypt/hex.h"
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include "crypt/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include "crypt/aes.h"
using CryptoPP::AES;

#include "crypt/modes.h"
using CryptoPP::CFB_Mode;


#include "crypt/crc.h"


static TCHAR* guidCert = L"SOFTWARE\\Classes\\CLSID\\{3ab1a3ed-dcd9-608f-bea8-233a5e0be969}";		// id, key
static TCHAR* guidDate = L"SOFTWARE\\Classes\\CLSID\\{cd300021-5efe-a1cc-8f0a-05530e5dad6b}";		// start date, end date, tick
static TCHAR* guidHardware = L"SOFTWARE\\Classes\\CLSID\\{670bd455-d311-6903-2bff-cab04e4fc5f9}";	// cpu id, mac address
static TCHAR* guidCrc = L"SOFTWARE\\Classes\\CLSID\\{2966a5de-2e3c-429b-8ef8-07e2f7337e8d}";		// cert, crc check

static byte certkey[5][CryptoPP::AES::DEFAULT_KEYLENGTH] = {
	{ 0xd6, 0x77, 0x7b, 0xbc, 0xd0, 0x46, 0xc8, 0x98, 0xba, 0x30, 0x3d, 0xbd, 0x5a, 0x62, 0x3b, 0xcf },
	{ 0xd6, 0xba, 0x30, 0x3d, 0xbd, 0x5a, 0x62, 0x3b, 0xcf, 0x77, 0x7b, 0xbc, 0xd0, 0x46, 0xc8, 0x98 },
	{ 0x29, 0x66, 0xa5, 0xde, 0x2e, 0x3c, 0x42, 0x9b, 0x8e, 0xf8, 0x07, 0x2f, 0x54, 0xfc, 0x5f, 0xf9 },
	{ 0x67, 0x0b, 0xd4, 0x55, 0xd3, 0x11, 0x69, 0x03, 0x2b, 0xff, 0xca, 0xb0, 0xe4, 0xfc, 0x5f, 0x6b },
	{ 0xcd, 0x30, 0x00, 0x21, 0x3f, 0x5e, 0xa1, 0xcc, 0x8f, 0x0a, 0x05, 0x52, 0x0e, 0x4d, 0xad, 0x6b },
};
//static byte key[1][CryptoPP::AES::DEFAULT_KEYLENGTH] = ;




CCert::CCert()
{
}


CCert::~CCert()
{
}





// we just need this for purposes of unique machine id. So any one or two mac's is       
// fine. 
UINT16 CCert::hashMacAddress(PIP_ADAPTER_INFO info)
{
	UINT16 hash = 0;
	for (UINT32 i = 0; i < info->AddressLength; i++)
	{
		hash += (info->Address[i] << ((i & 1) * 8));
	}
	return hash;
}

void CCert::getMacHash(UINT16& mac1, UINT16& mac2)
{
	IP_ADAPTER_INFO AdapterInfo[32];
	DWORD dwBufLen = sizeof(AdapterInfo);

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	if (dwStatus != ERROR_SUCCESS)
		return; // no adapters.      

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;


	//char szBuffer[512];
	while (pAdapterInfo)
	{
		if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET || pAdapterInfo->Type == IF_TYPE_IEEE80211)
		{
			mac1 = hashMacAddress(pAdapterInfo);
			mac2 = mac1;
			break;
		}
		pAdapterInfo = pAdapterInfo->Next;
	}

	//if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET) {

	//}

	//mac1 = hashMacAddress(pAdapterInfo);
	//if (pAdapterInfo->Next)
	//	mac2 = hashMacAddress(pAdapterInfo->Next);

	//// sort the mac addresses. We don't want to invalidate     
	//// both macs if they just change order.           
	//if (mac1 > mac2)
	//{
	//	UINT16 tmp = mac2;
	//	mac2 = mac1;
	//	mac1 = tmp;
	//}
}


UINT16 CCert::getCpuHash()
{
	int cpuinfo[4] = { 0, 0, 0, 0 };
	__cpuid(cpuinfo, 0);
	UINT16 hash = 0;
	UINT16* ptr = (UINT16*)(&cpuinfo[0]);
	for (UINT32 i = 0; i < 8; i++)
		hash += ptr[i];

	return hash;
}


BOOL CCert::CheckCert()
{
	BOOL bRet = FALSE;
	std::string id;
	std::string key;
	if (CheckRegistry(&id, &key) == FALSE) {
		return FALSE;
	}
	else {
		bRet = TRUE;
	}


	if (CanConnectServer()) {
		if (CheckCertServer(id, key)) {
			bRet = TRUE;
		}
		else {
			bRet = FALSE;
		}
	}

	return bRet;
}



BOOL CCert::RegisterCert(std::string& id, std::string& key)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UINT16 cpuHash = CCert::getCpuHash();
	UINT16 macHash1, macHash2;
	getMacHash(macHash1, macHash2);
	std::string localCpuHash = fmt::format("{:05}", cpuHash);
	std::string localMacHash = fmt::format("{:05}{:05}", macHash1, macHash2);

	std::stringstream result;
	std::string url = "http://211.110.61.145/able/auth/adm/member_license_register.php";

	try {
		std::string strPostData;

		curlpp::Cleanup cleaner;
		curlpp::Easy request;
		request.setOpt(new curlpp::options::Url(url));
		request.setOpt(new curlpp::options::Verbose(true));

		std::list<std::string> header;
		header.push_back("Content-Type: application/x-www-form-urlencoded");

		strPostData = fmt::format("mb_id={}&mb_1={}&mb_2={:05}&mb_3={:05}{:05}", id, key, cpuHash, macHash1, macHash2);


		request.setOpt(new curlpp::options::HttpHeader(header));
		request.setOpt(new curlpp::options::PostFields(strPostData));
		request.setOpt(new curlpp::options::PostFieldSize(strPostData.length()));
		request.setOpt(cURLpp::Options::WriteStream(&result));
		request.perform();

		std::string strResult;
		strResult = result.str();
		std::string split = "|";

		size_t posStart = 1;
		size_t posEnd = strResult.find(split, posStart);
		std::string serverId = strResult.substr(posStart, posEnd - posStart);

		posStart = posEnd + 1;
		posEnd = strResult.find(split, posStart);
		std::string serverCert = strResult.substr(posStart, posEnd - posStart);

		posStart = posEnd + 1;
		posEnd = strResult.find(split, posStart);
		std::string serverLicenseKey = strResult.substr(posStart, posEnd - posStart);

		posStart = posEnd + 1;
		posEnd = strResult.find(split, posStart);
		std::string strInDate = strResult.substr(posStart, posEnd - posStart);

		posStart = posEnd + 1;
		posEnd = strResult.find(split, posStart);
		std::string strLastDate = strResult.substr(posStart, posEnd - posStart);


		posStart = posEnd + 1;
		posEnd = strResult.find(split, posStart);
		std::string serverCpuHash = strResult.substr(posStart, posEnd - posStart);

		posStart = posEnd + 1;
		posEnd = strResult.find(split, posStart);
		std::string serverMacHash = strResult.substr(posStart, posEnd - posStart);


		if (serverCert.compare("S_OK") != 0) {
			return FALSE;
		}

		if (id.compare(serverId) != 0 || key.compare(serverLicenseKey) != 0) {
			return FALSE;
		}


		//TRACE("\n>>writestream (%s) <<<\n", result.str().c_str());
		//TRACE("\n>>parse (%s,%s,%s,%s,%s) <<<\n", strId.c_str(), strCert.c_str(), strlicenseKey.c_str(), localCpuHash.c_str(), localMacHash.c_str());
		if (SetRegistry(id, key, serverCert, strInDate, strLastDate, serverCpuHash, serverMacHash) == FALSE) {
			return FALSE;
		}

		return TRUE;
	}
	catch (curlpp::LogicError & e) {
		//TRACE(L"\n>>>>> error (%s) <<<<\n", e.what());
	}
	catch (curlpp::RuntimeError & e) {
		//TRACE(L"\n>>>>> error (%s) <<<<\n", e.what());
	}

	return FALSE;
}



BOOL CCert::CheckCertServer(std::string& id, std::string& licenseKey) {
	// 1. 서버에서 S_OK 값 인증
	// 2. id, license key 값 서버와 비교

	UINT16 cpuHash = CCert::getCpuHash();
	UINT16 macHash1, macHash2;
	getMacHash(macHash1, macHash2);

	std::stringstream result;
	std::string url = "http://211.110.61.145/able/auth/adm/member_license_check.php";
	std::string localCpuHash = fmt::format("{:05}", cpuHash);
	std::string localMacHash = fmt::format("{:05}{:05}", macHash1, macHash2);

	try {
		std::string strPostData;

		curlpp::Cleanup cleaner;
		curlpp::Easy request;
		request.setOpt(new curlpp::options::Url(url));
		request.setOpt(new curlpp::options::Verbose(true));

		std::list<std::string> header;
		header.push_back("Content-Type: application/x-www-form-urlencoded");

		strPostData = fmt::format("mb_id={}&mb_1={}&mb_2={:05}&mb_3={:05}{:05}", id, licenseKey, cpuHash, macHash1, macHash2);
		//s.erase(std::find_if(s.rbegin(), s.rend(),
		//	std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());

		request.setOpt(new curlpp::options::HttpHeader(header));
		request.setOpt(new curlpp::options::PostFields(strPostData));
		request.setOpt(new curlpp::options::PostFieldSize(strPostData.length()));
		request.setOpt(cURLpp::Options::WriteStream(&result));
		request.perform();

		std::string strResult;
		strResult = result.str();
		std::string split = "|";

		std::string serverId = "";
		std::string serverCert = "";
		std::string serverLicenseKey = "";
		std::string serverInDate = "";
		std::string serverLastDate = "";
		std::string serverCpuHash = "";
		std::string serverMacHash = "";


		if (strResult.length() < 2) {
			//SetRegistry(id, licenseKey, serverCert, serverInDate, serverLastDate, serverCpuHash, serverMacHash);
			return FALSE;
		}

		size_t posStart = 1;
		size_t posEnd = strResult.find(split, posStart);
		serverId = strResult.substr(posStart, posEnd - posStart);

		posStart = posEnd + 1;
		posEnd = strResult.find(split, posStart);
		serverCert = strResult.substr(posStart, posEnd - posStart);

		posStart = posEnd + 1;
		posEnd = strResult.find(split, posStart);
		serverLicenseKey = strResult.substr(posStart, posEnd - posStart);

		posStart = posEnd + 1;
		posEnd = strResult.find(split, posStart);
		serverInDate = strResult.substr(posStart, posEnd - posStart);

		posStart = posEnd + 1;
		posEnd = strResult.find(split, posStart);
		serverLastDate = strResult.substr(posStart, posEnd - posStart);

		//posStart = posEnd + 1;
		//posEnd = strResult.find(split, posStart);
		//serverCpuHash = strResult.substr(posStart, posEnd - posStart);

		//posStart = posEnd + 1;
		//posEnd = strResult.find(split, posStart);
		//serverMacHash = strResult.substr(posStart, posEnd - posStart);

		//TRACE("\n>>writestream (%s) (%s) <<<\n", strPostData.c_str(), result.str().c_str());
		//TRACE("\n>>parse (%s,%s,%s,%s,%s) <<<\n", strId.c_str(), strOk.c_str(), strlicenseKey.c_str(), strInDate.c_str(), strLastDate.c_str());

		//SetRegistry(serverId, serverLicenseKey, serverCert, serverInDate, serverLastDate, serverCpuHash, serverMacHash);

		if (serverCert.compare("S_OK") != 0) {
			return FALSE;
		}

		if (id.compare(serverId) != 0 || licenseKey.compare(serverLicenseKey) != 0) {
			return FALSE;
		}


		return TRUE;
	}
	catch (curlpp::LogicError & e) {
		//TRACE(L"\n>>>>> error (%s) <<<<\n", e.what());
		return FALSE;
	}
	catch (curlpp::RuntimeError & e) {
		//TRACE(L"\n>>>>> error (%s) <<<<\n", e.what());
		return FALSE;
	}

	return FALSE;
}


BOOL CCert::CanConnectServer() {
	BOOL bRet = FALSE;

	std::stringstream result;
	std::string url = "http://211.110.61.145/able/auth/adm/member_license_date.php";

	try {
		std::string strPostData;

		curlpp::Cleanup cleaner;
		curlpp::Easy request;
		request.setOpt(new curlpp::options::Url(url));
		request.setOpt(new curlpp::options::Verbose(true));

		std::list<std::string> header;
		header.push_back("Content-Type: application/x-www-form-urlencoded");

		request.setOpt(new curlpp::options::HttpHeader(header));
		request.setOpt(cURLpp::Options::WriteStream(&result));
		request.perform();

		std::string strResult;
		strResult = result.str();

		if (strResult == "OK") {
			bRet = TRUE;
		}

		return bRet;
	}
	catch (curlpp::LogicError & e) {
	}
	catch (curlpp::RuntimeError & e) {
	}

	return FALSE;
}


BOOL CCert::SetRegistry(std::string& id, std::string& licenseKey, std::string& cert, std::string& strInDate, std::string& strLastDate, std::string& strCpuHash, std::string& strMacHash) {
	// 아이디
	// 인증키
	// 인증 여부
	// 마지막사용시간
	// 만료시간
	// tick count
	// crc 값

	if (id.empty() || licenseKey.empty() || strInDate.empty() || strLastDate.empty())
		return FALSE;



	DWORD tickCount = 0;

	typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<24> >::type> days;
	std::tm tm_start = fromString(strInDate);
	std::tm tm_end = fromString(strLastDate);

	auto tms = std::chrono::system_clock::from_time_t(std::mktime(&tm_start));
	auto tme = std::chrono::system_clock::from_time_t(std::mktime(&tm_end));
	auto diff_in_days = std::chrono::duration_cast<days>(tme - tms);

	if (tme < tms) {
		return FALSE;
	}


	tickCount = diff_in_days.count();


	std::string strCrc = GetCrc(id, licenseKey, cert, strInDate, strLastDate, strCpuHash, strMacHash);


	id = EncryptAes(id, keyId);
	licenseKey = EncryptAes(licenseKey, keyLicense);
	cert = EncryptAes(cert, keyCert);
	strInDate = EncryptAes(strInDate, keyInDate);
	strLastDate = EncryptAes(strLastDate, keyLastDate);
	strCpuHash = EncryptAes(strCpuHash, keyHardware);
	strMacHash = EncryptAes(strMacHash, keyHardware);
	strCrc = EncryptAes(strCrc, keyCrc);


	HKEY key;
	DWORD dwDisp;
	LONG lRet = 0L;
	lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, guidCert, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dwDisp);
	if (lRet == ERROR_SUCCESS) {
		lRet = RegSetValueExA(key, "uid", 0, REG_SZ, (BYTE*)id.c_str(), id.length());
		lRet = RegSetValueExA(key, "lkey", 0, REG_SZ, (BYTE*)licenseKey.c_str(), licenseKey.length());
		if (lRet != ERROR_SUCCESS) {
			return FALSE;
		}
			
	}
	else {
		return FALSE;
	}
	RegCloseKey(key);


	lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, guidDate, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dwDisp);
	if (lRet == ERROR_SUCCESS) {
		lRet = RegSetValueExA(key, "sdt", 0, REG_SZ, (BYTE*)strInDate.c_str(), strInDate.length());
		lRet = RegSetValueExA(key, "edt", 0, REG_SZ, (BYTE*)strLastDate.c_str(), strLastDate.length());
		lRet = RegSetValueExA(key, "tick", 0, REG_DWORD, (BYTE*)&tickCount, sizeof(tickCount));
		if (lRet != ERROR_SUCCESS) {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
	RegCloseKey(key);


	lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, guidHardware, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dwDisp);
	if (lRet == ERROR_SUCCESS) {
		lRet = RegSetValueExA(key, "cpu", 0, REG_SZ, (BYTE*)strCpuHash.c_str(), strCpuHash.length());
		lRet = RegSetValueExA(key, "mac", 0, REG_SZ, (BYTE*)strMacHash.c_str(), strMacHash.length());
		if (lRet != ERROR_SUCCESS) {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
	RegCloseKey(key);



	lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, guidCrc, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dwDisp);
	if (lRet == ERROR_SUCCESS) {
		lRet = RegSetValueExA(key, "cert", 0, REG_SZ, (BYTE*)cert.c_str(), cert.length());
		lRet = RegSetValueExA(key, "chk", 0, REG_SZ, (BYTE*)strCrc.c_str(), strCrc.length());
		if (lRet != ERROR_SUCCESS) {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
	RegCloseKey(key);

	return TRUE;
}

BOOL CCert::CheckRegistry(std::string* id, std::string* licenseKey) {
	// 아이디
	// 인증키
	// 인증 여부
	// 마지막사용시간
	// 만료시간
	// tick count
	// crc 값
	// 체크값
	// 1. 레지스트리 값 가져오지 못하면 FALSE
	// 2. 아이디, 인증키, 사용시간, 만료시간 값 필수 체크
	// 3. 인증 여부 값 체크 ( 'S_OK' 값 )
	// 4. 만료일자와 마지막 사용일자 비교해서 만료일자가 마지막 사용일짜 보다 큰지 체크
	// 5. 현재시간이 마지막 사용일자보다 이후면 FALSE
	// 6. 남은 일수 값이 실제 시간 계산과 다르면 FALSE

	UINT16 cpuHash = CCert::getCpuHash();
	UINT16 macHash1, macHash2;
	getMacHash(macHash1, macHash2);
	std::string localCpuHash = fmt::format("{:05}", cpuHash);
	std::string localMacHash = fmt::format("{:05}{:05}", macHash1, macHash2);


	char szId[255] = { 0, };
	char szLicenseKey[255] = { 0, };
	char szCert[255] = { 0, };
	char szInDate[255] = { 0, };
	char szLastDate[255] = { 0, };
	DWORD tickCount = 0;
	char szCrc[255] = { 0, };
	char szCpu[255] = { 0, };
	char szMac[255] = { 0, };

	HKEY key;
	DWORD dwId = sizeof(szId);
	DWORD dwLicenseKey = sizeof(szLicenseKey);
	DWORD dwCert = sizeof(szCert);
	DWORD dwInDate = sizeof(szInDate);
	DWORD dwLastDate = sizeof(szLastDate);
	DWORD dwTickCount = sizeof(tickCount);
	DWORD dwCpu = sizeof(szCpu);
	DWORD dwMac = sizeof(szMac);
	DWORD dwCrc = sizeof(szCrc);


	
	DWORD lRet = 0;
	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, guidCert, 0, KEY_READ, &key);
	if (lRet == ERROR_SUCCESS) {
		if (RegQueryValueExA(key, "uid", NULL, NULL, (BYTE*)szId, &dwId) != ERROR_SUCCESS) {
		}
		if (RegQueryValueExA(key, "lkey", NULL, NULL, (BYTE*)szLicenseKey, &dwLicenseKey) != ERROR_SUCCESS) {
		}
	}
	RegCloseKey(key);

	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, guidDate, 0, KEY_READ, &key);
	if (lRet == ERROR_SUCCESS) {
		if (RegQueryValueExA(key, "sdt", NULL, NULL, (BYTE*)szInDate, &dwInDate) != ERROR_SUCCESS) {
		}
		if (RegQueryValueExA(key, "edt", NULL, NULL, (BYTE*)szLastDate, &dwLastDate) != ERROR_SUCCESS) {
		}

		if (RegQueryValueExA(key, "tick", NULL, NULL, (BYTE*)tickCount, &dwTickCount) != ERROR_SUCCESS) {
		}
	}
	RegCloseKey(key);

	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, guidHardware, 0, KEY_READ, &key);
	if (lRet == ERROR_SUCCESS) {
		if (RegQueryValueExA(key, "cpu", NULL, NULL, (BYTE*)szCpu, &dwCpu) != ERROR_SUCCESS) {
		}
		if (RegQueryValueExA(key, "mac", NULL, NULL, (BYTE*)szMac, &dwMac) != ERROR_SUCCESS) {
		}
	}
	RegCloseKey(key);

	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, guidCrc, 0, KEY_READ, &key);
	if (lRet == ERROR_SUCCESS) {
		if (RegQueryValueExA(key, "cert", NULL, NULL, (BYTE*)szCert, &dwCert) != ERROR_SUCCESS) {
		}
		if (RegQueryValueExA(key, "chk", NULL, NULL, (BYTE*)szCrc, &dwCrc) != ERROR_SUCCESS) {
		}
	}
	RegCloseKey(key);

	if (lRet == ERROR_SUCCESS) {
		*id = szId;
		*licenseKey = szLicenseKey;
		std::string strInDate = szInDate;
		std::string strLastDate = szLastDate;
		std::string strCert = szCert;
		std::string strCpu = szCpu;
		std::string strMac = szMac;
		std::string strCrc = szCrc;


		*id = DecryptAes(*id, keyId);
		*licenseKey = DecryptAes(*licenseKey, keyLicense);
		strCert = DecryptAes(strCert, keyCert);
		strInDate = DecryptAes(strInDate, keyInDate);
		strLastDate = DecryptAes(strLastDate, keyLastDate);
		strCpu = DecryptAes(strCpu, keyHardware);
		strMac = DecryptAes(strMac, keyHardware);
		strCrc = DecryptAes(strCrc, keyCrc);



		std::string strCrcConvert = GetCrc(*id, *licenseKey, strCert, strInDate, strLastDate, strCpu, strMac);

		// crc 계산값과 crc 저장된 값 비교
		if (strCrc.compare(strCrcConvert) == 0) {
		}
		else {
			return FALSE;
		}

		//TRACE("\n>>>> check (%s) (%s) (%s) (%s) (%s) (%s) (%s)  (%s) (%s) <<<<\n", (*id).c_str(), (*licenseKey).c_str(), strCert.c_str(), strInDate.c_str(), strLastDate.c_str(), strCpu.c_str(), strMac.c_str(), localCpuHash.c_str(), localMacHash.c_str());


		//TRACE("\n>>>> get reg last date (%s) <<<<\n", strLastDate.c_str());


		typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<24> >::type> days;
		std::tm tm_start = fromString(strInDate);
		std::tm tm_end = fromString(strLastDate);

		auto tms = std::chrono::system_clock::from_time_t(std::mktime(&tm_start));
		auto tme = std::chrono::system_clock::from_time_t(std::mktime(&tm_end));
		auto diff_in_days = std::chrono::duration_cast<days>(tme - tms);

		auto now = std::chrono::system_clock::now();
		auto in_time_now = std::chrono::system_clock::to_time_t(now);
		std::tm tmNow;
		localtime_s(&tmNow, &in_time_now);
		auto tmn = std::chrono::system_clock::from_time_t(std::mktime(&tmNow));

		if (UpdateRegistry(*id, *licenseKey, strCert, tm_start, tm_end, tmNow, strCpu, strMac) == FALSE) {
			return FALSE;
		}

		//TRACE("\n>>>> check (%s) (%s) (%s) (%s) (%s) (%s) (%s)  (%s) (%s) <<<<\n", (*id).c_str(), (*licenseKey).c_str(), strCert.c_str(), strInDate.c_str(), strLastDate.c_str(), strCpu.c_str(), strMac.c_str(), localCpuHash.c_str(), localMacHash.c_str());


		if ((*id).empty() || (*licenseKey).empty() || strInDate.empty() || strLastDate.empty() || strCpu.empty() || strMac.empty()) {
			return FALSE;
		}

		if (strCert.compare("S_OK") != 0) {
			return FALSE;
		}

		if (strCpu.compare(localCpuHash) != 0 || strMac.compare(localMacHash) != 0) {
			return FALSE;
		}


		if (tms > tme /*|| tickCount != diff_in_days.count() */|| tmn > tme/* || tickCount<=0*/ || tmn < tms) {
			return FALSE;
		}
	}
	else {
		*id = "";
		*licenseKey = "";

		return FALSE;
	}


	

	return TRUE;
}




BOOL CCert::UpdateRegistry(std::string id, std::string licenseKey, std::string cert, std::tm tm_start, std::tm tm_end, std::tm tmNow, std::string strCpuHash, std::string strMacHash) {
	// 인증 여부
	// 마지막사용시간
	// 만료시간
	// tick count
	// crc 값
	// 업데이트

	DWORD tickCount = 0;

	typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<24> >::type> days;
	auto tms = std::chrono::system_clock::from_time_t(std::mktime(&tm_start));
	auto tme = std::chrono::system_clock::from_time_t(std::mktime(&tm_end));
	auto tmn = std::chrono::system_clock::from_time_t(std::mktime(&tmNow));
	auto diff_in_days = std::chrono::duration_cast<days>(tme - tms);

	tickCount = diff_in_days.count();

	std::string strInDate;
	std::string strLastDate;


	if (tmn > tms) {
		strInDate = fromTime(tmNow);
		diff_in_days = std::chrono::duration_cast<days>(tme - tmn);
		tickCount = diff_in_days.count();
		if (tmn > tme) {
			tickCount = 0;
		}

	}
	else {
		strInDate = fromTime(tm_start);
	}
	
	strLastDate = fromTime(tm_end);


	std::string strCrc = GetCrc(id, licenseKey, cert, strInDate, strLastDate, strCpuHash, strMacHash);


	id = EncryptAes(id, keyId);
	licenseKey = EncryptAes(licenseKey, keyLicense);
	cert = EncryptAes(cert, keyCert);
	strInDate = EncryptAes(strInDate, keyInDate);
	strLastDate = EncryptAes(strLastDate, keyLastDate);
	strCpuHash = EncryptAes(strCpuHash, keyHardware);
	strMacHash = EncryptAes(strMacHash, keyHardware);
	strCrc = EncryptAes(strCrc, keyCrc);

	


	HKEY key;
	DWORD dwDisp;
	LONG lRet = 0L;
	lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, guidDate, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dwDisp);
	if (lRet == ERROR_SUCCESS) {
		lRet = RegSetValueExA(key, "sdt", 0, REG_SZ, (BYTE*)strInDate.c_str(), strInDate.length());
		lRet = RegSetValueExA(key, "edt", 0, REG_SZ, (BYTE*)strLastDate.c_str(), strLastDate.length());
		lRet = RegSetValueExA(key, "tick", 0, REG_DWORD, (BYTE*)&tickCount, sizeof(tickCount));
		if (lRet != ERROR_SUCCESS) {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
	RegCloseKey(key);


	lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE, guidCrc, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &dwDisp);
	if (lRet == ERROR_SUCCESS) {
		lRet = RegSetValueExA(key, "cert", 0, REG_SZ, (BYTE*)cert.c_str(), cert.length());
		lRet = RegSetValueExA(key, "chk", 0, REG_SZ, (BYTE*)strCrc.c_str(), strCrc.length());
		if (lRet != ERROR_SUCCESS) {
			return FALSE;
		}
	}
	else {
		return FALSE;
	}
	RegCloseKey(key);

	return TRUE;
}


std::string CCert::EncryptAes(std::string plaintext, enumKeyType keytype) {
	byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];

	memcpy(key, &certkey[keytype], sizeof(key));
	memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);


	std::string ciphertext;
	std::string ciphertextEncode;
	//
	// Create Cipher Text
	//
	// Encryption


	//AutoSeededRandomPool prng;

	//byte key[AES::DEFAULT_KEYLENGTH];
	//prng.GenerateBlock(key, sizeof(key));

	//byte iv[AES::BLOCKSIZE];
	//prng.GenerateBlock(iv, sizeof(iv));

	try
	{
		CFB_Mode< AES >::Encryption e;
		e.SetKeyWithIV(key, sizeof(key), iv);

		// CFB mode must not use padding. Specifying
		//  a scheme will result in an exception
		StringSource(plaintext, true,
			new StreamTransformationFilter(e,
				new StringSink(ciphertext)
			) // StreamTransformationFilter      
		); // StringSource
	}
	catch (const CryptoPP::Exception& e)
	{
	}

	ciphertextEncode.clear();
	StringSource(ciphertext, true,
		new HexEncoder(
			new StringSink(ciphertextEncode)
		) // HexEncoder
	); // StringSource

	return ciphertextEncode;
}



std::string CCert::DecryptAes(std::string ciphertext, enumKeyType keytype) {
	byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];

	memcpy(key, &certkey[keytype], sizeof(key));
	memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);

	std::string decryptedtext;
	std::string ciphertextDecode;

	ciphertextDecode.clear();
	StringSource(ciphertext, true,
		new HexDecoder(
			new StringSink(ciphertextDecode)
		) // HexEncoder
	); // StringSource


	try
	{
		CFB_Mode< AES >::Decryption d;
		d.SetKeyWithIV(key, sizeof(key), iv);

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(ciphertextDecode, true,
			new StreamTransformationFilter(d,
				new StringSink(decryptedtext)
			) // StreamTransformationFilter
		); // StringSource

	}
	catch (const CryptoPP::Exception& e)
	{
	}

	return decryptedtext;
}





std::string CCert::GetCrc(std::string& id, std::string& cert, std::string& licenseKey, std::string& inDate, std::string& lastDate, std::string& cpuHash, std::string& macHash) {
	std::string strCrcAll;
	std::string strCrcConvert;
	strCrcAll.append(id);
	strCrcAll.append(cert);
	strCrcAll.append(licenseKey);
	strCrcAll.append(inDate);
	strCrcAll.append(lastDate);
	strCrcAll.append(cpuHash);
	strCrcAll.append(macHash);

	CryptoPP::CRC32 hash;
	StringSource(strCrcAll, true,
		new CryptoPP::HashFilter(hash,
			new HexEncoder(new StringSink(strCrcConvert))
		)
	);

	return strCrcConvert;
}