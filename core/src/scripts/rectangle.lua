function ui:rectangle(id)
  return function(data)
    local r = Rectangle.new(id)
    for k, v in pairs(data) do
      if k == "x" then
        r:setX(v)
      elseif k == "y" then
        r:setY(v)
      elseif k == "width" then
        r:setWidth(v)
      elseif k == "height" then
        r:setHeight(v)
      elseif k == "color" then
        r:setColor(v)
      elseif k == "texture" then
        r:setTexture(v) -- READ ONLY!!!
      elseif k == "radius" then
        r:setRadius(v)
      elseif k == "radiusOfA" then
        r:setRadiusOfA(v)
      elseif k == "radiusOfB" then
        r:setRadiusOfB(v)
      elseif k == "radiusOfC" then
        r:setRadiusOfC(v)
      elseif k == "radiusOfD" then
        r:setRadiusOfD(v)
      elseif k == "alpha" then
        r:setAlpha(v)
      elseif k == "border" then
        r:setBorderWidth(v.width) -- READ ONLY!!! TODO: MAKE WRITE
        r:setBorderColor(v.color) -- READ ONLY!!! TODO: MAKE WRITE
      else
        -- if k is number
        r:addChild(v)
      end
    end
    ui[id] = r
    r:sync()
    return r
  end
end
