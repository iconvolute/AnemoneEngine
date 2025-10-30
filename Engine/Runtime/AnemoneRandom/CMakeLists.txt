target_sources(AnemoneRuntime
    PRIVATE
        "Distribution.cxx"
        "Generator.cxx"
        "Utils.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "Generator.hxx"
        "Distribution.hxx"
        "Utils.hxx"
)
