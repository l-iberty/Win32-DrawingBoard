#ifndef _DRAGGER_H_
#define _DRAGGER_H_

#include "shape.h"

class Dragger {
  public:
    Dragger() = default;
    ~Dragger() = default;

    Dragger(const Dragger &) = delete;
    Dragger operator=(const Dragger &) = delete;

    void Start(const POINT &pt) {
        m_start = pt;
    }

    void Drag(Shape *shape, const POINT &pt);

  private:
    POINT m_start;
};

void Dragger::Drag(Shape *shape, const POINT &pt) {
    int dx = pt.x - m_start.x;
    int dy = pt.y - m_start.y;

    // update m_start.
    m_start = pt;

    const std::vector<POINT>  &points = shape->GetPoints();
    for (size_t i = 0; i < points.size(); i++) {
        POINT pt = { points[i].x + dx, points[i].y + dy };
        shape->SetPoint(pt, i);
    }
}


#endif // _DRAGGER_H_
