#ifndef BOX_H
#define BOX_H

// Templated 2D Box class
template<typename T>
class Box {
public:
    Box() = default;
    Box(T xMin, T xMax, T yMin, T yMax) 
        : m_XMin(xMin), m_XMax(xMax), m_YMin(yMin), m_YMax(yMax) {}
    ~Box() = default;
    
    T getXMin() const { return m_XMin; }
    T getXMax() const { return m_XMax; }
    T getYMin() const { return m_YMin; }
    T getYMax() const { return m_YMax; }
    
    void setXMin(T xMin) { m_XMin = xMin; }
    void setXMax(T xMax) { m_XMax = xMax; }
    void setYMin(T yMin) { m_YMin = yMin; }
    void setYMax(T yMax) { m_YMax = yMax; }
    
private:
    T m_XMin{0};
    T m_XMax{0};
    T m_YMin{0};
    T m_YMax{0};
};

#endif // BOX_H

