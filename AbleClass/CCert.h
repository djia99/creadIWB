#pragma once


#include "lib/fmt/format.h"


enum enumKeyType {
	keyCert = 0,
	keyLicense = 0,
	keyId = 1,
	keyInDate = 1,
	keyLastDate = 2,
	keyHardware = 3,
	keyCrc = 4,
};

class CCert
{
public:
	CCert();
	~CCert();

	UINT16 getCpuHash();
	void getMacHash(UINT16& mac1, UINT16& mac2);
	UINT16 hashMacAddress(PIP_ADAPTER_INFO info);
	BOOL RegisterCert(std::string& id, std::string& key);
	BOOL CheckCert();
	BOOL CheckCertServer(std::string& id, std::string& licenseKey);
	BOOL CanConnectServer();

	BOOL CheckRegistry(std::string* id, std::string* licenseKey);
	BOOL SetRegistry(std::string& id, std::string& licenseKey, std::string& cert, std::string& strInDate, std::string& strLastDate, std::string& strCpuHash, std::string& strMacHash);
	BOOL UpdateRegistry(std::string id, std::string licenseKey, std::string cert, std::tm tm_start, std::tm tm_end, std::tm tmNow, std::string strCpuHash, std::string strMacHash);


	// cript
	std::string EncryptAes(std::string plaintext, enumKeyType keytype);
	std::string DecryptAes(std::string ciphertext, enumKeyType keytype);

	// crc
	std::string GetCrc(std::string& id, std::string& licenseKey, std::string& cert, std::string& inDate, std::string& lastDate, std::string& cpuHash, std::string& macHash);

	// time convert
	std::tm fromString(const std::string& str)
	{
		// only 0000-00-00 00:00:00
		std::tm time = { 0, };

		if (str.length() > 18) {
			time.tm_year = atoi(str.substr(0, 4).c_str()) - 1900;
			time.tm_mon = atoi(str.substr(5, 2).c_str()) - 1;
			time.tm_mday = atoi(str.substr(8, 2).c_str());
			time.tm_hour = atoi(str.substr(11, 2).c_str());
			time.tm_min = atoi(str.substr(14, 2).c_str());
			time.tm_sec = atoi(str.substr(17, 2).c_str());
		}
		return time;
	}

	// time convert
	std::string fromTime(const std::tm& tm_time) {
		return fmt::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}", tm_time.tm_year+1900, tm_time.tm_mon+1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
	}
};

