src=${dir}
obj-m += ${name}.o
${name}-y := ${objs}
ccflags-y += ${module_compile_options}
