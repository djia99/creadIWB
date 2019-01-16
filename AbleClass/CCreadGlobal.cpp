#include "stdafx.h"
#include "CCreadGlobal.h"
#include <memory>

CCreadGlobal::CCreadGlobal()
{
}


CCreadGlobal::~CCreadGlobal()
{
	//m_animationManagerEdit = nullptr;
	//m_animationTimerEdit = nullptr;
	//m_transitionLibraryEdit = nullptr;
	//m_animationManagerList = nullptr;
	//m_animationTimerList = nullptr;
	//m_transitionLibraryList = nullptr;
}


std::string CCreadGlobal::sFormat(const std::string string, ...) {
	int final_n, n = ((int)string.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1) {
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy(&formatted[0], string.c_str());
		va_start(ap, string);
		final_n = vsnprintf(&formatted[0], n, string.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}


std::wstring CCreadGlobal::sFormat(const std::wstring string, ...) {
	int final_n, n = ((int)string.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::unique_ptr<wchar_t[]> formatted;
	va_list ap;
	while (1) {
		formatted.reset(new wchar_t[n]); /* Wrap the plain char array into the unique_ptr */
		wcscpy(&formatted[0], string.c_str());
		va_start(ap, string);
		final_n = _vsnwprintf(&formatted[0], n, string.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::wstring(formatted.get());
}


//std::wstring CCreadGlobal::s2ws(const std::string& s)
//{
//	int len;
//	int slength = (int)s.length() + 1;
//	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
//	wchar_t* buf = new wchar_t[len];
//	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
//	std::wstring r(buf);
//	delete[] buf;
//	return r;
//}

std::wstring CCreadGlobal::s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0) * sizeof(TCHAR);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, &r[0], len);
	return r;
}

std::string CCreadGlobal::ws2s(const std::wstring& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
	std::string r(len, '\0');
	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
	return r;
}


//#include <locale>
//#include <codecvt>
//#include <string>
//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//std::wstring wide = converter.from_bytes(strNow);


//std::wstring utf8toUtf16(const std::string & str)
//{
//	if (str.empty())
//		return std::wstring();
//
//	size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
//		str.data(), (int)str.size(), NULL, 0);
//	if (charsNeeded == 0) {
//		return L"";
//	}
//
//
//	std::vector<wchar_t> buffer(charsNeeded);
//	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
//		str.data(), (int)str.size(), &buffer[0], buffer.size());
//	if (charsConverted == 0)
//		return L"";
//
//	return std::wstring(&buffer[0], charsConverted);
//}
