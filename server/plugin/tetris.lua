local json=require "json"

require "tfl_blazeface"

-- Describe this function...
function pipe(json_data)
  data = json.decode(json_data)
  proteo.opencv.imdecode(data['frame'],video_frame)
  bbox = invoke_face_detect(video_frame)
  size = proteo.opencv.size(video_frame)
  for _, b in ipairs(bbox) do
    feed_image = generate_face_landmark_input_image(video_frame,b)
    b['landmarks']=invoke_face_landmark(feed_image)
  end
  data = {}
  data['type']='BBOX'
  data['data']=bbox
  data['size']=size
  return json.encode(data)
end



video_frame = proteo.opencv.img()
local zmq_socket_tetris=nil
update_tetris = function(dt)
  if zmq_socket_tetris ~= nil then
    buffer = proteo.zmq.recv(zmq_socket_tetris,proteo.zmq.flag.ZMQ_DONTWAIT)
    if buffer ~= nil then
      proteo.zmq.send (zmq_socket_tetris,(pipe(buffer)),proteo.zmq.flag.ZMQ_DONTWAIT)
    end
  end
 end

t = proteo.system.createTimer(50,update_tetris)

proteo.route.post("tetris/stop",
 function(username,permission,data,param)
  proteo.system.stopTimer(t)
  proteo.zmq.socket_close(zmq_socket_tetris)
  zmq_context_tetris = nil
  zmq_socket_tetris = nil
  ret = {}
  ret['message']='OK'

 return json.encode(ret)
end
)

proteo.route.post("tetris/start",
 function(username,permission,data,param)
  if zmq_socket_tetris == nil then
    zmq_context_tetris = proteo.zmq.ctx_new()
    zmq_socket_tetris = proteo.zmq.socket_new(zmq_context_tetris,proteo.zmq.sockType.ZMQ_REP)
    proteo.zmq.bind(zmq_socket_tetris,'tcp://*:5555')
  end
  proteo.system.startTimer(t)
  ret = {}
  ret['message']='OK'

 return json.encode(ret)
end
)

proteo.route.get("tetris/permissions",
 function(username,permission,data,param)
  permissions = {'tetris:start'}

 return json.encode(permissions)
end
)

proteo.route.get("tetris/info",
 function(username,permission,data,param)
  info = {}
  info['type']='INFO'
  info['version']='0.1'
  info['ticket']=10
  info['result']='OK'

 return json.encode(info)
end
)
