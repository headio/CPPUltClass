/*
** 提供字符串操作
** author
**   taoabc@gmail.com
** 为了方便函数返回字符直接进行操作，字符串操作头文件中的函数大部分通过返回值返回
*/
#ifndef ULT_STRING_OPERATE_H_
#define ULT_STRING_OPERATE_H_

#include <string>
#include <vector>
#include <Windows.h>
#include <cwctype>
#include <ctime>
#include <cstdlib>

namespace ult {

namespace detail {

struct MultiByteToUnicode {
  std::wstring operator()(const char* src, int len, unsigned int codepage) {
    std::wstring result;
    if (len > 0) {
      wchar_t* buf = new wchar_t[len];
      int outlen = ::MultiByteToWideChar(codepage, 0, src, len, buf, len);
      result.assign(buf, outlen);
      delete[] buf;
    }
    return result;
  }
};

struct UnicodeToMultiByte {
  std::string operator()(const wchar_t* src, int len, unsigned int codepage) {
    std::string result;
    if (len > 0) {
      char* buf = new char[len*3];
      int outlen = ::WideCharToMultiByte(codepage, 0, src, len, buf, len*3, NULL, NULL);
      result.assign(buf, outlen);
      delete[] buf;
    }
    return result;
  }
};

struct SplitString {
  bool operator()(const std::wstring& src,
                  const std::wstring& separator,
                  std::vector<std::wstring>* vec) {
    if (src.empty()) {
      return false;
    }
    vec->clear();
    if (separator.empty()) {
      vec->push_back(src);
      return true;
    }
    size_t pos;
    std::wstring tmp(src);
    std::wstring item;
    size_t separator_len = separator.length();
    while ((pos = tmp.find(separator)) != std::wstring::npos) {
      item = tmp.substr(0, pos);
      if (!item.empty()) {
        vec->push_back(item);
      }
      tmp = tmp.substr(pos + separator_len);
    }
    if (!tmp.empty()) {
      vec->push_back(tmp);
    }
    return true;
  }
};

struct CompareStringNoCase {
  int operator()(const std::wstring& comp1, const std::wstring& comp2) {
    size_t len1 = comp1.length();
    size_t len2 = comp2.length();
    if (len1 != len2) {
      return len1 < len2 ? -1 : 1;
    }
    for (size_t i = 0; i < len1; ++i) {
      if (!EqWchar(comp1.at(i), comp2.at(i))) {
        return LtWchar(comp1.at(i), comp2.at(i)) ? -1 : 1;
      }
    }
    return 0;
  }

private:
  bool EqWchar(const wchar_t& c1, const wchar_t& c2) {
    return std::towupper(c1) == std::towupper(c2);
  }

