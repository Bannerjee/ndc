#include <ndc/window.h>
#include <ndc/gl.h>
#include <ndc/audio.h>
#include <ndc/allocator.h>
#include <ndc/error.h>
#include <ndc/io.h>
#include <ndc/gl_utils.h>
#include <ndc/math.h>
#include <stdio.h>

void key_callback(ndc_window* win,ndc_input_key_t key,uint32_t count,uint32_t scancode,ndc_input_action_t action)
{
    if(action != NDC_PRESS) return;
    switch(key)
    {
        case NDC_KEY_ESCAPE:
        {
            win->alive = 0;
        }
        break;
        default:return;
    }
}
void size_callback(ndc_window* win,uint32_t width,uint32_t height)
{
    glViewport(0,0,width,height);
}
void click_callback(ndc_window* win,uint32_t width,uint32_t height,ndc_input_key_t key,ndc_input_action_t action)
{
    if(action != NDC_PRESS) return;
    switch(key)
    {
        case NDC_KEY_LMB:
        printf("LMB\n");
        break;
        case NDC_KEY_RMB:
        printf("RMB\n");
        break;
        case NDC_KEY_MMB:
        printf("MMB\n");
        break;
        default: return;
    }
}

const char vertex_source[] = 
R"(//V
#version 460
struct vertex
{
    vec2 pos;
};

layout(binding = 0, std430) readonly buffer ssbo1 
{
    vertex vertices[];
};

layout(location = 0) out vec2 uv;
layout(location = 1) out vec4 out_color;
layout(location = 1) uniform mat4 trans;
void main()
{
    gl_Position = trans * vec4(vertices[gl_VertexID].pos,0,1);

}
)";
const char frag_source[] = 
R"(//F
#version 460
layout(location = 0) uniform sampler2D ourTexture;

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 in_color;
void main()
{   
    FragColor = vec4(1,0,0,0);
    //FragColor = texture(ourTexture, uv);
}
)";

typedef struct 
{
    uint32_t    count;
    uint32_t    instanceCount;
    uint32_t    firstIndex;
    int32_t     baseVertex;
    uint32_t    baseInstance;
} ndc_draw_elements_indirect_command;


// everything is relative towards parent, if orphan - towards window

typedef struct ndc_ui_element ndc_ui_element;
struct ndc_ui_element
{
    ndc_vec2 offset;
    ndc_vec2 size;
    ndc_vec4 color;
    ndc_matrix mat;

    uint8_t flags;
    uint8_t num_children;

    ndc_ui_element* parent;
    ndc_ui_element* children;

    void* widget_data;
};
typedef struct
{
    uint32_t cmd_buf;
    uint32_t index_buf;
    uint32_t vertex_buf;

    ndc_ui_element* root;
} ndc_ui_layout;


/*
ndc_ui_element* ndc_create_ui_element(
    ndc_ui_element* parent,
    const ndc_vec2* size,
    const ndc_vec2* offset,
    const ndc_vec4* color)
{
    ndc_ui_element* el = ndc_malloc(sizeof(ndc_ui_element));
    memset(el,0,sizeof(ndc_ui_element));
    el->mat = ndc_identity_matrix_4x4();
    if(offset)
    {
        el->offset = *offset;
        ndc_vec3 v3 = {offset->x,offset->y,0};
        ndc_translate(&el->mat,&v3);
    }

        
    if(size) el->size = *size;
    if(color) el->color = *color;
    if(parent) el->parent = parent;
    
    return el;
}

ndc_ui_layout* ndc_create_ui_layout(ndc_ui_element* root)
{
    assert(root);
    ndc_ui_layout* layout = ndc_malloc(sizeof(ndc_ui_layout));
    layout->cmd_buf = 0;
    layout->index_buf = 0;
    layout->vertex_buf = 0;
    return layout;
}
void ndc_add_child(ndc_ui_element* element,ndc_ui_element* child)
{
    assert(child && element);
    assert(child != element);
    assert(!child->parent);
    child->parent = element;
    element->children = ndc_realloc(element->children,++element->num_children * sizeof(ndc_ui_element));
    element->children[element->num_children-1] = *child;
}
void ndc_build_ui(const ndc_ui_element* els,uint32_t* vb,uint32_t* ib,uint32_t* cb)
{
    if(!cb || !ib || !vb) return;
    //ndc_draw_elements_indirect_command cmd = {num_indices,1,0,0,0};

    //glCreateBuffers(1,&cb);

    //glNamedBufferStorage(*cb, sizeof(ndc_draw_elements_indirect_command),&cmd, 0);


    //glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmd_buf)

    glCreateBuffers(1,&vb);
    glNamedBufferStorage(vbuf, num_vertices * sizeof(ndc_vec3),vertices, 0);

    glCreateBuffers(1,&ib);
    glNamedBufferStorage(ibuf, num_indices * sizeof(uint32_t),indices, 0);
}*/
//ndc_ui_element* ndc_create_ui_element(ndc_ui_element* parent,)

