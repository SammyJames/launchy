project('launchy', 'cpp', 
    default_options : 
    [
        'cpp_std=c++1z'
    ], 
    license : 'MIT')

boost = dependency('boost', 
    modules : 
    [
        'system', 
        'thread', 
        'filesystem'
    ], 
    static : true)

openssl = dependency('openssl', 
    static : true)

subdir('deps')

executable('launchy', 
    [ 
        'src/main.cpp', 
        'src/step.cpp', 
        'src/launcher.cpp', 
        'src/client.cpp',
        'src/bytebuffer.cpp',
    ],
    dependencies : [ spdlog, boost, openssl, beast, tinyxml2 ],
    include_directories : [  ])
