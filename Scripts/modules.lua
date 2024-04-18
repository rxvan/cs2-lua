local modules = {
    schema_system = utl.get_module_entry('schemasystem.dll'),
}

return modules -- allow require('modules') to return the modules table
