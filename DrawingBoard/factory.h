#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "shape.h"
#include "painter.h"

class ShapeFactory {
  public:
    ShapeFactory() = default;
    virtual ~ShapeFactory() = default;

    ShapeFactory(const ShapeFactory &) = delete;
    ShapeFactory& operator=(const ShapeFactory &) = delete;

    virtual Shape* CreateShape() = 0;
};

class PainterFactory {
  public:
    PainterFactory() = default;
    virtual ~PainterFactory() = default;

    PainterFactory(const PainterFactory &) = delete;
    PainterFactory& operator=(const PainterFactory &) = delete;

    virtual Painter* CreatePainter() = 0;
};


#endif // _FACTORY_H_
