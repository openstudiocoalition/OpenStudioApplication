/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "LoopLibraryDialog.hpp"
#include <QLabel>
#include <QFrame>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>

namespace openstudio {

LoopLibraryDialog::LoopLibraryDialog(QWidget* parent) : QDialog(parent) {
  setObjectName("GrayWidget");

  setFixedSize(280, 584);

  setWindowTitle("Add HVAC System");
  setWindowFlags(Qt::WindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint));

  auto* mainVLayout = new QVBoxLayout();
  mainVLayout->setContentsMargins(0, 0, 0, 0);
  mainVLayout->setSpacing(0);
  setLayout(mainVLayout);

  auto* loopsLabel = new QLabel("HVAC Systems");
  loopsLabel->setStyleSheet("QLabel { margin-left: 5px; }");
  mainVLayout->addSpacing(5);
  mainVLayout->addWidget(loopsLabel);
  mainVLayout->addSpacing(5);

  auto* divider = new QFrame();
  divider->setFrameShape(QFrame::HLine);
  divider->setFrameShadow(QFrame::Sunken);
  mainVLayout->addWidget(divider);

  auto* scrollAreaWidget = new QWidget();
  scrollAreaWidget->setContentsMargins(0, 0, 0, 0);
  scrollAreaWidget->setObjectName("GrayWidget");

  auto* scrollAreaLayout = new QVBoxLayout();
  scrollAreaLayout->setContentsMargins(0, 0, 0, 0);
  scrollAreaLayout->setSpacing(0);
  scrollAreaLayout->setAlignment(Qt::AlignTop);

  scrollAreaWidget->setLayout(scrollAreaLayout);

  m_scrollArea = new QScrollArea();
  m_scrollArea->setFrameStyle(QFrame::NoFrame);
  m_scrollArea->setWidget(scrollAreaWidget);
  m_scrollArea->setWidgetResizable(true);
  m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  mainVLayout->addWidget(m_scrollArea, 100);

  mainVLayout->addStretch();

  //newItem( ADDTOMODEL_SYSTEM_TYPE_1,
  //         QString("Packaged Terminal \nAir Conditioner"),
  //         QPixmap(":/images/system_type_1.png") );

  //newItem( ADDTOMODEL_SYSTEM_TYPE_2,
  //         QString("Packaged Terminal Heat Pump"),
  //         QPixmap(":/images/system_type_2.png") );

  newItem(ADDTOMODEL_SYSTEM_TYPE_3, QString("Packaged Rooftop Unit"), QPixmap(":/images/system_type_3.png"));

  newItem(ADDTOMODEL_SYSTEM_TYPE_4, QString("Packaged Rooftop Heat Pump"), QPixmap(":/images/system_type_4.png"));

  newItem(ADDTOMODEL_SYSTEM_TYPE_5, QString("Packaged DX Rooftop VAV \nwith Reheat"), QPixmap(":/images/system_type_5.png"));

  newItem(ADDTOMODEL_SYSTEM_TYPE_6, QString("Packaged Rooftop \nVAV with Parallel Fan \nPower Boxes and reheat"),
          QPixmap(":/images/system_type_6.png"));

  newItem(ADDTOMODEL_SYSTEM_TYPE_7, QString("Packaged Rooftop \nVAV with Reheat"), QPixmap(":/images/system_type_7.png"));

  newItem(ADDTOMODEL_SYSTEM_TYPE_8, QString("VAV with Parallel Fan-Powered \nBoxes and Reheat"), QPixmap(":/images/system_type_8.png"));

  newItem(ADDTOMODEL_SYSTEM_TYPE_9, QString("Warm Air Furnace \nGas Fired"), QPixmap(":/images/system_type_9.png"));

  newItem(ADDTOMODEL_SYSTEM_TYPE_10, QString("Warm Air Furnace \nElectric"), QPixmap(":/images/system_type_10.png"));

  newItem(ADDTOMODEL_AIRLOOPHVAC, QString("Empty Air Loop"), QPixmap(":/images/air_loop_icon.png"));

  newItem(ADDTOMODEL_DUAL_AIRLOOPHVAC, QString("Dual Duct Air Loop"), QPixmap(":/images/air_loop_icon.png"));

  newItem(ADDTOMODEL_PLANTLOOP, QString("Empty Plant Loop"), QPixmap(":/images/plant_loop_icon.png"));

  newItem(ADDTOMODEL_SHWLOOP, QString("Service Hot Water Plant Loop"), QPixmap(":/images/shw_loop_icon.png"));
}

void LoopLibraryDialog::paintEvent(QPaintEvent* event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void LoopLibraryDialog::onAddToModelClicked(AddToModelEnum addToModelEnum) {
  m_addToModelEnum = addToModelEnum;

  accept();
}

boost::optional<AddToModelEnum> LoopLibraryDialog::addToModelEnum() const {
  return m_addToModelEnum;
}

void LoopLibraryDialog::newItem(const AddToModelEnum& addToModelEnum, const QString& detailedText, const QPixmap& pixmap) {
  auto* itemView = new LoopItemView(addToModelEnum, detailedText, pixmap);

  connect(itemView, &LoopItemView::addToModelClicked, this, &LoopLibraryDialog::onAddToModelClicked);

  m_scrollArea->widget()->layout()->addWidget(itemView);

  auto* divider = new QFrame();
  divider->setFrameShape(QFrame::HLine);
  divider->setFrameShadow(QFrame::Sunken);

  m_scrollArea->widget()->layout()->addWidget(divider);
}

LoopItemView::LoopItemView(const AddToModelEnum& addToModelEnum, const QString& detailedText, const QPixmap& pixmap, QWidget* parent)
  : QWidget(parent), m_addToModelEnum(addToModelEnum), m_detailedText(detailedText), m_pixmap(pixmap) {
  auto* mainVLayout = new QVBoxLayout();

  setLayout(mainVLayout);

  auto* label = new QLabel(detailedText);
  mainVLayout->addWidget(label, 0, Qt::AlignLeft);

  auto* imageLabel = new QLabel();
  imageLabel->setFixedSize(m_pixmap.size());
  imageLabel->setPixmap(m_pixmap);
  mainVLayout->addWidget(imageLabel, 0, Qt::AlignCenter);

  auto* button = new QPushButton();
  button->setText("Add to Model");
  button->setObjectName("StandardGrayButton");
  mainVLayout->addWidget(button, 0, Qt::AlignRight);

  connect(button, &QPushButton::clicked, this, &LoopItemView::emitAddToModelClicked);
}

void LoopItemView::emitAddToModelClicked() {
  emit addToModelClicked(m_addToModelEnum);
}

//void LoopItemView::paintEvent ( QPaintEvent * event )
//{
//  QStyleOption opt;
//  opt.init(this);
//  QPainter p(this);
//  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//}

}  // namespace openstudio
