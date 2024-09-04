/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_TABLEWIDGET_HPP
#define MODELEDITOR_TABLEWIDGET_HPP

#include <QTableWidget>

#include <openstudio/model/Building.hpp>
#include "ModelEditorAPI.hpp"

namespace modeleditor {

class MODELEDITOR_API TableWidget : public QTableWidget
{
  Q_OBJECT

 public:
  explicit TableWidget(QWidget* parent = nullptr);
  virtual ~TableWidget();
  void populateTable(const openstudio::WorkspaceObject& obj);

 public slots:

 signals:

 protected:
 private:
};

}  // namespace modeleditor

#endif  // MODELEDITOR_TABLEWIDGET_HPP
