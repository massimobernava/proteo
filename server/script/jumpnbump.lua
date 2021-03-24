--require 'room'

json=require "json"
md5 = require 'md5'

BAN_VOID = 0
BAN_SOLID = 1
BAN_WATER = 2
BAN_ICE = 3
BAN_SPRING = 4

--NUM_POBS=200
--NUM_OBJECTS=200
--NUM_LEFTOVERS=50
NUM_FLIES=20

MOVE_ADD=0.5 -- 0.05
MOVE_BRAKE=0.8 -- 1.6
MOVE_JUMP= -9.6 -- -3.1
MOVE_GRAVITY=1.0 -- 0.1
MOVE_ENDJUMP=0.1

MULTI_BOX=1.5  -- 2.0

ZOOM=3
DUST_OBJECTS=4
RED_BF_OBJECTS=3
YELLOW_BF_OBJECTS=3

DIR_NULL=0
DIR_LEFT=1
DIR_RIGHT=2
DIR_UP=3
DIR_DOWN=4

--+++++GLOBAL+++++--
left={0,0,0,0}
right={0,0,0,0}
up={0,0,0,0}

flies_sprite={nil,nil,nil,nil,nil,nil,nil,nil,nil,nil,nil,nil,nil,nil,nil,nil,nil,nil,nil,nil}
flies_x={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
flies_y={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

bf_x={}
bf_y={}
bf_dir={}

--[[struct {
	char enabled, dead_flag;
	int bumps;
	int bumped[4];
	int x, y;
    int x_add, y_add;
    char direction, jump_ready, jump_abort, in_water;
    int anim, frame, frame_tick, image;
} player[4];
--]]
player_sprite={nil,nil,nil,nil}

player_enabled={true,true,false,false}
player_dead_flag={false,false,false,false}
player_x={0,0,0,0}
player_y={0,0,0,0}
player_x_add={0,0,0,0}
player_y_add={0,0,0,0}

player_frame_tick={0,0,0,0}
player_frame={0,0,0,0}
player_anim={0,0,0,0}
player_direction={0,0,0,0}
player_jump_ready={0,0,0,0}

ANIM_STOP_RIGHT = 0
ANIM_STOP_LEFT = 1
ANIM_RUN_RIGHT = 2
ANIM_RUN_LEFT = 3
ANIM_JUMP_RIGHT = 4
ANIM_JUMP_LEFT = 5
ANIM_DOWN_RIGHT = 6
ANIM_DOWN_LEFT = 7
ANIM_SWIM_RIGHT = 8
ANIM_SWIM_LEFT = 9
ANIM_SPLASH_RIGHT = 10
ANIM_SPLASH_LEFT = 11
--num_frames, restar_frame, frame, ticks
player_anims = {
    1, 0, 0, 0x7fff, 0, 0, 0, 0, 0, 0,  --ANIM_STOP_RIGHT 
    1, 0, 9, 0x7fff, 0, 0, 0, 0, 0, 0,  --ANIM_STOP_LEFT
    4, 0, 0, 1, 1, 1, 2, 1, 3, 1,       --ANIM_RUN_RIGHT
    4, 0, 9, 1, 10, 1, 11, 1, 12, 1,    --ANIM_RUN_LEFT
    1, 0, 4, 0x7fff, 0, 0, 0, 0, 0, 0,  --ANIM_JUMP_RIGHT
    1, 0, 13, 0x7fff, 0, 0, 0, 0, 0, 0,  --ANIM_JUMP_LEFT
    3, 2, 5, 2, 6, 1, 7, 0x7fff, 0, 0,       --ANIM_DOWN_RIGHT
    4, 2, 14, 2, 15, 1, 16, 0x7fff, 0, 0,  --ANIM_DOWN_LEFT
    1, 0, 5, 0x7fff, 0, 0, 0, 0, 0, 0,  --ANIM_SPLASH_RIGHT
    1, 0, 14, 0x7fff, 0, 0, 0, 0, 0, 0,  --ANIM_SPLASH_LEFT
    3, 1, 6, 4, 4, 4, 5, 0x7fff, 0, 0,  --ANIM_SWIM_RIGHT
    3, 1, 15, 4, 13, 4, 14, 0x7fff, 0, 0,  --ANIM_SWIM_LEFT
    1, 0, 8, 2, 0, 0, 0, 0, 0, 0        
}

ban_map={
    1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,
    1,0,0,0,1,1,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,
    1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,
    1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,
    1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,
    1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,3,1,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,3,1,1,1,0,0,0,0,0,0,1,
    1,0,1,1,1,1,0,0,0,0,3,1,1,1,1,1,1,1,1,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,1,
    2,2,2,2,2,2,2,2,1,4,0,0,0,0,0,1,3,3,3,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
}

objects={}
objects_anim={}
objects_tick={}
objects_frames={}

network_enabled=false
player_sel=nil

player_username=nil
update_tick=0

function roomserver_init()
    proteo.network.roomConnect("game/room","gameroom_callback")
end

function gameroom_callback(room,data)
    --print("gameroom_callback: "..data)
    if data=="connected" then 
        
        local send_data={}
        send_data["command"]="get_player"
        send_data["username"]=md5.Calc(USERNAME..math.random(100))
        player_username=send_data["username"]
        proteo.network.roomSend("game/room",json.encode(send_data))

    --proteo.network.roomSend("game/room","ping")
    --elseif data=="pong" then proteo.network.roomSend("game/room","ping") 
    else
        --print("Data: "..  data)
        local jdata=json.decode(data)

        if jdata.command=="player" and jdata.username==player_username then
            network_enabled=true
            player_sel=jdata.player
        elseif jdata.command=="position" then
            for c1=1,4 do
                if player_sel~=nil and c1~=player_sel then
                    player_x[c1]=jdata.position[c1].x
                    player_y[c1]=jdata.position[c1].y
                    player_x_add[c1]=jdata.direction[c1].x
                    player_y_add[c1]=jdata.direction[c1].y
                end
            end
        end
    end
end

------------------
--+++++INIT+++++--
------------------
ximg=nil  --Proteo Image
cap=nil --OpenCV VideoCapture
net=nil --OpenCV Net

img=nil --OpenCV Image
img_in=nil --OpenCV Image
img_out=nil --OpenCV Image
probMap=nil --OpenCV Image

frameWidth = 0 --OpenCV Image input size
frameHeight = 0
H=0 --OpenCV Image output size
W=0

--[[PAIRS = 
    {   
    {0,1}, {1,2}, {2,3},
    {3,4}, {1,5}, {5,6},
    {6,7}, {1,14}, {14,8}, {8,9},
    {9,10}, {14,11}, {11,12}, {12,13}
    }
]]--

PAIRS =  {{1, 2}, {1, 5}, {2, 3}, {3, 4}, {5, 6}, {6, 7}, {1, 8}, {8, 9}, {9, 10}, {1, 11},
                     {11, 12}, {12, 13}, {1, 0}, {0, 14}, {14, 16}, {0, 15}, {15, 17}, {2, 16}, {5, 17}}

function pose()
    print("FPS: "..1000/dt)
    proteo.opencv.frame(cap,img)
    --proteo.opencv.cropImg(img,img_in,280,0,720,720) --pose/mpi/pose_deploy_linevec_faster_4_stages.prototxt
    proteo.opencv.cropImg(img,img_in,184,0,912,512) --pose/human-pose-estimation.onnx
    proteo.opencv.forward(net,img_in,img_out,0.2,"stage_1_output_1_heatmaps")

    points={}
    for c=0,17 do
        proteo.opencv.sliceImg(img_out,probMap,0,c)
        points[c]=proteo.opencv.minMaxLoc(probMap)
        if points[c].max.v>0.1 then
            points[c].max.x=points[c].max.x*frameWidth / W 
            points[c].max.y=points[c].max.y*frameHeight / H 
            proteo.opencv.circle(img_in,points[c].max.x,points[c].max.y,8,"#FF0000")
        end
        --print("A-X: "..points[c].max.x.." Y: "..points[c].max.y)
    end

    for c=1,17 do
        if points[PAIRS[c][1] ].max.v>0.1 and points[PAIRS[c][2] ].max.v>0.1 then
            proteo.opencv.line(img_in,points[PAIRS[c][1] ].max.x,points[PAIRS[c][1] ].max.y,points[PAIRS[c][2] ].max.x,points[PAIRS[c][2] ].max.y,8,"#FFFF00")
        end
    end
    proteo.graphics.changeImage(ximg,img_in)
end

function pose_init()
    img=proteo.opencv.img()
    img_in=proteo.opencv.img()
    img_out=proteo.opencv.img()
    probMap=proteo.opencv.img()

    cap=proteo.opencv.videocapture(0)
    proteo.opencv.frame(cap,img)
    --proteo.opencv.cropImg(img,img_in,280,0,720,720) --pose/mpi/pose_deploy_linevec_faster_4_stages.prototxt
    proteo.opencv.cropImg(img,img_in,184,0,912,512) --pose/human-pose-estimation.onnx
    size=proteo.opencv.size(img_in)
    frameHeight=size[1]
    frameWidth=size[2]
    proteo.opencv.infoImg(img_in)

--
--"pose/coco/pose_deploy_linevec.prototxt","pose/coco/pose_iter_440000.caffemodel"
    --net=proteo.opencv.readnet("pose/mpi/pose_deploy_linevec_faster_4_stages.prototxt", "pose/mpi/pose_iter_160000.caffemodel")
    net=proteo.opencv.readnet("pose/human-pose-estimation.onnx","")
    --net=proteo.opencv.readnet("pose/human-pose-estimation-0001.xml","pose/human-pose-estimation-0001.bin")
    proteo.opencv.infoNet(net)
    proteo.opencv.forward(net,img_in,img_out,0.2,"stage_1_output_1_heatmaps")
    size=proteo.opencv.size(img_out)
    proteo.opencv.infoImg(img_out)

    H=size[3]
    W=size[4]

    ximg=proteo.graphics.newImage("webcam","",200,200,200,200)
    proteo.graphics.changeImage(ximg,img_in)
end

function init()
    
    --pose_init()

    proteo.audio.infoDrivers()
    print("Platform: "..proteo.system.getPlatform())
    if proteo.audio.init() then
        proteo.audio.infoDevices()
        proteo.audio.quit()
    end

    --proteo.network.roomInit("localhost:8888","roomserver_init")

    math.randomseed(os.time())
    
    --IMPOSTA MOSCHE
    s1=math.random(250)+50
    s2=math.random(150)+50
  
    --FLIES
    for index=1,NUM_FLIES do
        flies_sprite[index]=proteo.graphics.newSprite("fl"..index,proteo.system.document().."rabbit.png",0,0,ZOOM*2,ZOOM*2)
        proteo.graphics.addFrame(flies_sprite[index],0, 0, 20, 20)
        proteo.graphics.setFrame(flies_sprite[index],0);
        proteo.graphics.setLayer(flies_sprite[index],1);
        --dust.setShader_Sprite(flies_sprite[index],"Fly");
        
        while true do
            flies_x[index]=s1+math.random(101)-50
            flies_y[index]=s2+math.random(101)-50
            
            f1=math.floor((flies_x[index])/16)
            f2=math.floor((flies_y[index])/16)
            
            if ban_map[f1+1+f2*22]==0 then break end
        end
        --print(ban_map[f1+1+f2*22])
        proteo.graphics.setPosition(flies_sprite[index],ZOOM*flies_x[index],ZOOM*flies_y[index])
    end
     
    background=proteo.graphics.newSprite("level",proteo.system.document().."level.png",0,0,ZOOM*400,ZOOM*256)
    proteo.graphics.addFrame(background,0, 0, 400, 256)
    proteo.graphics.setFrame(background,0);
    proteo.graphics.setLayer(background,0);
    proteo.graphics.setPosition(background,0,0)
    print("background")

    mask=proteo.graphics.newSprite("mask",proteo.system.document().."mask.png",0,0,ZOOM*400,ZOOM*256)
    proteo.graphics.addFrame(mask,0, 0, 400, 256)
    proteo.graphics.setFrame(mask,0);
    proteo.graphics.setLayer(mask,3);
    proteo.graphics.setPosition(mask,0,0)
    print("mask")
    
    --DOTT
    dott=proteo.graphics.newSprite("dott",proteo.system.document().."rabbit.png",0,0,ZOOM*16,ZOOM*16)
    proteo.graphics.addFrame(dott,0, 0, 16, 16)
    proteo.graphics.addFrame(dott,19, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*2, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*3, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*4, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*5, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*6, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*7, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*8, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*9, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*10, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*11, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*12, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*13, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*14, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*15, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*16, 0, 16, 16)
    proteo.graphics.addFrame(dott,19*17, 0, 16, 16)
    
    proteo.graphics.setFrame(dott,0)
    proteo.graphics.setLayer(dott,1)
    --proteo.graphics.setPosition(dott,20,20)
    
    player_sprite[1]=dott
    
    position_player(1)
    proteo.graphics.setPosition(dott,ZOOM*player_x[1],ZOOM*player_y[1])
    print("dott")
    
    --JIFFY
    jiffy=proteo.graphics.newSprite("jiffy",proteo.system.document().."rabbit.png",0,0,ZOOM*16,ZOOM*16)
    proteo.graphics.addFrame(jiffy,19*18, 0, 16, 16)
    proteo.graphics.addFrame(jiffy,19*19, 0, 16, 16)
    proteo.graphics.addFrame(jiffy,19*20, 0, 16, 16)
    proteo.graphics.addFrame(jiffy,0, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*2, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*3, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*4, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*5, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*6, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*7, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*8, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*9, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*10, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*11, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*12, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*13, 16, 16, 16)
    proteo.graphics.addFrame(jiffy,19*14, 16, 16, 16)

    proteo.graphics.setFrame(jiffy,0);
    proteo.graphics.setLayer(jiffy,1);
    
    player_sprite[2]=jiffy
    
    position_player(2)
    proteo.graphics.setPosition(jiffy,ZOOM*player_x[2],ZOOM*player_y[2])
    print("jiffy")
    
    objects[1]=proteo.graphics.newSprite("jump",proteo.system.document().."objects.png",434,649,ZOOM*17,ZOOM*12)
    proteo.graphics.addFrameSource(objects[1],0, 0, 17, 12,ZOOM*17,ZOOM*12)
    proteo.graphics.addFrameSource(objects[1],32, 0, 17, 12,ZOOM*17,ZOOM*12)
    proteo.graphics.addFrameSource(objects[1],64, 0, 17, 12,ZOOM*17,ZOOM*12)
    proteo.graphics.addFrameSource(objects[1],96, 0, 17, 12,ZOOM*17,ZOOM*12)
    proteo.graphics.addFrameSource(objects[1],128, 0, 17, 12,ZOOM*17,ZOOM*12)
    proteo.graphics.addFrameSource(objects[1],160, 0, 17, 12,ZOOM*17,ZOOM*12)
    proteo.graphics.setFrame(objects[1],0);
    proteo.graphics.setLayer(objects[1],1);
    objects_anim[1]=0
    objects_tick[1]=-1
    objects_frames[1]=6

    for c=0,3 do
        for d=1,DUST_OBJECTS do 
            local i=(c*DUST_OBJECTS)+d+1
            objects[i]=proteo.graphics.newSprite("dust"..i,proteo.system.document().."objects.png",0,0,ZOOM*5,ZOOM*5)
            proteo.graphics.addFrameSource(objects[i],94, 14, 5, 5,ZOOM*5,ZOOM*5)
            proteo.graphics.addFrameSource(objects[i],127, 13, 5, 5,ZOOM*5,ZOOM*5)
            proteo.graphics.addFrameSource(objects[i],160, 14, 5, 5,ZOOM*5,ZOOM*5)
            proteo.graphics.addFrameSource(objects[i],192, 14, 5, 5,ZOOM*5,ZOOM*5)
            proteo.graphics.addFrameSource(objects[i],224, 14, 5, 5,ZOOM*5,ZOOM*5)
            proteo.graphics.setFrame(objects[i],math.random(3));
            proteo.graphics.setLayer(objects[i],2);
            objects_anim[i]=0
            objects_tick[i]=0
            objects_frames[i]=5
        end
    end

    for rb=1,RED_BF_OBJECTS do 
        local i=1+(DUST_OBJECTS*4)+rb
        objects[i]=proteo.graphics.newSprite("rb"..i,proteo.system.document().."objects.png",0,0,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],256, 28, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],288, 28, 7, 5,ZOOM*7,ZOOM*5)
        --proteo.graphics.addFrameSource(objects[i],320, 28, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],352, 28, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],0, 42, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],32, 42, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],64, 42, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],96, 42, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],128, 42, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],160, 42, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],192, 42, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.setFrame(objects[i],0);
        proteo.graphics.setLayer(objects[i],2);
        objects_anim[i]=math.random(5)
        objects_tick[i]=0
        objects_frames[i]=5
        position_bf(rb)
    end

    for yb=1,YELLOW_BF_OBJECTS do 
        local i=1+(DUST_OBJECTS*4)+RED_BF_OBJECTS+yb
        objects[i]=proteo.graphics.newSprite("yb"..i,proteo.system.document().."objects.png",0,0,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],256, 14, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],288, 14, 7, 5,ZOOM*7,ZOOM*5)
        --proteo.graphics.addFrameSource(objects[i],320, 14, 7, 5,ZOOM*6,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],352, 14, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],0, 28, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],32, 28, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],64, 28, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],96, 28, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],128, 28, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],160, 28, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.addFrameSource(objects[i],192, 28, 7, 5,ZOOM*7,ZOOM*5)
        proteo.graphics.setFrame(objects[i],0);
        proteo.graphics.setLayer(objects[i],2);
        objects_anim[i]=math.random(5)
        objects_tick[i]=0
        objects_frames[i]=5
        position_bf(RED_BF_OBJECTS+yb)
    end

    for w=1,4 do 
        local i=1+(DUST_OBJECTS*4)+RED_BF_OBJECTS+YELLOW_BF_OBJECTS+w
        objects[i]=proteo.graphics.newSprite("w"..i,proteo.system.document().."objects.png",0,0,ZOOM*31,ZOOM*12)
        proteo.graphics.addFrame(objects[i],192, 0, 31, 12)
        proteo.graphics.addFrame(objects[i],224, 0, 31, 12)
        proteo.graphics.addFrame(objects[i],256, 0, 31, 12)
        proteo.graphics.addFrame(objects[i],288, 0, 31, 12)
        proteo.graphics.addFrame(objects[i],320, 0, 31, 12)
        proteo.graphics.addFrame(objects[i],352, 0, 31, 12)
        proteo.graphics.addFrame(objects[i],0, 13, 31, 12)
        proteo.graphics.addFrame(objects[i],32, 13, 31, 12)
        proteo.graphics.addFrame(objects[i],64, 13, 31, 12)
        proteo.graphics.setFrame(objects[i],0);
        proteo.graphics.setLayer(objects[i],2);
        proteo.graphics.setHidden(objects[i],true)
        objects_anim[i]=0
        objects_tick[i]=-1
        objects_frames[i]=9
    end

