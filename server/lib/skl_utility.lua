
local inspect = require "inspect"

function svg2shape (svgfile, shape)

for i=1,#svgfile do
        if VERBOSE then print("Shape "..i..") "..svgfile[i].id.." color: "..svgfile[i].fill_color) end
        for j=1,#svgfile[i] do
            if VERBOSE then print("   Path "..j) end
            local p=proteo.graphics.newPolygon(svgfile[i].id.."_"..j,"clear",svgfile[i].fill_color,0,0)
            proteo.graphics.addItem(shape,p)
            for k=1,#svgfile[i][j] do
                if VERBOSE then rint("      Bezier "..k..") x0:"..svgfile[i][j][k].x0.." y0:"..svgfile[i][j][k].y0.." x3:"..svgfile[i][j][k].x3.." y3:"..svgfile[i][j][k].y3) end
                proteo.graphics.addBezier(p,svgfile[i][j][k].x0,svgfile[i][j][k].y0,
                    svgfile[i][j][k].x1,svgfile[i][j][k].y1,
                    svgfile[i][j][k].x2,svgfile[i][j][k].y2,
                    svgfile[i][j][k].x3,svgfile[i][j][k].y3)
            end
        end
    end

end

function json2skl(jsonfile,skeleton)
    for k, j in pairs(jsonfile.joints) do
        if VERBOSE then print("Joint ("..k..") x: "..j.x.." y: "..j.y) end
        proteo.graphics.addJoint(k,skeleton,j.x,j.y)
    end
    for k, b in pairs(jsonfile.bones) do
        if VERBOSE then print("Bone ("..k..") a: "..b.a.." yb: "..b.b.." group: "..b.group) end
        proteo.graphics.addBone(k,skeleton,b.a,b.b,b.group)
    end
end

function tprint (tbl, indent)
  if not indent then indent = 0 end
  for k, v in pairs(tbl) do
    formatting = string.rep("  ", indent) .. k .. ": "
    if type(v) == "table" then
      print(formatting)
      tprint(v, indent+1)
    elseif type(v) == 'boolean' then
      print(formatting .. tostring(v))      
    else
      print(formatting .. v)
    end
  end
end