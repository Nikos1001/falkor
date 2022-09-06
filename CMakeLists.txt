
project(falkor)

add_library(
    falkor
        context.c pkg.c func.c block.c value.c 
        type.c builder.c str.c compiler.c
        write.c
        verify.c 
)
