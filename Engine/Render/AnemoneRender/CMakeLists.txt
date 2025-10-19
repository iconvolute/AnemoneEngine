target_sources(AnemoneRender
    PRIVATE
        "Device.cxx"
        "Module.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Device.hxx"
        "Module.hxx"
)
