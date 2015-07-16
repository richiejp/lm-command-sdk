#!Python

import os

#env = Environment()

AddOption('--analyze', 
					dest='analyze', 
					action='store_true',
					default='false',
					help="Pass on environment variables set by Clang's scan-build")


#if GetOption('analyze'):
#	env['CC'] = os.getenv('CC') or env['CC']
#	env['CXX'] = os.getenv('CXX') or env['CXX']
#	env['ENV'].update(x for x in os.environ.items() if x[0].startswith('CCC_'))

SConscript('src/SConscript', 
					 variant_dir='build', 
					 src='src')
#					 exports='env')