  bool LtWchar(const wchar_t& c1, const wchar_t& c2) {
    return std::towupper(c1) < std::towupper(c2);
  }
};

struct UInt64ToString {
  std::wstring operator()(unsigned __int64 num) {
    wchar_t temp[32];
    std::wstring result;
    int pos = 0;
    do {
      temp[pos++] = (wchar_t)(L'0' + (int)(num % 10));
      num /= 10;
    } while (0 != num);
    do {
      result += temp[--pos];
    } while (pos > 0);
    return result;
  }
};


struct UrlEncode {
  std::string operator()(const char* s, size_t len) {
    std::string encoded;
    char* buf = new char[16];
    unsigned char c;
    for (size_t i = 0; i < len; ++i) {
      c = s[i];
      if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_' ||
        c == '!' || c == '~' || c == '*' || c == '\'' || c == '(' || c == ')') {
          encoded += c;
          continue;
      }
      if (c == ' ') {
        encoded += '+';
        continue;
      }
      sprintf_s(buf, 16, "%x", c);
      encoded += '%';
      encoded += buf;
    }
    delete[] buf;
    return encoded;
  }
};


struct GetRandomString {
  std::wstring operator()(const size_t len, const std::wstring& random_table) {
    std::wstring random_string;
    std::wstring random_table_real(random_table);
    if (random_table_real.empty()) {
      random_table_real = L"1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    }
    int sreal = (int)random_table_real.length();
    for (size_t i = 0; i < len; ++i) {
      int r = GetRandomInteger(0, sreal);
      random_string.push_back(random_table_real.at(r));
    }
    return random_string;
  }

  GetRandomString(void) {
    SRand();
  }

private:
  int GetRandomInteger(int min_number, int max_number) {
    if (min_number > max_number) {
      std::swap(min_number, max_number);
    }
    return (int)((double)std::rand() / (RAND_MAX + 1) * (max_number - min_number) + min_number);
  }

  void SRand(void) {
    std::srand((unsigned int)std::time(NULL));
  }
};

struct StringReplace {
  std::wstring operator()(const std::wstring& str, const std::wstring& match, const std::wstring& replaced) {
    std::wstring result(str);
    size_t pos;
    while ((pos = result.find(match)) != std::wstring::npos) {
      result.replace(pos, match.length(), replaced);
    }
    return result;
  }
};

struct StringLTrim {
  std::wstring operator()(const std::wstring& str) {
    size_t pos = 0;
    while (L' ' == str.at(pos)) {
      ++pos;
    }
    if (pos != 0) {
      return str.substr(pos);
    } else {
      return str;
    }
  }
};

struct StringRTrim {
  std::wstring operator()(const std::wstring& str) {
    size_t pos = str.length() - 1;
    while (L' ' == str.at(pos)) {
      --pos;
    }
    if (pos != str.length()-1) {
      return str.substr(0, pos + 1); //second parameter is cout, not position
    } else {
      return str;
    }
  }
};
} //namespace detail

inline std::wstring Utf8ToUnicode(const char* src, int len) {
  return detail::MultiByteToUnicode()(src, len, CP_UTF8);
}

inline std::wstring Utf8ToUnicode(const std::string& src) {
  return Utf8ToUnicode(src.c_str(), static_cast<int>(src.length()));
}

inline std::string UnicodeToUtf8(const wchar_t* src, int len) {
  return detail::UnicodeToMultiByte()(src, len, CP_UTF8);
}

inline std::string UnicodeToUtf8(const std::wstring& src) {
  return UnicodeToUtf8(src.c_str(), static_cast<int>(src.length()));
}

inline std::wstring AnsiToUnicode(const char* src, int len) {
  return detail::MultiByteToUnicode()(src, len, CP_ACP);
}

inline std::wstring AnsiToUnicode(const std::string& src) {
  return AnsiToUnicode(src.c_str(), static_cast<int>(src.length()));
}

inline std::string UnicodeToAnsi(const wchar_t* src, int len) {
  return detail::UnicodeToMultiByte()(src, len, CP_ACP);
}

inline std::string UnicodeToAnsi(const std::wstring& src) {
  return UnicodeToAnsi(src.c_str(), static_cast<int>(src.length()));
}

inline std::string AnsiToUtf8(const char* src, int len) {
  return UnicodeToUtf8(AnsiToUnicode(src, len));
}

inline std::string AnsiToUtf8(const std::string& src) {
  return UnicodeToUtf8(AnsiToUnicode(src));
}

inline std::string Utf8ToAnsi(const char* src, int len) {
  return UnicodeToAnsi(Utf8ToUnicode(src, len));
}

inline std::string Utf8ToAnsi(const std::string& src) {
  return UnicodeToAnsi(Utf8ToUnicode(src));
}

inline bool SplitString(const std::wstring& src,
                        const std::wstring& separator,
                        std::vector<std::wstring>* vec) {
  return detail::SplitString()(src, separator, vec);
}

inline int CompareStringNoCase(const std::wstring& comp1,
                               const std::wstring& comp2) {
  return detail::CompareStringNoCase()(comp1, comp2);
}

inline std::wstring IntToString(__int64 num) {
  std::wstring result;
  if (num < 0) {
    result += L'-';
    num = -num;
  }
  result.append(detail::UInt64ToString()(num));
  return result;
}

inline std::wstring UIntToString(unsigned __int64 num) {
  return detail::UInt64ToString()(num);
}

inline std::string UrlEncode(const char* s, size_t len) {
  return detail::UrlEncode()(s, len);
}

inline std::string UrlEncode(const std::string& s) {
  return UrlEncode(s.c_str(), s.length());
}

inline std::string UrlEncode(const wchar_t* s, size_t len) {
  return UrlEncode(UnicodeToAnsi(s, len));
}

inline std::string UrlEncode(const std::wstring& s) {
  return UrlEncode(UnicodeToAnsi(s));
}

inline std::wstring GetRandomString(const size_t len, const std::wstring& random_table = L"") {
  return detail::GetRandomString()(len, random_table);
}

inline std::wstring StringReplace(const std::wstring& str, const std::wstring& match, const std::wstring& replaced) {
  return detail::StringReplace()(str, match, replaced);
}

inline std::wstring StringLTrim(const std::wstring& str) {
  return detail::StringLTrim()(str);
}

inline std::wstring StringRTrim(const std::wstring& str) {
  return detail::StringRTrim()(str);
}

inline std::wstring StringTrim(const std::wstring& str) {
  return detail::StringLTrim()(detail::StringRTrim()(str));
}

} //namespace ult

#endif // ULT_STRING_H_