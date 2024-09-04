/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_DESIGNDAYGRIDVIEW_HPP
#define OPENSTUDIO_DESIGNDAYGRIDVIEW_HPP

#include "../shared_gui_components/OSGridController.hpp"

#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>

#include <QWidget>

class OpenStudioLibFixture;

namespace openstudio {

class DesignDayGridController;

class DesignDayGridView : public QWidget
{
  Q_OBJECT

 public:
  DesignDayGridView(bool isIP, const model::Model& model, QWidget* parent = 0);

  virtual ~DesignDayGridView() {}

  std::set<model::ModelObject> selectedObjects() const;

 private:
  // for testing
  friend class ::OpenStudioLibFixture;

  void addObject(const model::ModelObject& modelObject);

  void copyObject(const openstudio::model::ModelObject& modelObject);

  void removeObject(openstudio::model::ModelObject modelObject);

  void purgeObjects(const IddObjectType& iddObjectType);

  DesignDayGridController* m_gridController = nullptr;

  OSGridView* m_gridView = nullptr;

  bool m_isIP;

 signals:

  void toggleUnitsClicked(bool displayIP);

  void dropZoneItemClicked(OSItem* item);

 public slots:

  void onAddClicked();

  void onCopyClicked();

  void onRemoveClicked();

  void onPurgeClicked();
};

class DesignDayGridController : public OSGridController
{

  Q_OBJECT

 public:
  DesignDayGridController(bool isIP, const QString& headerText, IddObjectType iddObjectType, const model::Model& model,
                          const std::vector<model::ModelObject>& modelObjects);

  virtual ~DesignDayGridController() {}

  virtual void refreshModelObjects() override;

 protected:
  void setCategoriesAndFields();

  virtual void addColumns(const QString& t_category, std::vector<QString>& fields) override;

  virtual void checkSelectedFields() override;

  virtual QString getColor(const model::ModelObject& modelObject) override;

 public slots:

  virtual void onItemDropped(const OSItemId& itemId) override;

  virtual void onComboBoxIndexChanged(int index) override;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_DESIGNDAYGRIDVIEW_HPP
