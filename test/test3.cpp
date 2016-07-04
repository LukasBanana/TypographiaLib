/*
 * test3.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/Typo.h>
#include <iostream>
#include <chrono>
#include <memory>
#include <array>
#include <functional>
#include <ctime>
#include <thread>

#if defined(_WIN32)
#   include <Windows.h>
#   include <gl/glut.h>
#elif defined(__APPLE__)
#   include <GLUT/glut.h>
#endif


// ----- MACROS -----

#define ENABLE_FULLSCREEN

#define MAX_CODE_LENGTH     27

#define FRAME_DELAY         70 // delay between frames in milliseconds

#define FONT_SIZE           30

#define CODE_CHANGE_PROB    15

#define CODE_COLOR_R        97
#define CODE_COLOR_G        244
#define CODE_COLOR_B        99


// ----- CLASSES -----

class TexturedFont : public Tg::Font
{
    
    public:
        
        TexturedFont(const Tg::FontDescription& desc, const Tg::FontModel& fontModel);
        ~TexturedFont();

        void bind() const;
        void unbind() const;

        const Tg::Size& getSize() const;

    private:
        
        GLuint      texName_ = 0;
        Tg::Size    texSize_;

};

class CodeBuffer
{

    public:

        bool init();

        void resize(int w, int h);

        void draw();

    private:
        
        struct Code
        {
            int t = 0;
            char c = 0;
            double brightness = 1.0;

            void color(unsigned char& r, unsigned char& g, unsigned char& b) const;
            void reset(char chr, double bright);
            void clear();
        };

        struct CodeString
        {
            int x = 0;
            int y = 0;
            double brightness = 1.0;
        };

        char randomChar() const;
        Code& getCode(int x, int y);

        void appendCodePatterns(char begin, char end);

        void appendCodeString();

        std::vector<Code> codes_;
        std::vector<CodeString> codeStrings_;
        std::unique_ptr<TexturedFont> font_;

        std::array<char, 128> codePattern_;

        size_t codePatternSize_ = 0;
        int w_ = 0, h_ = 0;
        int cw_ = 0, ch_ = 0;

};


// ----- VARIABLES -----

using Matrix4x4 = std::array<float, 16>;

int resX = 800, resY = 600;

CodeBuffer codeBuffer;


// ----- CLASS "TexturedFont" FUNCTIONS -----

TexturedFont::TexturedFont(const Tg::FontDescription& desc, const Tg::FontModel& fontModel) :
    Tg::Font( desc, fontModel.glyphSet )
{
    const auto& image = fontModel.image;
    texSize_ = image.GetSize();

    glGenTextures(1, &texName_);

    bind();
    {
        // setup GL parameters for hardware texture and upload pixel data
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize_.width, texSize_.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image.GetImageBuffer().data());
    }
    unbind();
}

TexturedFont::~TexturedFont()
{
    glDeleteTextures(1, &texName_);
}

void TexturedFont::bind() const
{
    glBindTexture(GL_TEXTURE_2D, texName_);
}

void TexturedFont::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

const Tg::Size& TexturedFont::getSize() const
{
    return texSize_;
}


// ----- GLOBAL FUNCTIONS -----

void initGL()
{
    // setup GL configuration
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glFrontFace(GL_CW);
}

std::unique_ptr<TexturedFont> loadFont(const std::string& fontFilename, int size, int flags = 0)
{
    auto fontDesc = Tg::FontDescription{ fontFilename, size, flags };
    return std::unique_ptr<TexturedFont>(new TexturedFont(fontDesc, Tg::BuildFont(fontDesc)));
}

void movePen(int x, int y)
{
    // move GL transformation
    glTranslatef(static_cast<int>(x), static_cast<int>(y), 0.0f);
}

void emitVertex(int x, int y, int tx, int ty, float invTexWidth, float invTexHeight)
{
    // emit data for the next vertex
    glTexCoord2f(invTexWidth * tx, invTexHeight * ty);
    glVertex2i(x, y);
}

void setColor(unsigned char r, unsigned char g, unsigned b)
{
    // set color by bit mask
    glColor4ub(r, g, b, 255);
}

void drawFontGlyph(const Tg::FontGlyph& glyph, float invTexWidth, float invTexHeight)
{
    // move pen to draw the glyph with its offset
    movePen(glyph.xOffset, -glyph.yOffset);

    // draw glyph triangles
    glBegin(GL_TRIANGLE_STRIP);
    {
        emitVertex(0          , glyph.height, glyph.rect.left , glyph.rect.bottom, invTexWidth, invTexHeight);
        emitVertex(0          , 0           , glyph.rect.left , glyph.rect.top   , invTexWidth, invTexHeight);
        emitVertex(glyph.width, glyph.height, glyph.rect.right, glyph.rect.bottom, invTexWidth, invTexHeight);
        emitVertex(glyph.width, 0           , glyph.rect.right, glyph.rect.top   , invTexWidth, invTexHeight);
    }
    glEnd();

    // move pen back to its previous position
    movePen(-glyph.xOffset, glyph.yOffset);
}

void drawBox(int left, int top, int right, int bottom)
{
    glBegin(GL_LINE_LOOP);
    {
        glVertex2i(left , top   );
        glVertex2i(right, top   );
        glVertex2i(right, bottom);
        glVertex2i(left , bottom);
    }
    glEnd();
}

void drawText(
    const TexturedFont& font, int posX, int posY, const std::string& text,
    unsigned char r, unsigned char g, unsigned b)
{
    // setup additive blending, to avoid overdrawing of font glyphs
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // set text color
    setColor(r, g, b);

    font.bind();
    glPushMatrix();
    {
        // move pen to begin with text drawing
        movePen(posX, posY + font.GetDesc().height);

        auto invTexWidth = 1.0f / font.getSize().width;
        auto invTexHeight = 1.0f / font.getSize().height;

        for (const auto& chr : text)
        {
            const auto& glyph = font.GetGlyphSet()[chr];

            // draw current font glyph
            drawFontGlyph(glyph, invTexWidth, invTexHeight);

            // move pen to draw the next glyph
            movePen(glyph.advance, 0);
        }
    }
    glPopMatrix();
    font.unbind();
}

bool isChar(char chr, const std::vector<char>& list)
{
    return std::find(list.begin(), list.end(), chr) != list.end();
}

// sets the matrix 'm' to a planar projection
void setupProjection(Matrix4x4& m)
{
    // build planar projection with a left-top origin in a right-handed coordinate system (for OpenGL)
    m[ 0] = 2.0f/resX;
    m[ 1] = 0.0f;
    m[ 2] = 0.0f;
    m[ 3] = 0.0f;

    m[ 4] = 0.0f;
    m[ 5] = -2.0f/resY;
    m[ 6] = 0.0f;
    m[ 7] = 0.0f;

    m[ 8] = 0.0f;
    m[ 9] = 0.0f;
    m[10] = 1.0f;
    m[11] = 0.0f;

    m[12] = -1.0f;
    m[13] = 1.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
}

void drawScene()
{
    // setup projection matrix
    glMatrixMode(GL_PROJECTION);
    Matrix4x4 projection;
    setupProjection(projection);
    glLoadMatrixf(projection.data());

    // setup model-view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // draw text
    codeBuffer.draw();
}

void displayCallback()
{
    // draw frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        drawScene();
    }
    glutSwapBuffers();
}

void idleCallback()
{
    glutPostRedisplay();
}

void reshapeCallback(GLsizei w, GLsizei h)
{
    resX = w;
    resY = h;

    glViewport(0, 0, w, h);

    displayCallback();
}

void keyboardCallback(unsigned char key, int x, int y)
{
    auto modMask = glutGetModifiers();

    switch (key)
    {
        case 27: // ESC
            exit(0);
            break;
    }
}

void specialCallback(int key, int x, int y)
{
    auto modMask = glutGetModifiers();

    switch (key)
    {
        case GLUT_KEY_HOME:
            break;

        case GLUT_KEY_END:
            break;

        case GLUT_KEY_LEFT:
            break;

        case GLUT_KEY_RIGHT:
            break;

        case GLUT_KEY_INSERT:
            break;
    }
}

int main(int argc, char* argv[])
{
    const auto desktopResX = glutGet(GLUT_SCREEN_WIDTH);
    const auto desktopResY = glutGet(GLUT_SCREEN_HEIGHT);

    srand(time(nullptr));

    std::cout << "Matrix Digital Rain" << std::endl;
    std::cout << "Copyright (c) 2016 by Lukas Hermanns" << std::endl;
    std::cout << " -> https://www.twitter.com/LukasBanana" << std::endl;
    std::cout << " -> https://www.github.com/LukasBanana" << std::endl;
    std::cout << std::endl;
    std::cout << "Font by 'Norfok Incredible Font Design'" << std::endl;
    std::cout << " -> http://www.fontspace.com/norfok-incredible-font-design/matrix-code-nfi" << std::endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    #ifdef ENABLE_FULLSCREEN
    resX = desktopResX;
    resY = desktopResY;
    #endif

    glutInitWindowSize(resX, resY);
    glutInitWindowPosition(desktopResX/2 - resX/2, desktopResY/2 - resY/2);
    glutCreateWindow("Matrix Digital Rain");
    glutSetCursor(GLUT_CURSOR_NONE);
    
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutIdleFunc(idleCallback);
    glutSpecialFunc(specialCallback);
    glutKeyboardFunc(keyboardCallback);

    #ifdef ENABLE_FULLSCREEN
    glutFullScreen();
    #endif

    initGL();

    if (codeBuffer.init())
    {
        codeBuffer.resize(resX, resY);
        glutMainLoop();
    }
    else
    {
        #ifdef _WIN32
        system("pause");
        #endif
    }

    return 0;
}


// ----- CLASS "CodeBuffer" FUNCTIONS -----

bool CodeBuffer::init()
{
    try
    {
        // load font
        font_ = loadFont("matrix_font.ttf", FONT_SIZE);

        appendCodePatterns('a', 'z');
        appendCodePatterns('!', '/');
        appendCodePatterns(':', '?');
    }
    catch (const std::exception& err)
    {
        std::cerr << err.what() << std::endl;
        return false;
    }

    return true;
}

void CodeBuffer::resize(int w, int h)
{
    if (font_)
    {
        auto size = font_->GetDesc().height;
        if (size == 0)
            size = 1;

        cw_ = size*3/5;
        ch_ = size;

        w_ = w / cw_ + 1;
        h_ = h / ch_ + 1;

        codes_.resize(w_*h_);
    }
}

void CodeBuffer::draw()
{
    // Draw codes
    for (int i = 0; i < h_; ++i)
    {
        for (int j = 0; j < w_; ++j)
        {
            auto& c = getCode(j, i);

            if (c.t > 0)
            {
                int x = j*cw_;
                int y = i*ch_;

                // Draw current code character
                unsigned char r, g, b;
                c.color(r, g, b);
                drawText(*font_, x, y, std::string(1, c.c), r, g, b);
                ++c.t;

                if (rand() % CODE_CHANGE_PROB == 0)
                    c.c = randomChar();

                if (c.t > MAX_CODE_LENGTH)
                    c.clear();
            }
        }
    }

    // Process code strings
    for (auto it = codeStrings_.begin(); it != codeStrings_.end();)
    {
        if (it->y < h_)
        {
            getCode(it->x, it->y).reset(randomChar(), it->brightness);
            ++(it->y);
            ++it;
        }
        else
            it = codeStrings_.erase(it);
    }

    // New codes
    auto n = rand() % 5;
    for (int i = 0; i < n; ++i)
        appendCodeString();

    // Wait a moment
    std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_DELAY));
}

CodeBuffer::Code& CodeBuffer::getCode(int x, int y)
{
    return codes_[y*w_ + x];
}

void CodeBuffer::appendCodePatterns(char begin, char end)
{
    do
    {
        codePattern_[codePatternSize_++] = begin;
    }
    while (begin++ != end);
}

static double random()
{
    return static_cast<double>(rand()) / RAND_MAX;
}

static double random(double a, double b)
{
    return a + random()*(b - a);
}

void CodeBuffer::appendCodeString()
{
    CodeString codeString;
    {
        codeString.x = (rand() % w_);
        codeString.y = 0;
        codeString.brightness = random(0.5, 1.0);
    }
    codeStrings_.push_back(codeString);
}

char CodeBuffer::randomChar() const
{
    return codePattern_[rand() % codePatternSize_];
}

void CodeBuffer::Code::reset(char chr, double bright)
{
    t = 1;
    c = chr;
    brightness = bright;
}

void CodeBuffer::Code::clear()
{
    t = 0;
    c = 0;
    brightness = 1.0;
}

static unsigned char fadeColor(unsigned char c, double fade)
{
    return static_cast<unsigned char>(static_cast<double>(c)*fade);
}

static unsigned char fadeColor(unsigned char c1, unsigned char c2, double fade)
{
    auto a = static_cast<double>(c1);
    auto b = static_cast<double>(c2);
    return static_cast<unsigned char>(a + (b - a)*fade);
}

void CodeBuffer::Code::color(unsigned char& r, unsigned char& g, unsigned char& b) const
{
    const auto boundaryStart = MAX_CODE_LENGTH/3;
    const auto boundarySize = MAX_CODE_LENGTH - boundaryStart;

    if (t > boundaryStart)
    {
        auto f = (1.0 - static_cast<double>(t - boundaryStart) / static_cast<double>(boundarySize)) ;
        r = fadeColor(CODE_COLOR_R, f);
        g = fadeColor(CODE_COLOR_G, f);
        b = fadeColor(CODE_COLOR_B, f);
    }
    else if (t <= 4)
    {
        auto f = static_cast<double>(t - 1);
        r = fadeColor(255, CODE_COLOR_R, f);
        g = fadeColor(255, CODE_COLOR_G, f);
        b = fadeColor(255, CODE_COLOR_B, f);
    }
    else
    {
        r = CODE_COLOR_R;
        g = CODE_COLOR_G;
        b = CODE_COLOR_B;
    }

    r = fadeColor(r, brightness);
    g = fadeColor(g, brightness);
    b = fadeColor(b, brightness);
}


