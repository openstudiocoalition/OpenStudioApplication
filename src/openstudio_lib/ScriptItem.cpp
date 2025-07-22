/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "ScriptItem.hpp"
#include "OSAppBase.hpp"
#include "OSDocument.hpp"
#include "../model_editor/Utilities.hpp"

//#include "../runmanager/lib/RunManager.hpp"
//#include "../runmanager/lib/RubyJobUtils.hpp"

#include <openstudio/utilities/core/Assert.hpp>
#include <openstudio/utilities/core/Containers.hpp>

#include <QDir>
#include <QMessageBox>

namespace openstudio {

OSItemId scriptToItemId(const openstudio::path& t_path) {
  return OSItemId(openstudio::toQString(t_path), openstudio::toQString(t_path.parent_path().filename()), false, "");
}

ScriptItem::ScriptItem(const openstudio::path& t_path, OSItemType type, QWidget* parent)
  : OSItem(scriptToItemId(t_path), type, parent),
    m_removed(false)
// m_scriptInfo(t_path, true, false)
{
  setText(openstudio::toQString(t_path.filename()));
  setLeftPixmap(QPixmap(":/images/icon_scripts.png"));
  //if (boost::regex_search(toString(itemId().sourceId()),boost::regex("resource"))) {
  //  m_scriptInfo.isUserScript = false;
  //}
  //else {
  //  try {
  //    m_scriptInfo = runmanager::RubyJobBuilder::updateArgumentsFromDb(m_scriptInfo);
  //  } catch (const runmanager::ScriptDetectionError &e) {
  //    // Nothing to display here in the constructor
  //    m_scriptInfo = e.scriptInfo;
  //  }
  //}

  //std::shared_ptr<OSDocument> osDoc = OSAppBase::instance()->currentDocument();
  //connect(this, &ScriptItem::argChanged, osDoc.get(), &OSDocument::markAsModified);
}

openstudio::path ScriptItem::path() const {
  //return m_scriptInfo.scriptPath;
  return openstudio::path();
}

openstudio::path ScriptItem::argsDbPath() const {
  //return m_scriptInfo.dbPath;
  return openstudio::path();
}

bool ScriptItem::isUserScript() const {
  //return m_scriptInfo.isUserScript;
  return false;
}

void ScriptItem::setIsUserScript(bool isUserScript) {
  //m_scriptInfo.isUserScript = isUserScript;
}

//void ScriptItem::refreshArgumentsFromScript(runmanager::RunManager t_rm)
//{
//  if (t_rm.getConfigOptions().getTools().getAllByName("ruby").tools().size() == 0)
//  {
//    QMessageBox::critical(this,
//        "Ruby Not Installed",
//        "Please install and locate ruby to use user scripts in OpenStudio.",
//        QMessageBox::Ok);
//  } else {
//    try {
//      m_scriptInfo = runmanager::RubyJobBuilder::refreshArgumentsFromScript(t_rm, m_scriptInfo);
//    } catch (const runmanager::ScriptDetectionError &e) {
//      m_scriptInfo = e.scriptInfo;
//
//      QMessageBox::information(this,
//          "Could not retrieve user script arguments",
//          e.what(),
//          QMessageBox::Ok);
//    }
//  }
//}

std::vector<measure::OSArgument> ScriptItem::osArguments() const {
  measure::OSArgumentVector result;
  //for (const auto & elem : m_scriptInfo.arguments)
  //{
  //  result.push_back(elem.second);
  //}
  return result;
}

boost::optional<measure::OSMeasureInfo> ScriptItem::userScriptInfo() const {
  if (isUserScript()) {
    OS_ASSERT(false);
    //return measure::OSMeasureInfo(path(),osArguments());
  }
  return boost::none;
}

openstudio::path ScriptItem::resourcesPath() const {
  return openstudio::path();
}

void ScriptItem::setOSArgument(const measure::OSArgument& arg) {
  //m_scriptInfo.arguments[arg.name()] = arg;
  //m_scriptInfo.argsChanged = true;
  //emit argChanged();
}

void ScriptItem::saveArgumentsToDb() {
  //try {
  //  m_scriptInfo = runmanager::RubyJobBuilder::saveArgumentsToDb(m_scriptInfo);
  //} catch (const runmanager::ScriptDetectionError &e) {
  //  QMessageBox::information(this,
  //      "User Script Argument Values not Saved",
  //      e.what(),
  //      QMessageBox::Ok);
  //  m_scriptInfo = e.scriptInfo;
  //}
}

void ScriptItem::deleteDb() {
  m_removed = true;
  boost::system::error_code ec;
  if (openstudio::filesystem::exists(argsDbPath()), ec) {
    openstudio::filesystem::remove(argsDbPath(), ec);
    openstudio::filesystem::remove(toPath(toString((argsDbPath())) + "-journal"), ec);
  }
}

bool ScriptItem::removed() const {
  return m_removed;
}

bool ScriptItem::equal(const openstudio::OSItem* otherItem) const {
  if (otherItem) {
    return otherItem->itemId() == itemId();
  }
  return false;
}

void ScriptItem::onObjectChanged() {
  //this->setText(openstudio::toQString(m_scriptInfo.scriptPath.filename()));
}

bool ScriptItem::updateArgumentsFromDb() {
  //try {
  //  m_scriptInfo = runmanager::RubyJobBuilder::updateArgumentsFromDb(m_scriptInfo);
  //} catch (const runmanager::ScriptDetectionError &e) {
  //  m_scriptInfo = e.scriptInfo;

  //  QMessageBox::information(this,
  //      "Could not retrieve user script arguments",
  //      e.what(),
  //      QMessageBox::Ok);
  //}
  //return m_scriptInfo.detectionSucceeded;
  return false;
}

}  // namespace openstudio
