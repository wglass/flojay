#!/usr/bin/env python
from setuptools import setup, Extension

yajl_sources = [
    'flojay/yajl/src/' + file_
    for file_ in (
        'yajl.c', 'yajl_gen.c', 'yajl_alloc.c',
        'yajl_lex.c', 'yajl_tree.c',
        'yajl_encode.c', 'yajl_version.c',
        'yajl_buf.c', 'yajl_parser.c'
    )
]

flojay_sources = [
    'flojay/' + filename
    for filename in (
        "module.c", "encoder.c", "parser.c",
        "generator.c", "error.c", "stack.c"
    )
]

flojay = Extension(
    'flojay',
    define_macros=[
        ('MAJOR_VERSION', '0'),
        ('MINOR_VERSION', '1')
    ],
    extra_compile_args=['--std=c99'],
    include_dirs=['flojay/yajl/include', 'flojay/yajl/src'],
    sources=yajl_sources + flojay_sources
)


setup(name='flojay',
      version='0.1',
      description='Streaming or event-based JSON parser based on yajl',
      author='Robert Church',
      author_email='rchurch@myemma.com',
      url='',
      ext_modules=[flojay],
      packages=['flojay'],
      install_requires=[
          'nose==1.1.2',
      ])
