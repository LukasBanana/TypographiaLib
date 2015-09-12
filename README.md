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
This library makes use of the FreeType project (see http://freetype.org/)

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
    
    return 0;
}
```

Font Atlas Image Examples
-------------------------

<p align="center">Times New Roman, 34px:</p>

<p align="center"><img src="media/fontatlas_times.png"/></p>

![Times New Roman, 34px](media/fontatlas_times.png)

Kaiu, 34px:
![Kaiu, 34px](media/fontatlas_kaiu.png)

Edwardian Script ITC, 60px:
![Edwardian Script ITC, 60px](media/fontatlas_edwardian_script.png)

</p>
