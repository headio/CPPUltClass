/*
** ͬ��������
** winhttp
** author
**   taoabc@gmail.com
*/
#ifndef ULT_NET_SYNCWINHTTPDOWNLOADER_H_
#define ULT_NET_SYNCWINHTTPDOWNLOADER_H_

#include "./sync-winhttp-request.h"

namespace ult {

class SyncWinHttpDownloader : public SyncWinHttpRequest {

public:

  SyncWinHttpDownloader(void) :
      content_length_(0) {
  }

  ~SyncWinHttpDownloader(void) {
  }

  int DownloadString(const wchar_t* url) {
    WinHttpSession session;
    HRESULT hr = session.Initialize();
    if (FAILED(hr)) {
      return hr;
    }
    WinHttpConnection connection;
    URL_COMPONENTS uc;
    UltWinHttpCrackUrl(url, &uc);
    std::wstring host_name(uc.lpszHostName, uc.dwHostNameLength);
    hr = connection.Initialize(session.GetHandle(), host_name.c_str(), uc.nPort);
    if (FAILED(hr)) {
      return hr;
    }
    hr = Initialize(connection.GetHandle(), L"GET", uc.lpszUrlPath, uc.nScheme);
    if (FAILED(hr)) {
      return hr;
    }
    SendRequest(NULL, 0, NULL, 0, 0);
    hr = RecieveResponse();
    if (FAILED(hr)) {
      return hr;
    }
    return S_OK;
  }

private:

  HRESULT OnContentLength(DWORD length) {
    content_length_ = length;
    return S_OK;
  }

  HRESULT OnReadComplete(const void* info, DWORD length) {
    return S_OK;
  }

  DWORD content_length_;

}; //class SyncWinHttpDownloader
} //namespace ult

#endif