end

--------------------
--+++++UPDATE+++++--
--------------------

function update(dt)
        
    --pose()
    --proteo.network.roomCheck()

    --MUOVI I PLAYER
    for c1=1,4 do
        update_player(c1)
    end
    --if player_sel~=nil then update_player(player_sel) end

    --ANALIZZA INTERAZIONI
    for c3=1,6 do
        
        if c3==1 then c1=0 c2=1 end
        if c3==2 then c1=0 c2=2 end
        if c3==3 then c1=0 c2=3 end
        if c3==4 then c1=1 c2=2 end
        if c3==5 then c1=1 c2=3 end
        if c3==6 then c1=2 c2=3 end
    
        if (player_enabled[c1] == true and player_enabled[c2] == true) then
            
        end
    end
    
    --AGGIORNA OGGETTI
    update_objects()
    
    --INTERAZIONE MOSCHE
    --CALCOLA IL CENTRO DELLO STORMO
    local s1=0
    local s2=0
    for c1=1,NUM_FLIES do
        s1=s1+flies_x[c1]
        s2=s2+flies_y[c1]
    end
    s1=s1/NUM_FLIES
    s2=s2/NUM_FLIES

    --RICALCOLA POSIZIONE
    for c1=1,NUM_FLIES do  
        update_fly(c1,s1,s2)
        proteo.graphics.setPosition(flies_sprite[c1],ZOOM*flies_x[c1],ZOOM*flies_y[c1])
    end

    for b=1,RED_BF_OBJECTS+YELLOW_BF_OBJECTS do  
        update_bf_position(b)
        local i=1+(DUST_OBJECTS*4)+b
        proteo.graphics.setPosition(objects[i],ZOOM*bf_x[b],ZOOM*bf_y[b])
    end
    
    --DISEGNA?
    for c1=1,4 do
        if player_sprite[c1]~=nil then
            proteo.graphics.setPosition(player_sprite[c1],ZOOM*player_x[c1],ZOOM*player_y[c1])
        end
    end

    update_tick=update_tick+1
    if update_tick>30 then
        data={}
        data.command="player_position"
        data.player=player_sel
        data.x=player_x[player_sel]
        data.y=player_y[player_sel]
        data.add_x=player_x_add[player_sel]
        data.add_y=player_y_add[player_sel]
        if network_enabled then 
        --proteo.network.roomSend("game/room",json.encode(data)) 
        end
        update_tick=0
    end
