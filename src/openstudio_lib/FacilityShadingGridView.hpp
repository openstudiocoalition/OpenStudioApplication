/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_FACILITYSHADINGGRIDVIEW_HPP
#define OPENSTUDIO_FACILITYSHADINGGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "GridViewSubTab.hpp"
#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

class QComboBox;
class QLineEdit;

namespace openstudio {

class OSQuantityEdit2;

class FacilityShadingGridController;

class FacilityShadingGridView : public GridViewSubTab
{
  Q_OBJECT

 public:
  FacilityShadingGridView(bool isIP, bool displayAdditionalProps, const model::Model& model, QWidget* parent = 0);

  virtual ~FacilityShadingGridView() {}

  QLineEdit* m_nameFilter = nullptr;

  QLineEdit* m_tiltGreaterThanFilter = nullptr;

  QLineEdit* m_tiltLessThanFilter = nullptr;

  QComboBox* m_typeFilter = nullptr;

  QLineEdit* m_orientationGreaterThanFilter = nullptr;

  QLineEdit* m_orientationLessThanFilter = nullptr;

 private:
  REGISTER_LOGGER("openstudio.FacilityShadingGridView");

  virtual void addObject(const openstudio::IddObjectType& iddObjectType) override;

  // Purges empty Shading Surface Groups (groups with no shading Surfaces)
  // Also purges Shading Surfaces which do not belong to a Shading Surface Group (these won't be translated to IDF)
  virtual void purgeObjects(const openstudio::IddObjectType& iddObjectType) override;

  void filterChanged();

  std::set<openstudio::model::ModelObject> m_objectsFilteredByName;

  std::set<openstudio::model::ModelObject> m_objectsFilteredByTilt;

  std::set<openstudio::model::ModelObject> m_objectsFilteredByType;

  std::set<openstudio::model::ModelObject> m_objectsFilteredByOrientation;

 protected slots:

  virtual void onSelectItem() override;

  virtual void onClearSelection() override;

 private slots:

  void tiltFilterChanged();

  void orientationFilterChanged();

  void nameFilterChanged();

  void typeFilterChanged(const QString& text);
};

class FacilityShadingGridController : public OSGridController
{

  Q_OBJECT

 public:
  FacilityShadingGridController(bool isIP, bool displayAdditionalProps, const QString& headerText, IddObjectType iddObjectType,
                                const model::Model& model, const std::vector<model::ModelObject>& modelObjects);

  virtual ~FacilityShadingGridController() {}

  virtual void refreshModelObjects() override;

 protected:
  void setCategoriesAndFields();

  virtual void addColumns(const QString& category, std::vector<QString>& fields) override;

  virtual void checkSelectedFields() override;

  virtual QString getColor(const model::ModelObject& modelObject) override;

 public slots:

  virtual void onCategorySelected(int index) override;

  virtual void onItemDropped(const OSItemId& itemId) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_FACILITYSHADINGGRIDVIEW_HPP
