/*
 * SystemFontPath.mm (MacOS)
 * 
 * This file is part of the "TypographiaLib" project (Copyright (c) 2015 by Lukas Hermanns)
 * See "LICENSE.txt" for license information.
 */

#include <Typo/SystemFontPath.h>

#import <Foundation/Foundation.h>


#define MAX_FILENAME_SIZE 512

static bool GetFontFile(const std::string& fontName, std::string& fontFilename)
{
    static char result[MAX_FILENAME_SIZE] = { 0 };
    
    NSString* homeDir = NSHomeDirectory();
    NSString* fontPathUser = [homeDir stringByAppendingString:@"/Library/Fonts/"];
    NSString* fontPathSystem = @"/Library/Fonts/";
    
    NSArray* fontPaths = [NSArray arrayWithObjects:fontPathSystem, fontPathUser, nil];

    NSFileManager* fileMngr = [NSFileManager defaultManager];
    
    NSMutableString* filename = [NSMutableString stringWithUTF8String:fontName.c_str()];
    [filename appendString:@".ttf"];
    
    /* If file exists, return its filename */
    for (id path in fontPaths)
    {
        NSString* fullPath = [path stringByAppendingString:filename];
        if ([fileMngr isReadableFileAtPath:fullPath] == YES)
        {
            strcpy(result, [fullPath UTF8String]);
            fontFilename = std::string(result);
            return true;
        }
    }
    
    return false;
}


namespace Tg
{


std::string SystemFontPath(std::string fontName, int flags)
{
    std::string fontFilename;
    GetFontFile(fontName, fontFilename);
    return fontFilename;
}


} // /namespace Tg



// ================================================================================
