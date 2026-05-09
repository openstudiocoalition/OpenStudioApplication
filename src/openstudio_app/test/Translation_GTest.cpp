/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <gtest/gtest.h>

#include "OpenStudioAppFixture.hpp"
#include "../../utilities/OpenStudioApplicationPathHelpers.hpp"
#include "../../model_editor/Utilities.hpp"

#include <QCoreApplication>
#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QTranslator>

using namespace openstudio;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static openstudio::path translationsSourceDir() {
  return getOpenStudioApplicationSourceDirectory() / toPath("translations");
}

// Locate the compiled .qm file.  It is generated into the build tree under
// Products/Release/translations/ on Windows and into the equivalent on other
// platforms.  We try a few candidate paths so the test works from different
// build configurations.
static openstudio::path findQmFile(const std::string& language) {
  const std::string filename = "OpenStudioApp_" + language + ".qm";

  // 1. Next to the test executable (CTest sets the working directory here)
  openstudio::path candidates[] = {
    toPath("translations") / toPath(filename),
    toPath("../translations") / toPath(filename),
    toPath("../../Products/Release/translations") / toPath(filename),
    toPath("../../Products/Debug/translations") / toPath(filename),
    translationsSourceDir() / toPath(filename),  // committed .qm (if present)
  };

  for (const auto& p : candidates) {
    if (openstudio::filesystem::exists(p)) {
      return p;
    }
  }
  return {};  // empty = not found
}

// ---------------------------------------------------------------------------
// Test Suite: Translation_ts  (validates the .ts source file – no build dep)
// ---------------------------------------------------------------------------

class Translation_ts : public OpenStudioAppFixture
{
 protected:
  QDomDocument m_doc;

  void SetUp() override {
    openstudio::path tsPath = translationsSourceDir() / toPath("OpenStudioApp_es.ts");
    ASSERT_TRUE(openstudio::filesystem::exists(tsPath))
      << "Translation source file not found: " << tsPath;

    QFile file(toQString(tsPath));
    ASSERT_TRUE(file.open(QIODevice::ReadOnly)) << "Cannot open OpenStudioApp_es.ts";

    QString errorMsg;
    int errorLine = 0;
    ASSERT_TRUE(m_doc.setContent(&file, &errorMsg, &errorLine))
      << "XML parse error in OpenStudioApp_es.ts at line " << errorLine << ": "
      << errorMsg.toStdString();
  }
};

TEST_F(Translation_ts, ValidXml) {
  // Root element should be <TS>
  EXPECT_EQ(m_doc.documentElement().tagName(), "TS");
}

TEST_F(Translation_ts, HasExpectedContexts) {
  // Verify contexts we introduced are present in the file
  const QStringList requiredContexts = {
    "openstudio::SimSettingsView",
    "openstudio::RunView",
    "openstudio::RunTabView",
    "openstudio::ResultsView",
    "openstudio::ResultsTabController",
    "openstudio::VariablesList",
    "openstudio::ScriptsTabView",
    "openstudio::LocalLibraryView",
    "openstudio::measuretab::WorkflowController",
    "openstudio::measuretab::NewMeasureDropZone",
    "IDD",
    "OutputVariables",
    "TaxonomyCategories",
  };

  QSet<QString> foundContexts;
  QDomNodeList contextNodes = m_doc.elementsByTagName("context");
  for (int i = 0; i < contextNodes.count(); ++i) {
    QDomElement nameEl = contextNodes.at(i).firstChildElement("name");
    if (!nameEl.isNull()) {
      foundContexts.insert(nameEl.text());
    }
  }

  for (const QString& ctx : requiredContexts) {
    EXPECT_TRUE(foundContexts.contains(ctx))
      << "Missing translation context: " << ctx.toStdString();
  }
}

TEST_F(Translation_ts, TranslationCountIsSubstantial) {
  // Sanity check: the file should contain at least 2000 translated messages.
  // This catches accidental truncation of the file.
  int count = 0;
  QDomNodeList messages = m_doc.elementsByTagName("message");
  for (int i = 0; i < messages.count(); ++i) {
    QDomElement translation = messages.at(i).firstChildElement("translation");
    if (!translation.isNull() && translation.attribute("type") != "unfinished"
        && !translation.text().isEmpty()) {
      ++count;
    }
  }
  EXPECT_GE(count, 2000) << "Unexpectedly few finished translations: " << count;
}

TEST_F(Translation_ts, IddContextHasEntries) {
  int iddCount = 0;
  QDomNodeList contextNodes = m_doc.elementsByTagName("context");
  for (int i = 0; i < contextNodes.count(); ++i) {
    QDomElement nameEl = contextNodes.at(i).firstChildElement("name");
    if (!nameEl.isNull() && nameEl.text() == "IDD") {
      iddCount = contextNodes.at(i).toElement().elementsByTagName("message").count();
      break;
    }
  }
  EXPECT_GT(iddCount, 50) << "IDD context has unexpectedly few entries: " << iddCount;
}

TEST_F(Translation_ts, OutputVariablesContextHasEntries) {
  int count = 0;
  QDomNodeList contextNodes = m_doc.elementsByTagName("context");
  for (int i = 0; i < contextNodes.count(); ++i) {
    QDomElement nameEl = contextNodes.at(i).firstChildElement("name");
    if (!nameEl.isNull() && nameEl.text() == "OutputVariables") {
      count = contextNodes.at(i).toElement().elementsByTagName("message").count();
      break;
    }
  }
  // There are 1051 output variable names
  EXPECT_GE(count, 1000) << "OutputVariables context has unexpectedly few entries: " << count;
}

