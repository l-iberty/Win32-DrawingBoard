#ifndef _PAINTER_H_
#define _PAINTER_H_

#define NOMINMAX
#include <Windows.h>
#include <vector>

#include "shape.h"

class Painter {
  public:
    Painter() = default;
    virtual ~Painter() = default;

    Painter(const Painter &) = delete;
    Painter& operator=(const Painter &) = delete;

    virtual void Draw(HDC hdc, const std::vector<POINT> &points, COLORREF brushColor) const = 0;

    virtual void StartDrawing(Shape *shape, const POINT &pt) const = 0;

    virtual void Update(Shape *shape, const POINT &pt) const = 0;
};

#endif // _PAINTER_H_
