local schema_system = require('schema_system')
local ffi = require('ffi')
local string = require("string")

if schema_system.get_type_scope_for_module then
    local client_type_scope = schema_system.get_type_scope_for_module('client.dll')
    if not client_type_scope then
        error('client_type_scope not found')
    end

    print(string.format('client_type_scope: 0x%X', tonumber(ffi.cast('unsigned long long', client_type_scope))))
end
