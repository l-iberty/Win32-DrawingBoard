#include <algorithm>

#include "../DrawingBoard/shape.h"
#include "../DrawingBoard/painter.h"
#include "../DrawingBoard/factory.h"

class MyRectangle : public Shape {
  public:
    MyRectangle() : m_brushColor(RGB(255, 255, 255)) {}
    virtual ~MyRectangle() = default;

    MyRectangle(const MyRectangle&) = delete;
    MyRectangle &operator=(const MyRectangle&) = delete;

    virtual const std::vector<POINT>& GetPoints() const override {
        return m_points;
    }

    virtual void AddPoint(const POINT &pt) override {
        m_points.push_back(pt);
    }

    virtual void ClearPoints() override {
        m_points.clear();
    }

    virtual void SetPoint(const POINT &pt, int index) override {
        m_points[index] = pt;
    }

    virtual Shape* Reset() const override {
        return reinterpret_cast<Shape*>(new MyRectangle);
    }

    virtual bool Contains(const POINT &pt) const override;

    virtual COLORREF GetBrushColor() const override {
        return m_brushColor;
    }

    virtual void SetBrushColor(COLORREF color) override {
        m_brushColor = color;
    }

  private:
    std::vector<POINT> m_points;
    COLORREF m_brushColor;
};

bool MyRectangle::Contains(const POINT &pt) const {
    int left = std::min(m_points[0].x, m_points[1].x);
    int right = std::max(m_points[0].x, m_points[1].x);
    int top = std::min(m_points[0].y, m_points[1].y);
    int bottom = std::max(m_points[0].y, m_points[1].y);

    return (pt.x > left && pt.x < right && pt.y > top && pt.y < bottom);
}

class RectangleFactory: public ShapeFactory {
  public:
    RectangleFactory() = default;
    virtual ~RectangleFactory() = default;

    RectangleFactory(const RectangleFactory &) = delete;
    RectangleFactory& operator=(const RectangleFactory &) = delete;

    virtual Shape* CreateShape() override {
        return new MyRectangle;
    }
};

class RectanglePainter: public Painter {
  public:
    RectanglePainter() = default;
    virtual ~RectanglePainter() = default;

    RectanglePainter(const RectanglePainter &) = delete;
    RectanglePainter& operator=(const RectanglePainter &) = delete;

    virtual void Draw(HDC hdc, const std::vector<POINT> &points, COLORREF brushColor) const override;

    virtual void StartDrawing(Shape *shape, const POINT &pt) const override;

    virtual void Update(Shape *shape, const POINT &pt) const override;
};

void RectanglePainter::Draw(HDC hdc, const std::vector<POINT> &points, COLORREF brushColor) const {
    ::SelectObject(hdc, ::GetStockObject(DC_BRUSH));
    ::SetDCBrushColor(hdc, brushColor);
    ::Rectangle(hdc, points[0].x, points[0].y, points[1].x, points[1].y);
}

void RectanglePainter::StartDrawing(Shape *shape, const POINT &pt) const {
    shape->ClearPoints();
    shape->AddPoint(pt);
}

void RectanglePainter::Update(Shape *shape, const POINT &pt) const {
    size_t n = shape->GetPoints().size();
    if (n == 1) {
        shape->AddPoint(pt);
    } else if (n > 1) {
        shape->SetPoint(pt, n - 1);
    }
}

class RectanglePainterFactory : public PainterFactory {
  public:
    RectanglePainterFactory() = default;
    virtual ~RectanglePainterFactory() = default;

    RectanglePainterFactory(const RectanglePainterFactory &) = delete;
    RectanglePainterFactory& operator=(const RectanglePainterFactory &) = delete;

    virtual Painter* CreatePainter() override {
        return new RectanglePainter;
    }
};

extern "C" __declspec(dllexport)
const char *PluginName() {
    return "rectangle";
}

extern "C" __declspec(dllexport)
ShapeFactory* CreateShapeFactory() {
    return new RectangleFactory;
}

extern "C" __declspec(dllexport)
PainterFactory* CreatePainterFactory() {
    return new RectanglePainterFactory;
}
