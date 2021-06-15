
local json=require "json"

require "tfl_blazepose"
require "tfl_blazeface"
require "tfl_hand"
require "tfl_emotion"

local deepcrimson_data={}

deepcrimson_data.zmq_socket=nil
deepcrimson_data.zmq_context=nil

deepcrimson_data.sessions={}

--t_profile=0
--last_profile=0

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
  elseif data["request"]=="TFLHAND" then

    bbox = invoke_hand_detect(deepcrimson_data.sessions[session].frame)
      size = proteo.opencv.size(deepcrimson_data.sessions[session].frame)
      for i=1,#bbox do
        feed_image = generate_hand_landmark_input_image(deepcrimson_data.sessions[session].frame,bbox[i])
        bbox[i].landmarks=invoke_hand_landmark(feed_image)
      end

      deepcrimson_data.sessions[session].bbox=bbox

      ret['type']='BBOX'
      ret['request']='TFLHAND'
      ret['data']=bbox
      ret['size']=size
  elseif data["request"]=="TFLHOLI" then
    bbox = invoke_pose_detect(deepcrimson_data.sessions[session].frame)
    face_bbox={}
      for i=1,#bbox do
        feed_image = generate_pose_landmark_input_image(deepcrimson_data.sessions[session].frame,bbox[i])
        bbox[i].landmarks=invoke_pose_landmark(feed_image)

        --Due possibilità:

        --Estraiamo un Roi per la faccia e lo facciamo analizzare al face_detect per ottimizzarlo prima di passarlo al face_landmark
        --face_frame=face_roi(deepcrimson_data.sessions[session].frame,bbox[i].landmarks)
        --bbox = invoke_face_detect (face_frame)

        --Generiamo direttamente un roi ottimizzato da passare al face_landmark
        pose_landmarks = get_landmark(bbox[i],deepcrimson_data.sessions[session].frame,bbox[i].landmarks)
        --print("Center X:"..pose_landmarks[1].x.." Y:"..pose_landmarks[1].y)
        face_bbox[i]=generate_face_bbox_from_pose(pose_landmarks,proteo.opencv.size(deepcrimson_data.sessions[session].frame))
        feed_image=generate_face_landmark_input_image (deepcrimson_data.sessions[session].frame, face_bbox[i])
        face_bbox[i].landmarks=invoke_face_landmark (feed_image)
      end

      deepcrimson_data.sessions[session].bbox=bbox

      ret['type']='BBOX'
      ret['request']='TFLHOLI'
      ret['data']=bbox
      ret['face']=face_bbox
      ret['size']=size
	end

  --ret['nframe']=data['nframe']
  
  	return json.encode(ret)
end

function face_roi(frame,pose_landmarks)

end

function generate_face_bbox_from_pose(pose_landmarks,size)

  local x_center = pose_landmarks[1].x/size[2]
  local y_center = pose_landmarks[1].y/size[1]

  local x_lefteye = pose_landmarks[3].x/size[2]
  local y_lefteye = pose_landmarks[3].y/size[1]
  local x_righteye = pose_landmarks[6].x/size[2]
  local y_righteye = pose_landmarks[6].y/size[1]

  local roi_size  = math.sqrt((x_lefteye - x_righteye) * (x_lefteye - x_righteye) + (y_lefteye - y_righteye) * (y_lefteye - y_righteye))
  local ret={}

  ret.score=0

  ret.topleft={x=x_center-roi_size/2,y=y_center-roi_size/2}
  ret.btmright={x=x_center+roi_size/2,y=y_center+roi_size/2}

  ret.keys={}
  ret.keys[1]={x=x_center,y=y_center}
  ret.keys[2]={x=x_lefteye,y=y_lefteye}
  ret.keys[3]={x=x_righteye,y=y_righteye}

  compute_rotation(ret,3,2)
  ret.rotation=ret.rotation - math.pi * 0.5

  local roi_scale = 4.0

  local dx = roi_size * roi_scale * 0.5
  local dy = roi_size * roi_scale * 0.5
  ret.roi_coord = {}
  ret.roi_coord[1] = {x= - dx, y= - dy}
  ret.roi_coord[2] = {x= dx, y= - dy}
  ret.roi_coord[3] = {x= dx, y= dy}
  ret.roi_coord[4] = {x= - dx, y= dy}

  for i = 1,4 do
    rot_vec (ret.roi_coord[i], ret.rotation)
    ret.roi_coord[i].x = ret.roi_coord[i].x + x_center
    ret.roi_coord[i].y = ret.roi_coord[i].y + y_center

    ret.roi_coord[i].x = ret.roi_coord[i].x
    ret.roi_coord[i].y = ret.roi_coord[i].y
  end

  return ret
end

deepcrimson_data.update= function(dt)
	if deepcrimson_data.zmq_socket ~= nil then
    buffer = proteo.zmq.recv(deepcrimson_data.zmq_socket,proteo.zmq.flag.ZMQ_DONTWAIT)
    if buffer ~= nil then

  --  last_profile=proteo.system.clock()

    proteo.zmq.send (deepcrimson_data.zmq_socket,deepcrimson_data.pipe(buffer),proteo.zmq.flag.ZMQ_DONTWAIT)
    
   -- t_profile=proteo.system.clock()-last_profile
   -- print(t_profile)

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

  --TODO chiude la sessione, se non ce ne sono più attive interrompe il timer

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
