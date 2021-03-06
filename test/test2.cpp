/*
 * test2.cpp
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
#include <algorithm>

#ifndef TG_UNICODE

#if defined(_WIN32)
#   include <Windows.h>
#   include <gl/glut.h>
#elif defined(__APPLE__)
#   include <GLUT/glut.h>
#elif defined(__linux__)
#   include <GL/gl.h>
#   include <GL/glut.h>
#endif

#ifdef __APPLE__
// Ignore deprecation of GLUT function when compiling with clang on MacOS
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif


// ----- MACROS -----

#define COLOR_WHITE         0xffffffff
#define COLOR_RED           0xff0000ff
#define COLOR_GREEN         0x00ff00ff
#define COLOR_BLUE          0x0000ffff
#define COLOR_YELLOW        0xffff00ff
#define COLOR_LIGHT_BLUE    0x8080ffff

#define SHOW_ROW_COL


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

class Blinker
{

    public:

        Blinker();

        bool visible() const;
        void refresh();

        long long timeInterval = 500;

    private:

        mutable std::chrono::system_clock::time_point lastTime_;

};


// ----- VARIABLES -----

using Matrix4x4 = std::array<float, 16>;

int resX = 800, resY = 600;

bool focusOnTextArea    = true;
bool showTerminal       = false;

Tg::TextFieldString mainTextField(
    //">$ This is an input text field! Use arrows, shift, and ctrl keys"
    "user@PC$ ls -la --color=never | grep -ri \"Foo bar\""
);
Blinker mainTextFieldBlinker;

std::unique_ptr<Tg::TextFieldMultiLineString> mainMlText;
std::unique_ptr<Tg::Terminal> terminal;

std::shared_ptr<TexturedFont> fontSmall, fontLarge, fontTerm;


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

    //auto geometries = Tg::BuildFontGeometrySet(fontModel);
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


// ----- CLASS "Blinker" FUNCTIONS -----

Blinker::Blinker()
{
    refresh();
}

bool Blinker::visible() const
{
    const auto timePoint = std::chrono::system_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint - lastTime_).count();

    if (duration < timeInterval || timeInterval <= 0)
        return true;
    else if (duration > timeInterval*2)
        lastTime_ = timePoint;

    return false;
}

void Blinker::refresh()
{
    lastTime_ = std::chrono::system_clock::now();
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

std::unique_ptr<TexturedFont> loadFont(const std::string& fontName, int size, int flags = 0)
{
    auto fontFilename = Tg::SystemFontPath(fontName, flags);
    auto fontDesc = Tg::FontDescription{ fontFilename, size, flags };
    return std::unique_ptr<TexturedFont>(new TexturedFont(fontDesc, Tg::BuildFont(fontDesc)));
}

bool initScene()
{
    try
    {
        // load font
        #if defined(_WIN32)
        //fontSmall = loadFont("Times New Roman", 32);
        fontSmall = loadFont("Consolas", 20);
        //fontLarge = loadFont("Edwardian Script", 80);
        //fontLarge = loadFont("Arial", 20);
        fontLarge = fontSmall;
        fontTerm = loadFont("Courier New", 20);
        #elif defined(__APPLE__)
        fontSmall = loadFont("Courier New", 32);
        fontLarge = fontSmall;//loadFont("Brush Script", 30);
        fontTerm = fontSmall;
        #elif defined(__linux__)
        fontSmall = loadFont("freefont/FreeMonoBold", 20);
        fontLarge = fontSmall;//loadFont("freefont/FreeSerif", 30);
        fontTerm = fontSmall;
        #endif
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        #ifdef _WIN32
        system("pause");
        #endif
        return false;
    }

    // setup multi-line string
    std::string str = (
        "Hello, World!\n\n"
        "This is a small example of a multi-line string within a restricted screen width\n"
        "The text field can be used for in-game text editors, terminal outputs or whatever.\n\n"
        "Move the cursor with the arrow keys, use Ctrl and Shift keys\n"
        "like you would do in any other text editor\n"
        "By moving the cursor up and down while holding the Ctrl key,\n"
        "you can jump to the previous or next paragraph.\n\n"
        "The interface of this class is straightforward.\n"
        "Here is an example:\n\n"
        "int main() {\n"
        "    /* ... */\n"
        "    Tg::TextFieldMultiLineString textField(fontGlyphSet, 100, \"Example Input Text\");\n"
        "    textField.MoveCursorX(-10);\n"
        "    textField.JumpUp();\n"
        "}\n"
    );

    mainMlText = std::unique_ptr<Tg::TextFieldMultiLineString>(new Tg::TextFieldMultiLineString(fontLarge->GetGlyphSet(), resX, str));
    mainMlText->wrapLines = true;
    mainMlText->StoreMemento();

    terminal = std::unique_ptr<Tg::Terminal>(new Tg::Terminal(fontTerm->GetGlyphSet(), resX));

    mainTextField.cursorLoopEnabled = true;

    terminal->out << "Terminal Example" << std::endl;
    terminal->out << "This should be overwritten!\rOVERWRITTEN";

    return true;
}