end
-------------------
--+++++EVENT+++++--
-------------------
function touch(x,y)
    print(string.format("touch (%f,%f)", x,y))   
    --local cX=math.floor(x/(ZOOM*16))
    --local cY=math.floor(y/(ZOOM*16))
    --print(string.format("touch c(%f,%f)", cX,cY)) 
    --local i=1+(DUST_OBJECTS*4)+1
    --proteo.graphics.setPosition(objects[i],x,y)
    --rbf_x[1]=x/ZOOM
    --rbf_y[1]=y/ZOOM
    --print(string.format("BAN value:%i",ban_map[cX+1+cY*22]))
    --player_x[1]=x/ZOOM
   --player_y[1]=y/ZOOM

    --if x<player_x[1]-20 then
    --    left[1]=1
    --elseif x>player_x[1]+20 then
    --    right[1]=1
    --end
    
    --if y>player_y[1] then
    --   up[1]=1
    --end
end

function keydown(k)
    player_sel=1
    --if network_enabled and player_sel~=nil then
        if k==0x4000004F then right[player_sel]=1 --RIGHT
        elseif k==0x40000050 then left[player_sel]=1 --LEFT
        elseif k==0x40000052 then up[player_sel]=1 --UP
        end
    --end

    if k==0x64 then right[2]=1 --RIGHT
    elseif k==0x61 then left[2]=1 --LEFT
    elseif k==0x77 then up[2]=1 --UP
    end
