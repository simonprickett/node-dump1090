{
  'targets': [
    {   
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")" ],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
      'target_name': 'dump1090',
      'sources': [ 'dump1090.c', 'anet.c' ],
      'conditions': [
        ['OS=="linux"', {
          'sources': ['node-dump1090.cc'],
          "libraries": [
            "-lrtlsdr",
            "-lusb-1.0",
            "-lpthread",
            "-lm"
          ], 
        }],
        ['OS=="mac"', {
          'sources': ['node-dump1090.cc'],
          "libraries": [
            "-lrtlsdr",
            "-lusb-1.0",
            "-lpthread",
            "-lm"
          ],
        }]
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }   
  ]
}
