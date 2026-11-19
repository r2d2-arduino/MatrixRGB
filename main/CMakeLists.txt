idf_component_register(
    SRCS "main.cpp" "hub75/hub75_dma.cpp" "hub75/i2s_parallel.cpp"
    INCLUDE_DIRS "." ".." "hub75"
    PRIV_REQUIRES driver esp_timer esp_hw_support
)
target_compile_definitions(${COMPONENT_LIB} PRIVATE GFX_FB_NO_BYTESWAP)
target_compile_options(${COMPONENT_LIB} PRIVATE -O2)