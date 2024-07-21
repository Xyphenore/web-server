project(
    'web-server', 'cpp',
    license : 'LGPL-3.0-or-later',
    license_files : 'LICENSE.md',
    meson_version : '>=1.4.0',
    version : files('VERSION.txt'),
    default_options : {
        'buildtype' : 'release',
        'cpp_std' : 'c++17',
    },
)

project_metadata = {
    'description' : 'This is a multithreaded Web server. It is an exercise of the Rust Book but converted to C++ 17.',
    'url' : 'https://github.com/Xyphenore/@0@'.format(meson.project_name()),
}

summary(
    {
        'Description' : project_metadata['description'],
        'Homepage' : project_metadata['url'],
    },
    section : 'Summary',
)


# -------------------
# Check C++ standard
# -------------------

message('Checking the C++ standard...')

if not get_option('cpp_std').startswith('c++')
    error(
        'Please use the C++ standard, not a constructor standard.',
        'Current standard: ' + get_option('cpp_std'),
    )
endif

if get_option('cpp_std').split('++')[-1].version_compare('<17')
    error(
        'Please use the C++ standard 17 or upper. The library uses concepts.',
        'Current standard: ' + get_option('cpp_std'),
    )
endif

message('The C++ standard is good.')

# -------------
# Check system
# -------------

message('Checking if the system is supported...')
supported_systems = ['linux', 'windows']

if host_machine.system() not in supported_systems
    error(
        f'Only @supported_systems@ are supported.',
        'You use ' + host_machine.system() + '.',
    )
endif

unset_variable('supported_systems')
message('System supported.')

# ---------------
# Check compiler
# ---------------

compile_options = {}

message('Checking if the compiler is supported...')
supported_compilers = ['gcc', 'clang', 'clang-cl']

compiler = meson.get_compiler('cpp')
if meson.get_compiler('cpp').get_id() not in supported_compilers
    error(
        f'Only supported compilers: @supported_compilers@',
        'You use the compiler: ' + meson.get_compiler('cpp').get_id() + '.',
    )
endif

unset_variable('supported_compilers')
message('The compiler is supported.')

# --------------
# Compile flags
# --------------

extra_flags = []

if get_option('debug')
    message('Setting up custom flags for your compiler...')

    gnu_symbols = 'default'

    extra_flags = [
        '-Wall', '-Wextra', '-Wpedantic', '-Weffc++',
        '-fstrict-enums',
        '-Wctor-dtor-privacy', '-Wnon-virtual-dtor',
        '-Wold-style-cast', '-Woverloaded-virtual',
        '-Wmismatched-tags', '-Wmismatched-new-delete',
        '-Wzero-as-null-pointer-constant', '-Wextra-semi',
        '-Wsuggest-override', '-Wdouble-promotion',
    ]

    if compiler.get_id() == 'gcc'
        if compiler.version().version_compare('<7')
            error(
                'Please use GCC-7 or upper to compile the executable,',
                'you use the version: ' + compiler.version() + '.',
                'Use the command like this: CC=gcc-7 CXX=g++-7 meson setup buildDir,',
                'just replace \'gcc-7\' and \'g++-7\' by alias of your compiler version',
                'and replace \'buildDir\' by the name of your build directory.',
            )
        endif

        # ------------
        # GCC options
        # ------------

        compile_options += {
            'RTTI' : get_option('cpp_rtti'),
            'ThreadSafe statics' : get_option('gcc_threadsafe_statics'),
            'Use CXA atexit' : get_option('gcc_cxa_atexit'),
            'Visibility inlines hidden' : get_option('gcc_visibility_inlines_hidden'),
            'Use GNU symbols and keywords' : get_option('gcc_gnu_keywords'),
        }

        if not get_option('cpp_rtti')
            extra_flags += ['-fno-rtti']
        endif

        if not get_option('gcc_threadsafe_statics')
            warning('TODO : Check if this good in tests.')
            extra_flags += ['-fno-threadsafe-statics']
        endif

        if get_option('gcc_cxa_atexit')
            warning('TODO : Check if this good in tests.')
            extra_flags += ['-fuse-cxa-atexit']
        endif

        if get_option('gcc_visibility_inlines_hidden')
            warning('TODO : Check if this good in tests.')
            extra_flags += ['-fvisibility-inlines-hidden']

            gnu_symbols = 'inlineshidden'
        endif

        if not get_option('gcc_gnu_keywords')
            # Possible values:
            # - default: This makes all symbols visible, which is the default behavior
            #       if gnu_symbol_visibility is not explicitly set.
            # - internal: This hides symbols unless they are explicitly marked to be
            #       visible, making them internal to the shared library.
            # - hidden: Similar to internal, this hides symbols from the dynamic
            #       symbol table unless they are explicitly marked as visible,
            #       which prevents them from being used by other modules or libraries.
            # - protected: Symbols are visible, but cannot be overridden by other
            #       modules. This is a middle ground between default and hidden/internal,
            #       allowing some control over symbol visibility while still enabling
            #       symbol resolution from outside the library.
            # - inlineshidden: This option specifically targets inline functions,
            #       making them hidden unless explicitly marked as visible. It's useful
            #       for hiding the symbols of inline functions while keeping other
            #       symbols as per the current visibility settings.
            gnu_symbols = 'hidden'

            extra_flags += ['-fno-gnu-keywords']
        endif

        # -------------------
        # GCC compiler flags
        # -------------------

        extra_flags += [
            '-fno-nonansi-builtins',
            '-fnothrow-opt',
            '-fno-operator-names',
            '-fchar8_t',
            '-Wnoexcept',
            '-Wstrict-null-sentinel',
            '-Wuseless-cast',
            '-Wsuggest-final-types',
            '-Wsuggest-final-methods',
        ]

        if compiler.version().version_compare('>=12')
            extra_flags += ['-fimplicit-constexpr']
        endif
    else
        warning('Extra flags for CLang is not defined.')
        extra_flags += []
    endif

    unset_variable('compiler')
    message('Custom flags are set.')

    summary(compile_options, section : 'Compile options', bool_yn : true)
endif

# -----------
# Executable
# -----------

message('Building the library \'asio\'...')
asio_dep = dependency('asio', version : ['>=1.30.2', '<2.0.0'], allow_fallback : true)
message('The library \'asio\' (' + asio_dep.version() + ') is built.')

message('Building the executable...')

web_server_src = files('src' / meson.project_name() / 'main.cpp')
web_server_templates = files(
    'templates' / 'index.html',
    'templates' / 'not_found.html',
    'templates' / 'slow_request.html',
)

web_server_executable = executable(
    meson.project_name(),
    sources : web_server_src,
    extra_files : web_server_templates,
    dependencies : asio_dep,
    gnu_symbol_visibility : get_option('debug') ? gnu_symbols : 'default',
    cpp_args : extra_flags,
    implicit_include_directories : true,
    include_directories : include_directories('src'),
    install_mode : false, # Default mode to have a read only library.
    win_subsystem : 'console',
)

message('The executable is built.')
