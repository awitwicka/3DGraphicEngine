#ifndef CADLOADER_H
#define CADLOADER_

#include "widget.h"

class CADLoader
{
    Widget *context;
public:
    CADLoader();
    CADLoader(Widget *context);
    ~CADLoader();
    void LoadFile();
    void SaveFile();

};

#endif // CADLOADER_H
