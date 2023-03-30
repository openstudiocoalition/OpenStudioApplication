/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) 2020-2023, OpenStudio Coalition and other contributors. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
*  following conditions are met:
*
*  (1) Redistributions of source code must retain the above copyright notice, this list of conditions and the following
*  disclaimer.
*
*  (2) Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following
*  disclaimer in the documentation and/or other materials provided with the distribution.
*
*  (3) Neither the name of the copyright holder nor the names of any contributors may be used to endorse or promote products
*  derived from this software without specific prior written permission from the respective party.
*
*  (4) Other than as required in clauses (1) and (2), distributions in any form of modifications or other derivative works
*  may not use the "OpenStudio" trademark, "OS", "os", or any other confusingly similar designation without specific prior
*  written permission from Alliance for Sustainable Energy, LLC.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) AND ANY CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
*  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S), ANY CONTRIBUTORS, THE UNITED STATES GOVERNMENT, OR THE UNITED
*  STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSCELLWRAPPER_HPP
#define SHAREDGUICOMPONENTS_OSCELLWRAPPER_HPP

#include "OSConcepts.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <vector>

#include <QSharedPointer>
#include <QWidget>
#include <QVBoxLayout>

class QButtonGroup;
class QCheckBox;
class QColor;
class QLabel;
class QPaintEvent;
class OpenStudioLibFixture;

namespace openstudio {

class GridCellLocation;
class GridCellInfo;
class OSGridView;
class OSGridController;
class OSObjectSelector;
class OSWidgetHolder;

// An OSCellWrapper has one or more OSWidgetHolders, one for each subrow in a cell
class OSCellWrapper : public QWidget
{
  Q_OBJECT

 public:
  OSCellWrapper(OSGridView* gridView, QSharedPointer<BaseConcept> baseConcept, OSObjectSelector* objectSelector, int modelRow, int gridRow,
                int column);

  virtual ~OSCellWrapper();

  void addOSWidget(QWidget* widget, const boost::optional<model::ModelObject>& obj, const bool isSelector, const bool isParent);

  void setGridController(OSGridController* gridController);
  void setModelObject(const model::ModelObject& modelObject);
  void refresh();

  void setCellProperties(const GridCellLocation& location, const GridCellInfo& info);

 private slots:

  void onRemoveWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle);

  void onAddWorkspaceObject(const WorkspaceObject& object, const openstudio::IddObjectType& iddObjectType, const openstudio::UUID& handle);

  void processNewModelObjects();

 signals:

  void rowNeedsStyle(int modelRow, int gridRow);

 private:
  // for testing
  friend class ::OpenStudioLibFixture;

  // Make the lowest level os widgets that corresponds to concepts (e.g. OSLineEdit2).
  // These will be put in container widgets to form the cell, regardless of the presence of sub rows.
  QWidget* createOSWidget(model::ModelObject t_mo, const QSharedPointer<BaseConcept>& t_baseConcept);

  void connectModelSignals();
  void disconnectModelSignals();
  void makeHeader();

  OSGridView* m_gridView;
  QGridLayout* m_layout;
  std::vector<OSWidgetHolder*> m_holders;
  QSharedPointer<BaseConcept> m_baseConcept;
  OSObjectSelector* m_objectSelector;
  int m_modelRow = 0;
  int m_gridRow = 0;
  int m_column = 0;
  bool m_hasSubRows = false;
  int m_refreshCount = 0;

  // only has these members if not a header cell
  boost::optional<model::ModelObject> m_modelObject;
  OSGridController* m_gridController;

  // set when connecting to a model signals
  boost::optional<model::Model> m_connectedmodel;

  // temp variable
  std::set<model::ModelObject> m_newModelObjects;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSCELLWRAPPER_HPP
