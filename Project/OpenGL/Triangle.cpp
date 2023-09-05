#include "triangle.h"

using namespace std;

float vertices[] =
{
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
    -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f
};
unsigned int indices[] = 
{
    0, 2, 4,
    0, 6, 8,
    0, 10, 12,
    0, 14, 16
};
//��ʱ������ɫ��
const char* m_vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 outPos;\n"
"out vec3 outColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   outPos = aPos;\n"
"   outColor = aColor;\n"
"}\0";

//��ʱƬ����ɫ��
const char* m_fragmentShaderSource = "#version 330 core\n"
"in vec3 outPos;"
"in vec3 outColor;"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(outColor * sqrt(outPos.x*outPos.x + outPos.y*outPos.y),1.0);\n"
"}\0";

//�ߴ�仯�ص�
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//���봦��
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

//����shader
bool CompileShader(unsigned int shader, const char* vertexShaderSource)
{
    int  success;
    char infoLog[512];

    /*glShaderSource������Ҫ�������ɫ��������Ϊ��һ��������
    �ڶ�����ָ���˴��ݵ�Դ���ַ�������������ֻ��һ����
    �����������Ƕ�����ɫ��������Դ�룬
    ���ĸ���������������ΪNULL��*/
    glShaderSource(shader, 1, &vertexShaderSource, NULL);

    //���Ա���
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
        return false;
    }
    else
    {
        return true;
    }
}

int Triangle()
{
#pragma region GLFW ��ʼ���Լ�����

    //��ʼ��GLFW
    glfwInit();

    /*ʹ��glfwWindowHint����������GLFW��
    glfwWindowHint�����ĵ�һ����������ѡ������ƣ����ǿ��ԴӺܶ���GLFW_��ͷ��ö��ֵ��ѡ��
    �ڶ�����������һ�����ͣ������������ѡ���ֵ��
    �ú��������ο���https://www.glfw.org/docs/latest/window.html#window_hints*/
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#pragma endregion

#pragma region ��������

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);

    //���ڳߴ緢���仯�ص�
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

#pragma endregion

#pragma region ��Ⱦ����

    //���㻺�����(Vertex Buffer Objects)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    /*glBufferData��һ��ר���������û���������ݸ��Ƶ���ǰ�󶨻���ĺ�����
    ���ĵ�һ��������Ŀ�껺������ͣ����㻺�����ǰ�󶨵�GL_ARRAY_BUFFERĿ���ϡ�
    �ڶ�������ָ���������ݵĴ�С(���ֽ�Ϊ��λ)����һ���򵥵�sizeof������������ݴ�С���С�
    ����������������ϣ�����͵�ʵ�����ݡ�
    ���ĸ�����ָ��������ϣ���Կ���ι�����������ݡ�����������ʽ��
    GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
    GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
    GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣
    �����ε�λ�����ݲ���ı䣬ÿ����Ⱦ����ʱ������ԭ������������ʹ�����������GL_STATIC_DRAW��
    ���������˵һ�������е����ݽ�Ƶ�����ı䣬��ôʹ�õ����;���GL_DYNAMIC_DRAW��GL_STREAM_DRAW����������ȷ���Կ������ݷ����ܹ�����д����ڴ沿�֡�*/
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /*glVertexAttribPointer�����Ĳ����ǳ��࣬�����һ���һ�������ǣ�
    ��һ������ָ������Ҫ���õĶ������ԡ�
    �ڶ�������ָ���������ԵĴ�С������������һ��vec3������3��ֵ��ɣ����Դ�С��3��
    ����������ָ�����ݵ����ͣ�������GL_FLOAT(GLSL��vec*�����ɸ�����ֵ��ɵ�)��
    �¸��������������Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣���ǰ�������ΪGL_FALSE��
    �����������������(Stride)�������������˼��˵���Ǵ�������Եڶ��γ��ֵĵط�����������0λ��֮���ж����ֽڡ�
    ���һ��������������void*��������Ҫ���ǽ��������ֵ�ǿ������ת��������ʾλ�������ڻ�������ʼλ�õ�ƫ����(Offset)������λ������������Ŀ�ͷ������������0��*/
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //������ɫ��
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    CompileShader(vertexShader, m_vertexShaderSource);
    //Ƭ����ɫ��
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    CompileShader(fragmentShader, m_fragmentShaderSource);
    //��ɫ��
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //��Ⱦѭ��
    while (!glfwWindowShouldClose(window))
    {
        //����
        processInput(window);

        //�����ɫ����
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //������ɫ��
        glUseProgram(shaderProgram);
        //���л���
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        //��������
        glfwSwapBuffers(window);
        //��ѯIO�¼�
        glfwPollEvents();
    }

    glfwTerminate();

#pragma endregion


    return 0;
}