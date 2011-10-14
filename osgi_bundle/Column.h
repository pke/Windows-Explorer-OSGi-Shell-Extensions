#pragma once

/**
    Represents a column for Explorer. 
    
    <p>
    It contains the SHCOLUMNINFO that can be used to configure a column in the
    explorer when it calls IColumnProvider::GetColumnInfo.
*/
class Column {
public:
  Column(REFGUID formatId, 
    const DWORD propertyId, 
    const VARTYPE vt = VT_LPSTR, 
    const DWORD flags = SHCOLSTATE_TYPE_STR,
    const int width = 32) {
      info.scid.fmtid = formatId;
      info.scid.pid = propertyId;
      info.vt = vt;
      info.fmt = LVCFMT_LEFT;
      info.csFlags = flags | SHCOLSTATE_SLOW;
      info.cChars = width;
      *info.wszDescription = 0;
      *info.wszTitle = 0;
  }

  Column& setDescription(LPCWSTR description) {
    _ASSERT(!IsBadStringPtrW(description, MAX_COLUMN_DESC_LEN));
    lstrcpynW(info.wszDescription, description, MAX_COLUMN_DESC_LEN);
    return *this;
  }

  Column& setTitle(LPCWSTR title) {
    _ASSERT(!IsBadStringPtrW(title, MAX_COLUMN_NAME_LEN));
    lstrcpynW(info.wszTitle, title, MAX_COLUMN_NAME_LEN);
    return *this;
  }

  boolean equals(const SHCOLUMNID& other) const {
    return (info.scid.fmtid == other.fmtid && info.scid.pid == other.pid);
  }

  virtual void visit(const org::osgi::framework::Bundle& bundle, ATL::CComVariant& result) const {
  }

  const SHCOLUMNINFO& getInfo() const {
    return info;
  }
  
private:
  SHCOLUMNINFO info;
};