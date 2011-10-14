#pragma once

#include "macros.h"

namespace java { namespace util { 

  class InputStream {
  public:
    InputStream(const char* content) : token(0), content(content) {
    }

    InputStream(IStream* stream) : token(0) {
      char buffer[8192];
      ULONG read;
      while (SUCCEEDED(stream->Read(buffer, sizeof(buffer), &read)) && read > 0) {
        content += ATL::CStringA(buffer, read);
      }
    }

    int readLine(char* buffer, int len) {
      if (token == -1) {
        return -1;
      }

      int prev = token;
      lstrcpynA(buffer, content.Tokenize("\r\n", token), len);
      return token != -1 ? (token - prev)-1 : -1;
    }

  private:
    ATL::CStringA content;    
    int token;
  };

  class Properties {
  public:
    Properties(const char delimiter = '=') : delimiter(delimiter) {
    }

   void read(InputStream is) {
      char buffer[80]; // see JAR File Specification
      ATL::CStringA currentName;
      ATL::CStringW currentValue;

      int len;
      while ((len = is.readLine(buffer, sizeof(buffer))) != -1) {
        ATL::CStringA line(buffer, len);
        if (skipLine(line)) {
          continue;
        }
        if (line.GetLength() == 0 || line[0] == '\r' || line[0] == '\n') {
          currentName = "";
          currentValue = "";
          continue;
        }
        if (line[0] == ' ') {
          currentValue += line.Trim();
        } else {
          if (currentName.GetLength() > 0) {
            put(currentName.Trim(), currentValue.Trim());
          }

          int delimiterIndex = line.Find(delimiter, 0);
          if (delimiterIndex == -1) {
            continue;
          }

          currentName = line.Left(delimiterIndex);
          currentValue = line.Right(len - delimiterIndex - 1);          
        }
      }
      if (currentName.GetLength() > 0) {
        put(currentName.Trim(), currentValue.Trim());
      }
    }

    void put(const ATL::CStringA& key, const ATL::CStringW& value) {
      entries.Add(key, value);
    }

    ATL::CStringW getValue(const ATL::CStringA& name) const{
      return entries.Lookup(name);
    }

    bool hasValue(const ATL::CStringA& name) const {
      return entries.FindKey(name) != -1;
    }
  protected:
    bool skipLine(const ATL::CStringA& line) const {
      return line[0] == '#';
    }

  private:
    const char delimiter;
    ATL::CSimpleMap<ATL::CStringA, ATL::CStringW> entries;

    DISALLOW_COPY_AND_ASSIGN(Properties);
  };

}} 