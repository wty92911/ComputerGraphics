// Hand Example
// Author: Yi Kangrui <yikangrui@pku.edu.cn>

//#define DIFFUSE_TEXTURE_MAPPING

#include "gl_env.h"

#include <cstdlib>
#include <cstdio>
#include <config.h>
#include <functional>
#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#include <iostream>

#include "skeletal_mesh.h"


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

namespace SkeletalAnimation {
    const char *vertex_shader_330 =
            "#version 330 core\n"
            "const int MAX_BONES = 100;\n"
            "uniform mat4 u_bone_transf[MAX_BONES];\n"
            "uniform mat4 u_mvp;\n"
            "layout(location = 0) in vec3 in_position;\n"
            "layout(location = 1) in vec2 in_texcoord;\n"
            "layout(location = 2) in vec3 in_normal;\n"
            "layout(location = 3) in ivec4 in_bone_index;\n"
            "layout(location = 4) in vec4 in_bone_weight;\n"
            "out vec2 pass_texcoord;\n"
            "void main() {\n"
            "    float adjust_factor = 0.0;\n"
            "    for (int i = 0; i < 4; i++) adjust_factor += in_bone_weight[i] * 0.25;\n"
            "    mat4 bone_transform = mat4(1.0);\n"
            "    if (adjust_factor > 1e-3) {\n"
            "        bone_transform -= bone_transform;\n"
            "        for (int i = 0; i < 4; i++)\n"
            "            bone_transform += u_bone_transf[in_bone_index[i]] * in_bone_weight[i] / adjust_factor;\n"
            "	 }\n"
            "    gl_Position = u_mvp * bone_transform * vec4(in_position, 1.0);\n"
            "    pass_texcoord = in_texcoord;\n"
            "}\n";

