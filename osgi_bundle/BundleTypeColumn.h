#pragma once

//#include <ntquery.h>
#include "Column.h"

class BundleTypeColumn : public Column {
public:
  BundleTypeColumn() : Column(getFmtId(), 4 /*PID_STG_STORAGETYPE*/, VT_BSTR) {
    setTitle(L"Bundle Type");
  }

  void visit(const org::osgi::framework::Bundle& bundle, ATL::CComVariant& result) const {
    if (bundle.isFragment()) {
      result = ATL::CComBSTR("Fragment bundle");
    } else {
      result = ATL::CComBSTR("Bundle");
    }
  }
private:
  static REFGUID getFmtId() {
    static const CLSID guid = { 0xb725f130, 0x47ef, 0x101a, { 0xa5, 0xf1, 0x02, 0x60, 0x8c, 0x9e, 0xeb, 0xac } }; //PSGUID_STORAGE;
    return guid;
  }
};
