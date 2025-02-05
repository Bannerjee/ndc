# kind of cursed opengl loader
import requests as r
import re
import argparse
import os

# objdump -p demo.exe | grep "DLL Name\|Import Address Table" -A 20
# objdump -p demo.exe | grep "DLL Name:"

GLCOREARGB_H = "https://registry.khronos.org/OpenGL/api/GL/glcorearb.h"

EXCLUDED_EXTENSIONS = [
	"GL_ARB_sample_shading",
	"GL_INTEL_performance_query",
	"GL_INTEL_framebuffer_CMAA",
	"GL_AMD_performance_monitor",
	"GL_EXT_EGL_image_storage",
	"GL_EXT_debug_marker",
	"GL_EXT_shader_framebuffer_fetch_non_coherent",
	"GL_MESA_framebuffer_flip_y",
	"GL_AMD_framebuffer_multisample_advanced"
]



def download(url):
	print(f'[NDC]Downloading {url}')

	response = r.get(url)
	if response.status_code != 200:
		print("[NDC]Failed to fetch the OpenGL")
		exit(1)
	return response.text

parser = argparse.ArgumentParser()
parser.add_argument('header', type=str)
parser.add_argument('source', type=str)
args = parser.parse_args()

if os.path.isfile(args.header) and os.path.isfile(args.source):
	exit(0)
#ndc/include/gl.h src/gl.c
txt = download(GLCOREARGB_H)

function_names = set(sorted(re.findall(r"GLAPI\s+(?:\w+\s+)+APIENTRY\s+(gl\w+)\s*\(",txt),key=len))
filtered = re.sub(r"^GLAPI.*\n", "", txt, flags=re.MULTILINE)

header = open(args.header, "w")
header.write(filtered)
for ext in EXCLUDED_EXTENSIONS:
	start = filtered.find(f"#ifndef {ext}\n#define {ext} 1\n")
	end = filtered.find(f"#endif /* {ext} */",0 if start == 0 else start)
	fstart = start
	while(True):
		fstart = filtered.find("PFN",fstart,end);
		if(fstart == -1):
			break;
		fend = filtered.find(' ',fstart,end)
		ftype = filtered[fstart:fend-1].lstrip("PFN").rstrip("PROC")

		function_names = {
    		item for item in function_names if item.casefold() != ftype.casefold()
		}

		function_names.discard(filtered[fstart:fend-1])
		fstart += fend - fstart

	filtered = filtered[:start] + filtered[end + len(f"#endif /* {ext} */"):]

for func in function_names:
	header.write(f"extern PFN{func.upper()}PROC {func};\n")

header.write("\nvoid ndc_load_gl_functions(void*(loader)(const char*));")
header.close()

source = open(args.source, "w")
source.write("#include <ndc/gl.h>\n\n")

for func in function_names:
	source.write(f'PFN{func.upper()}PROC {func} = NULL;\n')
source.write("void ndc_load_gl_functions(void*(loader)(const char*))\n{\n")
for func in function_names:
	source.write(f'\t{func} = (PFN{func.upper()}PROC)loader("{func}");\n')
source.write("}\n")
source.close()

