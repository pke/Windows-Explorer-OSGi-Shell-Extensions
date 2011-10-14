#include "stdafx.h"

#include "../osgi_bundle/Properties.h"

TEST(PropertiesTest, checkParsing) {
  java::util::Properties properties;
  properties.read(java::util::InputStream("Key1=Value"));
  ASSERT_STREQ(L"Value", properties.getValue("Key1"));
  ATLTRACE(L"Test1");
  ATLTRACE(L"Test2");
#if 0
  ATL::CComPtr<IShellFolder> pDesktopFolder;
  SHGetDesktopFolder(&pDesktopFolder);
  if (!pDesktopFolder) {
    return;
  }
  LPITEMIDLIST  pidl;
  ATL::CComBSTR bstrPath = L"c:\\";
  if FAILED(pDesktopFolder->ParseDisplayName(NULL, NULL, bstrPath, NULL, &pidl, NULL)) {
    return;
  }
  ATL::CComPtr<IShellFolder> pFolder;
  if FAILED(pDesktopFolder->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID*) &pFolder)) {
    return;
  }
  ATL::CComQIPtr<IShellFolder2> pFolder2(pFolder);
  if (!pFolder2) {
    return;
  }
  HRESULT hrLocal;
  UINT ndx = 0;
  SHELLDETAILS sd;
  while ((hrLocal = pFolder2->GetDetailsOf(NULL, ndx, &sd)) == S_OK) {
    LPTSTR szName = NULL;
    if FAILED(StrRetToStr(&sd.str, NULL, &szName)) {
      break;
    }
    ATL::CString csName(szName);
    CoTaskMemFree(szName);
    SHCOLUMNID scid;
    if (pFolder2->MapColumnToSCID(ndx, &scid) == S_OK) {
      LPOLESTR pStr;
      StringFromCLSID(scid.fmtid, &pStr);
      ATL::CString csMsg;
      csMsg.Format(L"Col: %d, Name: %s, FMTID: %s, PID: %d\n", ndx, csName, CString(pStr), scid.pid);
      CoTaskMemFree(pStr);
      ATLTRACE(csMsg);
    }
    ndx++;
  }
#endif 
}