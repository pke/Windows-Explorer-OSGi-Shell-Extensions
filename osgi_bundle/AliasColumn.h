#pragma once

/**
    Serves as an alias for another column, just with other FMTID and PID.

    @author_philk
*/
class AliasColumn : public Column {
public:
  AliasColumn(const Column& other, 
    FMTID formatId, 
    DWORD propertyId) : 
    Column(formatId, 
      propertyId, 
      other.getInfo().vt, 
      other.getInfo().csFlags, 
      other.getInfo().cChars), 
      columnRef(other) { 
  }

  /**
      Calls the referenced columns visit method.
  */
  void visit(const org::osgi::framework::Bundle& bundle, ATL::CComVariant& result) {
    columnRef.visit(bundle, result);
  }
private:
  const Column& columnRef;
};