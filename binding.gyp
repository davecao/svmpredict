{
  'variables':{
    # Definiton of variables that can be interpolated and used in various
    # other parts of the file.
#    'conditions': [
#      ['OS == "mac"', {
#        'mac_sdk_min%':'10.8',
#        'mac_deployment_target':'10.8',
#      }],
#      ['OS =="linux"',{
#      },
#      {}]
#    ],
    'mac_sdk_min%':'10.8',
    'mac_deployment_target':'10.8',
  },
  'includes':[
    # include other gyp settings suffix .gypi
  ],
  'target_defaults':{
    # settings that will apply to all of the targets defined
    # in this .gyp.
    'default_configuration':'Debug',
    'configurations' : {
      'Debug' : {
        'defines': [ 'DEBUG', '_DEBUG', 'VERIFY_HEAP' ],
        'cflags': [ '-g', '-O0' ],
        #'xcode_settings': {
        #  'MACOSX_DEPLOYMENT_TARGET':'<(mac_deployment_target)',
        #  'GCC_OPTIMIZATION_LEVEL': '0', # stop gyp from defaulting to -Os
        #},
      },
      'Release' : {
        'xcode_settings' : {
          # It will not work if set this in the following 'conditions'
          'MACOSX_DEPLOYMENT_TARGET':'<(mac_deployment_target)',
          }
       }
    }, # configurations
    'include_dirs' : [
      'include',
      '<(node_root_dir)/deps/v8/include'
    ],
   'conditions' : [
      # A list of condition specifications that can modify the contents of the items
      # in the global dictionary defined by this .gyp file based on the values of
      # different variables. The common use of a condition section in the top-level
      # dictionary is to add platform-specific targets to the targets list.
      ['OS=="mac"', {
        'xcode_settings': {
          'ALWAYS_SEARCH_USER_PATHS':'YES',
          'GCC_C_LANGUAGE_STANDARD': 'c99',         # -std=c99
          'GCC_CW_ASM_SYNTAX': 'NO',                # No -fasm-blocks
          'GCC_DYNAMIC_NO_PIC': 'NO',               # No -mdynamic-no-pic
                                                    # (Equivalent to -fPIC)
          'GCC_CW_ASM_SYNTAX': 'NO',                # No -fasm-blocks
          'GCC_ENABLE_CPP_EXCEPTIONS': 'NO',        # -fno-exceptions
          'GCC_ENABLE_CPP_RTTI': 'NO',              # -fno-rtti
          'GCC_ENABLE_PASCAL_STRINGS': 'NO',        # No -mpascal-strings
          'GCC_THREADSAFE_STATICS': 'NO',           # -fno-threadsafe-statics
          'GCC_TREAT_WARNINGS_AS_ERRORS': 'NO',     # -Werror
          'PREBINDING': 'NO',                       # No -Wl,-prebind

          # -mmacosx-version-min=10.5
          'MACOSX_DEPLOYMENT_TARGET':'<(mac_deployment_target)',
          #'MACOSX_DEPLOYMENT_TARGET':'10.8',
          'USE_HEADERMAP': 'NO',

          'GCC_OPTIMIZATION_LEVEL': '3',  # -O3     # optimize -O3

          # c++ 11 Options
          'CLANG_CXX_LANGUAGE_STANDARD': 'c++11',   # c++
          'OTHER_CPLUSPLUSFLAGS':['-stdlib=libc++'], # when using c++11
          #'OTHER_LDFLAGS':['-stdlib=libc++'],

          # Warn if automatic synthesis is triggered with
          # the -Wobjc-missing-property-synthesis flag.
          'CLANG_WARN_OBJC_MISSING_PROPERTY_SYNTHESIS': 'YES',

          'OTHER_CFLAGS': [
            '-fno-strict-aliasing',
            '-fstack-protector-all',  # Implies -fstack-protector
          ],

          'WARNING_CFLAGS': [
            '-Wall',
            '-Wendif-labels',
            '-Wextra',
            # Don't warn about unused function parameters.
            '-Wno-unused-parameter',
            # Don't warn about the "struct foo f = {0};" initialization
            # pattern.
            '-Wno-missing-field-initializers',
            '-W',
            # Match OS X clang C++11 warning settings.
            '-Wno-c++11-narrowing',
          ],
        },
      }],

      ['OS=="linux"', {
        'cflags': [
            #'<!@(pkg-config --cflags QtCore QtGui QtTest)'
          ],
        'cflags!':['-fno-exceptions'],
        'cflags_cc+': [
          '-fno-exceptions',
          '-fpermissive',
          '-std=c++11'
        ],
        'ldflags': [
            #'<!@(pkg-config --libs-only-L --libs-only-other QtCore QtGui QtTest)'
          ],
        'include_dirs':[
            'include'
          ],
        'libraries': [
            #'<!@(pkg-config --libs-only-l QtCore QtGui QtTest)'
          ],
        'linkflags':[
            '-v'
          ],
        'link_settings':{
             'libraries': [
                '-lm',
                '-lstdc++'
               ],
             'library_dirs': []
          },
        'conditions':[
            ['clang == 0 and gcc_version >=44',{
              'cflags!':['-fno-tree-sink','-fno-tree-vrp'] # disable default compiler option
              }
            ]
          ]
      }],

      ['OS!="mac" and OS!="win"', {
        'cflags': [ '-fno-omit-frame-pointer' ],
      }],
    ]
  },

  'targets': [
    # The list of targets for which this .gyp file can generate builds.
    # Each target is a dictionary that contains settings describing all
    # information necessary to build the target.
    {
      'target_name': 'svmpredict',
      #'type':'',
      #'variables': '',
      'sources': [
        #'src/svmpredict-node.cpp',
        'src/svm-predict.cpp',
        'src/svm.cpp'
       ],
      'include_dirs': [
        #'include',
      ]
    },
    {
      'target_name':'aaproperties',
      #'type':'executable',
      #'variables': '',
      'sources': [
          'src/feature.cpp'
       ],
      'include_dirs': [
          #'include',
       ],
    },
  ],
}
