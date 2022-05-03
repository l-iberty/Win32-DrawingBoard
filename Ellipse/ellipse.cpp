#include <algorithm>
#include <cmath>

#include "../DrawingBoard/shape.h"
#include "../DrawingBoard/painter.h"
#include "../DrawingBoard/factory.h"

class MyEllipse : public Shape {
  public:
    MyEllipse() : m_brushColor(RGB(255, 255, 255)) {}
    virtual ~MyEllipse() = default;

    MyEllipse(const MyEllipse&) = delete;
    MyEllipse &operator=(const MyEllipse&) = delete;

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
        return reinterpret_cast<Shape*>(new MyEllipse);
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

bool MyEllipse::Contains(const POINT &pt) const {
    int left = std::min(m_points[0].x, m_points[1].x);
    int right = std::max(m_points[0].x, m_points[1].x);
    int top = std::min(m_points[0].y, m_points[1].y);
    int bottom = std::max(m_points[0].y, m_points[1].y);

    double x0 = (left + right) / 2;
    double y0 = (top + bottom) / 2;
    double a = (right - left) / 2;
    double b = (bottom - top) / 2;

    double c = std::pow(pt.x - x0, 2) / std::pow(a, 2) + std::pow(pt.y - y0, 2) / std::pow(b, 2);

    return c < 1.0;
}

class EllipseFactory: public ShapeFactory {
  public:
    EllipseFactory() = default;
    virtual ~EllipseFactory() = default;

    EllipseFactory(const EllipseFactory &) = delete;
    EllipseFactory& operator=(const EllipseFactory &) = delete;

    virtual Shape* CreateShape() override {
        return new MyEllipse;
    }
};

class EllipsePainter : public Painter {
  public:
    EllipsePainter() = default;
    virtual ~EllipsePainter() = default;

    EllipsePainter(const EllipsePainter &) = delete;
    EllipsePainter& operator=(const EllipsePainter &) = delete;

    virtual void Draw(HDC hdc, const std::vector<POINT> &points, COLORREF brushColor) const override;

    virtual void StartDrawing(Shape *shape, const POINT &pt) const override;

    virtual void Update(Shape *shape, const POINT &pt) const override;
};

void EllipsePainter::Draw(HDC hdc, const std::vector<POINT> &points, COLORREF brushColor) const {
    ::SelectObject(hdc, ::GetStockObject(DC_BRUSH));
    ::SetDCBrushColor(hdc, brushColor);
    ::Ellipse(hdc, points[0].x, points[0].y, points[1].x, points[1].y);
}

void EllipsePainter::StartDrawing(Shape *shape, const POINT &pt) const {
    shape->ClearPoints();
    shape->AddPoint(pt);
}

void EllipsePainter::Update(Shape *shape, const POINT &pt) const {
    size_t n = shape->GetPoints().size();
    if (n == 1) {
        shape->AddPoint(pt);
    } else if (n > 1) {
        shape->SetPoint(pt, n - 1);
    }
}

class EllipsePainterFactory : public PainterFactory {
  public:
    EllipsePainterFactory() = default;
    virtual ~EllipsePainterFactory() = default;

    EllipsePainterFactory(const EllipsePainterFactory &) = delete;
    EllipsePainterFactory& operator=(const EllipsePainterFactory &) = delete;

    virtual Painter* CreatePainter() override {
        return new EllipsePainter;
    }
};


extern "C" __declspec(dllexport)
const char *PluginName() {
    return "ellipse";
}

extern "C" __declspec(dllexport)
ShapeFactory* CreateShapeFactory() {
    return new EllipseFactory;
}

extern "C" __declspec(dllexport)
PainterFactory* CreatePainterFactory() {
    return new EllipsePainterFactory;
}