end

function keyup(k)
    player_sel=1
    --if network_enabled and player_sel~=nil then
        if k==0x4000004F then right[player_sel]=0 --RIGHT
        elseif k==0x40000050 then left[player_sel]=0 --LEFT
        elseif k==0x40000052 then up[player_sel]=0 --UP
        end
    --end

    if k==0x64 then right[2]=0 --RIGHT
    elseif k==0x61 then left[2]=0 --LEFT
    elseif k==0x77 then up[2]=0 --UP
    end
end

function release(x,y)
    left[1]=0
    right[1]=0
    up[1]=0
end

-------------------
--+++++LOCAL+++++--
-------------------

function update_objects()

    --DUST
    for c=0,3 do
        for d=1,DUST_OBJECTS do 
            local i=(c*DUST_OBJECTS)+d+1
            if objects_tick[i]>=0 then
                objects_tick[i]=objects_tick[i]+1
                --if objects_tick[i]>3 then
                    objects_tick[i]=0
                    proteo.graphics.setFrame(objects[i],objects_anim[i])
                    objects_anim[i]=objects_anim[i]+1
                    if objects_anim[i]>objects_frames[i] then 
                        objects_anim[i]=math.random(3)
                        objects_tick[i]=-1
                        proteo.graphics.setHidden(objects[i],true)
                    end
                --end
            end
        end
    end

    --BOX
    if objects_tick[1]>=0 then
        objects_tick[1]=objects_tick[1]+1
        --if objects_tick[1]>3 then
            objects_tick[1]=0
            proteo.graphics.setFrame(objects[1],objects_anim[1])
            objects_anim[1]=objects_anim[1]+1
            if objects_anim[1]>objects_frames[1] then 
                objects_anim[1]=0
                objects_tick[1]=-1
                proteo.graphics.setFrame(objects[1],objects_anim[1])
            end
        --end
    end

    --BF
    for b=1,RED_BF_OBJECTS+YELLOW_BF_OBJECTS do 
        local i=1+(DUST_OBJECTS*4)+b

        local dir=0
        if bf_dir[b]==2 or bf_dir[b]==3 or bf_dir[b]==4 then dir=0 
        else dir=5 end

        if objects_tick[i]>=0 then
            objects_tick[i]=objects_tick[i]+1
            --if objects_tick[i]>3 then
                objects_tick[i]=0
                proteo.graphics.setFrame(objects[i],dir+objects_anim[i])
                objects_anim[i]=objects_anim[i]+1
                if objects_anim[i]>objects_frames[i] then 
                    objects_anim[i]=0
                    --objects_tick[i]=-1
                    --proteo.graphics.setHidden(objects[i],true)
                    proteo.graphics.setFrame(objects[i],dir+objects_anim[i])
                    --proteo.graphics.setHidden(objects[i],true)
                end
            --end
        end
    end

    --SPLASH
    for w=1,4 do 
        local i=1+(DUST_OBJECTS*4)+RED_BF_OBJECTS+YELLOW_BF_OBJECTS+w
        if objects_tick[i]>=0 then
            objects_tick[i]=objects_tick[i]+1
            --if objects_tick[i]>3 then
                objects_tick[i]=0
                proteo.graphics.setFrame(objects[i],objects_anim[i])
                objects_anim[i]=objects_anim[i]+1
                if objects_anim[i]>objects_frames[i] then 
                    objects_anim[i]=0
                    objects_tick[i]=-1
                    proteo.graphics.setHidden(objects[i],true)
                    proteo.graphics.setFrame(objects[i],objects_anim[i])
                    --proteo.graphics.setHidden(objects[i],true)
                end
            --end
        end
    end
