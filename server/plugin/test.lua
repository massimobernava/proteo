
local json=require "json"
local host_enet_test=nil
local zmq_context_test=nil
local zmq_socket_test=nil

--proteo.ffmpeg.avcodec_register_all()
--local codec=proteo.ffmpeg.avcodec_find_encoder(proteo.ffmpeg.CodecID.AV_CODEC_ID_H264)
--local context=proteo.ffmpeg.avcodec_alloc_context(codec)
--proteo.ffmpeg.setparam
--proteo.ffmpeg.avcodec_open(context, codec)
--local frame=proteo.ffmpeg.avcodec_alloc_frame()
--proteo.opencv.frame(cap,image)
--proteo.ffmpeg.copyImage(frame,image)
--local ret,pkt=proteo.ffmpeg.avcodec_encode_video(context, frame)


video_frame=proteo.opencv.img()
img_out=proteo.opencv.img()
probMap=proteo.opencv.img()

net_fast=proteo.opencv.readnet("pose/human-pose-estimation.onnx","")
joint_name={"Nose","Neck","RightShoulder","RightElbow","RightWrist","LeftShoulder","LeftElbow","LeftWrist","RightHip","RightKnee","RightAnkle","LeftHip","LeftKnee","LeftAnkle","RightEye","LeftEye","RightEar","LeftEar"}

function update_event(dt)
	--print("NOW:"..proteo.system.clock())

	if zmq_context_test~=nil then
		print("ZMQ update")
		buffer=proteo.zmq.recv(zmq_socket_test, 10)
		if buffer~=nil then
        	print(buffer)
        	proteo.zmq.send (zmq_socket_test, "World", 5)
        end
	end

	if host_enet_test==nil then return end
	
	local event = host_enet_test:service(0)

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
			proteo.opencv.forwardTable(net_fast,video_frame,0.5,out)

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
        		data.points[joint_name[c+1]]={}
        		data.points[joint_name[c+1]].v=point.max.v
            	data.points[joint_name[c+1]].x=point.max.x*frameWidth / W 
            	data.points[joint_name[c+1]].y=point.max.y*frameHeight / H 
            		--proteo.opencv.circle(img_in,points[c].max.x,points[c].max.y,8,"#FF0000")
        	--end

        	   --print(c.."-X: "..point.max.x.." Y: "..point.max.y.." V: "..point.max.v)
    		end

            js=json.encode(data)
            
            event.peer:send(js)
        end
	end
end

test_timer=proteo.system.createTimer(50,update_event)



proteo.route.get("test/permissions",

	function(username,permission,data,param) 

		local permissions={}
		permissions.permission={}
		permissions.permission[1]="test:starthost"

		return json.encode(permissions)
	end
)

proteo.route.get("test/info",

	function(username,permission,data,param) 
		local info={}
		info["type"]="INFO"
		info["version"]="0.1"
		info["ticket"]=10
		info["result"]="OK"

		return json.encode(info)
	end
)

proteo.route.post("test/starthost",

	function(username,permission,data,param) 
		if host_enet_test==nil then
			host_enet_test=proteo.enet.host_create("*:"..PORT)
			proteo.system.startTimer(test_timer)
		end

		if zmq_context_test==nil then
			zmq_context_test=proteo.zmq.ctx_new()
			zmq_socket_test=proteo.zmq.socket_new(zmq_context_test,proteo.zmq.sockType.ZMQ_REP)
			proteo.zmq.bind(zmq_socket_test,"tcp://*:5555")
		end

		ret={}
		ret.message="OK"
		ret.url=BASEURL..":"..PORT--"localhost:8888"

		return json.encode(ret)
	end
)