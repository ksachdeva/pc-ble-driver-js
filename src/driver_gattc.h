/* Copyright (c) 2016 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#ifndef DRIVER_GATTC_H
#define DRIVER_GATTC_H

#include "common.h"
#include "ble_gattc.h"

extern name_map_t gatt_status_map;

static name_map_t gattc_event_name_map = {
    NAME_MAP_ENTRY(BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP),
    NAME_MAP_ENTRY(BLE_GATTC_EVT_REL_DISC_RSP),
    NAME_MAP_ENTRY(BLE_GATTC_EVT_CHAR_DISC_RSP),
    NAME_MAP_ENTRY(BLE_GATTC_EVT_DESC_DISC_RSP),
    NAME_MAP_ENTRY(BLE_GATTC_EVT_ATTR_INFO_DISC_RSP),
    NAME_MAP_ENTRY(BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP),
    NAME_MAP_ENTRY(BLE_GATTC_EVT_READ_RSP),
    NAME_MAP_ENTRY(BLE_GATTC_EVT_CHAR_VALS_READ_RSP),
    NAME_MAP_ENTRY(BLE_GATTC_EVT_WRITE_RSP),
    NAME_MAP_ENTRY(BLE_GATTC_EVT_HVX),
    NAME_MAP_ENTRY(BLE_GATTC_EVT_TIMEOUT)
};

class GattcHandleRange : public BleToJs<ble_gattc_handle_range_t>
{
public:
    GattcHandleRange(ble_gattc_handle_range_t *service) : BleToJs<ble_gattc_handle_range_t>(service) {}
    GattcHandleRange(v8::Local<v8::Object> js) : BleToJs<ble_gattc_handle_range_t>(js) {}
    v8::Local<v8::Object> ToJs();
    ble_gattc_handle_range_t *ToNative();
};

class GattcService : public BleToJs<ble_gattc_service_t>
{
public:
    GattcService(ble_gattc_service_t *service) : BleToJs<ble_gattc_service_t>(service) {}
    GattcService(v8::Local<v8::Object> js) : BleToJs<ble_gattc_service_t>(js) {}
    v8::Local<v8::Object> ToJs();
};

class GattcIncludedService : public BleToJs<ble_gattc_include_t>
{
public:
    GattcIncludedService(ble_gattc_include_t *gattinclude) : BleToJs<ble_gattc_include_t>(gattinclude) {}
    GattcIncludedService(v8::Local<v8::Object> js) : BleToJs<ble_gattc_include_t>(js) {}
    v8::Local<v8::Object> ToJs();
};

class GattcCharacteristic : public BleToJs<ble_gattc_char_t>
{
public:
    GattcCharacteristic(ble_gattc_char_t *characteristic) : BleToJs<ble_gattc_char_t>(characteristic) {}
    GattcCharacteristic(v8::Local<v8::Object> js) : BleToJs<ble_gattc_char_t>(js) {}
    v8::Local<v8::Object> ToJs();
};

class GattcDescriptor : public BleToJs<ble_gattc_desc_t>
{
public:
    GattcDescriptor(ble_gattc_desc_t *descriptor) : BleToJs<ble_gattc_desc_t>(descriptor) {}
    GattcDescriptor(v8::Local<v8::Object> js) : BleToJs<ble_gattc_desc_t>(js) {}
    v8::Local<v8::Object> ToJs();
};

class GattcWriteParameters : public BleToJs<ble_gattc_write_params_t>
{
public:
    GattcWriteParameters(ble_gattc_write_params_t *writeparameters) : BleToJs<ble_gattc_write_params_t>(writeparameters) {}
    GattcWriteParameters(v8::Local<v8::Object> js) : BleToJs<ble_gattc_write_params_t>(js) {}
    ble_gattc_write_params_t *ToNative();
    v8::Local<v8::Object> ToJs();
};

template<typename EventType>
class BleDriverGattcEvent : public BleDriverEvent<EventType>
{
private:
    BleDriverGattcEvent() {}

    uint16_t gatt_status;
    uint16_t error_handle;

public:
    BleDriverGattcEvent(uint16_t evt_id, std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, EventType *evt)
        : BleDriverEvent<EventType>(evt_id, timestamp, conn_handle, evt),
        gatt_status(gatt_status),
        error_handle(error_handle)
    {
    }

    virtual void ToJs(v8::Local<v8::Object> obj)
    {
        BleDriverEvent<EventType>::ToJs(obj);
        Utility::Set(obj, "gatt_status", gatt_status);
        Utility::Set(obj, "gatt_status_name", ConversionUtility::valueToJsString(gatt_status, gatt_status_map, ConversionUtility::toJsString("Unknown GATT status")));
        Utility::Set(obj, "error_handle", error_handle);
    }

    virtual v8::Local<v8::Object> ToJs() = 0;
    virtual EventType *ToNative() { return new EventType(); }

    const char *getEventName() { return ConversionUtility::valueToString(this->evt_id, gattc_event_name_map, "Unknown Gattc Event"); }
};

class GattcPrimaryServiceDiscoveryEvent : BleDriverGattcEvent<ble_gattc_evt_prim_srvc_disc_rsp_t>
{
public:
    GattcPrimaryServiceDiscoveryEvent(std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, ble_gattc_evt_prim_srvc_disc_rsp_t *evt)
        : BleDriverGattcEvent<ble_gattc_evt_prim_srvc_disc_rsp_t>(BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP, timestamp, conn_handle, gatt_status, error_handle, evt) {}

    v8::Local<v8::Object> ToJs();
};

class GattcRelationshipDiscoveryEvent : BleDriverGattcEvent < ble_gattc_evt_rel_disc_rsp_t >
{
public:
    GattcRelationshipDiscoveryEvent(std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, ble_gattc_evt_rel_disc_rsp_t *evt)
        : BleDriverGattcEvent<ble_gattc_evt_rel_disc_rsp_t>(BLE_GATTC_EVT_REL_DISC_RSP, timestamp, conn_handle, gatt_status, error_handle, evt) {}

    v8::Local<v8::Object> ToJs();
};

class GattcCharacteristicDiscoveryEvent : BleDriverGattcEvent < ble_gattc_evt_char_disc_rsp_t >
{
public:
    GattcCharacteristicDiscoveryEvent(std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, ble_gattc_evt_char_disc_rsp_t *evt)
        : BleDriverGattcEvent<ble_gattc_evt_char_disc_rsp_t>(BLE_GATTC_EVT_CHAR_DISC_RSP, timestamp, conn_handle, gatt_status, error_handle, evt) {}

    v8::Local<v8::Object> ToJs();
};

class GattcDescriptorDiscoveryEvent : BleDriverGattcEvent < ble_gattc_evt_desc_disc_rsp_t >
{
public:
    GattcDescriptorDiscoveryEvent(std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, ble_gattc_evt_desc_disc_rsp_t *evt)
        : BleDriverGattcEvent<ble_gattc_evt_desc_disc_rsp_t>(BLE_GATTC_EVT_DESC_DISC_RSP, timestamp, conn_handle, gatt_status, error_handle, evt) {}

    v8::Local<v8::Object> ToJs();
};

class GattcHandleValue : public BleToJs<ble_gattc_handle_value_t>
{
private:
    uint16_t valueLength;
public:
    GattcHandleValue(ble_gattc_handle_value_t *handleValue, uint16_t valueLength) : BleToJs<ble_gattc_handle_value_t>(handleValue), valueLength(valueLength) {}
    GattcHandleValue(v8::Local<v8::Object> js) : BleToJs<ble_gattc_handle_value_t>(js) {}
    v8::Local<v8::Object> ToJs();
};

class GattcCharacteristicValueReadByUUIDEvent : BleDriverGattcEvent < ble_gattc_evt_char_val_by_uuid_read_rsp_t >
{
public:
    GattcCharacteristicValueReadByUUIDEvent(std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, ble_gattc_evt_char_val_by_uuid_read_rsp_t *evt)
        : BleDriverGattcEvent<ble_gattc_evt_char_val_by_uuid_read_rsp_t>(BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP, timestamp, conn_handle, gatt_status, error_handle, evt) {}

    v8::Local<v8::Object> ToJs();
};

class GattcReadEvent : BleDriverGattcEvent < ble_gattc_evt_read_rsp_t >
{
public:
    GattcReadEvent(std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, ble_gattc_evt_read_rsp_t *evt)
        : BleDriverGattcEvent<ble_gattc_evt_read_rsp_t>(BLE_GATTC_EVT_READ_RSP, timestamp, conn_handle, gatt_status, error_handle, evt) {}

    v8::Local<v8::Object> ToJs();
};

class GattcCharacteristicValueReadEvent : BleDriverGattcEvent < ble_gattc_evt_char_vals_read_rsp_t >
{
public:
    GattcCharacteristicValueReadEvent(std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, ble_gattc_evt_char_vals_read_rsp_t *evt)
        : BleDriverGattcEvent<ble_gattc_evt_char_vals_read_rsp_t>(BLE_GATTC_EVT_CHAR_VALS_READ_RSP, timestamp, conn_handle, gatt_status, error_handle, evt) {}

    v8::Local<v8::Object> ToJs();
};

class GattcWriteEvent : BleDriverGattcEvent < ble_gattc_evt_write_rsp_t >
{
public:
    GattcWriteEvent(std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, ble_gattc_evt_write_rsp_t *evt)
        : BleDriverGattcEvent<ble_gattc_evt_write_rsp_t>(BLE_GATTC_EVT_WRITE_RSP, timestamp, conn_handle, gatt_status, error_handle, evt) {}

    v8::Local<v8::Object> ToJs();
};

class GattcHandleValueNotificationEvent : BleDriverGattcEvent < ble_gattc_evt_hvx_t >
{
public:
    GattcHandleValueNotificationEvent(std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, ble_gattc_evt_hvx_t *evt)
        : BleDriverGattcEvent<ble_gattc_evt_hvx_t>(BLE_GATTC_EVT_HVX, timestamp, conn_handle, gatt_status, error_handle, evt) {}

    v8::Local<v8::Object> ToJs();
};

class GattcTimeoutEvent : BleDriverGattcEvent < ble_gattc_evt_timeout_t >
{
public:
    GattcTimeoutEvent(std::string timestamp, uint16_t conn_handle, uint16_t gatt_status, uint16_t error_handle, ble_gattc_evt_timeout_t *evt)
        : BleDriverGattcEvent<ble_gattc_evt_timeout_t>(BLE_GATTC_EVT_TIMEOUT, timestamp, conn_handle, gatt_status, error_handle, evt) {}

    v8::Local<v8::Object> ToJs();
};

///// Start GATTC Batons //////////////////////////////////////////////////////////////////////////////////

struct GattcDiscoverPrimaryServicesBaton : public Baton {
public:
    BATON_CONSTRUCTOR(GattcDiscoverPrimaryServicesBaton);
    uint16_t conn_handle;
    uint16_t start_handle;
    ble_uuid_t *p_srvc_uuid;
};

struct GattcDiscoverRelationshipBaton : public Baton {
public:
    BATON_CONSTRUCTOR(GattcDiscoverRelationshipBaton);
    uint16_t conn_handle;
    ble_gattc_handle_range_t *p_handle_range;
};

struct GattcDiscoverCharacteristicsBaton : public Baton {
public:
    BATON_CONSTRUCTOR(GattcDiscoverCharacteristicsBaton);
    uint16_t conn_handle;
    ble_gattc_handle_range_t *p_handle_range;
};

struct GattcDiscoverDescriptorsBaton : public Baton {
public:
    BATON_CONSTRUCTOR(GattcDiscoverDescriptorsBaton);
    uint16_t conn_handle;
    ble_gattc_handle_range_t *p_handle_range;
};

struct GattcCharacteristicByUUIDReadBaton : public Baton {
public:
    BATON_CONSTRUCTOR(GattcCharacteristicByUUIDReadBaton);
    uint16_t conn_handle;
    ble_uuid_t *p_uuid;
    ble_gattc_handle_range_t *p_handle_range;
};

struct GattcReadBaton : public Baton {
public:
    BATON_CONSTRUCTOR(GattcReadBaton);
    uint16_t conn_handle;
    uint16_t handle;
    uint16_t offset;
};

struct GattcReadCharacteristicValuesBaton : public Baton {
public:
    BATON_CONSTRUCTOR(GattcReadCharacteristicValuesBaton);
    uint16_t conn_handle;
    uint16_t *p_handles;
    uint16_t handle_count;
};

struct GattcWriteBaton : public Baton {
public:
    BATON_CONSTRUCTOR(GattcWriteBaton);
    uint16_t conn_handle;
    ble_gattc_write_params_t *p_write_params;
};

struct GattcConfirmHandleValueBaton : public Baton {
public:
    BATON_CONSTRUCTOR(GattcConfirmHandleValueBaton);
    uint16_t conn_handle;
    uint16_t handle;
};

///// End GATTC Batons //////////////////////////////////////////////////////////////////////////////////

extern "C" {
    void init_gattc(Nan::ADDON_REGISTER_FUNCTION_ARGS_TYPE target);
}

#endif