end

function update_bf_position(bf)
    local i=1+(DUST_OBJECTS*4)+bf
    
    if math.random(500)==1 then 
        bf_dir[bf]=math.random(8)
    end  

    local dX=0
    local dY=0
    if bf_dir[bf]==1 or bf_dir[bf]==2 or bf_dir[bf]==8 then dY=-0.2 end
    if bf_dir[bf]==4 or bf_dir[bf]==5 or bf_dir[bf]==6 then dY=0.2 end
    if bf_dir[bf]==2 or bf_dir[bf]==3 or bf_dir[bf]==4 then dX=0.2 end
    if bf_dir[bf]==6 or bf_dir[bf]==7 or bf_dir[bf]==8 then dX=-0.2 end


    local cX=math.floor((bf_x[bf]+10*dX)/16)
    local cY=math.floor((bf_y[bf]+10*dY)/16)
    local t=ban_map[cX+1+cY*22] or 0
    
    if t~=0 then bf_dir[bf]=math.random(8) end
    if bf_y[bf]<10 then dY=0.2 end

    bf_x[bf]=bf_x[bf]+dX
    bf_y[bf]=bf_y[bf]+dY
end

function update_fly(c1,s1,s2)
    --CALCOLA IL CONIGLIO PIU' VICINO
    dist = 0x7fff;
    for c2 = 1,4 do
        if player_enabled[c2] == true then
            cur_dist = math.sqrt((flies_x[c1] - player_x[c2]) * (flies_x[c1] - player_x[c2]) + (flies_y[c1] - player_y[c2]) * (flies_y[c1] - player_y[c2]));
            if cur_dist < dist then
                closest_player = c2;
                dist = cur_dist;
            end
        end
    end
    --PRIMA LA X
    s3=0
    if (s1-flies_x[c1])>30 then s3=s3+1
    elseif (s1-flies_x[c1])<-30 then s3=s3-1
    end
    
    if dist < 30 then
        if player_x[closest_player]>flies_x[c1] then s3=s3-1
        else s3=s3+1
        end
    end
 
    s4=math.random(0,2)-1+s3
    if flies_x[c1] + s4 <16 then s4=0 end
    if flies_x[c1] + s4 >351 then s4=0 end
    f1=math.floor((flies_x[c1] +s4)/16)
    f2=math.floor(flies_y[c1]/16)
    
    --if ban_map[f1+1+f2*22]>0 then s4=0 end
    flies_x[c1]=flies_x[c1]+s4
    
    
    --POI LA Y
    s3=0
    if s2-flies_y[c1]>30 then s3=s3+1
        elseif s2-flies_y[c1]<-30 then s3=s3-1
    end
    
    if dist < 30 then
        if player_y[closest_player]>flies_y[c1] then s3=s3-1
            else s3=s3+1
        end
    end

    s4=math.random(0,2)-1+s3
    if flies_y[c1] + s4 <0 then s4=0 end
    if flies_y[c1] + s4 >239 then s4=0 end
    f1=math.floor((flies_x[c1])/16)
    f2=math.floor((flies_y[c1]-s4)/16)
    
    --if ban_map[f1+1+f2*22]>0 then s4=0 end
    flies_y[c1]=flies_y[c1]+s4
