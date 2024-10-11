/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include <openstudio/resources.hxx>

#include "ModelEditorFixture.hpp"
#include "../Utilities.hpp"

#include <clocale>
#include <QUrl>

using openstudio::toPath;
using openstudio::toQString;
using openstudio::toString;

TEST_F(ModelEditorFixture, SimpleConversions) {
  std::string s("Hello world");
  const char* const cStr = "Hello world";
  const wchar_t* const wStr = L"Hello world";
  std::wstring w(L"Hello world");
  QString q("Hello world");
  openstudio::path p = toPath("Hello world");

  EXPECT_EQ(s, s);
  EXPECT_EQ(s, cStr);
  EXPECT_EQ(s, toString(w));
  EXPECT_EQ(s, toString(wStr));
  EXPECT_EQ(s, toString(q));
  EXPECT_EQ(s, toString(p));
  EXPECT_EQ(s, toString(toQString(s)));
  EXPECT_EQ(s, toString(toPath(s)));
  EXPECT_EQ(s, toString(toQString(toPath(s))));
  EXPECT_TRUE(q == toQString(s));
  EXPECT_TRUE(q == toQString(p));
  EXPECT_TRUE(p == toPath(s));
  EXPECT_TRUE(p == toPath(q));
}

TEST_F(ModelEditorFixture, Path_Conversions) {
  std::string s;
  std::wstring w;
  openstudio::path p;

  s = std::string("basic_path");
  w = std::wstring(L"basic_path");
  p = openstudio::path(s);
  EXPECT_EQ(s, toQString(p).toStdString());
  EXPECT_EQ(s, toQString(toString(p)).toStdString());
  EXPECT_EQ(s, std::string(toQString(toString(p)).toUtf8()));
  EXPECT_EQ(s, toPath(toQString(toString(p))).string());
  EXPECT_EQ(w, toQString(p).toStdWString());
  //EXPECT_EQ(w, toString(p));
  EXPECT_EQ(w, toQString(toString(p)).toStdWString());
  //EXPECT_EQ(w, std::wstring(toQString(toString(p)).toUtf16()));
  EXPECT_EQ(w, toPath(toQString(toString(p))).wstring());

  p = openstudio::path(w);
  EXPECT_EQ(s, toQString(p).toStdString());
  EXPECT_EQ(s, toQString(toString(p)).toStdString());
  EXPECT_EQ(s, std::string(toQString(toString(p)).toUtf8()));
  EXPECT_EQ(s, toPath(toQString(toString(p))).string());

  p = openstudio::path(w);
  EXPECT_EQ(s, toQString(p).toStdString());
  EXPECT_EQ(s, toQString(toString(p)).toStdString());
  EXPECT_EQ(s, std::string(toQString(toString(p)).toUtf8()));
  EXPECT_EQ(s, toPath(toQString(toString(p))).string());

  // http://utf8everywhere.org/

  // from http://www.nubaria.com/en/blog/?p=289

  // Chinese characters for "zhongwen" ("Chinese language").
  const char kChineseSampleText[] = {-28, -72, -83, -26, -106, -121, 0};

  // Arabic "al-arabiyya" ("Arabic").
  const char kArabicSampleText[] = {-40, -89, -39, -124, -40, -71, -40, -79, -40, -88, -39, -118, -40, -87, 0};

  // Spanish word "canon" with an "n" with "~" on top and an "o" with an acute accent.
  const char kSpanishSampleText[] = {99, 97, -61, -79, -61, -77, 110, 0};

  std::string t;
  QString q;

  t = std::string(kChineseSampleText);
  q = QString::fromUtf8(t.c_str());
  p = toPath(t);
  EXPECT_EQ(q, toQString(p));
  EXPECT_EQ(t, std::string(toQString(p).toUtf8()));
  EXPECT_EQ(t, toString(p));
  EXPECT_EQ(t, toString(toQString(toString(p))));
  EXPECT_EQ(t, std::string(toQString(toString(p)).toUtf8()));
  EXPECT_EQ(t, toString(toPath(toQString(toString(p)))));

  t = std::string(kArabicSampleText);
  q = QString::fromUtf8(t.c_str());
  p = toPath(t);
  EXPECT_EQ(q, toQString(p));
  EXPECT_EQ(t, std::string(toQString(p).toUtf8()));
  EXPECT_EQ(t, toString(toQString(toString(p))));
  EXPECT_EQ(t, std::string(toQString(toString(p)).toUtf8()));
  EXPECT_EQ(t, toString(toPath(toQString(toString(p)))));

  t = std::string(kSpanishSampleText);
  q = QString::fromUtf8(t.c_str());
  p = toPath(t);
  EXPECT_EQ(q, toQString(p));
  EXPECT_EQ(t, std::string(toQString(p).toUtf8()));
  EXPECT_EQ(t, toString(toQString(toString(p))));
  EXPECT_EQ(t, std::string(toQString(toString(p)).toUtf8()));
  EXPECT_EQ(t, toString(toPath(toQString(toString(p)))));
}

TEST_F(ModelEditorFixture, MorePath_Conversions) {
  struct PathTestCase
  {
    std::string inputPath;
    QString expectedPath;
    QString expectedUrl;
  };

  std::vector<PathTestCase> testCases = { 
    {"C:\\Users\\Test\\eplustbl.html", "C:/Users/Test/eplustbl.html", "file:///C:/Users/Test/eplustbl.html"},
    {"C:/Users/Test/eplustbl.html", "C:/Users/Test/eplustbl.html", "file:///C:/Users/Test/eplustbl.html"},
    {"C:\\Users/Test/eplustbl.html", "C:/Users/Test/eplustbl.html", "file:///C:/Users/Test/eplustbl.html"},
    {"C:\\Users\\Test# ^\\eplustbl.html", "C:/Users/Test# ^/eplustbl.html", "file:///C:/Users/Test%23%20%5E/eplustbl.html"},
    {"/home/Test/eplustbl.html", "/home/Test/eplustbl.html", "file:///home/Test/eplustbl.html"},
    {"/home/Test# ^/eplustbl.html", "/home/Test# ^/eplustbl.html", "file:///home/Test%23%20%5E/eplustbl.html"},
  };

  // double check the conversions in openstudio_lib/ResultsTabView.cpp
  for (const auto& testCase : testCases) {
    openstudio::path osPath = toPath(testCase.inputPath);
    QString qPath = toQString(osPath);
    EXPECT_EQ(qPath, testCase.expectedPath);
    QUrl url = QUrl::fromLocalFile(qPath);
    EXPECT_EQ(url.toString(QUrl::FullyEncoded), testCase.expectedUrl);

    std::cout << "Input: " << testCase.inputPath << ", "
      << "OS Path: " << osPath << ", "
      << "QPath: " << qPath.toStdString() << ", "
      << "Url: " << url.toString().toStdString() << std::endl;
  }
}