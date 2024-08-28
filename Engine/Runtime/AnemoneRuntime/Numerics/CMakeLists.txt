target_sources(AnemoneRuntime
    PRIVATE
        "Easing.cxx"
        "Easing.hxx"
        "Fixed.hxx"
        "Numbers.hxx"
        "Random.cxx"
        "Random.hxx"
        "Simd.cxx"
        "Simd.hxx"
        "Types.hxx"
        "Vector.cxx"
        "Vector.hxx"
)

add_subdirectory("Private")
