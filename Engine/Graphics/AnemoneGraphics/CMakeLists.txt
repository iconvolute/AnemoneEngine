target_sources(AnemoneGraphics
    PRIVATE
        "Histogram.cxx"
        "Image.cxx"
        "ImageDDS.cxx"
        "PixelFormat.cxx"
    PUBLIC FILE_SET HEADERS FILES
        "DxgiFormat.hxx"
        "Histogram.hxx"
        "Image.hxx"
        "PixelFormat.hxx"
)
