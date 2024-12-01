{
  'targets': [
    {
      'target_name': 'mix-player-native',
      'sources': [ 'mix-player.cpp' ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")", "lib/include"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'libraries': [ "../lib/dll/SDL2.lib", "../lib/dll/SDL2main.lib", "../lib/dll/SDL2_mixer.lib"],
      'copies': [{
        "destination": "build/Release",
        "files": [
          "lib/dll/SDL2.dll",
          "lib/dll/SDL2_mixer.dll"
        ]
        }],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
          'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}