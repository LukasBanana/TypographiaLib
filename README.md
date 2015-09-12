TypographiaLib - A simple C++ library for font drawing
======================================================

License
-------

[3-Clause BSD License](https://github.com/LukasBanana/GaussianLib/blob/master/LICENSE.txt)

Status
------

**Alpha**

Dependencies
------------
This library makes use of the *FreeType* project (see http://freetype.org/)

Example
-------

```cpp
#include <Typo/Typo.h>

int main()
{
    /*
    Build font with size 60 pixels.
    This will generate a font atlas image with tightly packed font glyphs.
    */
    auto fontModel = BuildFont({ "MyFontFile.ttf", 60 });
    
    //store font atlas image 'fontModel.image' ...
    //  use 'fontModel.image.GetSize()',
    //  or 'fontModel.image.GetImageBuffer()',
    //  or 'fontModel.glyphSet'
    
    /*
    Text field example. In a model-view-controller pattern, the "TextFieldString"
    is a model to easily manage a user input text field (like in a command line).
    */
    TextFieldString<char> textField;

    textField += "Hello, World";
    textField.MoveCursorBegin();     std::cout << std::string(textField) << std::endl;
    textField.MoveCursor(3);         std::cout << std::string(textField) << std::endl;
    textField.RemoveSequenceRight(); std::cout << std::string(textField) << std::endl;
    textField.InsertEx(char(127));   std::cout << std::string(textField) << std::endl;
    textField.InsertEx("ABC");       std::cout << std::string(textField) << std::endl;
    
    return 0;
}
```

Font Atlas Image Examples
-------------------------

<p align="center">Times New Roman, 34px, Glyphs [32, 255]:</p>
<p align="center"><img src="media/fontatlas_times.png" alt="media/fontatlas_times.png"/></p>

<p align="center">Kaiu, 34px, Glyphs [0x8C22, 0x8C35]:</p>
<p align="center"><img src="media/fontatlas_kaiu.png" alt="media/fontatlas_kaiu.png"/></p>

<p align="center">Edwardian Script ITC, 60px, Glyphs [32, 128]:</p>
<p align="center"><img src="media/fontatlas_edwardian_script.png" alt="media/fontatlas_edwardian_script.png"/></p>
