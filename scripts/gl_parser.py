import os,subprocess,argparse,requests

def download(url):
	print(f'[NDC]Downloading {url}')

	response = requests.get(url)
	if response.status_code != 200:
		print(f"[NDC]Failed to fetch {url}")
		exit(1)
	return response.text

parser = argparse.ArgumentParser()
parser.add_argument('header', type=str)
parser.add_argument('source', type=str)
args = parser.parse_args()
if os.path.isfile(args.header) and os.path.isfile(args.source):
	exit(0)

if not os.path.isfile("genreg.py"):
	script = download("https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/b53ca669bea4715b6d5fa53c459f47a1fecd7944/xml/reg.py")
	script += f"\nHEADER_FILE = \"{args.header}\"\nSOURCE_FILE = \"{args.source}\"\n"
	script += """
def download(url):
	print(f'[NDC]Downloading {url}')

	response = r.get(url)
	if response.status_code != 200:
		print(f'[NDC]Failed to fetch {url}')
		exit(1)
	return response.text

def download_file(url,file):
	with open(file,'w') as f:
		f.write(download(url))
if __name__ == '__main__':
	import requests as r
	import re
	
	regFilename = 'gl.xml'
	diagFilename = 'diag.txt'
	allVersions = '.*'


	download_file('https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/b53ca669bea4715b6d5fa53c459f47a1fecd7944/xml/gl.xml',regFilename)
	diag = open(diagFilename,'w')
	gen = COutputGenerator(diagFile = diag)
	prefix =  [
    '#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)',
    '#ifndef WIN32_LEAN_AND_MEAN',
    '#define WIN32_LEAN_AND_MEAN 1',
    '#endif',
    '#include <windows.h>',
    '#endif',
    '',
    '#ifndef APIENTRY',
    '#define APIENTRY',
    '#endif',
    '#ifndef APIENTRYP',
    '#define APIENTRYP APIENTRY *',
    '#endif',
    '#ifndef GLAPI',
    '#define GLAPI extern',
    '#endif','']

	exclude_pattern = r"^(GL_(AMD|EXT|INTEL|MESA|KHR)_[a-zA-Z0-9_]+)$"


	build_target = CGeneratorOptions(
	filename          = HEADER_FILE,
	apiname           = 'gl',
	profile           = 'core',
	versions          = allVersions,
	emitversions      = allVersions,
	defaultExtensions = 'glcore',
	addExtensions     = None,
	removeExtensions  = exclude_pattern,
	prefixText        = prefix,
	genFuncPointers   = True,
	protectFile       = True,
	protectFeature    = True,
	protectProto      = False,
	protectProtoStr   = "",
	apicall           = '//',
	apientry          = 'APIENTRY ',
	apientryp         = 'APIENTRYP ')

	reg = Registry()
	tree = etree.parse(regFilename)
	reg.loadElementTree(tree)
	reg.setGenerator(gen)
	reg.apiGen(build_target)

	diag.close()

	header_string = "" 



	source_string = '#include <ndc/gl.h>\\n'
	function_names = []
	with open(HEADER_FILE,'r') as f:
		header_string = re.sub(re.compile('//.*?\\n' ) ,'' ,f.read())
		header_string = header_string[:header_string.rfind("#ifdef __cplusplus")]

	with open(diagFilename,'r') as f:
		function_names = re.findall(re.compile(r'Emitting command decl for (\w+)\\n' ), f.read())

	for func in function_names:
		header_string += f'extern PFN{func.upper()}PROC {func};\\n'
		source_string += f'PFN{func.upper()}PROC {func} = NULL;\\n'

	source_string += 'void ndc_load_gl_functions(void*(loader)(const char*))\\n{\\n'

	for func in function_names:
		source_string += f'\\t{func} = (PFN{func.upper()}PROC)loader(\"{func}\");\\n'

	header_string += '\\nvoid ndc_load_gl_functions(void*(loader)(const char*));\\n#ifdef __cplusplus\\n}\\n#endif\\n#endif'

	with open(HEADER_FILE,'w') as f:
		f.write(header_string)

	with open(SOURCE_FILE,'w') as f:
		f.write(source_string + '\\n}')

	os.remove(diagFilename)
	os.remove(regFilename)
	"""
	with open("genreg.py","w") as f:
		f.write(script)
subprocess.run(["python", "genreg.py",args.header,args.source])