    const char *fragment_shader_330 =
            "#version 330 core\n"
            "uniform sampler2D u_diffuse;\n"
            "in vec2 pass_texcoord;\n"
            "out vec4 out_color;\n"
            "void main() {\n"
            #ifdef DIFFUSE_TEXTURE_MAPPING
            "    out_color = vec4(texture(u_diffuse, pass_texcoord).xyz, 1.0);\n"
            #else
            "    out_color = vec4(pass_texcoord, 0.0, 1.0);\n"
            #endif
            "}\n";
}

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}
int keys;
int choice = 19;
bool update_time = 0;
float move_step = 0.1;
const int choice_num = 4;
float angle_step = 0.02*M_PI;
glm::fvec3 camera_eye = glm::fvec3(0.0, 0.0, -1.0);
glm::fvec3 camera_center = glm::fvec3(0.0, 0.0, 0.0);
glm::fvec3 camera_up = glm::fvec3(0.0, 1.0, 0.0);
glm::fvec3 camera_back = glm::fvec3(0.0, 0.0, -1.0);
//call back
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    keys = key;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        choice = (choice_num + choice - 1) % choice_num;
    }
    else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        choice = (choice_num + choice + 1) % choice_num;
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        update_time ^= 1;
    }
    
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {

}
// drop!!!
// void change_camera(glm::fvec3&eye, glm::fvec3&center, glm::fvec3&up)
// {
//     if (keys == GLFW_KEY_W) {
//         eye = eye + move_step*up;
//         center = center + move_step*up;
//     }
//     else if (keys == GLFW_KEY_UP) {
//         glm::fvec3 rotate_axis = glm::cross(up,center - eye);
//         glm::fmat3 rotate_mat = glm::fmat3(glm::rotate(glm::identity<glm::fmat4>(),
//                                             -angle_step,
//                                             rotate_axis));
//         up = rotate_mat*up;
//         glm::normalize(up);
//         center = eye + rotate_mat*(center - eye);
//     }
//     else if (keys == GLFW_KEY_S) {
//         eye = eye - move_step*up;
//         center = center - move_step*up;
//     } 
//     else if (keys == GLFW_KEY_DOWN) {
//         glm::fvec3 rotate_axis = glm::cross(up,center - eye);
//         glm::fmat3 rotate_mat = glm::fmat3(glm::rotate(glm::identity<glm::fmat4>(),
//                                             angle_step,
//                                             rotate_axis));
//         up = rotate_mat*up;
//         glm::normalize(up);
//         center = eye + rotate_mat*(center - eye);
//     }
//     else if (keys == GLFW_KEY_A) {
//         glm::fvec3 left = glm::cross(up, center - eye);
//         glm::normalize(left);
//         eye = eye + move_step*left;
//         center = center + move_step*left;
//     }
//     else if (keys == GLFW_KEY_LEFT) {
//         glm::fvec3 rotate_axis = up;
//         glm::fmat3 rotate_mat = glm::fmat3(glm::rotate(glm::identity<glm::fmat4>(),
//                                             angle_step,
//                                             rotate_axis));
//         center = eye + rotate_mat*(center - eye);
//     }
//     else if (keys == GLFW_KEY_D) {
//         glm::fvec3 left = glm::cross(up, center - eye);
//         glm::normalize(left);
//         eye = eye - move_step*left;
//         center = center - move_step*left;
//     }
//     else if (keys == GLFW_KEY_RIGHT) {
//         glm::fvec3 rotate_axis = up;
//         glm::fmat3 rotate_mat = glm::fmat3(glm::rotate(glm::identity<glm::fmat4>(),
//                                             -angle_step,
//                                             rotate_axis));
//         center = eye + rotate_mat*(center - eye);
//     }
//     else if (keys == GLFW_KEY_F) {
//         eye -= camera_back;
//         // center -= move_step * camera_back;
//     }
//     else if (keys == GLFW_KEY_B) {
//         eye += camera_back;
//         // center += camera_back;
//     }
//     keys = 0;
// }
//camera
class camera {
public:
    glm::fvec3 pos;
    glm::quat ang;
    camera(glm::fvec3 pos, glm::quat ang):pos(pos), ang(ang){}
    camera(float x, float y, float z, float a, float b, float c) {
        pos = glm::fvec3(x, y, z);
        ang = glm::angleAxis(glm::radians(a), glm::vec3(1.0f, 0.0f, 0.0f));
        ang *= glm::angleAxis(glm::radians(b), glm::vec3(0.0f, 1.0f, 0.0f));
        ang *= glm::angleAxis(glm::radians(c), glm::vec3(0.0f, 0.0f, 1.0f));
    }
    camera(){}
    camera(const camera& c) {
        pos = c.pos;
        ang = c.ang;
    }
    camera operator - (camera t)const {
        t.pos = pos - t.pos;
        t.ang = ang - t.ang;
        return t;
    }
    camera operator + (camera t)const {
        t.pos = pos + t.pos;
        t.ang = ang + t.ang;
        return t;
    }
    camera operator * (float r)const {
        camera t(*this);
        t.pos *= r;
        t.ang *= r;
        return t;
    }
    static camera get_camera(float last_time, float cur_time, float next_time, const camera& last_camera, const camera& next_camera) {
        // std::cout << last_time << " " << cur_time << " " << next_time << std::endl;
        if (cur_time >= next_time) {
            return next_camera;
        }
        float ratio = 1;
        ratio = (cur_time - last_time) / (next_time - last_time);
        if (ratio < 0) ratio = 0;
        if (ratio > 1) ratio = 1;
        //interpolation
        ratio = 1 - std::pow((1 - ratio), 3);
        return last_camera + ((next_camera - last_camera) * ratio);
    }
    glm::mat4 to_mat4() {
        return glm::toMat4(ang) * glm::translate(glm::mat4(1.0f), -pos);
    } 
}cur_camera(0,1,0,0,0,0), last_camera(0,1,0,0,0,0), nxt_camera(-2.85,11.38,-8.94,-30.73,-26.37,54.15);
// IMGUI
//用于四元数计算
float pos_x = 0, pos_y = 0, pos_z = 0;
float ang_x = 0, ang_y = 0, ang_z = 0;
float interval_time = 3;
float last_time = 0;
float cur_time = 0;
float next_time = 3;
void imgui_init(GLFWwindow *window) {
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");
}
void imgui_draw() {
    ImGui::Checkbox("update", &update_time);
    bool modify = false; //unused
    
    modify |= ImGui::SliderFloat("interval", &interval_time, 1.0F, 5.0F, "%.2f", 1.0F);
    modify |= ImGui::SliderFloat("POS_X", &pos_x, -50.0F, 50.0F, "%.2f", 1.0F);
    modify |= ImGui::SliderFloat("POS_Y", &pos_y, -50.0F, 50.0F, "%.2f", 1.0F);
    modify |= ImGui::SliderFloat("POS_Z", &pos_z, -50.0F, 50.0F, "%.2f", 1.0F);
    modify |= ImGui::SliderFloat("ANG_X", &ang_x, -90.0F, 90.0F, "%.2f", 1.0F);
    modify |= ImGui::SliderFloat("ANG_Y", &ang_y, -180.0F, 180.0F, "%.2f", 1.0F);
    modify |= ImGui::SliderFloat("ANG_Z", &ang_z, -180.0F, 180.0F, "%.2f", 1.0F);
    if (ImGui::Button("Move")) {
        modify = true;
        last_time = cur_time;
        nxt_camera = camera(pos_x, pos_y, pos_z, ang_x, ang_y, ang_z);
        last_camera = cur_camera;
        next_time = cur_time + interval_time;
    }
}
int main(int argc, char *argv[]) {
    GLFWwindow *window;
    GLuint vertex_shader, fragment_shader, program;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__ // for macos
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(800, 800, "OpenGL output", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    // 鼠标位置变化事件
    glfwSetCursorPosCallback(window, mouse_callback);


// 鼠标滚轮事件
    glfwSetScrollCallback(window, scroll_callback);

    // 鼠标点击事件
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK)
        exit(EXIT_FAILURE);
    imgui_init(window);
    
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &SkeletalAnimation::vertex_shader_330, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &SkeletalAnimation::fragment_shader_330, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int linkStatus;
    if (glGetProgramiv(program, GL_LINK_STATUS, &linkStatus), linkStatus == GL_FALSE)
        std::cout << "Error occured in glLinkProgram()" << std::endl;

    SkeletalMesh::Scene &sr = SkeletalMesh::Scene::loadScene("Hand", DATA_DIR"/Hand.fbx");
    if (&sr == &SkeletalMesh::Scene::error)
        std::cout << "Error occured in loadMesh()" << std::endl;

    sr.setShaderInput(program, "in_position", "in_texcoord", "in_normal", "in_bone_index", "in_bone_weight");

    
    SkeletalMesh::SkeletonModifier modifier;
    float passed_time = 0;
    float tmp_passed_time = 0;
    glEnable(GL_DEPTH_TEST);
    choice = 10;
    float dtime = 0;
    float tmp_dtime = 0;
    while (!glfwWindowShouldClose(window)) {
        // change_camera(camera_eye, camera_center, camera_up);
        if (update_time) {
            dtime += tmp_dtime;
            tmp_dtime = 0;
            passed_time = (float) glfwGetTime() - dtime;
            tmp_passed_time = (float) glfwGetTime();
        } else {
            tmp_dtime =  glfwGetTime() - tmp_passed_time;
        }
        cur_time = (float) glfwGetTime();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //modify camera
       
        cur_camera = camera::get_camera(last_time, cur_time, next_time, last_camera, nxt_camera);
        
        // --- You may edit below ---

        // Example: Rotate the hand
        // * turn around every 4 seconds
        //l -> r -> l
        #define swing(l,r,sec) (abs(fmod(passed_time,sec) / sec * 2 - 1) * (r - l) + l)
        float zero = 0.f;
        float PI = M_PI;
        float PI_2 = M_PI / 2;
        float PI_3 = M_PI / 3;
        float PI_4 = M_PI / 4;
        float PI_5 = M_PI / 5;
        float PI_6 = M_PI / 6;
        float metacarpals_angle = passed_time * (M_PI / 4.0f);
        // * target = metacarpals
        // * rotation axis = (1, 0, 0)
        //modifier["metacarpals"] = glm::rotate(glm::identity<glm::mat4>(), metacarpals_angle, glm::fvec3(0.0, 1.0, 0.0));
        modifier["metacarpals"] = glm::rotate(glm::identity<glm::mat4>(), metacarpals_angle, glm::fvec3(1.0, 0.0, 0.0));
        float luckycat_angle = swing(0,PI_4,4);
        //modifier["metacarpals"] = glm::rotate(modifier["metacarpals"], luckycat_angle, glm::fvec3(0.0, 0.0, 1.0));
        /**********************************************************************************\
        *
        * To animate fingers, modify modifier["HAND_SECTION"] each frame,
        * where HAND_SECTION can only be one of the bone names in the Hand's Hierarchy.
        *
        * A virtual hand's structure is like this: (slightly DIFFERENT from the real world)
        *    5432 1
        *    ....        1 = thumb           . = fingertip
        *    |||| .      2 = index finger    | = distal phalange
        *    $$$$ |      3 = middle finger   $ = intermediate phalange
        *    #### $      4 = ring finger     # = proximal phalange
        *    OOOO#       5 = pinky           O = metacarpals
        *     OOO
        * (Hand in the real world -> https://en.wikipedia.org/wiki/Hand)
        *
        * From the structure we can infer the Hand's Hierarchy:
        *	- metacarpals
        *		- thumb_proximal_phalange
        *			- thumb_intermediate_phalange
        *				- thumb_distal_phalange
        *					- thumb_fingertip
        *		- index_proximal_phalange
        *			- index_intermediate_phalange
        *				- index_distal_phalange
        *					- index_fingertip
        *		- middle_proximal_phalange
        *			- middle_intermediate_phalange
        *				- middle_distal_phalange
        *					- middle_fingertip
        *		- ring_proximal_phalange
        *			- ring_intermediate_phalange
        *				- ring_distal_phalange
        *					- ring_fingertip
        *		- pinky_proximal_phalange
        *			- pinky_intermediate_phalange
        *				- pinky_distal_phalange
        *					- pinky_fingertip
        *
        * Notice that modifier["HAND_SECTION"] is a local transformation matrix,
        * where (1, 0, 0) is the bone's direction, and apparently (0, 1, 0) / (0, 0, 1)
        * is perpendicular to the bone.
        * Particularly, (0, 0, 1) is the rotation axis of the nearer joint.
        *
        \**********************************************************************************/

        // Example: Animate the index finger
        // * period = 2.4 seconds
        float period = 2.4f;
        float time_in_period = fmod(passed_time, period);
        // * angle: 0 -> PI/3 -> 0
        float thumb_angle = abs(time_in_period / (period * 0.5f) - 1.0f) * (M_PI / 3.0);
        // * target = proximal phalange of the index
        // * rotation axis = (0, 0, 1)
        if (choice == 0){ // thumb
            float angle[4] = {PI_2, PI_2, PI_2, PI_4};
            modifier["metacarpals"] = glm::rotate(glm::identity<glm::mat4>(), PI_2, glm::fvec3(0.0, 1, 0.0));
            modifier["metacarpals"] = glm::rotate(modifier["metacarpals"], metacarpals_angle, glm::fvec3(0.0, 0.0, 1.0));
            modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -PI_6,
                                                          glm::fvec3(0.0, 0, 1));
            modifier["thumb_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -PI_6,
                                                            glm::fvec3(0, 0, 1.0));  
            modifier["thumb_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), -PI_6,
                                                            glm::fvec3(0, 0, 1.0));                                                                                                  
            modifier["thumb_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), -PI_6,
                                                          glm::fvec3(0, 0, 1.0));  
            modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.0, 1.0));  
            modifier["index_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.0, 1.0));                                                                                                  
            modifier["index_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.0, 1.0)); 
            modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.15, 1.0));
            modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.15, 1.0));  
            modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.15, 1.0));                                                                                                  
            modifier["middle_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.15, 1.0)); 
            modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.2, 1.0));
            modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.2, 1.0));  
            modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.2, 1.0));                                                                                                  
            modifier["ring_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.2, 1.0));  
            modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.2, 1.0));
            modifier["pinky_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.2, 1.0));  
            modifier["pinky_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.2, 1.0));                                                                                                  
            modifier["pinky_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.2, 1.0)); 
        }
        else if (choice == 1) { // wave
            float wave_angle = swing(-PI_5, PI_5, period);
            float angle[4] = {0, 0, 0, PI_6};
            modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0, 1));
            modifier["thumb_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0, 0, 1.0));  
            modifier["thumb_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0, 0, 1.0));                                                                                                  
            modifier["thumb_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0, 0, 1.0));  
            modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.0, 1.0));  
            modifier["index_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.0, 1.0));                                                                                                  
            modifier["index_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.0, 1.0)); 
            modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.15, 1.0));
            modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.15, 1.0));  
            modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.15, 1.0));                                                                                                  
            modifier["middle_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.15, 1.0)); 
            modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.2, 1.0));
            modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.2, 1.0));  
            modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.2, 1.0));                                                                                                  
            modifier["ring_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.2, 1.0));  
            modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.2, 1.0));
            modifier["pinky_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.2, 1.0));  
            modifier["pinky_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.2, 1.0));                                                                                                  
            modifier["pinky_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.2, 1.0)); 
            modifier["metacarpals"] = glm::rotate(glm::identity<glm::mat4>(), wave_angle, glm::fvec3(0.0, 1, 0.0));
        }
        else if (choice == 2) {   // grasp
            float angle[4] = {swing(0, PI_2, period), swing(0, PI_2, period), swing(0, PI_2, period), swing(0, PI_4, period)};
            modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, -0.5, 1));
            modifier["thumb_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                            glm::fvec3(0, -0.5, 1.0));  
            modifier["thumb_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                            glm::fvec3(0, -0.5, 1.0));                                                                                                  
            modifier["thumb_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0, -0.5, 1.0));  
            modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.0, 1.0));  
            modifier["index_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.0, 1.0));                                                                                                  
            modifier["index_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.0, 1.0)); 
            modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.15, 1.0));
            modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.15, 1.0));  
            modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.15, 1.0));                                                                                                  
            modifier["middle_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.15, 1.0)); 
            modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.2, 1.0));
            modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.2, 1.0));  
            modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.2, 1.0));                                                                                                  
            modifier["ring_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.2, 1.0));  
            modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.2, 1.0));
            modifier["pinky_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.2, 1.0));  
            modifier["pinky_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.2, 1.0));                                                                                                  
            modifier["pinky_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.2, 1.0));                                                                                                                                    

        }
        else if (choice == 3) { // yeah
            float angle[4] = {PI_2, PI_2, PI_2, PI_4};
            modifier["metacarpals"] = glm::rotate(glm::identity<glm::mat4>(), metacarpals_angle, glm::fvec3(1.0, 0, 0.0));
            modifier["thumb_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0, 1));
            modifier["thumb_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                            glm::fvec3(0, 0, 1.0));  
            modifier["thumb_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                            glm::fvec3(0, 0, 1.0));                                                                                                  
            modifier["thumb_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0, 0, 1.0));  
            modifier["index_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), zero,
                                                          glm::fvec3(0.0, 0.0, 1.0));
            modifier["index_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), zero,
                                                            glm::fvec3(0.0, 0.0, 1.0));  
            modifier["index_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), zero,
                                                            glm::fvec3(0.0, 0.0, 1.0));                                                                                                  
            modifier["index_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), zero,
                                                          glm::fvec3(0.0, 0.0, 1.0)); 
            modifier["middle_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), zero,
                                                          glm::fvec3(0.0, 0.15, 1.0));
            modifier["middle_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), zero,
                                                            glm::fvec3(0.0, 0.15, 1.0));  
            modifier["middle_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), zero,
                                                            glm::fvec3(0.0, 0.15, 1.0));                                                                                                  
            modifier["middle_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), zero,
                                                          glm::fvec3(0.0, 0.15, 1.0)); 
            modifier["ring_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.2, 1.0));
            modifier["ring_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.2, 1.0));  
            modifier["ring_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.2, 1.0));                                                                                                  
            modifier["ring_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.2, 1.0));  
            modifier["pinky_proximal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[0],
                                                          glm::fvec3(0.0, 0.2, 1.0));
            modifier["pinky_intermediate_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[1],
                                                            glm::fvec3(0.0, 0.2, 1.0));  
            modifier["pinky_distal_phalange"] = glm::rotate(glm::identity<glm::mat4>(), angle[2],
                                                            glm::fvec3(0.0, 0.2, 1.0));                                                                                                  
            modifier["pinky_fingertip"] = glm::rotate(glm::identity<glm::mat4>(), angle[3],
                                                          glm::fvec3(0.0, 0.2, 1.0)); 
        }

        // --- You may edit above ---

        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glClearColor(0.5, 0.5, 0.5, 1.0);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 model;
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 view;
        // 注意，我们将矩阵向我们要进行移动场景的反方向移动。
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        //glm::mat4 projection;
        //projection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);

        glUseProgram(program);
        glm::fmat4 mvp = glm::ortho(-12.5f * ratio, 12.5f * ratio, -5.f, 20.f, -20.f, 20.f)
                         *
                         cur_camera.to_mat4();
        glUniformMatrix4fv(glGetUniformLocation(program, "u_mvp"), 1, GL_FALSE, (const GLfloat *) &mvp);
        glUniform1i(glGetUniformLocation(program, "u_diffuse"), SCENE_RESOURCE_SHADER_DIFFUSE_CHANNEL);
        SkeletalMesh::Scene::SkeletonTransf bonesTransf;
        sr.getSkeletonTransform(bonesTransf, modifier);
        if (!bonesTransf.empty())
            glUniformMatrix4fv(glGetUniformLocation(program, "u_bone_transf"), bonesTransf.size(), GL_FALSE,
                               (float *) bonesTransf.data());
        sr.render();
        imgui_draw();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    SkeletalMesh::Scene::unloadScene("Hand");

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}