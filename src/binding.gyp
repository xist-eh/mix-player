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
        ["OS==\"win\"", {      
          'libraries': [ "../lib/win-dll/SDL2.lib", "../lib/win-dll/SDL2main.lib", "../lib/win-dll/SDL2_mixer.lib"],
          'copies': [{
            "destination": "build/Release",
            "files": [
              "lib/win-dll/SDL2.dll",
              "lib/win-dll/SDL2_mixer.dll"
            ]
          }],
          'msvs_settings': {
            'VCCLCompilerTool': { 'ExceptionHandling': 1 },
          },
        }
        ],
        ["OS==\"linux\"", {
          'libraries': [ "-lSDL2main", "-lSDL2 ", "-lSDL2_mixer"],
        }]
      ]
    }
  ]
}