
var driver = require('../build/Debug/ble_driver_js');

var evt_count = 0;
var connectionHandle = 0;
var interval;
var valueHandle = 0;
var characteristicHandle = 0;
var addedVSUUIDType = -1;

driver.open(
    'COM19',
    {
        'baudRate': 115200,
        'parity': 'none',
        'flowControl': 'none',
        'eventInterval': 200,
        'logCallback': function(severity, message) {
            if (severity > 0) {
                console.log("log: " + severity + ", " + message);
            }
        },
        'eventCallback': onBleEvent,
    },
    function(err) {
        if(err) {
            console.log('Error occurred opening serial port: %d', err);
            return;
        }

        addVsUuid();
    }
);

function onBleEvent(event_array) {
    console.log("event_array length: " + event_array.length)

    for (var i = 0; i < event_array.length; i++)
    {
        event = event_array[i];
        evt_count = evt_count + 1;
        console.log("evt #" +  evt_count  + ", id: " + event.id + ", name: " + event.name);
        console.log("time:" + event.time);
        //console.log("JSON: %s", JSON.stringify(event));

        if(event.name === 'BLE_GAP_EVT_ADV_REPORT') {
            console.log("ADDRESS: %s", event.peer_addr.address);
            console.log("RSSI: %s", event.rssi);
        }
        else if (event.name === 'BLE_GAP_EVT_TIMEOUT') {
            console.log("Timeout source: %s", event.src);
        }
        else if (event.name === 'BLE_GAP_EVT_CONNECTED')
        {
            connectionHandle = event.conn_handle;
            console.log("Connected. Handle: %d", connectionHandle);
            connSecGet();
        }
        else if (event.name === 'BLE_GATTS_EVT_SYS_ATTR_MISSING')
        {
            driver.gatts_set_system_attribute(connectionHandle, 0, 0, 0, function(err) {
                if (err)
                {
                    console.log('Failed setting system attributes');
                    console.log(err);
                }
            });
        }
        else if (event.name === 'BLE_GATTS_EVT_WRITE')
        {
            if (event.context.char_uuid.uuid == 0x2A37)
            {
                var write_data = event.data[0];
                if (write_data === driver.BLE_GATT_HVX_NOTIFICATION)
                {
                    heartRate = 10;
                    interval = setInterval(function () {
                        heartRate += 5;
                        hvxParams = {
                            'handle': valueHandle,
                            'type': driver.BLE_GATT_HVX_NOTIFICATION,
                            'offset': 0,
                            'p_len': [1],
                            'p_data': [heartRate, 0],
                        };
                        driver.gatts_hvx(connectionHandle, hvxParams, function(err, hvx_length) {
                            if (err) {
                                console.log("HVX error");
                                console.log(err);
                            }
                        })
                    }, 1000);
                }
                else
                {
                    clearInterval(interval);
                }
            }
        }
        else if (event.name === 'BLE_GAP_EVT_SEC_PARAMS_REQUEST')
        {
            console.log("GapSecParamsRequest: " + JSON.stringify(event));
            setTimeout(secParamsReply, 1000);
        }
    }
}

function addVsUuid() {
    driver.add_vs_uuid({'uuid128': '11220000-3344-5566-7788-99aabbccddee'},
        function(err, type) {
            if (err)
            {
                console.log('Error occured when adding 128-bit UUID');
                console.log(err);
                return;
            }

        console.log('Added 128-bit UUID with type %d', type);
        addedVSUUIDType = type;

        encodeUUID();
        decodeUUID();

        addService();
    });
}

function encodeUUID() {
    driver.encode_uuid({'uuid': 0x2A37, 'type': addedVSUUIDType}, function(err, len, uuid, uuidString) {
        if (err)
        {
            console.log('Error occured when encoding UUID');
            console.log(err);
            return;
        }

        console.log('Encoded uuid. Result: Length: %d Full UUID: %s UUIDString: %s', len, JSON.stringify(uuid), uuidString)
    });
}

function decodeUUID() {
    driver.decode_uuid(16, '11222A3733445566778899AABBCCDDEE', function(err, uuid) {
        if (err)
        {
            console.log('Error occured when decoding UUID');
            console.log(err);
            return;
        }

        console.log('Decoded uuid. Result: %s', JSON.stringify(uuid))
    });
}

function addService() {
    driver.gatts_add_service(1, {'uuid': 0x180D, 'type': driver.BLE_UUID_TYPE_BLE},
        function(err, handle) {
            if (err) {
                console.log('Error occured when adding service');
                console.log(err);
                return;
            }

            console.log('Added service with handle %d', handle);

            addCharacteristic(handle);
        }
    );
}

