/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SCRIPTITEM_HPP
#define OPENSTUDIO_SCRIPTITEM_HPP

#include "OSItem.hpp"

#include <openstudio/measure/OSArgument.hpp>
#include <openstudio/measure/OSMeasureInfoGetter.hpp>
//#include "../runmanager/lib/RunManager.hpp"
//#include "../runmanager/lib/RubyJobUtils.hpp"

#include <openstudio/utilities/core/Path.hpp>

#include <QWidget>

class QPushButton;
class QLabel;

namespace openstudio {

OSItemId scriptToItemId(const openstudio::path& t_path);

//QString modelToSourceId(const openstudio::model::Model& model);

class ScriptItem : public OSItem
{
  Q_OBJECT

 public:
  ScriptItem(const openstudio::path& t_path, OSItemType type, QWidget* parent = nullptr);

  virtual ~ScriptItem() {}

  openstudio::path path() const;

  openstudio::path argsDbPath() const;

  bool isUserScript() const;

  void setIsUserScript(bool isUserScript);

  //void refreshArgumentsFromScript(runmanager::RunManager t_rm);

  std::vector<measure::OSArgument> osArguments() const;

  boost::optional<measure::OSMeasureInfo> userScriptInfo() const;

  /** Returns the resources path that corresponds to this script's path, if possible.
   *  Otherwise, returns the root scripts path. */
  openstudio::path resourcesPath() const;

  bool updateArgumentsFromDb();

  void saveArgumentsToDb();

  void deleteDb();

  bool removed() const;

  virtual bool equal(const openstudio::OSItem*) const override;

 signals:

  void argChanged();

 public slots:

  void setOSArgument(const measure::OSArgument& arg);

 private slots:

  void onObjectChanged();

 private:
  bool m_removed;
  //runmanager::ScriptInfo m_scriptInfo;

  REGISTER_LOGGER("openstudio.ScriptItem");
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SCRIPTITEM_HPP
