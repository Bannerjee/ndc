#include <ndc/gl_utils.h>
#include <ndc/gl.h>
#include <ndc/allocator.h>
#include <ndc/error.h>

uint32_t ndc_compile_shader(const char* src,uint32_t type)  
{
    uint32_t id = glCreateShader(type);
    glShaderSource(id,1,&src,NULL);
    glCompileShader(id);
    
    int32_t success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        int32_t len = 512;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
        char* log = ndc_malloc(len+1);
        log[len] = '\0';
        glGetShaderInfoLog(id, len, NULL,log);
        glDeleteShader(id);
        NDC_WARNING("shader compilation failure");
        NDC_WARNING(log);
        ndc_free(log);
        return 0;
    }
    return id;
}
char ndc_link_program(uint32_t program)
{
    glLinkProgram(program);
    int32_t success= 0;
    glGetProgramiv(program,GL_LINK_STATUS,&success);
    if(!success)
    {
        int32_t len = 512;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        char* log = ndc_malloc(len+1);
        log[len] = '\0';
        glGetProgramInfoLog(program, len, NULL, log);
        NDC_WARNING("program linking failure");
        NDC_WARNING(log);
        ndc_free(log);
        return -1;
    }
    return 1;
}
uint32_t ndc_compile_program(uint32_t count,...)
{
    uint32_t program = glCreateProgram();
    va_list args;
    va_start(args,count);
    for(uint8_t i=0;i<count;++i)
    {
        const char* src = va_arg(args,const char*);
        NDC_ASSERT(strlen(src) >= 2);
        uint32_t shader_type = src[3] << 8 | (src[2] == 'S' || src[2] == 'E') ? src[2] : '\0';

        switch(shader_type)
        {
            case 'V': shader_type = GL_VERTEX_SHADER; break;
            case 'F': shader_type = GL_FRAGMENT_SHADER; break;
            case 'C': shader_type = GL_COMPUTE_SHADER; break;
            case 'G': shader_type = GL_GEOMETRY_SHADER; break;
            case 'T' << 8 | 'S': shader_type = GL_TESS_CONTROL_SHADER; break;
            case 'T' << 8 | 'E': shader_type = GL_TESS_EVALUATION_SHADER; break;
            default:
                NDC_WARNING("Invalid shader type provided");
                return 0;
        };
        glAttachShader(program,ndc_compile_shader(src,shader_type));
    }
    va_end(args);
    return ndc_link_program(program);
}