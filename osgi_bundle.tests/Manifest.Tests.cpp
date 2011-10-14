#include "stdafx.h"
#include "../osgi_bundle/Bundle.h"

class ManifestTest : public ::testing::Test {
public:
  void SetUp() {
    bundle = new org::osgi::framework::Bundle(_T("bundles/org.eclipse.equinox.cm_1.0.100.v20090520-1800.jar"));
    ASSERT_TRUE(bundle->isValid());
  }

  void TearDown() {
    delete bundle;
  }

protected:
  org::osgi::framework::Bundle* bundle;
};

TEST_F(ManifestTest, readIn) {
  ASSERT_STREQ(L"1.0", bundle->getHeader("Manifest-Version"));
  ASSERT_STREQ(L"1.0.100.v20090520-1800", bundle->getHeader("Bundle-Version"));
  ASSERT_STREQ(L"org.eclipse.equinox.internal.cm.Activator", bundle->getHeader("Bundle-Activator"));
}

TEST_F(ManifestTest, l10n) {
  ASSERT_STREQ(L"Eclipse.org - Equinox", bundle->getHeader("Bundle-Vendor"));
  ASSERT_STREQ(L"Configuration Admin", bundle->getHeader("Bundle-Name"));
}