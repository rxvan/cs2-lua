local modules = require('modules')
local schema_system = modules.schema_system

interfaces = {}

local function get_interfaces(mod_name, mod_entry)
    if not interfaces[mod_name] then
        interfaces[mod_name] = {}
    end

    local module_create_interface = utl.get_module_export(mod_entry, 'CreateInterface')
    if not module_create_interface then
        error('CreateInterface not found in ' .. mod_name)
    end

    ffi.cdef([[
        typedef unsigned long long(__cdecl* factory_fn)();
        struct interface_entry_t {
            factory_fn factory;
            const char* name;
            unsigned long long next;
        };
    ]])

    module_create_interface = module_create_interface + 0x3
    local interface_entry_addr = ffi.cast('unsigned long long**', module_create_interface + 0x4 + ffi.cast('int*', module_create_interface)[0])
    local interface_entry = ffi.cast('struct interface_entry_t**', interface_entry_addr)[0]

    repeat
        local interface_name = ffi.string(interface_entry.name)
        local factory = interface_entry.factory
        interfaces[mod_name][interface_name] = ffi.cast('factory_fn', factory)()
        interface_entry = ffi.cast('struct interface_entry_t*', interface_entry.next)
    until interface_entry == nil
end

get_interfaces('schemasystem.dll', schema_system)

return interfaces
