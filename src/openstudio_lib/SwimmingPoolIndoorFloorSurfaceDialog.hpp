/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SWIMMINGPOOLINDOORFLOORSURFACEDIALOG_HPP
#define OPENSTUDIO_SWIMMINGPOOLINDOORFLOORSURFACEDIALOG_HPP

#include <openstudio/model/Model.hpp>
#include <openstudio/model/Surface.hpp>
#include <openstudio/model/SwimmingPoolIndoor.hpp>

#include "../shared_gui_components/OSDialog.hpp"

class QLabel;
class QComboBox;

namespace openstudio {

class SwimmingPoolIndoorFloorSurfaceDialog : public OSDialog
{
  Q_OBJECT

 public:
  explicit SwimmingPoolIndoorFloorSurfaceDialog(model::SwimmingPoolIndoor pool, QWidget* parent = nullptr);

  virtual ~SwimmingPoolIndoorFloorSurfaceDialog() = default;

  boost::optional<model::Surface> floorSurface() const;

 private slots:

  void onCurrentIndexChanged(int index);

 private:
  // void createWidgets();

  model::SwimmingPoolIndoor m_pool;
  model::Model m_model;
  boost::optional<model::Surface> m_floorSurface;

  QComboBox* m_floorSurfaceComboBox;
  QLabel* m_spaceName;
  QLabel* m_thermalZoneName;

 protected slots:

  virtual void on_okButton(bool checked) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SWIMMINGPOOLINDOORFLOORSURFACEDIALOG_HPP
