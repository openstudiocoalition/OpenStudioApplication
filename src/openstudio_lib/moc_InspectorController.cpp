/****************************************************************************
** Meta object code from reading C++ file 'InspectorController.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "InspectorController.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'InspectorController.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10openstudio19InspectorControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto openstudio::InspectorController::qt_create_metaobjectdata<qt_meta_tag_ZN10openstudio19InspectorControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "openstudio::InspectorController",
        "toggleUnitsClicked",
        "",
        "displayIP",
        "toggleDisplayAdditionalPropsClicked",
        "displayAdditionalProps",
        "itemRemoveClicked",
        "OSItem*",
        "removeButtonClicked",
        "workspaceObjectRemoved",
        "layoutModelObject",
        "model::OptionalModelObject&",
        "readOnly",
        "addBranchForZone",
        "model::ThermalZone&",
        "removeBranchForZone",
        "moveBranchForZoneSupply",
        "zone",
        "Handle",
        "newPlenumHandle",
        "moveBranchForZoneReturn",
        "addToLoop",
        "model::Loop&",
        "boost::optional<model::HVACComponent>&",
        "removeFromLoop",
        "toggleUnits",
        "onViewDestroyed",
        "object"
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
        // Signal 'itemRemoveClicked'
        QtMocHelpers::SignalData<void(OSItem *)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 2 },
        }}),
        // Signal 'removeButtonClicked'
        QtMocHelpers::SignalData<void(bool)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 2 },
        }}),
        // Signal 'workspaceObjectRemoved'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'layoutModelObject'
        QtMocHelpers::SlotData<void(model::OptionalModelObject &, bool)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 2 }, { QMetaType::Bool, 12 },
        }}),
        // Slot 'addBranchForZone'
        QtMocHelpers::SlotData<void(model::ThermalZone &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 2 },
        }}),
        // Slot 'removeBranchForZone'
        QtMocHelpers::SlotData<void(model::ThermalZone &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 2 },
        }}),
        // Slot 'moveBranchForZoneSupply'
        QtMocHelpers::SlotData<void(model::ThermalZone &, const Handle &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 17 }, { 0x80000000 | 18, 19 },
        }}),
        // Slot 'moveBranchForZoneReturn'
        QtMocHelpers::SlotData<void(model::ThermalZone &, const Handle &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 17 }, { 0x80000000 | 18, 19 },
        }}),
        // Slot 'addToLoop'
        QtMocHelpers::SlotData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 22, 2 }, { 0x80000000 | 23, 2 },
        }}),
        // Slot 'removeFromLoop'
        QtMocHelpers::SlotData<void(model::Loop &, boost::optional<model::HVACComponent> &)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 22, 2 }, { 0x80000000 | 23, 2 },
        }}),
        // Slot 'toggleUnits'
        QtMocHelpers::SlotData<void(bool)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 3 },
        }}),
        // Slot 'onViewDestroyed'
        QtMocHelpers::SlotData<void(QObject *)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QObjectStar, 27 },
        }}),
        // Slot 'onViewDestroyed'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate | QMC::MethodCloned, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<InspectorController, qt_meta_tag_ZN10openstudio19InspectorControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject openstudio::InspectorController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio19InspectorControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio19InspectorControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10openstudio19InspectorControllerE_t>.metaTypes,
    nullptr
} };

void openstudio::InspectorController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<InspectorController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->toggleUnitsClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 1: _t->toggleDisplayAdditionalPropsClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 2: _t->itemRemoveClicked((*reinterpret_cast<std::add_pointer_t<OSItem*>>(_a[1]))); break;
        case 3: _t->removeButtonClicked((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 4: _t->workspaceObjectRemoved(); break;
        case 5: _t->layoutModelObject((*reinterpret_cast<std::add_pointer_t<model::OptionalModelObject&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2]))); break;
        case 6: _t->addBranchForZone((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1]))); break;
        case 7: _t->removeBranchForZone((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1]))); break;
        case 8: _t->moveBranchForZoneSupply((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Handle>>(_a[2]))); break;
        case 9: _t->moveBranchForZoneReturn((*reinterpret_cast<std::add_pointer_t<model::ThermalZone&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Handle>>(_a[2]))); break;
        case 10: _t->addToLoop((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 11: _t->removeFromLoop((*reinterpret_cast<std::add_pointer_t<model::Loop&>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<boost::optional<model::HVACComponent>&>>(_a[2]))); break;
        case 12: _t->toggleUnits((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 13: _t->onViewDestroyed((*reinterpret_cast<std::add_pointer_t<QObject*>>(_a[1]))); break;
        case 14: _t->onViewDestroyed(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (InspectorController::*)(bool )>(_a, &InspectorController::toggleUnitsClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorController::*)(bool )>(_a, &InspectorController::toggleDisplayAdditionalPropsClicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorController::*)(OSItem * )>(_a, &InspectorController::itemRemoveClicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorController::*)(bool )>(_a, &InspectorController::removeButtonClicked, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (InspectorController::*)()>(_a, &InspectorController::workspaceObjectRemoved, 4))
            return;
    }
}

const QMetaObject *openstudio::InspectorController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *openstudio::InspectorController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10openstudio19InspectorControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int openstudio::InspectorController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void openstudio::InspectorController::toggleUnitsClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void openstudio::InspectorController::toggleDisplayAdditionalPropsClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void openstudio::InspectorController::itemRemoveClicked(OSItem * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void openstudio::InspectorController::removeButtonClicked(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void openstudio::InspectorController::workspaceObjectRemoved()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
