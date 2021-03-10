import qbs

CppApplication {
    consoleApplication: true

    Depends { name: "BuildConfig" }

    Depends { name: "Qt.core" }
    Depends { name: "Qt.network" }

    cpp.useCxxPrecompiledHeader: true
    Group {
        name: "pch"
        fileTags: "cpp_pch_src"
        files: ["Qt.pch"]
    }

    Group {
        qbs.install: true
        Properties {
            condition: qbs.targetOS.contains("windows")
            qbs.installDir: "www"
            files: [
                "res/www/*.*"
            ]
        }
    }

    Group {
        name: ""
        fileTagsFilter: "application"
        qbs.install: true
        //qbs.installDir: "bin"
    }

    files: [
        "**/*.h",
        "**/*.hpp",
        "**/*.cpp",
        "**/*.rem",
//        "**/*.html",
//        "**/*.js",
    ]
}
