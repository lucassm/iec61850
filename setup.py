from distutils.core import setup, Extension

module1 = Extension('iec61850',
				 include_dirs = ['/usr/local/include'],
				 libraries = ['iec61850', 'pthread'],
				 library_dirs = ['/usr/local/lib'],
				 #runtime_library_dirs = ['/usr/local/lib'],
				 #extra_compile_args = ['-fPIC'],
				 #extra_link_args = ['-fPIC'],
				 sources=['iec61850.c']
				 )

setup(name = 'iec61850',
	  version = '1.0',
	  description = 'A bind of libiec61850 for Python',
	  author = 'Lucas S Melo',
	  ext_modules = [module1]
	  )