#include "../DrawingBoard/shape.h"
#include "../DrawingBoard/painter.h"
#include "../DrawingBoard/factory.h"

class MyPolygon : public Shape {
  public:
    MyPolygon() : m_brushColor(RGB(255, 255, 255)) {}
    virtual ~MyPolygon() = default;

    MyPolygon(const MyPolygon&) = delete;
    MyPolygon &operator=(const MyPolygon&) = delete;

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
        return reinterpret_cast<Shape*>(new MyPolygon);
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

//
// https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/PROJ2/InsidePoly.html
// https://blog.csdn.net/zsjzliziyang/article/details/108813349
//
bool MyPolygon::Contains(const POINT &pt) const {
    POINTFLOAT pt0 = { (FLOAT)pt.x, (FLOAT)pt.y };
    std::vector<POINTFLOAT> points;
    for (auto &p : m_points) {
        points.push_back({ (FLOAT)p.x, (FLOAT)p.y });
    }

    size_t crossings = 0, n = points.size();
    for (size_t i = 0; i < n; i++) {
        double slope = (points[(i + 1) % n].y - points[i].y) / (points[(i + 1) % n].x - points[i].x);
        bool cond1 = (points[i].x <= pt0.x) && (pt0.x < points[(i + 1) % n].x);
        bool cond2 = (points[(i + 1) % n].x <= pt0.x) && (pt0.x < points[i].x);
        bool above = (pt0.y < slope * (pt0.x - points[i].x) + points[i].y);
        if ((cond1 || cond2) && above) {
            crossings++;
        }
    }
    return (crossings % 2 != 0);
}

class PolygonFactory: public ShapeFactory {
  public:
    PolygonFactory() = default;
    virtual ~PolygonFactory() = default;

    PolygonFactory(const PolygonFactory &) = delete;
    PolygonFactory& operator=(const PolygonFactory &) = delete;

    virtual Shape* CreateShape() override {
        return new MyPolygon;
    }
};

class PolygonPainter: public Painter {
  public:
    PolygonPainter() = default;
    virtual ~PolygonPainter() = default;

    PolygonPainter(const PolygonPainter &) = delete;
    PolygonPainter& operator=(const PolygonPainter &) = delete;

    virtual void Draw(HDC hdc, const std::vector<POINT> &points, COLORREF brushColor) const override;

    virtual void StartDrawing(Shape *shape, const POINT &pt) const override;

    virtual void Update(Shape *shape, const POINT &pt) const override;
};

void PolygonPainter::Draw(HDC hdc, const std::vector<POINT> &points, COLORREF brushColor) const {
    ::SelectObject(hdc, ::GetStockObject(DC_BRUSH));
    ::SetDCBrushColor(hdc, brushColor);
    ::Polygon(hdc, points.data(), points.size());
}

void PolygonPainter::StartDrawing(Shape *shape, const POINT &pt) const {
    shape->AddPoint(pt);
}

void PolygonPainter::Update(Shape *shape, const POINT &pt) const {
    size_t n = shape->GetPoints().size();
    if (n == 1) {
        shape->AddPoint(pt);
    } else if (n > 1) {
        shape->SetPoint(pt, n - 1);
    }
}

class PolygonPainterFactory : public PainterFactory {
  public:
    PolygonPainterFactory() = default;
    virtual ~PolygonPainterFactory() = default;

    PolygonPainterFactory(const PolygonPainterFactory &) = delete;
    PolygonPainterFactory& operator=(const PolygonPainterFactory &) = delete;

    virtual Painter* CreatePainter() override {
        return new PolygonPainter;
    }
};


extern "C" __declspec(dllexport)
const char *PluginName() {
    return "polygon";
}

extern "C" __declspec(dllexport)
ShapeFactory* CreateShapeFactory() {
    return new PolygonFactory;
}

extern "C" __declspec(dllexport)
PainterFactory* CreatePainterFactory() {
    return new PolygonPainterFactory;
}
