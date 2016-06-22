echo "#ifndef LIBC_PATH" > defines.h
echo "#define LIBC_PATH \"$(ldd $(which ls)|awk '/\tlibc\./ {print $3}')\"" >> defines.h
echo "#endif" >> defines.h
