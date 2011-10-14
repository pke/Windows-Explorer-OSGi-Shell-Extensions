#pragma once

#include "macros.h"
#include "Manifest.h"

/**
  Allows to query for a file.

  TODO: add enum/find/iterate method with visitor
  @author_philk
*/
struct IFileLocator {
  /**
    @param path [in] to the file to load as a stream.
  */
  virtual IStream* createStream(LPCTSTR path) = 0;
};

/**
    Implementation that will locate files inside a JAR/Zip archiv.
    @author_philk
*/
class JarFileLocator : public IFileLocator {
public:
  JarFileLocator(LPCTSTR path) : jar(path) {
  }

  IStream* createStream(LPCTSTR path) {
    return jar.get(path);
  }
private:
  java::util::jar::JarFile jar;
};

/**
    A file locator that operates on the filesystem inside a folder.
    @author_philk
*/
class FolderFileLocator : public IFileLocator {
public:
  FolderFileLocator(LPCTSTR folder) : folder(folder) {
    this->folder += L'/';
  }

  IStream* createStream(LPCTSTR path) {
    IStream* stream;
    if SUCCEEDED(::SHCreateStreamOnFile(folder + ATL::CStringW(path), STGM_READ | STGM_SHARE_DENY_NONE, &stream)) {
      return stream;
    }
    return 0;
  }
private:
  ATL::CString folder;
};

namespace org { namespace osgi { namespace framework {


/**
    Minimal implementation of an OSGi bundle.

    <p>
    This basically only offers to query the bundles headers.
    
    @author_philk
*/
class Bundle {
public:
  Bundle(LPCWSTR path) : valid(false), locator(0) {
    const DWORD attribs = ::GetFileAttributes(path);
    if (attribs == -1) {
      return;
    }
    if (attribs & FILE_ATTRIBUTE_DIRECTORY) {
      locator = new FolderFileLocator(path);
    } else {
      locator = new JarFileLocator(path);
    }
    ATL::CComPtr<IStream> stream(locator->createStream(L"/META-INF/MANIFEST.MF"));
    if (stream) {
      manifest.read(java::util::InputStream(stream));
      valid = !manifest.getValue("Bundle-SymbolicName").IsEmpty();
    }
    loadBundleLocalization();
  }

  const bool isValid() const { return valid; }

  const bool hasHeader(const ATL::CStringA& header) const {
    return manifest.hasValue(header);
  }

  const bool isFragment() const {
    return hasHeader("Fragment-Host");
  }

  /**
    Checks whether the bundle contains service components.

    <p>
    It actually only checks the "Service-Component" bundle header and not the
    actual defined components.
    @return if the bundle contains service components.
  */
  const bool hasServiceComponents() const {
    return hasHeader("Service-Component");
  }

  ATL::CStringW getHeader(const ATL::CStringA& header) const {
    ATL::CStringW value = manifest.getValue(header);
    if (value[0] == L'%') {
      ATL::CStringA l10nKey(value.GetString()+1);
      if (l10n.hasValue(l10nKey)) {
        return l10n.getValue(l10nKey);
      }
      return ATL::CStringW(l10nKey);
    }
    return value;
  }

  ~Bundle() {
    delete locator;
  }

  void loadBundleLocalization() {
    ATL::CStringW location = manifest.getValue("Bundle-Localization");
    if (location.IsEmpty()) {
      location = L"/OSGI-INF/l10n/bundle";
    }
    // The order to read them would be:
    // _locale.properties
    // _en.properties
    // .properties
    // Each time l10n.read with the stream
    // But it would have also search in fragments...
    ATL::CComPtr<IStream> stream(locator->createStream(location + L".properties"));
    if (stream) {
      l10n.read(java::util::InputStream(stream));
    }
  }

private:
  DISALLOW_COPY_AND_ASSIGN(Bundle);

  bool valid;
  // Will hold the bundles localized headers
  java::util::Properties l10n;
  // Holds the bundles manifest
  java::util::jar::Manifest manifest;
  // Describes how we find our files
  IFileLocator* locator;
};

}}} // org::osgi::framework