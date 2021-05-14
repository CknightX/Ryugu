set_config("cxxflags","-std=c++14")

target("ryugu")
    set_kind("static")
    add_syslinks("pthread")
    add_files("Ryugu/*.cpp")

target("test")
    add_deps("ryugu")
    set_kind("binary")
    add_includedirs("./")
    add_files("examples/Test/*.cpp")

target("webserver")
    add_deps("ryugu")
    set_kind("binary")
    add_includedirs("./")
    add_files("examples/WebServer/*.cpp")
