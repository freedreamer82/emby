# Enable assembler files preprocessing
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:-x$<SEMICOLON>assembler-with-cpp>)
# -mcmse to enable Cortex-M Security Extensions (CMSE) , -mfp16-format=ieee  for hal precision 16bit -mfpu=fpv5-d16
set(COMPILE_OPTS -mcpu=Cortex-M55 -mfpu=fpv5-d16 -mthumb -Wall   -mfloat-abi=hard -mcmse -flax-vector-conversions)
add_compile_options("${COMPILE_OPTS}")
add_link_options("${COMPILE_OPTS}")
set(CMAKE_EXE_LINKER_FLAGS  "${COMPILE_OPTS}")