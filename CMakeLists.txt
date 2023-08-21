cmake_minimum_required(VERSION 3.16)

# set the project name
project(esp-unlock VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(esp-unlock.h.in esp-unlock.h)

set(EXTRA_LINKER_ARGS "-lpthread -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0")

# add the executable
add_executable(esp-unlock 
        command_reader.cpp 
        gtk_app.cpp 
        otp_entry.cpp 
        otp_add_entry.cpp 
        otp_entry_add_window.cpp 
        hash_command.cpp 
        totp_command.cpp 
        totp_command_exception.cpp 
        totp_list_command.cpp 
        totp_add_command.cpp 
        main_gui.cpp)

# Include files derived from `pkg-config --cflags-only-I gtk+-3.0`
set_source_files_properties(gtk_app.cpp otp_entry.cpp otp_add_entry.cpp otp_entry_add_window.cpp PROPERTIES
  COMPILE_FLAGS "-I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/fribidi -I/usr/include/harfbuzz -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/uuid -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include")

target_include_directories(esp-unlock PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

# Library flags derived from `pkg-config --libs gtk+-3.0`
target_link_libraries(esp-unlock pthread gtk-3 gdk-3 pangocairo-1.0 pango-1.0 harfbuzz atk-1.0 cairo-gobject cairo gdk_pixbuf-2.0 gio-2.0 gobject-2.0 glib-2.0 baseencode)
