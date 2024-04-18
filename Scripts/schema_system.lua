local ffi = require('ffi')
local interfaces = require('interfaces')
local vfunc = require('vfunc')

local function get_schema_system()
    local schema_system = interfaces['schemasystem.dll']
    if not schema_system then
        error('schema_system not found')
    end

    return schema_system['SchemaSystem_001']
end

-- find_declared_class_fn 3rd argument is schema_class_info_data_t*

ffi.cdef([[
    typedef void*(__thiscall* get_type_scope_for_module_fn)(void*, const char*);
    typedef void(__thiscall* find_declared_class_fn)(void*, void*, const char*);

    struct schema_class_field_data_t {
        const char* field_name;
        void* pSomeUNKData;
        int32_t field_offset;
        char pad[0xC];
    };

    struct schema_class_info_data_t {
        void* pSomeUNKData;
        const char* name;
        const char* description;
        int32_t size_of;
        uint8_t field_count;
        char pad[0x5];
        uint8_t alignment;
        uint8_t number_of_inherited_classes;
        char pad2[0x4];
        struct schema_class_field_data_t* pFields;
        char pad3[0x38]; // 0x8 + 0x8 + 0x28 = 0x38
    };
]])

local ptr = get_schema_system()
if not ptr then
    error('SchemaSystem_001 not found')
end

-- get index 13 from the vtable
schema_system = {
    get_type_scope_for_module = (function(mod_name)
        local fn = ffi.cast('get_type_scope_for_module_fn', vfunc.get_vfunc(ptr, 13))
        return fn(ffi.cast('void*', ptr), mod_name)
    end)
}

return schema_system
