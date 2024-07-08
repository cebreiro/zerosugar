#pragma once

namespace zerosugar::collision
{
    class Arc;
    class Circle;
    class OBB;
}

namespace zerosugar::collision
{
    bool Intersect(const Arc& arc, const Circle& circle);
    bool Intersect(const OBB& obb, const Circle& circle);

    bool Intersect(const Circle& lhs, const Circle& rhs);
    bool Intersect(const Circle& circle, const Arc& arc);
    bool Intersect(const Circle& circle, const OBB& obb);
}
