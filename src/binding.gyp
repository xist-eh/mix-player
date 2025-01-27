{
  'targets': [
    {
      'target_name': 'mix-player-native',
      'sources': [ 'mix-player.cpp' ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")", "lib/include"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'cflags!': [ '-fno-exceptions', "--static" ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ["OS==\"mac\"", {      
          'xcode_settings': {
            'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'CLANG_CXX_LIBRARY': 'libc++',
            'MACOSX_DEPLOYMENT_TARGET': '10.7'
          },
        }
        ],

      ]
    }
  ]
}