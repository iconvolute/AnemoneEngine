target_sources(AnemoneRuntime.VM
    PRIVATE
        "CodeGenerator.cxx"
        "Label.cxx"
        "OpCode.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "CodeGenerator.hxx"
        "Label.hxx"
        "OpCode.hxx"
)