function addCharacteristic(handle) {
    driver.gatts_add_characteristic(handle,
        {
            'char_props':
            {
                'broadcast': false,
                'read': true,
                'write_wo_resp': false,
                'write': false,
                'notify': true,
                'indicate': false,
                'auth_signed_wr': false
            },
            'char_ext_props': {'reliable_wr': false, 'wr_aux': false},
            'char_user_desc_max_size': 0,
            'char_user_desc_size': 0,
            'p_char_pf': 0, // Presentation format (ble_gatts_char_pf_t) May be 0
            'p_user_desc_md': 0, // User Description descriptor (ble_gatts_attr_md_t) May be 0
            'p_cccd_md': // Client Characteristic Configuration Descriptor (ble_gatts_attr_md_t) May be 0
            {
                'read_perm': {'sm': 1, 'lv': 1},
                'write_perm': {'sm': 1, 'lv': 1},
                'vlen': 0,
                'vloc': driver.BLE_GATTS_VLOC_STACK,
                'rd_auth': 0,
                'wr_auth': 0,
            },
            'p_sccd_md': 0, // Server Characteristic Configuration Descriptor (ble_gatts_attr_md_t) May be 0
        },
        {
            'p_uuid': {'uuid': 0x2A37, 'type': addedVSUUIDType},
            'p_attr_md': {
                'read_perm': {'sm': 1, 'lv': 1},
                'write_perm': {'sm': 1, 'lv': 1},
                'vlen': 0,
                'vloc': 1,
                'rd_auth': 0,
                'wr_auth': 0,
            },
            'init_len': 1,
            'init_offs': 0,
            'max_len': 1,
            'p_value': [43],
        },
        function(err, handles) {
            if (err) {
                console.log('Error occured when adding characteristics');
                console.log(err);
                return;
            }

            console.log('Added characteristics with handles %s', JSON.stringify(handles));
            valueHandle = handles.value_handle;
            characteristicHandle = valueHandle - 1;

            addDescriptor();
    });

}

function addDescriptor() {
    driver.gatts_add_descriptor(valueHandle,
        {
            'p_uuid': {'uuid': 0x2A38, 'type': 2},
            'p_attr_md': {
                'read_perm': {'sm': 1, 'lv': 1},
                'write_perm': {'sm': 1, 'lv': 1},
                'vlen': 0,
                'vloc': 1,
                'rd_auth': 0,
                'wr_auth': 0,
            },
            'init_len': 1,
            'init_offs': 0,
            'max_len': 1,
            'p_value': [43],
        },
        function(err, handle) {
            if (err) {
                console.log('Error occured when adding descriptor.');
                console.log(err);
                return;
            }

            console.log('Added descriptor with handle %d', handle)

            startAdvertising();
    });
}

function startAdvertising() {
    driver.gap_start_advertising({
            'type': driver.BLE_GAP_ADV_TYPE_ADV_IND,
            'fp': driver.BLE_GAP_ADV_FP_ANY,
            'interval': 40,
            'timeout': 180,
            'channel_mask': {
                'ch_37_off': 0,
                'ch_38_off': 0,
                'ch_39_off': 0,
            },
        },
        function(err) {
            if(err) {
                console.log('Error occured when starting advertisement');
                console.log(err);
                return;
            }

            console.log('Started advertising');

            //setTimeout(stopAdvertising, 60000);
        }
    );
}

function connSecGet() {
    /* gap_conn_sec_get(connHandle, callback)
     * signature of callback: function(err, connSec)
     * 
     * connSec: {
     *       'sec_mode': {
     *           'sm': <1 to 2>,
     *           'lv': <1 to 3>,
     *       },
     *       'encr_key_size': <7 to 16>,
     *   }
     *
     * http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.s130.api.v1.0.0/group___b_l_e___g_a_p___f_u_n_c_t_i_o_n_s.html?cp=2_7_2_1_0_2_1_4_10#ga05ed7aaeb2d1f1ff91019a1ffeaf9fc0
     */
    driver.gap_conn_sec_get(connectionHandle, function(err, connSec) {
        if (err) {
            console.log('Error occured in gap_conn_sec_get');
            return;
        }
        console.log('GapConnSecGet: ' + JSON.stringify(connSec));
    });
}

function stopAdvertising() {
    driver.gap_stop_advertising(function(err) {
        if (err) {
            console.log('Error occured when stopping advertising');
        }

        console.log('Stopped advertising');
    });
}

function secParamsReply() {
    driver.gap_sec_params_reply(
        connectionHandle,
        0, //sec_status
        { //sec_params
            'bond': true,
            'mitm': false,
            'io_caps': driver.BLE_GAP_IO_CAPS_NONE,
            'oob': false,
            'min_key_size': 7,
            'max_key_size': 16,
            'kdist_periph': {
                'enc': true,
                'id': false,
                'sign': false
            },
            'kdist_central': {
                'enc': true,
                'id': false,
                'sign': false
            }
        },
        null, //sec_keyset
        function(err) {
            if (err) {
                console.log('Error occured in gap_sec_params_reply');
            }

            console.log('gap_sec_params_reply completed');
        }
    );
}

