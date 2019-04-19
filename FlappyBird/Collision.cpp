#include "main.h"
#include "Collision.h"
#include <math.h>

namespace games
{
    bool Collision::PointPointHitTest(const D2D1_POINT_2F & p1, const D2D1_POINT_2F & p2, float radius)
    {
        return ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) <= radius * radius);
    }

    bool Collision::PointRectHitTest(const D2D1_POINT_2F & p, const D2D1_RECT_F & r)
    {
        return (r.left <= p.x && r.right >= p.x && r.top <= p.y && r.bottom >= p.y);
    }

    bool Collision::CircleLineHitTest(const D2D1_ELLIPSE & e, const D2D1_POINT_2F & p1, const D2D1_POINT_2F & p2)
    {
        float a = (p1.y - p2.y), b = -(p1.x - p2.x), c = p1.y * (p1.x - p2.x) - p1.x * (p1.y - p2.y);
        float dis = fabs(a * e.point.x + b * e.point.y + c) / sqrtf(a * a + b * b);
        return dis <= e.radiusX;
    }

    bool Collision::OBBCircleHitTest(const D2D1_RECT_F &r1, const D2D1_ELLIPSE &e1)
    {
        D2D1_POINT_2F p1{ r1.left, r1.top }, 
            p2{ r1.right, r1.top },
            p3{ r1.left, r1.bottom }, 
            p4{ r1.right, r1.bottom };

        D2D1_POINT_2F p = e1.point;

        float radius = e1.radiusX;

        if (PointRectHitTest(e1.point, r1))
            return true;

        //�ֱ��жϾ���4��������Բ�ĵľ����Ƿ�<=Բ�뾶�����<=��˵����ײ�ɹ�  
        if (PointPointHitTest(p, p1, radius) ||
            PointPointHitTest(p, p2, radius) ||
            PointPointHitTest(p, p3, radius) ||
            PointPointHitTest(p, p4, radius))
            return true;

        //�жϵ�Բ�ĵ�Y������������ʱX��λ�ã����X��(rectX-arcR)��(rectX+rectW+arcR)�����Χ�ڣ�����ײ�ɹ�  
        if (r1.top <= p.y && r1.bottom >= p.y) 
        {
            if (r1.left <= p.x + radius && r1.right >= p.x - radius)
                return true;
        }
        
        //�жϵ�Բ�ĵ�X������������ʱY��λ�ã����X��(rectY-arcR)��(rectY+rectH+arcR)�����Χ�ڣ�����ײ�ɹ�
        if (r1.left <= p.x && r1.right >= p.x)
        {
            if (r1.top <= p.y + radius && r1.bottom >= p.y - radius)
                return true;
        }

        return false;
    }
}