set_languages("cxx14")
set_symbols("debug") -- debug mode

-- lib
target("ryugu")
    set_kind("static")
    add_syslinks("pthread")
    add_includedirs("./")
    add_files("Ryugu/net/*.cpp")
    add_files("Ryugu/base/*.cpp")
    add_files("Ryugu/base/log/Logging.cpp")
-- examples

    -- test
target("test")
    set_kind("binary")
    add_deps("ryugu")
    add_includedirs("./")
    add_files("examples/Test/*.cpp")
target("timer_test")
    set_kind("binary")
    add_deps("ryugu")
    add_includedirs("./")
    add_files("examples/Test/TimerTest/*.cpp")
target("logger_test")
    set_kind("binary")
    add_deps("ryugu")
    add_includedirs("./")
	add_files("Ryugu/base/log/*.cpp")
    -- others
target("webserver")
    set_kind("binary")
    add_deps("ryugu")
    add_includedirs("./")
    add_files("examples/WebServer/*.cpp")
