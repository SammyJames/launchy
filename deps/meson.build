spdlog_inc = include_directories('spdlog/include')
spdlog = declare_dependency(include_directories : spdlog_inc)

beast_inc = include_directories('beast/include')
beast = declare_dependency(include_directories : beast_inc)

tinyxml_src = ['tinyxml2/tinyxml2.cpp']
tinyxml_inc = include_directories('tinyxml2')
tinyxml_lib = static_library('tinyxml2', tinyxml_src, include_directories : tinyxml_inc)

tinyxml2 = declare_dependency(include_directories : tinyxml_inc, link_with : tinyxml_lib)