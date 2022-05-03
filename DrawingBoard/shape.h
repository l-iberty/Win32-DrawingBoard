#ifndef _SHAPE_H_
#define _SHAPE_H_

#define NOMINMAX
#include <Windows.h>
#include <vector>

class Shape {
  public:
    Shape() = default;
    virtual ~Shape() = default;

    Shape(const Shape&) = delete;
    Shape &operator=(const Shape&) = delete;

    virtual const std::vector<POINT>& GetPoints() const = 0;

    virtual void AddPoint(const POINT &pt) = 0;

    virtual void ClearPoints() = 0;

    virtual void SetPoint(const POINT &pt, int index) = 0;

    virtual Shape* Reset() const = 0;

    virtual bool Contains(const POINT &pt) const = 0;

    virtual COLORREF GetBrushColor() const = 0;

    virtual void SetBrushColor(COLORREF color) = 0;
};

#endif // _SHAPE_H_