int main(int argc, char **argv) 
{
    ndc_window* window = ndc_create_window("test",400,400);
    ndc_show_window(window);
    ndc_set_key_callback(window,key_callback);
    ndc_set_size_callback(window,size_callback);
    ndc_set_mouse_click_callback(window,click_callback);
   // ndc_set_mouse_move_callback(window,mm_callback);
   
    /*ndc_ui_element* ui_elements = ndc_malloc(sizeof(ndc_ui_element) * 1);
    ui_elements[0].offset.x = 0;
    ui_elements[0].offset.y = 0;
    ui_elements[0].extent.x = 2;
    ui_elements[0].extent.y = 2;
    ui_elements[0].color.x = 0;
    ui_elements[0].color.y = 255;
    ui_elements[0].color.z = 0;
    ui_elements[0].color.w = 0;
*/
    uint32_t num_vertices =4;
    uint32_t num_indices = 6;
    ndc_vec2 vertices[] = {
        {-0.5,-0.5},
        {0.5,0.5},
        {-0.5,0.5},
        {0.5,-0.5},
    };
    //uint32_t indices[] = {0,2,1, 0,3,1};
    //ndc_vec3* vertices = generateCircle(0.3,num_vertices,&indices,&num_indices);

    ndc_draw_elements_indirect_command cmd = {num_indices,1,0,0,0};
   /* uint32_t cmd_buf;
    glCreateBuffers(1,&cmd_buf);
    glNamedBufferStorage(cmd_buf, sizeof(ndc_draw_elements_indirect_command),&cmd, 0);


    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmd_buf);


    glEnable(GL_FRAMEBUFFER_SRGB);

    unsigned int vbuf = 0;
    unsigned int ibuf = 0;

    glCreateBuffers(1,&vbuf);
    glNamedBufferStorage(vbuf, num_vertices * sizeof(ndc_vec3),vertices, 0);

    glCreateBuffers(1,&ibuf);
    glNamedBufferStorage(ibuf, num_indices * sizeof(uint32_t),indices, 0);
*/
    uint32_t program = ndc_compile_program(2,vertex_source,frag_source);

    uint32_t vao = 0;
    glCreateVertexArrays(1,&vao);
    glBindVertexArray(vao);

    glUseProgram(program);

    /*int32_t width,height;
    uint8_t* image = ndc_read_bmp("sample1.bmp",&width,&height,NULL,NULL);

    unsigned int tex = 0;
    glCreateTextures(GL_TEXTURE_2D,1,&tex);
    glTextureStorage2D(tex,1,GL_SRGB8,width,height);
    glTextureSubImage2D(tex,0,0,0,width,height,GL_BGR,GL_UNSIGNED_BYTE,image);
    glBindTextureUnit(0, tex);
    ndc_free(image);
*/
    ndc_matrix mmm = ndc_identity_matrix_4x4();
    ndc_vec3 v = {0.5,0.5,0.5};
    ndc_translate(&mmm,&v);
 
    glUniformMatrix4fv(1,1,0,mmm.data);
    ndc_free_matrix(&mmm);

    glEnable(GL_FRAMEBUFFER_SRGB);
    while(window->alive)
    {

        glClearColor(0.1,0.1,0.1,0);
        glClear(GL_COLOR_BUFFER_BIT);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glMultiDrawElementsIndirect(GL_TRIANGLES,GL_UNSIGNED_INT,0,1,0);
        ndc_poll_events(window);
        ndc_swap_buffers(window);
    }
    //glDeleteTextures(1,&tex);
    glDeleteVertexArrays(1,&vao);
    glDeleteProgram(program);
    ndc_destroy_window(window);


    printf("LOST BYTES:%llu",ndc_get_total_allocated());
}
