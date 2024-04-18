local ffi = require('ffi')

return {
    call_vfunc = (function(ptr, index, ...)
        local vtable = ffi.cast('unsigned long long**', ptr)[0]
        local fn = vtable[index]
        return fn(ptr, ...)
    end),
    get_vfunc = (function(ptr, index)
        local vtable = ffi.cast('unsigned long long**', ptr)[0]
        return vtable[index]
    end)
}