end

--MUOVI I PLAYER
function update_player(c1)

    if player_enabled[c1]==true then
        if player_dead_flag[c1]==false then
            --CONTROLLIAMO SE I PLAYER SI STANNO SPOSTANDO
            if left[c1]==1 and right[c1]==1 then
            
            --LEFT
            elseif left[c1]==1 then
                --s1,s2=get_player_coord(c1)
                --CONTROLLA PAVIMENTO
                --ban_map[cX+1+cY*22]
                
                --per adesso no
                player_move(c1,DIR_LEFT)

            --RIGHT
            elseif right[c1]==1 then
                --s1,s2=get_player_coord(c1)
                --CONTROLLA PAVIMENTO
                --ban_map[cX+1+cY*22]
                
                --per adesso no
                player_move(c1,DIR_RIGHT)

            --NULL
            elseif left[c1]==0 and right[c1]==0 then
                
                --BRAKE
                player_stop(c1)
            end
            
            --JUMP
            if up[c1]==1 and player_jump_ready[c1]==1 then
                player_jump_ready[c1]=0
                player_y_add[c1]=MOVE_JUMP
            
            end    
            if up[c1]==0 then
                --player_jump_ready[c1]=1
                if player_y_add[c1]<0 then

                    player_y_add[c1]=player_y_add[c1]+MOVE_ENDJUMP
                    if player_y_add[c1] >0 then
                       player_y_add[c1]=0
                   end
                end
            end
            
            --SPOSTA X
            player_x[c1]=player_x[c1]+player_x_add[c1]
            
            --CONTROLLA SPOSTAMTENO
            if player_x[c1] < 0 then
                player_x[c1]=0
                player_x_add[c1]=0
            end
            
            if player_x[c1]>335 then
               player_x[c1]=335
               player_x_add[c1]=0
            end
            
            s1,s2,t=get_coord_right(c1)
            if t>0 and t~=BAN_WATER and player_x_add[c1]>0 then --player_direction[c1]==DIR_RIGHT then--==BAN_SOLID then
                player_x[c1]=(s1-1)*16--s1+(s2-1)*22
                player_x_add[c1]=0
            end
            
            s1,s2,t=get_coord_left(c1)
            if t>0 and t~=BAN_WATER  and player_x_add[c1]<0 then --player_direction[c1]==DIR_LEFT then--==BAN_SOLID then
                player_x[c1]=(s1+1)*16--s1+(s2-1)*22
                player_x_add[c1]=0
            end
            
            --SPOSTA Y
            player_y[c1]=player_y[c1]+player_y_add[c1]

            --CONTROLLA SPOSTAMTENO
            if player_y[c1]>237 then
                player_y[c1]=236
                player_y_add[c1]=0
            end

            s1,s2,t=get_coord_down(c1)

            --GRAVITA'
            if t>0 and t~=BAN_WATER then--==BAN_SOLID then
                player_y[c1]=(s2-1)*16
                player_y_add[c1]=0
                player_jump_ready[c1]=1
                if player_anim[c1] == ANIM_DOWN_LEFT then
                    set_anim(c1,ANIM_STOP_LEFT)
                elseif player_anim[c1] == ANIM_DOWN_RIGHT then
                    set_anim(c1,ANIM_STOP_RIGHT)
                end
                if t==4 then object_box(c1) end
            --else
            elseif t==BAN_WATER and player_y_add[c1]>=0 then 
                player_jump_ready[c1]=1
                --player_y[c1]=(s2-1)*16 +8
                if player_y_add[c1]>0 then
                    object_splash(c1)
                    player_y[c1]=(s2-1)*16 +8
                    --player_y_add[c1]=-0.5
                    --player_y_add[c1]=player_y_add[c1]+MOVE_GRAVITY
                    player_y_add[c1]=0
                    if player_anim[c1] == ANIM_DOWN_LEFT then
                        set_anim(c1,ANIM_SPLASH_LEFT)
                    else 
                        set_anim(c1,ANIM_SPLASH_RIGHT)
                    end
                else
                    player_y[c1]=(s2-1)*16 +8
                end
                --player_y[c1]=(s2-1)*16
                --player_y_add[c1]=0
                --player_jump_ready[c1]=1
                --if player_anim[c1] == ANIM_RUN_LEFT or player_anim[c1] == ANIM_JUMP_LEFT then
                --    set_anim(c1,ANIM_SPLASH_LEFT)
                --else
                --    set_anim(c1,ANIM_SPLASH_RIGHT)
                --end
            else
                player_jump_ready[c1]=0
                player_y_add[c1]=player_y_add[c1]+MOVE_GRAVITY

                if player_y_add[c1]>0 then 
                    if player_anim[c1] == ANIM_RUN_LEFT or player_anim[c1] == ANIM_JUMP_LEFT then
                        set_anim(c1,ANIM_DOWN_LEFT)
                    elseif player_anim[c1] == ANIM_RUN_RIGHT or player_anim[c1] == ANIM_JUMP_RIGHT then
                        set_anim(c1,ANIM_DOWN_RIGHT)
                    end
                elseif player_y_add[c1]<0 then 
                    if player_anim[c1] == ANIM_RUN_LEFT or player_anim[c1] == ANIM_STOP_LEFT or player_anim[c1] == ANIM_SWIM_LEFT then
                        set_anim(c1,ANIM_JUMP_LEFT)
                    elseif player_anim[c1] == ANIM_RUN_RIGHT  or player_anim[c1] == ANIM_STOP_RIGHT  or player_anim[c1] == ANIM_SWIM_RIGHT then
                        set_anim(c1,ANIM_JUMP_RIGHT)
                    end
                end
            end

            s1,s2,t=get_coord_up(c1)
            if t>0 then
                if player_y_add[c1]<0 then player_y_add[c1]=0 end---player_y_add[c1] end
            end
        end
        --AGGIORNA FRAME
        update_frame(c1)
    end

