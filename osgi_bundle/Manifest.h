#pragma once
#include "zip/unzip.h"
#include "zip/iowin32.h"

#include "Properties.h"

namespace java { namespace util { namespace jar {

  /**
    A JAR file MANIFEST.MF.

    <p>
    This is not an exact representation of the Java class.
    Its basically just a Properties subclass with a different delimiter.

    @author_philk
  */
  class Manifest : public Properties {
  public:
    Manifest() : Properties(':') {
    }

  private:
    DISALLOW_COPY_AND_ASSIGN(Manifest);
  };

  /**
      Represents a JAR file.

      @author_philk
  */
  class JarFile {
  public:
    JarFile(LPCTSTR path) {
      zlib_filefunc_def ffunc;
      fill_win32_filefunc(&ffunc);      
      file = unzOpen2((LPCSTR)path, &ffunc);
    }

    IStream* get(LPCTSTR path) {
      if (file) {
        USES_CONVERSION;
        if (UNZ_OK != unzLocateFile(file, T2A(path[0] == '/' ? path+1 : path), 1)) {
          return 0;
        }
        unz_file_info fileInfo;
        unzGetCurrentFileInfo(file, &fileInfo, 0, 0, 0, 0, 0, 0);
        ATL::CStringA manifest;
        unzOpenCurrentFile(file);
        LPVOID buffer = GlobalAlloc(GPTR, fileInfo.uncompressed_size);
        if (buffer) {
          unzReadCurrentFile(file, buffer, fileInfo.uncompressed_size);
          unzCloseCurrentFile(file);
          IStream* stream;
          if SUCCEEDED(CreateStreamOnHGlobal(buffer, true, &stream)) {
            return stream;
          }
        } else {
          unzCloseCurrentFile(file);
        }
      }
      return 0;
    }

    ~JarFile() {
      if (file) {
        unzCloseCurrentFile(file);
        unzClose(file);
      }
    }

  private:
    unzFile file;
    DISALLOW_COPY_AND_ASSIGN(JarFile);
  };

}}} // java::util::jar
