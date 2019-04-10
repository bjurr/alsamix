{
    "targets": [{
        "target_name": "alsamix",
        "sources": [ "./src/alsamix.c" ],
        "include_dirs": [
            '<!@(pkg-config alsa --cflags-only-I | sed s/-I//g)'
        ],
        "libraries": [
            '<!@(pkg-config alsa --libs)'
        ]
    }]
}
