/*
** �ṩ����ת��
** author
**   taoabc@gmail.com
*/
#ifndef ULT_CONV_H_
#define ULT_CONV_H_
#pragma once

#include <string>

namespace ult {
	std::wstring Utf8ToUnicode(const char* src, int len);
	std::wstring Utf8ToUnicode(const std::string& src);
	std::string  UnicodeToUtf8(const wchar_t* src, int len);
	std::string  UnicodeToUtf8(const std::wstring& src);
	std::wstring AnsiToUnicode(const char* src, int len);
	std::wstring AnsiToUnicode(const std::string& src);
	std::string  UnicodeToAnsi(const wchar_t* src, int len);
	std::string  UnicodeToAnsi(const std::wstring& src);
}

#endif // ULT_CONV_H_