
local json=require "json"

require "tfl_blazepose"
require "tfl_blazeface"
require "tfl_emotion"

local deepcrimson_data={}

deepcrimson_data.zmq_socket=nil
deepcrimson_data.zmq_context=nil

deepcrimson_data.sessions={}

deepcrimson_data.pipe = function (json_data)
  data = json.decode(json_data)

  --[[data["type"] -- FRAME, VIDEO, IMAGE ?
  data["session"] 
  data["frame"]
  data["encoding"] -- JPEG, H264
  data["request"] --TFLFACE, TFLPOSE
  data["frameback"] --true, false
  data["save"] ]]--

  	session=data["session"]
	ret={}

  	if deepcrimson_data.sessions[session]==nil then
  		
  		ret['type']='ERROR'
  		ret['data']=""

  		return json.encode(ret)
  	end

	if data["encoding"]=="JPEG" then
  		proteo.opencv.imdecode(data['frame'],deepcrimson_data.sessions[session].frame)
	end


	if data["request"]=="TFLFACE" then
		bbox = invoke_face_detect (deepcrimson_data.sessions[session].frame)
        size=proteo.opencv.size(deepcrimson_data.sessions[session].frame)
        for i=1,#bbox do
                --print("Generate Landmark BBox "..i)
            feed_image=generate_face_landmark_input_image (deepcrimson_data.sessions[session].frame, bbox[i])
            bbox[i].landmarks=invoke_face_landmark (feed_image)
        end

        deepcrimson_data.sessions[session].bbox=bbox

       	ret["type"]="BBOX"
       	ret['request']='TFLFACE'
        ret["data"]=bbox
        ret["size"]=size

	elseif data["request"]=="TFLPOSE" then
		bbox = invoke_pose_detect(deepcrimson_data.sessions[session].frame)
  		size = proteo.opencv.size(deepcrimson_data.sessions[session].frame)
  		for i=1,#bbox do
    		feed_image = generate_pose_landmark_input_image(deepcrimson_data.sessions[session].frame,bbox[i])
    		bbox[i].landmarks=invoke_pose_landmark(feed_image)
  		end

  		deepcrimson_data.sessions[session].bbox=bbox

  		ret['type']='BBOX'
  		ret['request']='TFLPOSE'
  		ret['data']=bbox
  		ret['size']=size
  		
	end
  
  	return json.encode(ret)
end

deepcrimson_data.update= function(dt)
	if deepcrimson_data.zmq_socket ~= nil then
    buffer = proteo.zmq.recv(deepcrimson_data.zmq_socket,proteo.zmq.flag.ZMQ_DONTWAIT)
    if buffer ~= nil then
      proteo.zmq.send (deepcrimson_data.zmq_socket,deepcrimson_data.pipe(buffer),proteo.zmq.flag.ZMQ_DONTWAIT)
    end
  end
end

deepcrimson_data.timer = proteo.system.createTimer(50,deepcrimson_data.update)

local charset = {}  do -- [0-9a-zA-Z]
    for c = 48, 57  do table.insert(charset, string.char(c)) end
    for c = 65, 90  do table.insert(charset, string.char(c)) end
    for c = 97, 122 do table.insert(charset, string.char(c)) end
end

local function randomString(length)
    if not length or length <= 0 then return '' end
    math.randomseed(os.clock()^5)
    return randomString(length - 1) .. charset[math.random(1, #charset)]
end


proteo.route.get("deepcrimson/info",

	function(username,permission,data,param) 

		local info={}
		info["type"]="INFO"
		info["version"]="0.1"
		info["ticket"]=10
		info["result"]="OK"

		return json.encode(info)
	end
)

proteo.route.get("deepcrimson/permissions",

	function(username,permission,data,param) 
		local permissions={}

		permissions.permission={}
		permissions.permission[1]="deepcrimson:deepcrimson"


		return json.encode(permissions)
	end
)

proteo.route.post("deepcrimson/stop",
 function(username,permission,data,param)
  proteo.system.stopTimer(deepcrimson_data.timer)
  if deepcrimson_data.zmq_socket~=nil then
  	proteo.zmq.socket_close(deepcrimson_data.zmq_socket)
  end
  deepcrimson_data.zmq_context = nil
  deepcrimson_data.zmq_socket = nil

  ret = {}
  ret['message']='OK'

 return json.encode(ret)
end
)

proteo.route.post("deepcrimson/start",
 function(username,permission,data,param)
  if deepcrimson_data.zmq_socket == nil then
    deepcrimson_data.zmq_context = proteo.zmq.ctx_new()
    deepcrimson_data.zmq_socket = proteo.zmq.socket_new(deepcrimson_data.zmq_context,proteo.zmq.sockType.ZMQ_REP)
    
    proteo.zmq.bind(deepcrimson_data.zmq_socket,'tcp://*:5555')

    proteo.system.startTimer(deepcrimson_data.timer)
  end

  ret = {}
  ret['message']='OK'
  ret['bind']='tcp://*:5555'
  ret['port']=5555
  ret['url']=''

  session=randomString(16)
  deepcrimson_data.sessions[session]={}
  deepcrimson_data.sessions[session].username=username
  deepcrimson_data.sessions[session].start=os.clock()
  deepcrimson_data.sessions[session].frame = proteo.opencv.img()

  ret['session']=session

 return json.encode(ret)
end
)


proteo.route.post("deepcrimson/landmarks",

  function(username,permission,data,param) 
      return deepcrimson_data.pipe(data)
    end
)