end

function update_frame(c1)
    player_frame_tick[c1]=player_frame_tick[c1]+1
    ticks=player_anims[player_anim[c1]*10 + player_frame[c1]*2 +4]
    --print(ticks)
    num_frames=player_anims[player_anim[c1]*10+1]
    --print(num_frames)
    restart_frame=player_anims[player_anim[c1]*10 +2]
    --print(restart_frame)
    
    if player_frame_tick[c1] >= ticks then
        player_frame[c1]=player_frame[c1]+1
        if player_frame[c1]>= num_frames then
            player_frame[c1]=restart_frame
        end
        player_frame_tick[c1]=0
        
        frame=player_anims[player_anim[c1]*10 + player_frame[c1]*2 +3]
        proteo.graphics.setFrame(player_sprite[c1],frame)--+player_direction[c1]*9)
    end
end
--POSIZIONA IL PLAYER
function position_player(player_num)
    
    while true do
        while true do
            s1=math.random(22)
            s2=math.random(15)
            --CERCA UNA CASELLA DEL TIPO CORRETTO
            if ban_map[s1+(s2-1)*22]==BAN_VOID then --and (ban_map[s1+1+s2*22]==BAN_SOLID or ban_map[s1+1+s2*22]==BAN_ICE) then
                break
            end
        end
        print(string.format("position_player f1 (%i,%i)", s1,s2))
        --CHE NON SIA GIA' OCCUPATA DA UN ALTRO
        c1=1
        while c1<5 do
             if c1 ~= player_num and player_enabled[c1]== true then
                 --t1=math.floor((player_x[c1])/16)
                 --t2=(math.floor((256-player_y[c1])/16))
                 t1,t2=get_player_coord(c1)
                 if t1==s1 or t2==s2 then break end
             end
            c1=c1+1
        end
        --print(string.format("position_player f2 (%i)", c1))
        if c1==5 then
            player_x[player_num]=(s1-1)*16
            player_y[player_num]=(s2-1)*16
            player_dead_flag[player_num]=false
            player_x_add[player_num]=0
            player_y_add[player_num]=0
            player_direction[player_num]=0
            player_jump_ready[player_num]=1
            player_anim[player_num]=0
            player_frame[player_num]=0
            player_frame_tick[player_num]=0
            break
        end
    end
end

function player_stop(c1)
    s1,s2,t=get_coord_down(c1)

    if t==BAN_ICE then --ON ICE

        if player_x_add[c1]<0 then
            player_x_add[c1]=player_x_add[c1]/1.01
        elseif player_x_add[c1]>0 then
            player_x_add[c1]=player_x_add[c1]/1.01
        end

        if player_anim[c1] == ANIM_RUN_LEFT then
            set_anim(c1,ANIM_STOP_LEFT)
        elseif player_anim[c1] == ANIM_RUN_RIGHT then
            set_anim(c1,ANIM_STOP_RIGHT)
        end

        return
    end

    s1,s2,t=get_player_coord(c1)
    if t==BAN_WATER then --ON SWIM

        --if player_x_add[c1]<0 then
        --    player_x_add[c1]=player_x_add[c1]/1.01
        --elseif player_x_add[c1]>0 then
        --    player_x_add[c1]=player_x_add[c1]/1.01
        --end

        --if player_anim[c1] == ANIM_RUN_LEFT then
        --    set_anim(c1,ANIM_STOP_LEFT)
        --elseif player_anim[c1] == ANIM_RUN_RIGHT then
         --   set_anim(c1,ANIM_STOP_RIGHT)
        --end

        return
    end


    if player_x_add[c1]<0 then
       player_x_add[c1]=player_x_add[c1]+MOVE_BRAKE
        if player_x_add[c1] >0 then
            player_x_add[c1]=0
        end
    elseif player_x_add[c1]>0 then
        player_x_add[c1]=player_x_add[c1]-MOVE_BRAKE
        if player_x_add[c1]<0 then
            player_x_add[c1]=0
        end
    end
    
    if player_anim[c1] == ANIM_RUN_LEFT then
        set_dust(c1,ANIM_STOP_LEFT)
        set_anim(c1,ANIM_STOP_LEFT)
    elseif player_anim[c1] == ANIM_RUN_RIGHT then
        set_dust(c1,ANIM_STOP_RIGHT)
        set_anim(c1,ANIM_STOP_RIGHT)
    end
