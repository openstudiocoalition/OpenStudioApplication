/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "DefaultConstructionSetsView.hpp"
#include "DefaultConstructionSetInspectorView.hpp"
#include "ModelObjectListView.hpp"

#include <openstudio/model/Model_Impl.hpp>
#include <QStyleOption>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QScrollArea>
#include <sstream>
#include <openstudio/utilities/idd/IddEnums.hxx>

namespace openstudio {

DefaultConstructionSetsView::DefaultConstructionSetsView(const openstudio::model::Model& model, const QString& tabLabel, bool hasSubTabs,
                                                         QWidget* parent)
  : ModelSubTabView(new ModelObjectListView(IddObjectType::OS_DefaultConstructionSet, model, true, false, parent),
                    new DefaultConstructionSetInspectorView(model, parent), false, parent) {}

}  // namespace openstudio
