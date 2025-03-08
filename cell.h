#pragma once
#ifndef CELL_H
#define CELL_H

class Cell {
private:
    bool m_Alive;
public:
    Cell() : m_Alive(false) {}
    
    void Create() { m_Alive = true; }
    void Erase() { m_Alive = false; }
    bool IsAlive() const { return m_Alive; }
};

#endif // CELL_H