end

function player_move(c1,direction)
    local move_add=0
    local anim=ANIM_RUN_RIGHT
    s1,s2,t=get_coord_down(c1)

    if direction==DIR_LEFT then 
        if player_direction[c1]==DIR_RIGHT and t~=BAN_ICE and t~=BAN_WATER then player_x_add[c1]=0 end
        move_add=-MOVE_ADD
        anim=ANIM_RUN_LEFT
    else
        if player_direction[c1]==DIR_LEFT and t~=BAN_ICE and t~=BAN_WATER then player_x_add[c1]=0 end
        move_add=MOVE_ADD
        anim=ANIM_RUN_RIGHT
        --if player_y_add[c1]<MOVE_JUMP/2 then anim=ANIM_JUMP_RIGHT end
    end
    
    if t==BAN_VOID then move_add=move_add/2 end

    s1,s2,t=get_player_coord(c1)
    if t==BAN_WATER then
        if direction==DIR_LEFT then anim=ANIM_SWIM_LEFT
        else anim=ANIM_SWIM_RIGHT end
    end

    player_x_add[c1]=player_x_add[c1]+move_add

    player_direction[c1] = direction;
    if player_anim[c1] ~= anim then
        set_anim(c1,anim)
    end
end

function get_player_coord(c1)
    local cX=math.floor((player_x[c1]+8)/16)
    local cY=math.floor((player_y[c1]+8)/16)
    local t=ban_map[cX+1+cY*22] or 0
    return cX,cY,t
end

function get_coord_down(c1)
    local cX=math.floor((player_x[c1]+8)/16)
    local cY=math.floor((player_y[c1]+17)/16)
    local t=ban_map[cX+1+cY*22] or 0
    return cX,cY,t
end

function get_coord_up(c1)
    local cX=math.floor((player_x[c1]+8)/16)
    local cY=math.floor((player_y[c1]-1)/16)
    local t=ban_map[cX+1+cY*22] or 0
    return cX,cY,t
end

function get_coord_left(c1)
    local cX=math.floor((player_x[c1]-1)/16)
    local cY=math.floor((player_y[c1]+8)/16)
    local t=ban_map[cX+1+cY*22] or 0
    return cX,cY,t
end

function get_coord_right(c1)
    local cX=math.floor((player_x[c1]+17)/16)
    local cY=math.floor((player_y[c1]+8)/16)
    local t=ban_map[cX+1+cY*22] or 0
    return cX,cY,t
end

function set_anim(c1,anim)
    player_anim[c1] = anim;
    player_frame[c1] = 0;
    player_frame_tick[c1] = 0;
    local frame=player_anims[player_anim[c1]*10 + player_frame[c1]*2 +3]
    proteo.graphics.setFrame(player_sprite[c1],frame)
end

function set_dust(c1,anim)
    s1,s2,t=get_coord_down(c1)
    local dust_center=4
    if anim==ANIM_STOP_LEFT then dust_center=-4 end
    if t==1 then
        for d=1,DUST_OBJECTS do 
            local i=((c1-1)*DUST_OBJECTS)+d+1
            --proteo.graphics.setFrame(objects[d+1],math.random(5));
            objects_tick[i]=0
            proteo.graphics.setHidden(objects[i],false)
            proteo.graphics.setPosition(objects[i],ZOOM*(player_x[c1]+dust_center+math.random(10)),ZOOM*(player_y[c1]+14-math.random(4)))
        end
    end
end

function object_box(c1)
    objects_tick[1]=0
    player_y_add[c1]=MULTI_BOX*MOVE_JUMP
end

function object_splash(c1)
    local i=1+(DUST_OBJECTS*4)+RED_BF_OBJECTS+YELLOW_BF_OBJECTS+c1
    if objects_tick[i]<0 then
        objects_tick[i]=0
        s1,s2,t=get_coord_down(c1)
        local y=(s2-1)*16
        proteo.graphics.setPosition(objects[i],ZOOM*(player_x[c1]-8),ZOOM*(y+5)) --(player_y[c1]+4+up)
        proteo.graphics.setHidden(objects[i],false)
    end
end

function position_bf(bf)
    while true do
        s1=math.random(22)
        s2=math.random(15)
            --CERCA UNA CASELLA DEL TIPO CORRETTO
        if ban_map[s1+(s2-1)*22]==BAN_VOID then --and (ban_map[s1+1+s2*22]==BAN_SOLID or ban_map[s1+1+s2*22]==BAN_ICE) then
            break
        end
    end
    bf_x[bf]=(s1-1)*16
    bf_y[bf]=(s2-1)*16
    bf_dir[bf]=math.random(8)
end
-------------------
--+++++++++++++++--
-------------------
