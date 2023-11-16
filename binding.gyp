{
    "target_defaults": {
        "include_dirs": []
    },
    "targets": [
        {
            "target_name": "mapper",
            "type": "none",
            "sources": [
                "./srcCpp/types/geojson.hpp",
                "./srcCpp/mapper/helpers/EmptyOr.hpp",
                "./srcCpp/mapper/helpers/index.cpp",
                "./srcCpp/mapper/FindRoutes.cpp",
                "./srcCpp/mapper/Preprocessor.cpp",
                "./srcCpp/mapper/RoundCoordinate.cpp",
                "./srcCpp/mapper/TestFunc.cpp",
                "./srcCpp/index.cpp"
            ],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "./srcCpp/*"
            ],
            "defines": [
                "NAPI_DISABLE_CPP_EXCEPTIONS"
            ],
            "action": ["bear -- make"]
        },
        {
            "target_name": "action_after_build",
            "type": "none",
            "dependencies": [
                "mapper"
            ],
            "copies": [
                {
                    "files": [
                        "./build/Release"
                    ],
                    "destination": "./dist/cjs/build"
                }
            ]
        }
    ]
}
