target_sources(AnemoneRender
    PRIVATE
        "Device.cxx"
        "Module.cxx"
        "SwapChain.cxx"
        "Gpu.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Device.hxx"
        "SwapChain.hxx"
        "Gpu.hxx"
)