TEST_F(Translation_ts, TaxonomyCategoriesContextHasEntries) {
  int count = 0;
  QDomNodeList contextNodes = m_doc.elementsByTagName("context");
  for (int i = 0; i < contextNodes.count(); ++i) {
    QDomElement nameEl = contextNodes.at(i).firstChildElement("name");
    if (!nameEl.isNull() && nameEl.text() == "TaxonomyCategories") {
      count = contextNodes.at(i).toElement().elementsByTagName("message").count();
      break;
    }
  }
  EXPECT_GT(count, 30) << "TaxonomyCategories context has unexpectedly few entries: " << count;
}

// ---------------------------------------------------------------------------
// Test Suite: Translation_qm  (validates the compiled .qm and live translate)
// ---------------------------------------------------------------------------

class Translation_qm : public OpenStudioAppFixture
{
 protected:
  QTranslator m_translator;
  bool m_loaded = false;

  void SetUp() override {
    openstudio::path qmPath = findQmFile("es");
    if (!qmPath.empty()) {
      m_loaded = m_translator.load(toQString(qmPath));
      if (m_loaded) {
        QCoreApplication::installTranslator(&m_translator);
      }
    }
  }

  void TearDown() override {
    if (m_loaded) {
      QCoreApplication::removeTranslator(&m_translator);
    }
  }
};

TEST_F(Translation_qm, QmFileLoads) {
  openstudio::path qmPath = findQmFile("es");
  if (qmPath.empty()) {
    GTEST_SKIP() << "OpenStudioApp_es.qm not found in candidate paths; skipping runtime translation tests. "
                    "Build the translations target and re-run.";
  }
  EXPECT_TRUE(m_loaded) << "QTranslator::load() failed for: " << qmPath;
}

TEST_F(Translation_qm, SpanishSimSettingsStringsTranslated) {
  if (!m_loaded) {
    GTEST_SKIP() << "Spanish .qm not loaded.";
  }

  // Spot-check a few strings from the Simulation Settings tab
  EXPECT_EQ(QCoreApplication::translate("openstudio::SimSettingsView", "Run Period"),
            QString("Período de Ejecución"));
  EXPECT_EQ(QCoreApplication::translate("openstudio::SimSettingsView", "Timestep"),
            QString("Paso de Tiempo"));
  EXPECT_EQ(QCoreApplication::translate("openstudio::SimSettingsView", "Shadow Calculation"),
            QString("Cálculo de Sombras"));
  EXPECT_EQ(QCoreApplication::translate("openstudio::SimSettingsView", "Algorithm"),
            QString("Algoritmo"));
}

TEST_F(Translation_qm, SpanishRunViewStringsTranslated) {
  if (!m_loaded) {
    GTEST_SKIP() << "Spanish .qm not loaded.";
  }

  EXPECT_EQ(QCoreApplication::translate("openstudio::RunView", "Run"), QString("Ejecutar"));
  EXPECT_EQ(QCoreApplication::translate("openstudio::RunView", "Verbose"), QString("Detallado"));
  EXPECT_EQ(QCoreApplication::translate("openstudio::RunView", "Show Simulation"),
            QString("Mostrar Simulación"));
  EXPECT_EQ(QCoreApplication::translate("openstudio::RunView", "Initializing workflow."),
            QString("Inicializando flujo de trabajo."));
}

TEST_F(Translation_qm, TaxonomyCategoriesTranslated) {
  if (!m_loaded) {
    GTEST_SKIP() << "Spanish .qm not loaded.";
  }

  EXPECT_EQ(QCoreApplication::translate("TaxonomyCategories", "Envelope"), QString("Envolvente"));
  EXPECT_EQ(QCoreApplication::translate("TaxonomyCategories", "HVAC"), QString("HVAC"));
  EXPECT_EQ(QCoreApplication::translate("TaxonomyCategories", "Refrigeration"),
            QString("Refrigeración"));
  EXPECT_EQ(QCoreApplication::translate("TaxonomyCategories", "Whole Building"),
            QString("Edificio Completo"));
  EXPECT_EQ(QCoreApplication::translate("TaxonomyCategories", "Troubleshooting"),
            QString("Solución de Problemas"));
}

TEST_F(Translation_qm, OutputVariablesSampleTranslated) {
  if (!m_loaded) {
    GTEST_SKIP() << "Spanish .qm not loaded.";
  }

  // A sampling of output variable names
  EXPECT_EQ(QCoreApplication::translate("OutputVariables", "Zone Air Temperature"),
            QString("Temperatura del Aire de la Zona"));
  EXPECT_EQ(QCoreApplication::translate("OutputVariables", "Fan Electricity Energy"),
            QString("Energía Eléctrica del Ventilador"));
  EXPECT_EQ(QCoreApplication::translate("OutputVariables", "Boiler Heating Energy"),
            QString("Energía de Calefacción de la Caldera"));
}

TEST_F(Translation_qm, EnglishStringsReturnedWithoutTranslator) {
  // Remove the translator to verify English fallback works
  if (m_loaded) {
    QCoreApplication::removeTranslator(&m_translator);
  }

  // tr() / translate() must return the source string when no translator is loaded
  EXPECT_EQ(QCoreApplication::translate("openstudio::RunView", "Run"), QString("Run"));
  EXPECT_EQ(QCoreApplication::translate("TaxonomyCategories", "Envelope"), QString("Envelope"));
  EXPECT_EQ(QCoreApplication::translate("openstudio::SimSettingsView", "Timestep"),
            QString("Timestep"));

  // Re-install for TearDown
  if (m_loaded) {
    QCoreApplication::installTranslator(&m_translator);
  }
}
