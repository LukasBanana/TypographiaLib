/*
 * matrix_digital_rain.cpp
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#define _USE_MATH_DEFINES
#include <cmath>

#include <Typo/Typo.h>
#include <iostream>
#include <chrono>
#include <memory>
#include <array>
#include <functional>
#include <ctime>
#include <thread>
#include <fstream>
#include <sstream>

#if defined(_WIN32)
#   include <Windows.h>
#   include <gl/glut.h>
#elif defined(__APPLE__)
#   include <GLUT/glut.h>
#endif


// ----- MACROS -----

#define WINDOW_WIDTH       1024
#define WINDOW_HEIGHT      768

#define FRAME_DELAY         70 // delay between frames in milliseconds

#define CODE_CHANGE_PROB    15

#define CODE_BOX_PROB       100

#define CODE_BOX_MOVE       0.035
#define CODE_BOX_SIZE       3

#define CODE_COLOR_R        97
#define CODE_COLOR_G        244
#define CODE_COLOR_B        99

#define MIN_CODE_BRIGHTNESS 0.5

#define VERSION_STR         "v1.01"

struct Options
{
    bool load(const std::string& filename);

    bool        fullscreen      = false;
    bool        autoBoxes       = false;
    bool        blending        = true;
    int         fontSize        = 25;//30
    int         maxCodeLength   = 30;
    float       maxBlendScale   = 1.5f;
    std::string showText1       = "THE MATRIX HAS YOU";
    std::string showText2       = "WAKE UP NEO";
    std::string showText3       = "IT IS PURPOSE THAT CREATED US";
};

static Options options;


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
        void update();

        void boxFlash();
        void blendFlash();

        void showText(const std::string& s);

    private:
        
        // --- STRUCTURES --- //

        struct Code
        {
            int t = 0;
            char c = 0;
            double brightness = 1.0;
            bool flash = false;

            void color(unsigned char& r, unsigned char& g, unsigned char& b);
            void reset(char chr, double bright);
            void reset(char chr, double bright, int tm);
            void clear();
        };

        struct CodeString
        {
            int x = 0;
            int y = 0;
            double brightness = 1.0;
        };

        struct CodeBox
        {
            double size = 0.0;
        };

        // --- FUNCTIONS --- //

        int boxLeft(const CodeBox& b) const;
        int boxTop(const CodeBox& b) const;
        int boxRight(const CodeBox& b) const;
        int boxBottom(const CodeBox& b) const;

        int showTextLeft() const;
        int showTextRight() const;
        int showTextY() const;

        char randomChar() const;
        Code& getCode(int x, int y);

        void appendCodePatterns(char begin, char end);
        void appendCodeString();
        void appendCodeString(int x, int y);
        void appendCodeBox();

        // --- MEMBERS --- //

        std::unique_ptr<TexturedFont> font_;

        std::vector<Code> codes_;
        std::vector<CodeString> codeStrings_;
        std::vector<CodeBox> codeBoxes_;

        std::array<char, 128> codePattern_;

        size_t codePatternSize_ = 0;
        int w_ = 0, h_ = 0;
        int cw_ = 0, ch_ = 0;

        bool blendFlash_ = false;
        float blendScale_ = 0.0f;
        float blendAlpha_ = 0.0f;

        std::string showText_;

};


// ----- VARIABLES -----

using Matrix4x4 = std::array<float, 16>;

int resX = WINDOW_WIDTH, resY = WINDOW_HEIGHT;

CodeBuffer codeBuffer;

clock_t prevTimestamp = 0, elapsedTime = 0;


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
    glFrontFace(GL_CW);

    // setup additive blending, to avoid overdrawing of font glyphs
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
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

void emitVertex2i(int x, int y, int tx, int ty, float invTexWidth, float invTexHeight)
{
    // emit data for the next vertex
    glTexCoord2f(invTexWidth * tx, invTexHeight * ty);
    glVertex2i(x, y);
}

void emitVertex2f(float x, float y, int tx, int ty, float invTexWidth, float invTexHeight)
{
    // emit data for the next vertex
    glTexCoord2f(invTexWidth * tx, invTexHeight * ty);
    glVertex2f(x, y);
}

void setColor(unsigned char r, unsigned char g, unsigned char b)
{
    // set color by bit mask
    glColor4ub(r, g, b, 255);
}

void drawFontGlyph(
    const Tg::FontGlyph& glyph, float invTexWidth, float invTexHeight,
    unsigned char r, unsigned char g, unsigned char b, float blendScale)
{
    // move pen to draw the glyph with its offset
    movePen(glyph.xOffset, -glyph.yOffset);

    // draw glyph triangles
    glColor4ub(r, g, b, 255);

    glBegin(GL_TRIANGLE_STRIP);
    {
        emitVertex2i(0          , glyph.height, glyph.rect.left , glyph.rect.bottom, invTexWidth, invTexHeight);
        emitVertex2i(0          , 0           , glyph.rect.left , glyph.rect.top   , invTexWidth, invTexHeight);
        emitVertex2i(glyph.width, glyph.height, glyph.rect.right, glyph.rect.bottom, invTexWidth, invTexHeight);
        emitVertex2i(glyph.width, 0           , glyph.rect.right, glyph.rect.top   , invTexWidth, invTexHeight);
    }
    glEnd();

    if (options.blending)
    {
        static const unsigned char alpha[] = { 130, 80, 50 };
        float s = 1.0f;

        for (int i = 0; i < 3; ++i, s += blendScale)
        {
            glColor4ub(r, g, b, alpha[i]);

            glBegin(GL_TRIANGLE_STRIP);
            {
                emitVertex2f(-s              ,  s + glyph.height, glyph.rect.left , glyph.rect.bottom, invTexWidth, invTexHeight);
                emitVertex2f(-s              , -s               , glyph.rect.left , glyph.rect.top   , invTexWidth, invTexHeight);
                emitVertex2f( s + glyph.width,  s + glyph.height, glyph.rect.right, glyph.rect.bottom, invTexWidth, invTexHeight);
                emitVertex2f( s + glyph.width, -s               , glyph.rect.right, glyph.rect.top   , invTexWidth, invTexHeight);
            }
            glEnd();
        }
    }

    // move pen back to its previous position
    movePen(-glyph.xOffset, glyph.yOffset);
}

void drawText(
    const TexturedFont& font, int posX, int posY, const std::string& text,
    unsigned char r, unsigned char g, unsigned b, float blendScale)
{
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
            drawFontGlyph(glyph, invTexWidth, invTexHeight, r, g, b, blendScale);

            // move pen to draw the next glyph
            movePen(glyph.advance, 0);
        }
    }
    glPopMatrix();
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

void updateElapsedTime()
{
    auto t = clock();
    
    elapsedTime = t - prevTimestamp;

    if (elapsedTime >= FRAME_DELAY)
    {
        elapsedTime -= FRAME_DELAY;
        prevTimestamp += FRAME_DELAY;

        codeBuffer.update();
    }
}

void displayCallback()
{
    updateElapsedTime();

    // draw frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    {
        drawScene();
    }
    glutSwapBuffers();
}

void switchFullscreen(bool enable)
{
    if (options.fullscreen != enable)
    {
        const auto desktopResX = glutGet(GLUT_SCREEN_WIDTH);
        const auto desktopResY = glutGet(GLUT_SCREEN_HEIGHT);

        if (enable)
        {
            resX = desktopResX;
            resY = desktopResY;
            glutPositionWindow(0, 0);
            glutReshapeWindow(resX, resY);
            glutFullScreen();
        }
        else
        {
            resX = WINDOW_WIDTH;
            resY = WINDOW_HEIGHT;
            glutPositionWindow(desktopResX/2 - resX/2, desktopResY/2 - resY/2);
            glutReshapeWindow(resX, resY);
        }

        options.fullscreen = enable;
    }
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

    codeBuffer.resize(resX, resY);
}

void keyboardCallback(unsigned char key, int x, int y)
{
    auto modMask = glutGetModifiers();

    switch (key)
    {
        case 27: // ESC
            exit(0);
            break;

        case '\r': // RETURN
            switchFullscreen(!options.fullscreen);
            break;

        case '1':
            codeBuffer.boxFlash();
            break;

        case '2':
            codeBuffer.blendFlash();
            break;

        case '3':
            codeBuffer.showText(options.showText1);
            break;

        case '4':
            codeBuffer.showText(options.showText2);
            break;

        case '5':
            codeBuffer.showText(options.showText3);
            break;

        case '6':
            codeBuffer.showText("");
            break;
    }
}

int main(int argc, char* argv[])
{
    const auto desktopResX = glutGet(GLUT_SCREEN_WIDTH);
    const auto desktopResY = glutGet(GLUT_SCREEN_HEIGHT);

    srand(time(nullptr));

    std::cout << "Matrix Digital Rain (" << VERSION_STR << ")" << std::endl;
    std::cout << "Copyright (c) 2016 by Lukas Hermanns" << std::endl;
    std::cout << " -> https://www.twitter.com/LukasBanana" << std::endl;
    std::cout << " -> https://www.github.com/LukasBanana" << std::endl;
    std::cout << std::endl;
    std::cout << "Font by 'Norfok Incredible Font Design'" << std::endl;
    std::cout << " -> http://www.fontspace.com/norfok-incredible-font-design/matrix-code-nfi" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << " ESC   -> Quit" << std::endl;
    std::cout << " ENTER -> Switch Fullscreen" << std::endl;
    std::cout << " 1     -> Show 'box flash'" << std::endl;
    std::cout << " 2     -> Show 'blend flash'" << std::endl;
    std::cout << " 3     -> Show text (1)" << std::endl;
    std::cout << " 4     -> Show text (2)" << std::endl;
    std::cout << " 5     -> Show text (3)" << std::endl;
    std::cout << " 6     -> Clear text" << std::endl;

    options.load("matrix_digital_rain.ini");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    if (options.fullscreen)
    {
        resX = desktopResX;
        resY = desktopResY;
        glutInitWindowPosition(0, 0);
        glutInitWindowSize(resX, resY);
    }
    else
    {
        resX = WINDOW_WIDTH;
        resY = WINDOW_HEIGHT;
        glutInitWindowPosition(desktopResX/2 - resX/2, desktopResY/2 - resY/2);
        glutInitWindowSize(resX, resY);
    }

    glutCreateWindow("Matrix Digital Rain");
    glutSetCursor(GLUT_CURSOR_NONE);
    
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutIdleFunc(idleCallback);
    glutKeyboardFunc(keyboardCallback);

    if (options.fullscreen)
        glutFullScreen();

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
        font_ = loadFont("matrix_font.ttf", options.fontSize);

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
    auto size = (font_ != nullptr ? font_->GetDesc().height : 0);
    if (size == 0)
        size = 1;

    cw_ = size*3/5;
    ch_ = size;

    w_ = w / cw_ + 1;
    h_ = h / ch_ + 1;

    codes_.clear();
    codes_.resize(w_*h_);

    codeBoxes_.clear();
    codeStrings_.clear();
}

void CodeBuffer::draw()
{
    // Update blending flash
    if (blendFlash_)
    {
        blendAlpha_ += 10.0;
        if (blendAlpha_ >= 180.0f)
        {
            blendScale_ = options.maxBlendScale;
            blendAlpha_ = 0.0f;
            blendFlash_ = false;
        }
        else
            blendScale_ = options.maxBlendScale + std::sin(blendAlpha_*M_PI/180.0f)*options.maxBlendScale;
    }

    // Update code boxes
    for (auto it = codeBoxes_.begin(); it != codeBoxes_.end();)
    {
        auto left = boxLeft(*it) - CODE_BOX_SIZE;
        auto top = boxTop(*it) - CODE_BOX_SIZE;
        auto right = boxRight(*it) + CODE_BOX_SIZE;
        auto bottom = boxBottom(*it) + CODE_BOX_SIZE;

        if (left >= 0 && top >= 0 && right < w_ && bottom < h_)
        {
            // Flash all codes which are affected by this box
            for (int i = left; i <= right; ++i)
            {
                for (int j = 0; j < CODE_BOX_SIZE; ++j)
                {
                    getCode(i, top + j).flash = true;
                    getCode(i, bottom - j).flash = true;
                }
            }

            for (int i = top + CODE_BOX_SIZE; i <= bottom - CODE_BOX_SIZE; ++i)
            {
                for (int j = 0; j < CODE_BOX_SIZE; ++j)
                {
                    getCode(left + j, i).flash = true;
                    getCode(right - j, i).flash = true;
                }
            }

            // Increase box size
            it->size += CODE_BOX_MOVE;

            ++it;
        }
        else
            it = codeBoxes_.erase(it);
    }

    // Draw codes
    font_->bind();

    for (int i = 0; i < h_; ++i)
    {
        for (int j = 0; j < w_; ++j)
        {
            auto& c = getCode(j, i);

            if (c.t != 0)
            {
                int x = j*cw_;
                int y = i*ch_;

                // Draw current code character
                unsigned char r, g, b;
                c.color(r, g, b);
                drawText(*font_, x, y, std::string(1, c.c), r, g, b, blendScale_);
            }
        }
    }

    font_->unbind();
}

void CodeBuffer::update()
{
    // Update codes
    for (int i = 0; i < h_; ++i)
    {
        for (int j = 0; j < w_; ++j)
        {
            auto& c = getCode(j, i);

            if (c.t > 0)
            {
                ++c.t;

                if (rand() % CODE_CHANGE_PROB == 0)
                    c.c = randomChar();

                if (c.t > options.maxCodeLength)
                    c.clear();
            }
        }
    }

    // Get show text boundary
    auto textLeft = showTextLeft();
    auto textRight = showTextRight();

    // Process code strings
    for (auto it = codeStrings_.begin(); it != codeStrings_.end();)
    {
        if (it->y < h_)
        {
            if (it->y >= 0)
            {
                auto& c = getCode(it->x, it->y);

                // Reset next code
                if (!showText_.empty() && it->y == showTextY() && it->x >= textLeft && it->x < textRight)
                    c.reset(showText_[it->x - textLeft], it->brightness, -1);
                else
                    c.reset(randomChar(), it->brightness);
            }

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

    // Add random effects
    if (options.autoBoxes && (rand() % CODE_BOX_PROB == 0))
        boxFlash();
}

void CodeBuffer::boxFlash()
{
    appendCodeBox();
}

void CodeBuffer::blendFlash()
{
    if (!blendFlash_)
    {
        blendFlash_ = true;
        blendAlpha_ = 0.0f;
    }
}

void CodeBuffer::showText(const std::string& s)
{
    // Clear previous text
    if (!showText_.empty())
    {
        auto textLeft = showTextLeft();
        auto textRight = showTextRight();

        for (int i = 0; size_t(i) < showText_.size(); ++i)
            getCode(textLeft + i, showTextY()).t = 1;
    }

    // Set new text
    showText_ = s.substr(0, w_);

    // Append code strings for the new text
    auto textLeft = showTextLeft();
    auto len = static_cast<int>(showText_.size());
    
    for (int i = 0; i < len; ++i)
        appendCodeString(textLeft + i, -(rand() % len));
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
        codeString.brightness = random(MIN_CODE_BRIGHTNESS, 1.0);
    }
    codeStrings_.push_back(codeString);
}

void CodeBuffer::appendCodeString(int x, int y)
{
    CodeString codeString;
    {
        codeString.x = x;
        codeString.y = y;
        codeString.brightness = random(MIN_CODE_BRIGHTNESS, 1.0);
    }
    codeStrings_.push_back(codeString);
}

void CodeBuffer::appendCodeBox()
{
    codeBoxes_.push_back(CodeBox());
}

char CodeBuffer::randomChar() const
{
    return codePattern_[rand() % codePatternSize_];
}

void CodeBuffer::Code::reset(char chr, double bright)
{
    if (t >= 0)
    {
        t = 1;
        c = chr;
        brightness = bright;
    }
}

void CodeBuffer::Code::reset(char chr, double bright, int tm)
{
    t = tm;
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

void CodeBuffer::Code::color(unsigned char& r, unsigned char& g, unsigned char& b)
{
    const auto boundaryStart = options.maxCodeLength/3;
    const auto boundarySize = options.maxCodeLength - boundaryStart;
    
    if (flash || t < 0)
    {
        r = 255;
        g = 255;
        b = 255;
        flash = false;
    }
    else
    {
        if (t > boundaryStart)
        {
            auto f = (1.0 - static_cast<double>(t - boundaryStart) / static_cast<double>(boundarySize)) ;
            r = fadeColor(CODE_COLOR_R, f);
            g = fadeColor(CODE_COLOR_G, f);
            b = fadeColor(CODE_COLOR_B, f);
        }
        else if (t <= 3)
        {
            auto f = static_cast<double>(t - 1)/2.0;
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
    }

    r = fadeColor(r, brightness);
    g = fadeColor(g, brightness);
    b = fadeColor(b, brightness);
}

int CodeBuffer::boxLeft(const CodeBox& b) const
{
    return w_/2 - static_cast<int>(b.size*(w_/2));
}

int CodeBuffer::boxTop(const CodeBox& b) const
{
    return h_/2 - static_cast<int>(b.size*(h_/2));
}

int CodeBuffer::boxRight(const CodeBox& b) const
{
    return w_/2 + static_cast<int>(b.size*(w_/2));
}

int CodeBuffer::boxBottom(const CodeBox& b) const
{
    return h_/2 + static_cast<int>(b.size*(h_/2));
}

int CodeBuffer::showTextLeft() const
{
    return w_/2 - static_cast<int>(showText_.size()/2);
}

int CodeBuffer::showTextRight() const
{
    return showTextLeft() + static_cast<int>(showText_.size());
}

int CodeBuffer::showTextY() const
{
    return h_/2;
}


bool Options::load(const std::string& filename)
{
    // Read configuration file
    std::ifstream f(filename);
    if (!f.good())
    {
        std::cerr << "failed to open configuration file: \"" << filename << '\"' << std::endl;
        return false;
    }

    // Parse parameters
    std::string line, param;

    int iVal = 0;
    double dVal = 0.0;
    std::string sVal;

    while (std::getline(f, line))
    {
        std::istringstream s(line);

        // Parse parameter and its value
        s >> param;

        if (param.empty())
            continue;

        auto start = line.find('\"', 0);
        if (start != std::string::npos)
        {
            auto end = line.find('\"', start + 1);
            if (end != std::string::npos)
                sVal = line.substr(start + 1, end - start - 1);
        }
        else if (line.find('.', 0) != std::string::npos)
            s >> dVal;
        else
            s >> iVal;

        // Store parameter settings
        if (param == "fullscreen")
            this->fullscreen = (iVal != 0);
        else if (param == "auto_boxes")
            this->autoBoxes = (iVal != 0);
        else if (param == "blending")
            this->blending = (iVal != 0);
        else if (param == "font_size")
            this->fontSize = iVal;
        else if (param == "max_code_length")
            this->maxCodeLength = iVal;
        else if (param == "max_blend_scale")
            this->maxBlendScale = dVal;
        else if (param == "show_text_1")
            this->showText1 = sVal;
        else if (param == "show_text_2")
            this->showText2 = sVal;
        else if (param == "show_text_3")
            this->showText3 = sVal;
        else
            std::cerr << "invalid parameter in initialization file: \"" << param << '\"' << std::endl;
    }

    return true;
}


