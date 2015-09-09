TypographiaLib - A simple C++ library for font drawing
======================================================

License
-------

[3-Clause BSD License](https://github.com/LukasBanana/GaussianLib/blob/master/LICENSE.txt)

Status
------

**Alpha**

Example
-------

```cpp
#include <Typo/Typo.h>

int main()
{
    /* Setup font description and glyph range */
    Tg::FontDescription fontDesc;
    Tg::FontGlyphRange glyphRange;
    
    glyphRange.first = 32;
    glyphRange.last  = 255;
    
    fontDesc.height = 60;//32;
    fontDesc.name   = "MyFontFile.ttf";
    
    /* Build font */
    fontModel = BuildFont(fontDesc, glyphRange);
    
    //store font atlas image 'fonmtModel.image' ...
    
    return 0;
}
```


