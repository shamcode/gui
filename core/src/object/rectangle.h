/*
 * Labs4Physics - visualisation of physics process
 * Copyright (C) 2013  Leyko Sergey powt81lsyu@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "basic2dentity.h"

#include "core/ogl/vertexbufferobject.h"
#include "core/ogl/vertexarrayobject.h"

class CShader;

namespace core {

class CRectangle : public Basic2dEntity
{
public:
    CRectangle();
    /**
     * @brief Прямоугольник красного цвета с вершинами в точках (-1, -1) (-1, 1) (1, 1) (1, -1).
     */
    explicit CRectangle(const std::string &id);

    virtual ~CRectangle();

    virtual void paint() const;
    virtual void setColor(const glm::vec3 &color);
    virtual float getXMin() const;
    virtual float getXMax() const;
    virtual float getYMin() const;
    virtual float getYMax() const;

    /**
     * @brief Установить координату X левого нижнего угла прямоугольника.
     * @note Плоскость монитора ограничивается интервалом [-1, 1].
     */
    void setX(float x);
    /**
     * @brief Установить координату Y левого нижнего угла прямоугольника.
     * @note Плоскость монитора ограничивается интервалом [-1, 1].
     */
    void setY(float y);
    /**
     * @brief Установить координату X левого нижнего угла прямоугольника.
     * @note Плоскость монитора ограничивается интервалом [-1, 1].
     * @note Длина  - неотрицательное значение!
     */
    void setWidth(float width);
    /**
     * @brief Установить координату X левого нижнего угла прямоугольника.
     * @note Плоскость монитора ограничивается интервалом [-1, 1].
     * @note Длина  - неотрицательное значение!
     */
    void setHeight(float height);
    /**
     * @brief Установить радиус скругления всех углов.
     * @note Длина радиуса - отношение к половине меньшей стороны прямоугольника.
     * @note Для радиуса верно: 0.0f=0%, 1.0f=100% от половины меньшей стороны прямоугольника.
     * @sa setRadiusOfA, setRadiusOfB, setRadiusOfC, setRadiusOfD
     */
    void setRadius(float radius);
    /**
     * @brief Установить радиус скругления угла при вершине A(нижний левый).
     */
    void setRadiusOfA(float rA);
    /**
     * @brief Установить радиус скругления угла при вершине B(верхний левый).
     */
    void setRadiusOfB(float rB);
    /**
     * @brief Установить радиус скругления угла при вершине C(верхний правый).
     */
    void setRadiusOfC(float rC);
    /**
     * @brief Установить радиус скругления угла при вершине D(нижний правый).
     */
    void setRadiusOfD(float rD);
    /**
     * @brief Установить толщину границы.
     */
    void setBorderWidth(float width);
    /**
     * @brief Установить цвет границы
     */
    void setBorderColor(const glm::vec3 &color);
    /**
     * @brief Получение текстуры при помощи менеджера текстур.
     * @param name имя файла текстуры без расширения .dds
     */
    void setTexture(const std::string &name);

protected:
    virtual void configure();

private:
    // Получаю с фабрики, не надо чистить память
    CShader *shader;
    CVertexArrayObject vao;
    CVertexBufferObject vertex;
    CVertexBufferObject color;
    CVertexBufferObject uv;

    float aspect;
    float x;
    float y;
    float width;
    float height;
    float rA;
    float rB;
    float rC;
    float rD;
    uint texture;
    Border border;
}; //class Rectangle

} // namespace Core

#endif // RECTANGLE_H
