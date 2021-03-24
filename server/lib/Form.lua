local LCS = require 'LCS'

proteo.gui.ControlAnchor={}
proteo.gui.ControlAnchor["Center"] = 0
proteo.gui.ControlAnchor["CenterLeft"] =  1
proteo.gui.ControlAnchor["CenterRight"] =  2
proteo.gui.ControlAnchor["TopCenter"] =  3
proteo.gui.ControlAnchor["TopLeft"] =  4
proteo.gui.ControlAnchor["TopRight"] =  5
proteo.gui.ControlAnchor["BottomCenter"] =  6
proteo.gui.ControlAnchor["BottomLeft"] =  7
proteo.gui.ControlAnchor["BottomRight"] =  8

----FORM----------
gForms = {}
Form =  LCS.class({name = 'Form'})

function Form:init(name)
self.name = name
self.controls = {}
self.backgroundColor='Black'

gForms[#gForms+1]=self
end

function Form:addControl(control)
self.controls[#self.controls+1]=control
end

function Form:show()

print("SHOW: " .. self.name)

proteo.gui.suspendDrawing(true)

for i=1,#gForms do
gForms[i]:hide(true)
end

--for i=1,#self.controls do
--	proteo.gui.setHidden(self.controls[i],false)
--	proteo.graphics.setHidden(self.controls[i],false)
--end
self:hide(false)

proteo.gui.suspendDrawing(false)
--proteo.graphics.setBackground(self.backgroundColor)

end

function Form:hide(hide)

for i=1,#self.controls do
proteo.gui.setHidden(self.controls[i],hide)
--proteo.graphics.setHidden(self.controls[i],hide)
end

if not hide then
proteo.graphics.setBackground(self.backgroundColor)
end

end
------------------

----GRIDFORM----------
GridForm = Form:extends() --LCS.class {name = 'GridForm'}

-- Creates and returns a new grid
function GridForm:init(name,width,height)
self.height=height
self.width=width
self.name=name
self.backgroundColor='Black'
self.margin=5
self.grid = {}
self.anchor = {}
self.grid_lines = {}
self.grid_rectangles= {}
self.grid_rectangles_y= {}
self.hiddenGrid=true
self.y_scroll=0

gForms[#gForms+1]=self
end

-- Clears the grid
function GridForm:clear()
self.grid = {}
self.anchor = {}
self:removeGrid()
self.grid_rectangles= {}
end

-- Creates a copy of the grid and returns it
function GridForm:copy()
local grid = GridForm()
for x,y,v in self:iterate() do
grid:set(x,y,v)
end
return grid
end

-- Sets the value of a single cell
function GridForm:addControl(x, y,anchor, value)
if not self.grid[x] then self.grid[x] = {} end
self.grid[x][y] = value

if not self.anchor[x] then self.anchor[x] = {} end
self.anchor[x][y] = anchor

if x>self.width then self.width=x end
if y>self.height then self.height=y end
end

-- Gets the value of a single cell
function GridForm:getControl(x, y)
return self.grid[x] and self.grid[x][y]
end

function GridForm:removeGrid()
for i=1,#self.grid_lines do
proteo.system.remove(self.grid_lines[i])
end
self.grid_lines={}
end

function GridForm:createGrid()

self:removeGrid()

local h=(MAX_Y / self.height)
local v=(MAX_X / self.width)
--H
for i=1,self.height-1 do
lineh=proteo.graphics.newLine(self.name .."_hline"..i,"White",1.0,MIN_X,i*h,MAX_X,i*h)
self.grid_lines[#self.grid_lines +1]=lineh
end
--V
for i=1,self.width-1 do
linev=proteo.graphics.newLine(self.name .."_vline"..i,"White",1.0,i*v,MIN_Y,i*v,MAX_Y)
self.grid_lines[#self.grid_lines +1]=linev
end

end

function GridForm:setColor(color,from_x,from_y,to_x,to_y)
local h=(MAX_Y / self.height)
local v=(MAX_X / self.width)

crect=proteo.graphics.newRect(self.name .."_crect",color,color,(from_x-1)*v,(from_y-1)*h,(to_x-from_x+1)*v,(to_y-from_y+1)*h)
self.grid_rectangles[#self.grid_rectangles +1]=crect
self.grid_rectangles_y[#self.grid_rectangles]=from_y*h
end

function GridForm:hideGrid(hide)
self.hiddenGrid=hide
end

function GridForm:scroll(y_scroll)

self.y_scroll=y_scroll

--for i=1,#self.grid_rectangles do
--proteo.graphics.setHidden(self.grid_rectangles[i],y_scroll~=0)
--end

self:updatePosition()
end

function GridForm:updatePosition()
for x, y, v in self:iterate() do
local x_anchor=0
local y_anchor=0
local x_margin=0
local y_margin=0

if self.anchor[x][y] == proteo.gui.ControlAnchor.Center then x_anchor=0.5 y_anchor=0.5
elseif self.anchor[x][y] == proteo.gui.ControlAnchor.CenterLeft then x_anchor=1 y_anchor=0.5 x_margin=self.margin end

local pos_x= x_margin + (MAX_X / self.width) * (x - x_anchor)
local pos_y= y_margin + (MAX_Y / self.height) * (y - y_anchor)
--print(self.name .." x: ".. pos_x .. " y: " .. pos_y)
proteo.gui.setPosition(v,pos_x,pos_y+self.y_scroll,self.anchor[x][y])
end

for i=1,#self.grid_rectangles do
pos=proteo.graphics.getPosition(self.grid_rectangles[i])
proteo.graphics.setPosition(self.grid_rectangles[i],pos.x,self.grid_rectangles_y[i]+self.y_scroll)
end
end

-- Iterate over all values in the grid using a for loop
-- Example:
-- for x, y, v in grid:iterate() do grid2:set(x, y, v) end
function GridForm:iterate()
local x, row = next(self.grid)
if x == nil then return function() end end
local y, val
return function()
repeat
y,val = next(row,y)
if y == nil then x,row = next(self.grid, x) end
until (val and x and y) or (not val and not x and not y)
return x,y,val
end
end

function GridForm:hide(hide)
for x, y, v in self:iterate() do
proteo.gui.setHidden(v,hide)
--proteo.graphics.setHidden(v,hide)
end

for i=1,#self.grid_rectangles do
proteo.graphics.setHidden(self.grid_rectangles[i],hide)
end

local tmp_hide=true
if hide then
tmp_hide=true
else
tmp_hide=self.hiddenGrid
end

for i=1,#self.grid_lines do
proteo.graphics.setHidden(self.grid_lines[i],tmp_hide)
end

if not hide then
proteo.graphics.setBackground(self.backgroundColor)
end
end

------------------
