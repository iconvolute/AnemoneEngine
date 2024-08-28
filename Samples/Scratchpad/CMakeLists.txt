anemone_add_executable(Scratchpad)
target_sources(Scratchpad
    PRIVATE
        "Main.cxx"
)
target_link_libraries(Scratchpad PUBLIC AnemoneRuntime)
