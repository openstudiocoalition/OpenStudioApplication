/****************************************************************************
** Meta object code from reading C++ file 'InspectorView.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "InspectorView.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'InspectorView.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10openstudio13InspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::InspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio13InspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::InspectorView",
        "addZoneClicked",
        "",
        "model::ThermalZone&",
        "removeZoneClicked",
        "addToLoopClicked",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked",
        "toggleUnitsClicked",
        "displayIP",
        "toggleDisplayAdditionalPropsClicked",
        "displayAdditionalProps",
        "itemRemoveClicked",
        "OSItem*",
        "removeButtonClicked",
        "workspaceObjectRemoved",
        "moveBranchForZoneSupplySelected",
        "zone",
        "Handle",
        "newPlenumHandle",
        "moveBranchForZoneReturnSelected",
        "moveBranchForZoneToNewSupplyPlenumSelected",
        "newPlenumZoneHandle",
        "moveBranchForZoneToNewReturnPlenumSelected",
        "layoutModelObject",
        "model::OptionalModelObject&",
        "readOnly",
        "toggleUnits",
        "enterEvent",
        "QEnterEvent*",
        "event",
        "leaveEvent",
        "QEvent*"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addZoneClicked'
        QtMocHelpers::SignalData<void(model::ThermalZone &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Signal 'removeZoneClicked'
        QtMocHelpers::SignalData<void(model::ThermalZone &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 2 }, { 0x80000000 | 7, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 6, 2 }, { 0x80000000 | 7, 2 },
        }}),
        // Signal 'toggleUnitsClicked'
        QtMocHelpers::SignalData<void(bool)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 10 },
        }}),
        // Signal 'toggleDisplayAdditionalPropsClicked'
        QtMocHelpers::SignalData<void(bool)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 12 },
        }}),
        // Signal 'itemRemoveClicked'
        QtMocHelpers::SignalData<void(OSItem *)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 2 },
        }}),
        // Signal 'removeButtonClicked'
        QtMocHelpers::SignalData<void(bool)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 2 },
        }}),
        // Signal 'workspaceObjectRemoved'
        QtMocHelpers::SignalData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'moveBranchForZoneSupplySelected'
        QtMocHelpers::SignalData<void(model::ThermalZone &, const Handle &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 18 }, { 0x80000000 | 19, 20 },
        }}),
        // Signal 'moveBranchForZoneReturnSelected'
        QtMocHelpers::SignalData<void(model::ThermalZone &, const Handle &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 18 }, { 0x80000000 | 19, 20 },
        }}),
        // Signal 'moveBranchForZoneToNewSupplyPlenumSelected'
        QtMocHelpers::SignalData<void(model::ThermalZone &, const Handle &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 18 }, { 0x80000000 | 19, 23 },
        }}),
        // Signal 'moveBranchForZoneToNewReturnPlenumSelected'
        QtMocHelpers::SignalData<void(model::ThermalZone &, const Handle &)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 18 }, { 0x80000000 | 19, 23 },
        }}),
        // Slot 'layoutModelObject'
        QtMocHelpers::SlotData<void(model::OptionalModelObject &, bool, bool, bool)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 26, 2 }, { QMetaType::Bool, 27 }, { QMetaType::Bool, 10 }, { QMetaType::Bool, 12 },
        }}),
        // Slot 'layoutModelObject'
        QtMocHelpers::SlotData<void(model::OptionalModelObject &, bool, bool)>(25, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { 0x80000000 | 26, 2 }, { QMetaType::Bool, 27 }, { QMetaType::Bool, 10 },
        }}),
        // Slot 'toggleUnits'
        QtMocHelpers::SlotData<void(bool)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 10 },
        }}),
        // Slot 'enterEvent'
        QtMocHelpers::SlotData<void(QEnterEvent *)>(29, 2, QMC::AccessProtected, QMetaType::Void, {{
            { 0x80000000 | 30, 31 },
        }}),
        // Slot 'leaveEvent'
        QtMocHelpers::SlotData<void(QEvent *)>(32, 2, QMC::AccessProtected, QMetaType::Void, {{
            { 0x80000000 | 33, 31 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<InspectorView, qt_meta_tag_ZN10openstudio13InspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::InspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio13InspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio13InspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio13InspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::InspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<InspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addZoneClicked((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1]))); break;
        case 1: _t->removeZoneClicked((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1]))); break;
        case 2: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 3: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 4: _t->toggleUnitsClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 5: _t->toggleDisplayAdditionalPropsClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->itemRemoveClicked((*reinterpret_cast<std::add_pointer_t<OSItem*>>(_a[1]))); break;
        case 7: _t->removeButtonClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 8: _t->workspaceObjectRemoved(); break;
        case 9: _t->moveBranchForZoneSupplySelected((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Handle>>(_a[2]))); break;
        case 10: _t->moveBranchForZoneReturnSelected((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Handle>>(_a[2]))); break;
        case 11: _t->moveBranchForZoneToNewSupplyPlenumSelected((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Handle>>(_a[2]))); break;
        case 12: _t->moveBranchForZoneToNewReturnPlenumSelected((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Handle>>(_a[2]))); break;
        case 13: _t->layoutModelObject((*reinterpret_cast<std::add_pointer_t<model::OptionalModelObject&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[4]))); break;
        case 14: _t->layoutModelObject((*reinterpret_cast<std::add_pointer_t<model::OptionalModelObject&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3]))); break;
        case 15: _t->toggleUnits((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 16: _t->enterEvent((*reinterpret_cast<std::add_pointer_t<QEnterEvent*>>(_a[1]))); break;
        case 17: _t->leaveEvent((*reinterpret_cast<std::add_pointer_t<QEvent*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(model::ThermalZone & )>(_a, &InspectorView::addZoneClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(model::ThermalZone & )>(_a, &InspectorView::removeZoneClicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &InspectorView::addToLoopClicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &InspectorView::removeFromLoopClicked, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(bool )>(_a, &InspectorView::toggleUnitsClicked, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(bool )>(_a, &InspectorView::toggleDisplayAdditionalPropsClicked, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(OSItem * )>(_a, &InspectorView::itemRemoveClicked, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(bool )>(_a, &InspectorView::removeButtonClicked, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)()>(_a, &InspectorView::workspaceObjectRemoved, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(model::ThermalZone & , const Handle & )>(_a, &InspectorView::moveBranchForZoneSupplySelected, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(model::ThermalZone & , const Handle & )>(_a, &InspectorView::moveBranchForZoneReturnSelected, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(model::ThermalZone & , const Handle & )>(_a, &InspectorView::moveBranchForZoneToNewSupplyPlenumSelected, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorView::*)(model::ThermalZone & , const Handle & )>(_a, &InspectorView::moveBranchForZoneToNewReturnPlenumSelected, 12))
            return;
    }
}

const QMetaObject *openstudio::InspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::InspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio13InspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int openstudio::InspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void openstudio::InspectorView::addZoneClicked(model::ThermalZone & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void openstudio::InspectorView::removeZoneClicked(model::ThermalZone & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void openstudio::InspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void openstudio::InspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void openstudio::InspectorView::toggleUnitsClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void openstudio::InspectorView::toggleDisplayAdditionalPropsClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void openstudio::InspectorView::itemRemoveClicked(OSItem * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void openstudio::InspectorView::removeButtonClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void openstudio::InspectorView::workspaceObjectRemoved()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void openstudio::InspectorView::moveBranchForZoneSupplySelected(model::ThermalZone & _t1, const Handle & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2);
}

// SIGNAL 10
void openstudio::InspectorView::moveBranchForZoneReturnSelected(model::ThermalZone & _t1, const Handle & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1, _t2);
}

// SIGNAL 11
void openstudio::InspectorView::moveBranchForZoneToNewSupplyPlenumSelected(model::ThermalZone & _t1, const Handle & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1, _t2);
}

// SIGNAL 12
void openstudio::InspectorView::moveBranchForZoneToNewReturnPlenumSelected(model::ThermalZone & _t1, const Handle & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio17BaseInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::BaseInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio17BaseInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::BaseInspectorView",
        "toggleUnitsClicked",
        "",
        "displayIP",
        "toggleDisplayAdditionalPropsClicked",
        "displayAdditionalProps",
        "removeButtonClicked",
        "workspaceObjectRemoved",
        "toggleUnits"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'toggleUnitsClicked'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Signal 'toggleDisplayAdditionalPropsClicked'
        QtMocHelpers::SignalData<void(bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Signal 'removeButtonClicked'
        QtMocHelpers::SignalData<void(bool)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 2 },
        }}),
        // Signal 'workspaceObjectRemoved'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'toggleUnits'
        QtMocHelpers::SlotData<void(bool)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<BaseInspectorView, qt_meta_tag_ZN10openstudio17BaseInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::BaseInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio17BaseInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio17BaseInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio17BaseInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::BaseInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<BaseInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->toggleUnitsClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->toggleDisplayAdditionalPropsClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->removeButtonClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->workspaceObjectRemoved(); break;
        case 4: _t->toggleUnits((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (BaseInspectorView::*)(bool )>(_a, &BaseInspectorView::toggleUnitsClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (BaseInspectorView::*)(bool )>(_a, &BaseInspectorView::toggleDisplayAdditionalPropsClicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (BaseInspectorView::*)(bool )>(_a, &BaseInspectorView::removeButtonClicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (BaseInspectorView::*)()>(_a, &BaseInspectorView::workspaceObjectRemoved, 3))
            return;
    }
}

const QMetaObject *openstudio::BaseInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::BaseInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio17BaseInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "Nano::Observer"))
        return static_cast< Nano::Observer*>(this);
    return QWidget::qt_metacast(_clname);
}

int openstudio::BaseInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void openstudio::BaseInspectorView::toggleUnitsClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void openstudio::BaseInspectorView::toggleDisplayAdditionalPropsClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void openstudio::BaseInspectorView::removeButtonClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void openstudio::BaseInspectorView::workspaceObjectRemoved()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
namespace {
struct qt_meta_tag_ZN10openstudio20GenericInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::GenericInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio20GenericInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::GenericInspectorView",
        "removeButtonClicked",
        ""
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'removeButtonClicked'
        QtMocHelpers::SignalData<void(bool)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<GenericInspectorView, qt_meta_tag_ZN10openstudio20GenericInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::GenericInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio20GenericInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio20GenericInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio20GenericInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::GenericInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<GenericInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->removeButtonClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (GenericInspectorView::*)(bool )>(_a, &GenericInspectorView::removeButtonClicked, 0))
            return;
    }
}

const QMetaObject *openstudio::GenericInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::GenericInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio20GenericInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::GenericInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void openstudio::GenericInspectorView::removeButtonClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN10openstudio26SplitterMixerInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::SplitterMixerInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio26SplitterMixerInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::SplitterMixerInspectorView",
        "addZoneClicked",
        "",
        "model::ThermalZone&",
        "removeZoneClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addZoneClicked'
        QtMocHelpers::SignalData<void(model::ThermalZone &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
        // Signal 'removeZoneClicked'
        QtMocHelpers::SignalData<void(model::ThermalZone &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SplitterMixerInspectorView, qt_meta_tag_ZN10openstudio26SplitterMixerInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::SplitterMixerInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio26SplitterMixerInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio26SplitterMixerInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio26SplitterMixerInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::SplitterMixerInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SplitterMixerInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addZoneClicked((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1]))); break;
        case 1: _t->removeZoneClicked((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SplitterMixerInspectorView::*)(model::ThermalZone & )>(_a, &SplitterMixerInspectorView::addZoneClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SplitterMixerInspectorView::*)(model::ThermalZone & )>(_a, &SplitterMixerInspectorView::removeZoneClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::SplitterMixerInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::SplitterMixerInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio26SplitterMixerInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::SplitterMixerInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::SplitterMixerInspectorView::addZoneClicked(model::ThermalZone & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void openstudio::SplitterMixerInspectorView::removeZoneClicked(model::ThermalZone & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN10openstudio15NewPlenumDialogE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::NewPlenumDialog::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio15NewPlenumDialogE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::NewPlenumDialog",
        "onCancelClicked",
        "",
        "onApplyClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onCancelClicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onApplyClicked'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NewPlenumDialog, qt_meta_tag_ZN10openstudio15NewPlenumDialogE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::NewPlenumDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio15NewPlenumDialogE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio15NewPlenumDialogE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio15NewPlenumDialogE_t>.metaTypes,
    nullptr
} };

void openstudio::NewPlenumDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NewPlenumDialog *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onCancelClicked(); break;
        case 1: _t->onApplyClicked(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *openstudio::NewPlenumDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::NewPlenumDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio15NewPlenumDialogE_t>.strings))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int openstudio::NewPlenumDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio17PlenumChooserViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::PlenumChooserView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio17PlenumChooserViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::PlenumChooserView"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<PlenumChooserView, qt_meta_tag_ZN10openstudio17PlenumChooserViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::PlenumChooserView::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio17PlenumChooserViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio17PlenumChooserViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio17PlenumChooserViewE_t>.metaTypes,
    nullptr
} };

void openstudio::PlenumChooserView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PlenumChooserView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::PlenumChooserView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::PlenumChooserView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio17PlenumChooserViewE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int openstudio::PlenumChooserView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio32RefrigerationWalkinInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::RefrigerationWalkinInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio32RefrigerationWalkinInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::RefrigerationWalkinInspectorView"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RefrigerationWalkinInspectorView, qt_meta_tag_ZN10openstudio32RefrigerationWalkinInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::RefrigerationWalkinInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio32RefrigerationWalkinInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio32RefrigerationWalkinInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio32RefrigerationWalkinInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::RefrigerationWalkinInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RefrigerationWalkinInspectorView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::RefrigerationWalkinInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::RefrigerationWalkinInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio32RefrigerationWalkinInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::RefrigerationWalkinInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio24ThermalZoneInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ThermalZoneInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio24ThermalZoneInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ThermalZoneInspectorView",
        "moveBranchForZoneSupplySelected",
        "",
        "model::ThermalZone&",
        "zone",
        "Handle",
        "newPlenumHandle",
        "moveBranchForZoneReturnSelected",
        "onSupplyPlenumChooserChanged",
        "newIndex",
        "onReturnPlenumChooserChanged",
        "onNewSupplyPlenumClicked",
        "onNewReturnPlenumClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'moveBranchForZoneSupplySelected'
        QtMocHelpers::SignalData<void(model::ThermalZone &, const Handle &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 5, 6 },
        }}),
        // Signal 'moveBranchForZoneReturnSelected'
        QtMocHelpers::SignalData<void(model::ThermalZone &, const Handle &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 5, 6 },
        }}),
        // Slot 'onSupplyPlenumChooserChanged'
        QtMocHelpers::SlotData<void(int)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Slot 'onReturnPlenumChooserChanged'
        QtMocHelpers::SlotData<void(int)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 9 },
        }}),
        // Slot 'onNewSupplyPlenumClicked'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onNewReturnPlenumClicked'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ThermalZoneInspectorView, qt_meta_tag_ZN10openstudio24ThermalZoneInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ThermalZoneInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio24ThermalZoneInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio24ThermalZoneInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio24ThermalZoneInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ThermalZoneInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ThermalZoneInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->moveBranchForZoneSupplySelected((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Handle>>(_a[2]))); break;
        case 1: _t->moveBranchForZoneReturnSelected((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Handle>>(_a[2]))); break;
        case 2: _t->onSupplyPlenumChooserChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->onReturnPlenumChooserChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->onNewSupplyPlenumClicked(); break;
        case 5: _t->onNewReturnPlenumClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ThermalZoneInspectorView::*)(model::ThermalZone & , const Handle & )>(_a, &ThermalZoneInspectorView::moveBranchForZoneSupplySelected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ThermalZoneInspectorView::*)(model::ThermalZone & , const Handle & )>(_a, &ThermalZoneInspectorView::moveBranchForZoneReturnSelected, 1))
            return;
    }
}

const QMetaObject *openstudio::ThermalZoneInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ThermalZoneInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio24ThermalZoneInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ThermalZoneInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ThermalZoneInspectorView::moveBranchForZoneSupplySelected(model::ThermalZone & _t1, const Handle & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ThermalZoneInspectorView::moveBranchForZoneReturnSelected(model::ThermalZone & _t1, const Handle & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio23WaterToAirInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::WaterToAirInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio23WaterToAirInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::WaterToAirInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked",
        "onWorkspaceObjectChanged",
        "layoutControllerObject"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Slot 'onWorkspaceObjectChanged'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'layoutControllerObject'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WaterToAirInspectorView, qt_meta_tag_ZN10openstudio23WaterToAirInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::WaterToAirInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio23WaterToAirInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio23WaterToAirInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio23WaterToAirInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::WaterToAirInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WaterToAirInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 2: _t->onWorkspaceObjectChanged(); break;
        case 3: _t->layoutControllerObject(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WaterToAirInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &WaterToAirInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WaterToAirInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &WaterToAirInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::WaterToAirInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::WaterToAirInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio23WaterToAirInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::WaterToAirInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void openstudio::WaterToAirInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::WaterToAirInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio58AirTerminalSingleDuctConstantVolumeCooledBeamInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio58AirTerminalSingleDuctConstantVolumeCooledBeamInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::AirTerminalSingleDuctConstantVolumeCooledBeamInspector"
        "View",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView, qt_meta_tag_ZN10openstudio58AirTerminalSingleDuctConstantVolumeCooledBeamInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio58AirTerminalSingleDuctConstantVolumeCooledBeamInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio58AirTerminalSingleDuctConstantVolumeCooledBeamInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio58AirTerminalSingleDuctConstantVolumeCooledBeamInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio58AirTerminalSingleDuctConstantVolumeCooledBeamInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::AirTerminalSingleDuctConstantVolumeCooledBeamInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio24AirTerminalInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::AirTerminalInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio24AirTerminalInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::AirTerminalInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AirTerminalInspectorView, qt_meta_tag_ZN10openstudio24AirTerminalInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::AirTerminalInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio24AirTerminalInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio24AirTerminalInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio24AirTerminalInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::AirTerminalInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AirTerminalInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AirTerminalInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &AirTerminalInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AirTerminalInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &AirTerminalInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::AirTerminalInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::AirTerminalInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio24AirTerminalInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::AirTerminalInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::AirTerminalInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::AirTerminalInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctConstantVolumeReheatInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::AirTerminalSingleDuctConstantVolumeReheatInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctConstantVolumeReheatInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::AirTerminalSingleDuctConstantVolumeReheatInspectorView"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AirTerminalSingleDuctConstantVolumeReheatInspectorView, qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctConstantVolumeReheatInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::AirTerminalSingleDuctConstantVolumeReheatInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<AirTerminalInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctConstantVolumeReheatInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctConstantVolumeReheatInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctConstantVolumeReheatInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::AirTerminalSingleDuctConstantVolumeReheatInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AirTerminalSingleDuctConstantVolumeReheatInspectorView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::AirTerminalSingleDuctConstantVolumeReheatInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::AirTerminalSingleDuctConstantVolumeReheatInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctConstantVolumeReheatInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return AirTerminalInspectorView::qt_metacast(_clname);
}

int openstudio::AirTerminalSingleDuctConstantVolumeReheatInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AirTerminalInspectorView::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio43AirTerminalSingleDuctVAVReheatInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::AirTerminalSingleDuctVAVReheatInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio43AirTerminalSingleDuctVAVReheatInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::AirTerminalSingleDuctVAVReheatInspectorView"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AirTerminalSingleDuctVAVReheatInspectorView, qt_meta_tag_ZN10openstudio43AirTerminalSingleDuctVAVReheatInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::AirTerminalSingleDuctVAVReheatInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<AirTerminalInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio43AirTerminalSingleDuctVAVReheatInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio43AirTerminalSingleDuctVAVReheatInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio43AirTerminalSingleDuctVAVReheatInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::AirTerminalSingleDuctVAVReheatInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AirTerminalSingleDuctVAVReheatInspectorView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::AirTerminalSingleDuctVAVReheatInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::AirTerminalSingleDuctVAVReheatInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio43AirTerminalSingleDuctVAVReheatInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return AirTerminalInspectorView::qt_metacast(_clname);
}

int openstudio::AirTerminalSingleDuctVAVReheatInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AirTerminalInspectorView::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio51AirTerminalSingleDuctParallelPIUReheatInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::AirTerminalSingleDuctParallelPIUReheatInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio51AirTerminalSingleDuctParallelPIUReheatInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::AirTerminalSingleDuctParallelPIUReheatInspectorView"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AirTerminalSingleDuctParallelPIUReheatInspectorView, qt_meta_tag_ZN10openstudio51AirTerminalSingleDuctParallelPIUReheatInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::AirTerminalSingleDuctParallelPIUReheatInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<AirTerminalInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio51AirTerminalSingleDuctParallelPIUReheatInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio51AirTerminalSingleDuctParallelPIUReheatInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio51AirTerminalSingleDuctParallelPIUReheatInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::AirTerminalSingleDuctParallelPIUReheatInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AirTerminalSingleDuctParallelPIUReheatInspectorView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::AirTerminalSingleDuctParallelPIUReheatInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::AirTerminalSingleDuctParallelPIUReheatInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio51AirTerminalSingleDuctParallelPIUReheatInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return AirTerminalInspectorView::qt_metacast(_clname);
}

int openstudio::AirTerminalSingleDuctParallelPIUReheatInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AirTerminalInspectorView::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio49AirTerminalSingleDuctSeriesPIUReheatInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::AirTerminalSingleDuctSeriesPIUReheatInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio49AirTerminalSingleDuctSeriesPIUReheatInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::AirTerminalSingleDuctSeriesPIUReheatInspectorView"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AirTerminalSingleDuctSeriesPIUReheatInspectorView, qt_meta_tag_ZN10openstudio49AirTerminalSingleDuctSeriesPIUReheatInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::AirTerminalSingleDuctSeriesPIUReheatInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<AirTerminalInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio49AirTerminalSingleDuctSeriesPIUReheatInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio49AirTerminalSingleDuctSeriesPIUReheatInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio49AirTerminalSingleDuctSeriesPIUReheatInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::AirTerminalSingleDuctSeriesPIUReheatInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AirTerminalSingleDuctSeriesPIUReheatInspectorView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::AirTerminalSingleDuctSeriesPIUReheatInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::AirTerminalSingleDuctSeriesPIUReheatInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio49AirTerminalSingleDuctSeriesPIUReheatInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return AirTerminalInspectorView::qt_metacast(_clname);
}

int openstudio::AirTerminalSingleDuctSeriesPIUReheatInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AirTerminalInspectorView::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView, qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<AirTerminalInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio54AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return AirTerminalInspectorView::qt_metacast(_clname);
}

int openstudio::AirTerminalSingleDuctVAVHeatAndCoolReheatInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AirTerminalInspectorView::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_tag_ZN10openstudio51ZoneHVACPackagedTerminalAirConditionerInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACPackagedTerminalAirConditionerInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio51ZoneHVACPackagedTerminalAirConditionerInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACPackagedTerminalAirConditionerInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACPackagedTerminalAirConditionerInspectorView, qt_meta_tag_ZN10openstudio51ZoneHVACPackagedTerminalAirConditionerInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACPackagedTerminalAirConditionerInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio51ZoneHVACPackagedTerminalAirConditionerInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio51ZoneHVACPackagedTerminalAirConditionerInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio51ZoneHVACPackagedTerminalAirConditionerInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACPackagedTerminalAirConditionerInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACPackagedTerminalAirConditionerInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACPackagedTerminalAirConditionerInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACPackagedTerminalAirConditionerInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACPackagedTerminalAirConditionerInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACPackagedTerminalAirConditionerInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACPackagedTerminalAirConditionerInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACPackagedTerminalAirConditionerInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio51ZoneHVACPackagedTerminalAirConditionerInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACPackagedTerminalAirConditionerInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACPackagedTerminalAirConditionerInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACPackagedTerminalAirConditionerInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio45ZoneHVACPackagedTerminalHeatPumpInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACPackagedTerminalHeatPumpInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio45ZoneHVACPackagedTerminalHeatPumpInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACPackagedTerminalHeatPumpInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACPackagedTerminalHeatPumpInspectorView, qt_meta_tag_ZN10openstudio45ZoneHVACPackagedTerminalHeatPumpInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACPackagedTerminalHeatPumpInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio45ZoneHVACPackagedTerminalHeatPumpInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio45ZoneHVACPackagedTerminalHeatPumpInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio45ZoneHVACPackagedTerminalHeatPumpInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACPackagedTerminalHeatPumpInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACPackagedTerminalHeatPumpInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACPackagedTerminalHeatPumpInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACPackagedTerminalHeatPumpInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACPackagedTerminalHeatPumpInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACPackagedTerminalHeatPumpInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACPackagedTerminalHeatPumpInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACPackagedTerminalHeatPumpInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio45ZoneHVACPackagedTerminalHeatPumpInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACPackagedTerminalHeatPumpInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACPackagedTerminalHeatPumpInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACPackagedTerminalHeatPumpInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio32WaterHeaterHeatPumpInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::WaterHeaterHeatPumpInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio32WaterHeaterHeatPumpInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::WaterHeaterHeatPumpInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<WaterHeaterHeatPumpInspectorView, qt_meta_tag_ZN10openstudio32WaterHeaterHeatPumpInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::WaterHeaterHeatPumpInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio32WaterHeaterHeatPumpInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio32WaterHeaterHeatPumpInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio32WaterHeaterHeatPumpInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::WaterHeaterHeatPumpInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<WaterHeaterHeatPumpInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (WaterHeaterHeatPumpInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &WaterHeaterHeatPumpInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (WaterHeaterHeatPumpInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &WaterHeaterHeatPumpInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::WaterHeaterHeatPumpInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::WaterHeaterHeatPumpInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio32WaterHeaterHeatPumpInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::WaterHeaterHeatPumpInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::WaterHeaterHeatPumpInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::WaterHeaterHeatPumpInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio36ZoneHVACFourPipeFanCoilInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACFourPipeFanCoilInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio36ZoneHVACFourPipeFanCoilInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACFourPipeFanCoilInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACFourPipeFanCoilInspectorView, qt_meta_tag_ZN10openstudio36ZoneHVACFourPipeFanCoilInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACFourPipeFanCoilInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio36ZoneHVACFourPipeFanCoilInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio36ZoneHVACFourPipeFanCoilInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio36ZoneHVACFourPipeFanCoilInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACFourPipeFanCoilInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACFourPipeFanCoilInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACFourPipeFanCoilInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACFourPipeFanCoilInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACFourPipeFanCoilInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACFourPipeFanCoilInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACFourPipeFanCoilInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACFourPipeFanCoilInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio36ZoneHVACFourPipeFanCoilInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACFourPipeFanCoilInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACFourPipeFanCoilInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACFourPipeFanCoilInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio44ZoneHVACLowTempRadiantConstFlowInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACLowTempRadiantConstFlowInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio44ZoneHVACLowTempRadiantConstFlowInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACLowTempRadiantConstFlowInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACLowTempRadiantConstFlowInspectorView, qt_meta_tag_ZN10openstudio44ZoneHVACLowTempRadiantConstFlowInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACLowTempRadiantConstFlowInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio44ZoneHVACLowTempRadiantConstFlowInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio44ZoneHVACLowTempRadiantConstFlowInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio44ZoneHVACLowTempRadiantConstFlowInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACLowTempRadiantConstFlowInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACLowTempRadiantConstFlowInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACLowTempRadiantConstFlowInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACLowTempRadiantConstFlowInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACLowTempRadiantConstFlowInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACLowTempRadiantConstFlowInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACLowTempRadiantConstFlowInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACLowTempRadiantConstFlowInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio44ZoneHVACLowTempRadiantConstFlowInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACLowTempRadiantConstFlowInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACLowTempRadiantConstFlowInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACLowTempRadiantConstFlowInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio42ZoneHVACLowTempRadiantVarFlowInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACLowTempRadiantVarFlowInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio42ZoneHVACLowTempRadiantVarFlowInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACLowTempRadiantVarFlowInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACLowTempRadiantVarFlowInspectorView, qt_meta_tag_ZN10openstudio42ZoneHVACLowTempRadiantVarFlowInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACLowTempRadiantVarFlowInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio42ZoneHVACLowTempRadiantVarFlowInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio42ZoneHVACLowTempRadiantVarFlowInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio42ZoneHVACLowTempRadiantVarFlowInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACLowTempRadiantVarFlowInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACLowTempRadiantVarFlowInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACLowTempRadiantVarFlowInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACLowTempRadiantVarFlowInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACLowTempRadiantVarFlowInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACLowTempRadiantVarFlowInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACLowTempRadiantVarFlowInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACLowTempRadiantVarFlowInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio42ZoneHVACLowTempRadiantVarFlowInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACLowTempRadiantVarFlowInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACLowTempRadiantVarFlowInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACLowTempRadiantVarFlowInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio39ZoneHVACWaterToAirHeatPumpInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACWaterToAirHeatPumpInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio39ZoneHVACWaterToAirHeatPumpInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACWaterToAirHeatPumpInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACWaterToAirHeatPumpInspectorView, qt_meta_tag_ZN10openstudio39ZoneHVACWaterToAirHeatPumpInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACWaterToAirHeatPumpInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio39ZoneHVACWaterToAirHeatPumpInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio39ZoneHVACWaterToAirHeatPumpInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio39ZoneHVACWaterToAirHeatPumpInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACWaterToAirHeatPumpInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACWaterToAirHeatPumpInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACWaterToAirHeatPumpInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACWaterToAirHeatPumpInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACWaterToAirHeatPumpInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACWaterToAirHeatPumpInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACWaterToAirHeatPumpInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACWaterToAirHeatPumpInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio39ZoneHVACWaterToAirHeatPumpInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACWaterToAirHeatPumpInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACWaterToAirHeatPumpInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACWaterToAirHeatPumpInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio45ZoneHVACBaseboardConvectiveWaterInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACBaseboardConvectiveWaterInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio45ZoneHVACBaseboardConvectiveWaterInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACBaseboardConvectiveWaterInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACBaseboardConvectiveWaterInspectorView, qt_meta_tag_ZN10openstudio45ZoneHVACBaseboardConvectiveWaterInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACBaseboardConvectiveWaterInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio45ZoneHVACBaseboardConvectiveWaterInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio45ZoneHVACBaseboardConvectiveWaterInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio45ZoneHVACBaseboardConvectiveWaterInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACBaseboardConvectiveWaterInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACBaseboardConvectiveWaterInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACBaseboardConvectiveWaterInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACBaseboardConvectiveWaterInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACBaseboardConvectiveWaterInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACBaseboardConvectiveWaterInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACBaseboardConvectiveWaterInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACBaseboardConvectiveWaterInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio45ZoneHVACBaseboardConvectiveWaterInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACBaseboardConvectiveWaterInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACBaseboardConvectiveWaterInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACBaseboardConvectiveWaterInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio52ZoneHVACBaseboardRadiantConvectiveWaterInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio52ZoneHVACBaseboardRadiantConvectiveWaterInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACBaseboardRadiantConvectiveWaterInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACBaseboardRadiantConvectiveWaterInspectorView, qt_meta_tag_ZN10openstudio52ZoneHVACBaseboardRadiantConvectiveWaterInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio52ZoneHVACBaseboardRadiantConvectiveWaterInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio52ZoneHVACBaseboardRadiantConvectiveWaterInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio52ZoneHVACBaseboardRadiantConvectiveWaterInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACBaseboardRadiantConvectiveWaterInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio52ZoneHVACBaseboardRadiantConvectiveWaterInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACBaseboardRadiantConvectiveWaterInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio55ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio55ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorVie"
        "w",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView, qt_meta_tag_ZN10openstudio55ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio55ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio55ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio55ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio55ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACCoolingPanelRadiantConvectiveWaterInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio31ZoneHVACUnitHeaterInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACUnitHeaterInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio31ZoneHVACUnitHeaterInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACUnitHeaterInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACUnitHeaterInspectorView, qt_meta_tag_ZN10openstudio31ZoneHVACUnitHeaterInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACUnitHeaterInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio31ZoneHVACUnitHeaterInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio31ZoneHVACUnitHeaterInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio31ZoneHVACUnitHeaterInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACUnitHeaterInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACUnitHeaterInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACUnitHeaterInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACUnitHeaterInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACUnitHeaterInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACUnitHeaterInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACUnitHeaterInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACUnitHeaterInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio31ZoneHVACUnitHeaterInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACUnitHeaterInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACUnitHeaterInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACUnitHeaterInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio35ZoneHVACUnitVentilatorInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ZoneHVACUnitVentilatorInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio35ZoneHVACUnitVentilatorInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ZoneHVACUnitVentilatorInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ZoneHVACUnitVentilatorInspectorView, qt_meta_tag_ZN10openstudio35ZoneHVACUnitVentilatorInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ZoneHVACUnitVentilatorInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio35ZoneHVACUnitVentilatorInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio35ZoneHVACUnitVentilatorInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio35ZoneHVACUnitVentilatorInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ZoneHVACUnitVentilatorInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ZoneHVACUnitVentilatorInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACUnitVentilatorInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACUnitVentilatorInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ZoneHVACUnitVentilatorInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &ZoneHVACUnitVentilatorInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::ZoneHVACUnitVentilatorInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ZoneHVACUnitVentilatorInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio35ZoneHVACUnitVentilatorInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ZoneHVACUnitVentilatorInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::ZoneHVACUnitVentilatorInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::ZoneHVACUnitVentilatorInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio37AirLoopHVACUnitarySystemInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::AirLoopHVACUnitarySystemInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio37AirLoopHVACUnitarySystemInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::AirLoopHVACUnitarySystemInspectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AirLoopHVACUnitarySystemInspectorView, qt_meta_tag_ZN10openstudio37AirLoopHVACUnitarySystemInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::AirLoopHVACUnitarySystemInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio37AirLoopHVACUnitarySystemInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio37AirLoopHVACUnitarySystemInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio37AirLoopHVACUnitarySystemInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::AirLoopHVACUnitarySystemInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AirLoopHVACUnitarySystemInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AirLoopHVACUnitarySystemInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &AirLoopHVACUnitarySystemInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AirLoopHVACUnitarySystemInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &AirLoopHVACUnitarySystemInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::AirLoopHVACUnitarySystemInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::AirLoopHVACUnitarySystemInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio37AirLoopHVACUnitarySystemInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::AirLoopHVACUnitarySystemInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::AirLoopHVACUnitarySystemInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::AirLoopHVACUnitarySystemInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio65AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio65AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::AirTerminalSingleDuctConstantVolumeFourPipeInductionIn"
        "spectorView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView, qt_meta_tag_ZN10openstudio65AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio65AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio65AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio65AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio65AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::AirTerminalSingleDuctConstantVolumeFourPipeInductionInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio60AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio60AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::AirTerminalSingleDuctConstantVolumeFourPipeBeamInspect"
        "orView",
        "addToLoopClicked",
        "",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoopClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'addToLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
        // Signal 'removeFromLoopClicked'
        QtMocHelpers::SignalData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 2 }, { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView, qt_meta_tag_ZN10openstudio60AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio60AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio60AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio60AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->addToLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 1: _t->removeFromLoopClicked((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::addToLoopClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::*)(model::Loop & , boost::optional<model::HVACComponent> & )>(_a, &AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::removeFromLoopClicked, 1))
            return;
    }
}

const QMetaObject *openstudio::AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio60AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void openstudio::AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::addToLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void openstudio::AirTerminalSingleDuctConstantVolumeFourPipeBeamInspectorView::removeFromLoopClicked(model::Loop & _t1, boost::optional<model::HVACComponent> & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}
namespace {
struct qt_meta_tag_ZN10openstudio28ScheduleRulesetInspectorViewE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::ScheduleRulesetInspectorView::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio28ScheduleRulesetInspectorViewE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::ScheduleRulesetInspectorView"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ScheduleRulesetInspectorView, qt_meta_tag_ZN10openstudio28ScheduleRulesetInspectorViewE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::ScheduleRulesetInspectorView::staticMetaObject = { {
    QMetaObject::SuperData::link<BaseInspectorView::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio28ScheduleRulesetInspectorViewE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio28ScheduleRulesetInspectorViewE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio28ScheduleRulesetInspectorViewE_t>.metaTypes,
    nullptr
} };

void openstudio::ScheduleRulesetInspectorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ScheduleRulesetInspectorView *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *openstudio::ScheduleRulesetInspectorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::ScheduleRulesetInspectorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio28ScheduleRulesetInspectorViewE_t>.strings))
        return static_cast<void*>(this);
    return BaseInspectorView::qt_metacast(_clname);
}

int openstudio::ScheduleRulesetInspectorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BaseInspectorView::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
