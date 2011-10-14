#pragma once

#include "Column.h"

class BundleColumn : public Column {
public:
  BundleColumn(LPCSTR bundleHeader, 
    REFGUID formatId, 
    const DWORD propertyId, 
    const VARTYPE vt = VT_LPWSTR, 
    const DWORD flags = SHCOLSTATE_TYPE_STR,
    const int width = 32) : 
    Column(formatId, propertyId, vt, flags, width),
    bundleHeader(bundleHeader) {
      setTitle(ATL::CStringW(bundleHeader));
  }

  void visit(const org::osgi::framework::Bundle& bundle, ATL::CComVariant& result) const {
    if (bundle.hasHeader(bundleHeader)) {
      result = bundle.getHeader(bundleHeader);
    }
  }

private:
  ATL::CStringA bundleHeader;
};
