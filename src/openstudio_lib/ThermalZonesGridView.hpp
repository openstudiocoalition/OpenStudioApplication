/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_THERMALZONESGRIDVIEW_HPP
#define OPENSTUDIO_THERMALZONESGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

#include <QWidget>

namespace openstudio {

class ModelSubTabView;

class ThermalZonesGridController;

class ThermalZonesGridView : public QWidget
{
  Q_OBJECT

 public:
  ThermalZonesGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = nullptr);

  virtual ~ThermalZonesGridView() {}

  std::set<model::ModelObject> selectedObjects() const;

 private:
  ThermalZonesGridController* m_gridController = nullptr;

  bool m_isIP;

  bool m_displayAdditionalProps;

 signals:

  void toggleUnitsClicked(bool displayIP);

  void toggleDisplayAdditionalPropsClicked(bool displayAdditionalProps);

  void dropZoneItemClicked(OSItem* item);

  void selectionCleared();
};

class ThermalZonesGridController : public OSGridController
{

  Q_OBJECT

 public:
  ThermalZonesGridController(bool isIP, bool displayAdditionalProps, const QString& headerText, IddObjectType iddObjectType,
                             const model::Model& model, const std::vector<model::ModelObject>& modelObjects);

  virtual ~ThermalZonesGridController() {}

  virtual void refreshModelObjects() override;

 protected:
  void setCategoriesAndFields();

  virtual void addColumns(const QString& t_category, std::vector<QString>& fields) override;

  virtual void checkSelectedFields() override;

  virtual QString getColor(const model::ModelObject& modelObject) override;

 private:
  REGISTER_LOGGER("openstudio.ThermalZonesGridController");

 public slots:

  virtual void onItemDropped(const OSItemId& itemId) override;

  virtual void onComboBoxIndexChanged(int index) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_THERMALZONESGRIDVIEW_HPP
