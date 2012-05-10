#include "StdAfx.h"
#include ".\ult-utility.h"
#include <string>
#include <Windows.h>
#include <io.h>

namespace ult{
std::wstring RegQueryString( HKEY parent_key, const std::wstring& key_name, const std::wstring& value_name )
{
  std::wstring result;
  result.clear();
  CRegKey regKey;
  int ret = regKey.Open(parent_key, key_name.c_str(), KEY_READ);
  if (ERROR_SUCCESS != ret) {
    DebugPrint(GetLastError());
    return result;
  }
  wchar_t buf_value[MAX_PATH];
  unsigned long len = MAX_PATH;
  ret = regKey.QueryStringValue(value_name.c_str(), buf_value, &len);
  if (ERROR_SUCCESS != ret) {
    DebugPrint(GetLastError());
    regKey.Close();
    return result;
  }
  regKey.Close();
  result.assign(buf_value);
  return result;
}

void DebugPrint( const std::wstring& str )
{
#ifdef _DEBUG
#ifndef DEBUG_PRE
#define DEBUG_PRE L"ult -- "
#endif
  OutputDebugString((DEBUG_PRE + str).c_str());
#endif
}

void DebugPrint( const std::string& str )
{
#ifdef _DEBUG
#ifndef DEBUG_PRE_A
#define DEBUG_PRE_A "ult -- "
#endif
  OutputDebugStringA((DEBUG_PRE_A + str).c_str());
#endif
}

void DebugPrint( int n )
{
#ifdef _DEBUG
  wchar_t num[100];
  swprintf(num, L"%d", n);
  DebugPrint(num);
#endif
}

bool ReadFileToString( const std::wstring& file, std::string* dest )
{
  HANDLE hfile = CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_READ,
      NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (INVALID_HANDLE_VALUE == hfile) {
    return false;
  }
  const int kBufLen = 1024;
  const int kMaxReadTime = 10;
  DWORD file_size = GetFileSize(hfile, NULL);
  bool ret = false;
  do {
    if (file_size > kBufLen * kMaxReadTime) {
      break;
    }
    char* buf = new char[kBufLen];
    DWORD dwread;
    for (int i = 0; i < kMaxReadTime; ++i) {
      BOOL b = ReadFile(hfile, buf, kBufLen, &dwread, NULL);
      if (0 == b || 0 == dwread) {
        break;
      }
      dest->append(buf, dwread);
    }
    delete[] buf;
    ret = true;
  } while (false);
  CloseHandle(hfile);
  return ret;
}

DWORD GetShellVersion( void )
{
  HMODULE hmodule_dll;
  DWORD dwversion = 0;
  hmodule_dll = LoadLibrary(L"Shell32.dll");
  if (NULL != hmodule_dll) {
    DLLGETVERSIONPROC pfn_dll_get_version;
    pfn_dll_get_version = (DLLGETVERSIONPROC)GetProcAddress(hmodule_dll, "DllGetVersion");
    if (NULL != pfn_dll_get_version) {
      DLLVERSIONINFO dvi;
      HRESULT hr;

      memset(&dvi, 0, sizeof(dvi));
      dvi.cbSize = sizeof(dvi);
      hr = pfn_dll_get_version(&dvi);
      if (SUCCEEDED(hr)) {
        dwversion = MAKELONG(dvi.dwMinorVersion, dvi.dwMajorVersion);
      }
    }
    FreeLibrary(hmodule_dll);
  }
  return dwversion;
}

bool RegQueryWord( HKEY parent_key, const wchar_t* key_name,
                  const wchar_t* value_name, DWORD* value )
{
  CRegKey regKey;
  int ret = regKey.Open(parent_key, key_name, KEY_READ);
  if (ERROR_SUCCESS != ret) {
    return false;
  }
  ret = regKey.QueryDWORDValue(value_name, *value);
  if (ERROR_SUCCESS != ret) {
    regKey.Close();
    return false;
  }
  regKey.Close();
  return true;
}

std::wstring NumberToString( int num )
{
  wchar_t buf[100];
  swprintf(buf, L"%d", num);
  return buf;
}

}