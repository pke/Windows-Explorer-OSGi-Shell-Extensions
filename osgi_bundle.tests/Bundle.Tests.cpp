#include "stdafx.h"
#include "../osgi_bundle/Bundle.h"

TEST(BundleTest, load_invalid_bundle) {
  ASSERT_FALSE((new org::osgi::framework::Bundle(L"bundles"))->isValid());
  ASSERT_FALSE((new org::osgi::framework::Bundle(L"bundles/examples.jar"))->isValid());
}

TEST(BundleTest, load_valid_bundle) {
  ASSERT_TRUE(org::osgi::framework::Bundle(L"bundles/org.eclipse.equinox.cm_1.0.100.v20090520-1800.jar").isValid());
  ASSERT_TRUE(org::osgi::framework::Bundle(L"bundles/org.eclipse.ant.core.nl_de_3.5.0.v20090524043402.jar").isValid());
}