void movePen(int x, int y)
{
    // move GL transformation
    glTranslatef(static_cast<float>(x), static_cast<float>(y), 0.0f);
}

void emitVertex(int x, int y, int tx, int ty, float invTexWidth, float invTexHeight)
{
    // emit data for the next vertex
    glTexCoord2f(invTexWidth * tx, invTexHeight * ty);
    glVertex2i(x, y);
}

void setColor(unsigned int color)
{
    // set color by bit mask
    glColor4ub(color >> 24, (color >> 16) & 0xff, (color >> 8) & 0xff, color & 0xff);
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

void drawBox(int left, int top, int right, int bottom, bool lines = false)
{
    glBegin(lines ? GL_LINE_LOOP : GL_QUADS);
    {
        glVertex2i(left , top   );
        glVertex2i(right, top   );
        glVertex2i(right, bottom);
        glVertex2i(left , bottom);
    }
    glEnd();
}

using CharCallback = std::function<void(char)>;

void drawText(
    const TexturedFont& font, int posX, int posY,
    const std::string& text, unsigned int color = COLOR_WHITE,
    const CharCallback& charCallback = nullptr)
{
    // setup additive blending, to avoid overdrawing of font glyphs
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // set text color
    setColor(color);

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

            // character callback
            if (charCallback)
                charCallback(chr);

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

void drawTextField(
    const TexturedFont& font, int posX, int posY,
    const Tg::TextFieldString& textField, unsigned int color = COLOR_WHITE)
{
    // draw selection
    if (textField.IsSelected())
    {
        setColor(COLOR_LIGHT_BLUE);

        Tg::TextFieldString::SizeType start, end;
        textField.GetSelection(start, end);

        drawBox(
            posX + font.TextWidth(textField.GetText(), 0, start),
            posY + 2,
            posX + font.TextWidth(textField.GetText(), 0, end),
            posY + font.GetDesc().height + 7
        );
    }

    // draw text
    drawText(
        font, posX, posY, textField.GetText(), color,
        [color](char chr)
        {
            // change color for special characters
            setColor(isChar(chr, { '#', '$', '|', '+', '*', '<', '>', '_', '@', '=' }) ? COLOR_RED : color);
        }
    );

    // draw cursor
    if (!focusOnTextArea && mainTextFieldBlinker.visible())
    {
        setColor(COLOR_WHITE);

        posX += font.TextWidth(textField.GetText(), 0, textField.GetCursorPosition());

        if (textField.IsInsertionActive())
        {
            drawBox(
                posX,
                posY + font.GetDesc().height + 4,
                posX + font.TextWidth(textField.GetText(), textField.GetCursorPosition(), 1),
                posY + font.GetDesc().height + 5
            );
        }
        else
        {
            drawBox(
                posX,
                posY + 5,
                posX + 1,
                posY + font.GetDesc().height + 5
            );
        }
    }
}

void drawMultiLineText(
    const TexturedFont& font, int posX, int posY,
    const Tg::TextFieldMultiLineString& textArea, unsigned int color = COLOR_WHITE)
{
    // draw bounding box around the text
    setColor(COLOR_WHITE);
    drawBox(posX, posY, posX + textArea.GetMaxWidth(), posY + textArea.GetLines().size()*font.GetDesc().height, true);

    // draw selection
    if (textArea.IsSelected())
    {
        setColor(COLOR_LIGHT_BLUE);

        Tg::Point start, end;
        textArea.GetSelectionCoordinate(start, end);

        if (start.y == end.y)
        {
            drawBox(
                posX + font.TextWidth(textArea.GetLineText(), 0, start.x),
                posY + start.y*font.GetDesc().height + 4,
                posX + font.TextWidth(textArea.GetLineText(), 0, end.x),
                posY + start.y*font.GetDesc().height + font.GetDesc().height + 4
            );
        }
        else
        {
            drawBox(
                posX + font.TextWidth(textArea.GetLineText(start.y), 0, start.x),
                posY + start.y*font.GetDesc().height + 4,
                posX + font.TextWidth(textArea.GetLineText(start.y), 0),
                posY + start.y*font.GetDesc().height + font.GetDesc().height + 4
            );

            for (auto y = start.y + 1; y < end.y; ++y)
            {
                drawBox(
                    posX,
                    posY + y*font.GetDesc().height + 4,
                    posX + font.TextWidth(textArea.GetLineText(y), 0),
                    posY + y*font.GetDesc().height + font.GetDesc().height + 4
                );
            }

            drawBox(
                posX,
                posY + end.y*font.GetDesc().height + 4,
                posX + font.TextWidth(textArea.GetLineText(end.y), 0, end.x),
                posY + end.y*font.GetDesc().height + font.GetDesc().height + 4
            );
        }
    }

    // draw cursor
    auto prevPosX = posX;
    auto prevPosY = posY;

    if (focusOnTextArea && !textArea.GetLines().empty() && mainTextFieldBlinker.visible())
    {
        setColor(COLOR_WHITE);

        const auto& text = textArea.GetLineText();
        posX += font.TextWidth(text, 0, textArea.GetCursorCoordinate().x);
        posY += textArea.GetCursorCoordinate().y * font.GetDesc().height;

        if (textArea.IsInsertionActive())
        {
            drawBox(
                posX,
                posY + font.GetDesc().height + 4,
                posX + font.TextWidth(text, textArea.GetCursorCoordinate().x, 1),
                posY + font.GetDesc().height + 5
            );
        }
        else
        {
            drawBox(
                posX,
                posY + 5,
                posX + 1,
                posY + font.GetDesc().height + 5
            );
        }
    }

    posX = prevPosX;
    posY = prevPosY;

    // draw each text line
    for (const auto& line : textArea.GetLines())
    {
        drawText(font, posX, posY, line.text, color);
        posY += font.GetDesc().height;
    }
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
    #ifdef SHOW_ROW_COL
    if (focusOnTextArea)
    {
        auto coord = mainMlText->GetCursorCoordinate();
        mainTextField.SetText(
            "Ln " + std::string(coord.y < 10 ? " " : "") + std::to_string(coord.y) +
            "    Col " + std::string(coord.x < 10 ? " " : "") + std::to_string(coord.x) +
            "    " + std::string(mainMlText->CanUndo() ? "Undo" : "    ") +
            "    " + std::string(mainMlText->CanRedo() ? "Redo" : "    ")
        );
    }
    #endif

    drawTextField(*fontSmall, 15, 15, mainTextField, COLOR_LIGHT_BLUE);

    static const int border = 15;

    if (showTerminal && terminal)
    {
        terminal->textField.SetMaxWidth(resX - border*2);
        drawMultiLineText(*fontTerm, border, border + 100, terminal->textField);
    }
    else if (mainMlText)
    {
        mainMlText->SetMaxWidth(resX - border*2);
        drawMultiLineText(*fontLarge, border, border + 100, *mainMlText);
    }
}

void displayCallback()
{
    // draw frame
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
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

void putChar(const std::string& s)
{
    if (showTerminal)
        terminal->out << s;
    else if (focusOnTextArea)
    {
        mainMlText->Put(s);
        if (s == "\n")
        {
            auto text = mainMlText->GetLineText(mainMlText->GetCursorCoordinate().y - 1);
            if (text.find(' ', 0) != std::string::npos)
            {
                auto pos = text.find_first_not_of(' ', 0);
                if (pos != std::string::npos)
                    mainMlText->Put(std::string(pos, ' '));
                else
                    mainMlText->Put(std::string(text.size(), ' '));
            }
        }
    }
    else
        mainTextField.Put(s);
}

void putChar(char c)
{
    putChar(std::string(1, c));
}

Tg::TextField& getTextField()
{
    Tg::TextField* textField = &mainTextField;
    if (focusOnTextArea)
        textField = mainMlText.get();
    return *textField;
}

void keyboardCallback(unsigned char key, int x, int y)
{
    auto modMask = glutGetModifiers();

    bool ctrl = ((modMask & GLUT_ACTIVE_CTRL) != 0);

    auto& textField = getTextField();

    switch (key)
    {
        case 27: // ESC
            exit(0);
            break;

        case '\r': // ENTER
            if (focusOnTextArea)
                putChar('\n');
            break;

        case '\t':
            putChar("  ");
            break;

        /*case 127:
            if (ctrl)
                textField.RemoveSequenceLeft();
            else
                textField.Put(char(127));
            break;*/

        default:
            if (key == 1) // CTRL+A
            {
                if (textField.IsAllSelected())
                {
                    textField.Deselect();
                    textField.RestoreSelection();
                }
                else
                {
                    textField.StoreSelection();
                    textField.SelectAll();
                }
            }
            else if (key == 26) // CTRL+Z
                textField.Undo();
            else if (key == 25) // CTRL+Y
                textField.Redo();
            else
                putChar(char(key));
            break;
    }
}

void specialCallback(int key, int x, int y)
{
    auto modMask = glutGetModifiers();

    bool ctrl = ((modMask & GLUT_ACTIVE_CTRL) != 0);
    bool shift = ((modMask & GLUT_ACTIVE_SHIFT) != 0);

    auto& textField = getTextField();

    textField.selectionEnabled = shift;

    switch (key)
    {
        case GLUT_KEY_HOME:
            if (focusOnTextArea)
            {
                if (ctrl)
                    mainMlText->MoveCursorTop();
                else
                    mainMlText->MoveCursorBegin();
            }
            else
                mainTextField.MoveCursorBegin();
            mainTextFieldBlinker.refresh();
            break;

        case GLUT_KEY_END:
            if (focusOnTextArea)
            {
                if (ctrl)
                    mainMlText->MoveCursorBottom();
                else
                    mainMlText->MoveCursorEnd();
            }
            else
                mainTextField.MoveCursorEnd();
            mainTextFieldBlinker.refresh();
            break;

        case GLUT_KEY_LEFT:
            if (ctrl)
                textField.JumpLeft();
            else
                textField.MoveCursor(-1);
            mainTextFieldBlinker.refresh();
            break;

        case GLUT_KEY_RIGHT:
            if (ctrl)
                textField.JumpRight();
            else
                textField.MoveCursor(1);
            mainTextFieldBlinker.refresh();
            break;

        case GLUT_KEY_UP:
            if (focusOnTextArea)
            {
                if (ctrl)
                    mainMlText->JumpUp();
                else
                    mainMlText->MoveCursorLine(-1);
                mainTextFieldBlinker.refresh();
            }
            break;

        case GLUT_KEY_DOWN:
            if (focusOnTextArea)
            {
                if (ctrl)
                    mainMlText->JumpDown();
                else
                    mainMlText->MoveCursorLine(1);
                mainTextFieldBlinker.refresh();
            }
            break;

        case GLUT_KEY_INSERT:
            textField.insertionEnabled = !textField.insertionEnabled;
            mainTextFieldBlinker.refresh();
            break;

        case GLUT_KEY_F1:
            focusOnTextArea = !focusOnTextArea;
            break;

        case GLUT_KEY_F2:
            std::cout << "Selected Text:" << std::endl << textField.GetSelectionText() << std::endl << std::endl;
            break;

        case GLUT_KEY_F3:
            showTerminal = !showTerminal;
            break;
    }
}

int main(int argc, char* argv[])
{
    const int desktopResX = 1920, desktopResY = 1080;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(resX, resY);
    glutInitWindowPosition(desktopResX/2 - resX/2, desktopResY/2 - resY/2);
    glutCreateWindow("TypographiaLib Test 2 (OpenGL, GLUT)");

    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutIdleFunc(idleCallback);
    glutSpecialFunc(specialCallback);
    glutKeyboardFunc(keyboardCallback);

    initGL();
    if (initScene())
        glutMainLoop();

    return 0;
}

#else

int main()
{
    return 0;
}

#endif

