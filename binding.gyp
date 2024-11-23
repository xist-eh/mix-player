{
    "targets": [
        {
            "target_name": "mix-player",
            "sources": ["src/addon.cpp"],
            "include_dirs": ["lib/include/"],
			'cflags': [ '-D_REENTRANT' ],
			'libraries': [ "../src/lib/lib/SDL2.lib", "../src/lib/lib/SDL2_mixer.lib", "../src/lib/lib/SDL2main.lib", "../src/lib/lib/SDL2test.lib" ],
            'copies': [
                {
                    "destination": "build/Release",
                    "files": [
                        "src/lib/dll/SDL2.dll",
                        "src/lib/dll/SDL2_mixer.dll"
                    ]
                }
            ]



        }
    ]
}