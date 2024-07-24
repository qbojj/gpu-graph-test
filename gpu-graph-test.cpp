#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <random>

constexpr auto vertex_shader = R"(
#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 color;
out vec4 frag_color;

void main() {
  gl_Position = vec4(pos, 0.0, 1.0);
  frag_color = color;
}
)";

constexpr auto fragment_shader = R"(
#version 330 core
in vec4 frag_color;
out vec4 color;

void main() {
  color = frag_color;
}
)";

struct Vertex {
  glm::vec2 pos;
  glm::u8vec4 color;
};
std::vector<Vertex> data;

void gen_data() {
  data.clear();
  std::minstd_rand0 rng{std::random_device{}()};
  std::uniform_real_distribution<> dist{-1.0f, 1.0f};
  std::uniform_int_distribution<> color{0, 255};

  std::size_t constexpr N = 100'000;
  float constexpr step = 2.0f / N;
  data.reserve(N);
  data.push_back({{-1.0f, 0.0f}, {color(rng), color(rng), color(rng), 255}});
  for (std::size_t i = 0; i < N; i++) {
    data.push_back({data.back().pos + glm::vec2{step, dist(rng)}, {color(rng), color(rng), color(rng), 255}});
  }

  // normalize Y
  float min_y = std::numeric_limits<float>::max();
  float max_y = std::numeric_limits<float>::min();
  for (auto& v : data) {
    min_y = std::min(min_y, v.pos.y);
    max_y = std::max(max_y, v.pos.y);
  }

  for (auto& v : data) {
    v.pos.y = (v.pos.y - min_y) / (max_y - min_y) * 2.0f - 1.0f;
  }
}

int main() {
  glfwInit();
  glfwInitHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwInitHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwInitHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwInitHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  glfwMakeContextCurrent(window);

  gladLoadGL((GLADloadfunc)glfwGetProcAddress);

  // enable debug output
  // glEnable(GL_DEBUG_OUTPUT);
  // glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
  //   if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
  //   fprintf(stderr, "GL Debug: %s\n", message);
  // }, nullptr);

  gen_data();

  GLuint vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &vertex_shader, NULL);
  glCompileShader(vert);

  GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, &fragment_shader, NULL);
  glCompileShader(frag);

  GLuint program = glCreateProgram();
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);

  glUseProgram(program);
  glDeleteShader(vert);
  glDeleteShader(frag);

  GLuint vbo;
  glCreateBuffers(1, &vbo);
  glNamedBufferData(vbo, data.size() * sizeof(Vertex), data.data(), GL_STATIC_DRAW);

  GLuint vao;
  glCreateVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glEnableVertexAttribArray(0);
  glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
  glVertexAttribBinding(0, 0);
  
  glEnableVertexAttribArray(1);
  glVertexAttribFormat(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, offsetof(Vertex, color));
  glVertexAttribBinding(1, 0);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  double dt = 0;

  double last_time = glfwGetTime();

  glfwSwapInterval(0);

  while (!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);

    glClear(GL_COLOR_BUFFER_BIT);

    bool update = false;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
      printf("+ update: ");
      gen_data();
      update = true;
    }

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS || update) {
      printf("+ reload: ");
      glNamedBufferData(vbo, data.size() * sizeof(Vertex), data.data(), GL_STATIC_DRAW);
    }
    
    glBindVertexArray(vao);
    glBindVertexBuffer(0, vbo, 0, sizeof(Vertex));
    glDrawArrays(GL_LINE_STRIP, 0, data.size());

    glfwSwapBuffers(window);
    glfwPollEvents();

    double now = glfwGetTime();
    dt = dt * 0.90 + 0.1 * (now - last_time);
    last_time = now;

    printf("dt: %g, fps: %g\n", dt, 1.0 / dt);
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(program);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
