#include "stdafx.h"
#include "Bundle.h"

#include "BundleColumn.h"
#include "BundleTypeColumn.h"
#include "AliasColumn.h"

#ifndef PSFMTID_VERSION
DEFINE_GUID(PSFMTID_VERSION, 0x0CEF7D53, 0xFA64, 0x11D1, 0xA2, 0x03, 0x00, 0x00, 0xF8, 0x1F, 0xED, 0xEE);
#define PIDVSI_FileDescription 3
#define PIDVSI_FileVersion 4
#define PIDVSI_ProductName 7
#endif 

class ATL_NO_VTABLE 
  DECLSPEC_UUID("d480098c-c50d-430e-bdec-aaab424078e5")
  OSGiExtensions : 
  public ATL::CComObjectRootEx<ATL::CComSingleThreadModel>,
  public ATL::CComCoClass<OSGiExtensions, &__uuidof(OSGiExtensions)>,
  //public IStreamInit,
  //public IQueryInfo,
  //public IShellIconOverlayIdentifier 
  public IExtractIcon, 
  public IColumnProvider,
#ifdef IPropertyStore
  public IPropertyStore,
  public IInitWithStream,
  public IInitWithFile,
#endif
  public IPersistFile {
public:
  BEGIN_COM_MAP(OSGiExtensions)
    COM_INTERFACE_ENTRY(IPersistFile)
    //COM_INTERFACE_ENTRY_IID(IID_IQueryInfo, IQueryInfo)
    COM_INTERFACE_ENTRY_IID(IID_IColumnProvider, IColumnProvider)
    COM_INTERFACE_ENTRY_IID(IID_IExtractIcon, IExtractIcon)
    //COM_INTERFACE_ENTRY_IID(IID_IShellIconOverlayIdentifier, IShellIconOverlayIdentifier)
  END_COM_MAP()

  STDMETHODIMP GetClassID(LPCLSID)      { ATLTRACENOTIMPL2(); }
  STDMETHODIMP IsDirty()                { ATLTRACENOTIMPL2(); }
  
  /**
      Called for before IQueryInfo methods.
  */
  STDMETHODIMP Load(LPCOLESTR filename, DWORD) {
    this->filename = filename;
    return S_OK;
  }
  
  STDMETHODIMP Save(LPCOLESTR, BOOL)    { ATLTRACENOTIMPL2(); }
  STDMETHODIMP SaveCompleted(LPCOLESTR) { ATLTRACENOTIMPL2(); }
  STDMETHODIMP GetCurFile(LPOLESTR*)    { ATLTRACENOTIMPL2(); }

  //
  // IQueryInfo 
  // 
  STDMETHODIMP GetInfoFlags(DWORD*)     { ATLTRACENOTIMPL2(); }
  
  STDMETHODIMP GetInfoTip(DWORD flags, LPWSTR* text) {
    ATLASSERT(!::IsBadWritePtr(text, sizeof(text)));
    *text = 0;
    return S_OK;
  }

  // IColumnProvider 
  STDMETHODIMP Initialize(LPCSHCOLUMNINIT psci) { return S_OK; }

  Column* getColumn(int index) {
    static BundleTypeColumn type;
    static BundleColumn version("Bundle-Version", PSFMTID_VERSION, PIDVSI_FileVersion);
    static BundleColumn author("Bundle-Author", FMTID_SummaryInformation, PIDSI_AUTHOR);
    static BundleColumn name("Bundle-Name", PSFMTID_VERSION, PIDVSI_ProductName);
    static AliasColumn name2(name, FMTID_SummaryInformation, PIDSI_TITLE);
    static BundleColumn vendor("Bundle-Vendor", FMTID_DocSummaryInformation, PIDDSI_COMPANY);
    static BundleColumn category("Bundle-Category", FMTID_DocSummaryInformation, PIDDSI_CATEGORY);
    static BundleColumn contact("Bundle-ContactAddress", FMTID_DocSummaryInformation, PIDDSI_MANAGER);
    static BundleColumn copyright("Bundle-Copyright", FMTID_MediaFileSummaryInformation, PIDMSI_COPYRIGHT);
    static BundleColumn desc("Bundle-Description", PSFMTID_VERSION, PIDVSI_FileDescription);
    // Add new columns here...
    static Column* columns[] = {
      &type, &version, &author, &name, &name2, &vendor, &category, &contact, &copyright, &desc,
      // ... and here.
    };
    static const int count = sizeof(columns) / sizeof(columns[0]);
    if (index >= count) {
      return 0;
    }
    return columns[index];
  }

  STDMETHODIMP GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO* psci) {
    ATLTRACEMETHOD();

    Column* column = getColumn(dwIndex);
    if (column) {
      CopyMemory(psci, &column->getInfo(), sizeof(*psci));;
      return S_OK;  
    }

    return S_FALSE;    
  }

  STDMETHODIMP GetItemData(LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, VARIANT* pvarData) {
    ATLTRACEMETHOD();
    
    if (pscd->dwFileAttributes & (/*FILE_ATTRIBUTE_DIRECTORY|*/FILE_ATTRIBUTE_OFFLINE)) {
      return S_FALSE;
    }
    bool isManifest = lstrcmpW(pscd->pwszExt, L".MF") == 0;
    if (lstrcmpiW(pscd->pwszExt, L".jar") != 0 && !isManifest && !(pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      return S_FALSE;
    }

    const org::osgi::framework::Bundle* bundle = cacheBundle(pscd->wszFile);
    if (!bundle->isValid()) {
      return S_FALSE;
    }

    ATL::CComVariant variant;
    Column* column = getColumn(0);
    int i=0;
    while (column) {
      if (column->equals(*pscid)) {
        column->visit(*bundle, variant);
        return variant.Detach(pvarData);
      }
      column = getColumn(++i);
    }  
    return S_FALSE;
  }

  STDMETHODIMP GetOverlayInfo(LPWSTR pwszIconFile, int cchMax, int *pIndex, DWORD *pdwFlags) {
    GetModuleFileName(ATL::_pModule->GetResourceInstance(), pwszIconFile, cchMax);
    *pIndex = 0;
    *pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;
    return S_OK;
  }

  STDMETHODIMP GetPriority(int *pPriority) {
    *pPriority = 0;
    return S_OK;
  }

  STDMETHODIMP IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib) {
    if (wcsstr(pwszPath, L".jar") != 0 || 
      ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) && (GetFileAttributes(ATL::CStringW(pwszPath) + L"/META-INF/MANIFEST.MF") != -1))) {
      return S_OK;
    }
    return S_FALSE;
  }

  STDMETHODIMP GetIconLocation(UINT uFlags, LPTSTR szIconFile, UINT cchMax,
    int* piIndex, UINT* pwFlags) {
    GetModuleFileName(ATL::_pModule->GetResourceInstance(), szIconFile, cchMax);
    const org::osgi::framework::Bundle* bundle = cacheBundle(filename);
    if (bundle->isValid()) {
      if (bundle->isFragment()) {
        *piIndex = 2;
      } else if (bundle->hasServiceComponents()) {
        *piIndex = 0;
      } else {
        *piIndex = 1;
      }
      *pwFlags = 0;
    } else {
      return S_FALSE;
    }
    return S_OK;
  }

  STDMETHODIMP Extract(LPCWSTR,UINT,HICON *,HICON *,UINT) {
    return S_FALSE;
  }

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  DECLARE_REGISTRY_RESOURCEID(IDR_OSGI_BUNDLE);

private:
  const org::osgi::framework::Bundle* cacheBundle(LPCWSTR path) {
    const int index = bundleCache.FindKey(path);
    if (index != -1) {
      return bundleCache.GetValueAt(index);
    }
    org::osgi::framework::Bundle* bundle = new org::osgi::framework::Bundle(path);
    if (!bundle->isValid()) {
      delete bundle;
      bundle = &invalidBundleCacheEntry;
    }
    bundleCache.Add(path, bundle);
    return bundle;
  }

  class InvalidBundleCacheEntry : public org::osgi::framework::Bundle {
  public:
    InvalidBundleCacheEntry() : org::osgi::framework::Bundle(L"") {
    }
  }invalidBundleCacheEntry;

  ATL::CSimpleMap<ATL::CStringW, org::osgi::framework::Bundle*> bundleCache;
  ATL::CStringW filename;
};
OBJECT_ENTRY_AUTO(__uuidof(OSGiExtensions), OSGiExtensions)