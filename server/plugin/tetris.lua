

local json=require "json"
local host_enet_tetris=nil

video_frame=proteo.opencv.img()
img_out=proteo.opencv.img()
probMap=proteo.opencv.img()

net_fast=proteo.opencv.readnet("pose/human-pose-estimation.onnx","")

--host_enet_tetris=proteo.enet.host_create("*:"..PORT)

local function update_tetris(dt)
	if host_enet_tetris==nil then return end
	
	local event = host_enet_tetris:service(0)

	if event then
		if event.type == "connect" then
            print("Connected to", event.peer)

        elseif event.type =="disconnect" then
        	
        elseif event.type == "receive" then
			--print("Got message: ", event.data, event.peer)
			data=json.decode(event.data)
            
            proteo.opencv.imdecode(data["frame"],video_frame)

            out={}
            out["stage_1_output_1_heatmaps"]=img_out
			proteo.opencv.forwardTable(net_fast,video_frame,0.2,out)

			size=proteo.opencv.size(video_frame)
    		frameHeight=size[1]
    		frameWidth=size[2]

    		--print("Frame size: "..frameWidth.."x"..frameHeight)

    		size=proteo.opencv.size(img_out)

    		H=size[3]
    		W=size[4]

    		--print("Out size: "..W.."x"..H)

            data={}
            data.type="POINTS"
            data.points={}

			for c=0,17 do
        		proteo.opencv.sliceImg(img_out,probMap,0,c)
        		point=proteo.opencv.minMaxLoc(probMap)

        		--if points[c].max.v>0.1 then
        		data.points[tostring(c)]={}
        		data.points[tostring(c)].v=point.max.v
            	data.points[tostring(c)].x=point.max.x*frameWidth / W 
            	data.points[tostring(c)].y=point.max.y*frameHeight / H 
            		--proteo.opencv.circle(img_in,points[c].max.x,points[c].max.y,8,"#FF0000")
        	--end

        	   --print(c.."-X: "..point.max.x.." Y: "..point.max.y.." V: "..point.max.v)
    		end

            js=json.encode(data)
            
            event.peer:send(js)
        end
	end
end

t=proteo.system.createTimer(50,update_tetris)
--proteo.system.startTimer(t)


proteo.route.get("tetris/permissions",

	function(username,permission,data,param) 
		local permissions={}
		permissions.permission={}
		permissions.permission[1]="tetris:start"

		return json.encode(permissions)
	end
)

proteo.route.get("tetris/info",

	function(username,permission,data,param) 
		local info={}
		info["type"]="INFO"
		info["version"]="0.1"
		info["ticket"]=10
		info["result"]="OK"

		return json.encode(info)
	end
)

proteo.route.post("tetris/start",

	function(username,permission,data,param) 
		if host_enet_tetris==nil then
			host_enet_tetris=proteo.enet.host_create("*:"..PORT)
			proteo.system.startTimer(t)
		end

		ret={}
		ret.message="OK"
		ret.url=BASEURL..":"..PORT--"localhost:8888"

		return json.encode(ret)
	end
)