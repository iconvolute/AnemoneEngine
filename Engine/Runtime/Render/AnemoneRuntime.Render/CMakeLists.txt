target_sources(AnemoneRuntime.Render
    PRIVATE
        "Device.cxx"
        "SwapChain.cxx"
        "Gpu.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Device.hxx"
        "SwapChain.hxx"
        "Gpu.hxx"
)
