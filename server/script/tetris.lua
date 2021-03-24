local json=require "json"


colors={}
colors[1]="red"
colors[2]="green"
colors[3]="orange"
colors[4]="purple"
colors[5]="yellow"
colors[6]="lightblue"
colors[7]="blue"

DOWN=1
LEFT=2
RIGHT=3
ROTATE=4

left_control={}
left_control[LEFT]=false
left_control[RIGHT]=false
left_control[ROTATE]=false

left_mean=-1
n_left_mean=0

left_points_value=0

------------------
--+++++INIT+++++--
------------------

local t={}
t[1]={}
t[2]={}
t[3]={}
t[4]={}
t[5]={}
t[6]={}
t[7]={}

t[1][1]={
                {0,0,0,0},
                {0,0,0,0},
                {1,1,0,0},
                {0,1,1,0}
            }
t[1][2]={
                {0,0,0,0},
                {0,1,0,0},
                {1,1,0,0},
                {1,0,0,0}
            }
t[2][1]={
                {0,0,0,0},
                {0,0,0,0},
                {0,2,2,0},
                {2,2,0,0}
            }
t[2][2]={
                {0,0,0,0},
                {2,0,0,0},
                {2,2,0,0},
                {0,2,0,0}
            }
t[3][1]={
                {0,0,0,0},
                {0,0,0,0},
                {3,3,3,0},
                {0,0,3,0}
            }
t[3][2]={
                {0,0,0,0},
                {3,3,0,0},
                {3,0,0,0},
                {3,0,0,0}
            }
t[3][3]={
                {0,0,0,0},
                {0,0,0,0},
                {3,0,0,0},
                {3,3,3,0}
            }
t[3][4]={
                {0,0,0,0},
                {0,3,0,0},
                {0,3,0,0},
                {3,3,0,0}
            }
t[4][1]={
                {0,0,0,0},
                {0,0,0,0},
                {4,4,4,0},
                {0,4,0,0}
            }
t[4][2]={
                {0,0,0,0},
                {4,0,0,0},
                {4,4,0,0},
                {4,0,0,0}
            }
t[4][3]={
                {0,0,0,0},
                {0,0,0,0},
                {0,4,0,0},
                {4,4,4,0}
            }
t[4][4]={
                {0,0,0,0},
                {0,4,0,0},
                {4,4,0,0},
                {0,4,0,0}
            }
t[5][1]={
                {0,0,0,0},
                {0,0,0,0},
                {5,5,0,0},
                {5,5,0,0}
            }
t[6][1]={
                {0,0,0,0},
                {0,0,0,0},
                {0,0,0,0},
                {6,6,6,6}
            }
t[6][2]={
                {6,0,0,0},
                {6,0,0,0},
                {6,0,0,0},
                {6,0,0,0}
            }
t[7][1]={
                {0,0,0,0},
                {0,0,0,0},
                {7,7,7,0},
                {7,0,0,0}
            }
t[7][2]={
                {0,0,0,0},
                {7,0,0,0},
                {7,0,0,0},
                {7,7,0,0}
            }
t[7][3]={
                {0,0,0,0},
                {0,0,0,0},
                {0,0,7,0},
                {7,7,7,0}
            }
t[7][4]={
                {0,0,0,0},
                {7,7,0,0},
                {0,7,0,0},
                {0,7,0,0}
            }

local floor_left={}  --graphics square matrix
local floor_left_state={
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0}
                       }

local floor_right={}  --graphics square matrix
local floor_right_state={
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0},
                        {0,0,0,0,0,0,0,0,0,0}
                       }

local t_left={}
local t_right={}

    t_left.type=math.random(#t)
    t_left.y=15
    t_left.x=5
    t_left.rotate=1

    t_right.type=math.random(#t)
    t_right.y=15
    t_right.x=5
    t_right.rotate=1


function update_t_event(dt)

    if check_t_left(DOWN)==1 then
        fix_t_left()
        new_t_left()
        return
    end

    if check_t_right(DOWN)==1 then
        fix_t_right()
        new_t_right()
        return
    end

    t_left.y=t_left.y-1
    --t_right.y=t_right.y-1

    update_left_state()
    update_t_left()
    
    --update_right_state()
    --update_t_right()

end

t_timer=proteo.system.createTimer(1000,update_t_event)

function webcam_event(dt)
    proteo.opencv.frame(left_cap,left_frame)
    proteo.opencv.addWeighted(left_frame,0.8,sagoma,0.2,left_frame,0.0)
    proteo.graphics.changeImage(left_img,left_frame)
    --proteo.graphics.changeImage(left_img,sagoma)
end

function init()

    title=proteo.gui.newLabel('title','Gym-TETRIS','Helvetica',50,"white","clear",proteo.gui.LabelAlignment.Center,MID_X-150,MIN_Y+30,300,50)
    
    proteo.graphics.newRect("leftRing_A","white","clear",MIN_X+20,MAX_Y-700,10,500)
    proteo.graphics.newRect("leftRing_B","white","clear",MIN_X+20,MAX_Y-200,320,10)
    proteo.graphics.newRect("leftRing_C","white","clear",MIN_X+330,MAX_Y-700,10,500)

    proteo.graphics.newRect("rightRing_A","white","clear",MAX_X-20,MAX_Y-700,10,500)
    proteo.graphics.newRect("rightRing_B","white","clear",MAX_X-330,MAX_Y-200,320,10)
    proteo.graphics.newRect("rightRing_C","white","clear",MAX_X-330,MAX_Y-700,10,500)

    left_frame=proteo.opencv.img()
    left_img=proteo.graphics.newImage("webcam","@webcam",MIN_X+200,MAX_Y-100,150,112)
    --proteo.opencv.setImg(left_frame,150,150,"#000000")

    points=proteo.gui.newLabel('points','Points:','Helvetica',30,"white","clear",proteo.gui.LabelAlignment.Center,MID_X-150,MID_Y+20,300,30)
    left_points=proteo.gui.newLabel('left_points','000','Helvetica',30,"white","clear",proteo.gui.LabelAlignment.Left,MID_X-150,MID_Y+70,300,30)

    start_button=proteo.gui.newButton('start_button',"Start",'Helvetica',20,"black","lightgrey",1,"black",true,MID_X - 20 ,MID_Y+100,60,30,"start")

    left_cap=proteo.opencv.videocapture(0)
    proteo.opencv.setFrameSize(left_cap,640,480)
    proteo.opencv.setBufferSize(left_cap,3)
    --proteo.opencv.frame(form.opencv_cap,form.opencv_frame)
    
    proteo.opencv.frame(left_cap,left_frame)
    sagoma=proteo.opencv.imread("sagoma.jpg")
    proteo.opencv.resize(sagoma,sagoma,640,480)
    webcam_timer=proteo.system.createTimer(300,webcam_event)
    proteo.system.startTimer(webcam_timer)

     for y=1,20 do
        for x=1,10 do
            create_left_square(x,y,0)
            create_right_square(x,y,0)
        end
    end

    update_left_state()
    update_t_left()

    update_right_state()
    update_t_right()
    
end


function start(sender)

    proteo.network.proteo_post("/tetris/start","{}","start_callback")
    
end

function start_callback(res,data)
    proteo.system.stopTimer(webcam_timer)
    proteo.system.startTimer(t_timer)

    print("CONNECT URL:"..data["url"])
    enet_host = proteo.enet.host_create()
    enet_server = enet_host:connect(data["url"])
    event_timer=proteo.system.createTimer(50,update_event)
    proteo.system.startTimer(event_timer)
end

function update_event(dt)
    local event = enet_host:service(0)
    --print(event)
    if event then
        if event.type == "connect" then
            print("Connected to", event.peer)

            proteo.opencv.frame(left_cap,left_frame)
            proteo.graphics.changeImage(left_img,left_frame)

            tmp=proteo.opencv.imencode(left_frame)

            data={}
            data.type="FRAME"
            data.frame=tmp
            js=json.encode(data)
            
            event.peer:send(js)

        elseif event.type =="disconnect" then
            print("Disconnected from", event.peer)
        elseif event.type == "receive" then
            --print("Got message: "..event.data)
            data=json.decode(event.data)
            
            proteo.opencv.frame(left_cap,left_frame)
            tmp=proteo.opencv.imencode(left_frame)

            for c=0,17 do
                if data.points[tostring(c)].v>0.1 then
                    proteo.opencv.circle(left_frame,data.points[tostring(c)].x,data.points[tostring(c)].y,12,"#FF0000")
                end
            end

            proteo.opencv.line(left_frame,0,140,640,140,8,"#00FF00") --JUMP LINE
            proteo.opencv.line(left_frame,0,240,640,240,8,"#00FF00") --MID LINE

            --Neck
            if data.points["1"].v>0.1 then
                if left_mean<0 then 
                    left_mean=data.points["1"].y 
                    n_left_mean=1
                else 
                    --left_mean = (left_mean + data.points["0"].y)/2 
                    n_left_mean=n_left_mean+1
                    left_mean = left_mean + ((data.points["1"].y - left_mean)/n_left_mean) --Cumulative moving average
                end 
            end
            proteo.opencv.line(left_frame,0,left_mean,640,left_mean,8,"#FFFF00")

            --Neck
            if data.points["1"].v>0.1 then
                    --proteo.opencv.circle(left_frame,data.points["1"].x,data.points["1"].y,8,"#FF0000")
                proteo.opencv.line(left_frame,0,data.points["1"].y,640,data.points["1"].y,8,"#FF0000")
                --proteo.opencv.line(left_frame,0,data.points["1"].y+50,640,data.points["1"].y+50,8,"#FFFF00")
                --proteo.opencv.line(left_frame,0,data.points["1"].y-50,640,data.points["1"].y-50,8,"#FFFF00")
            end
            
            if data.points["1"].y>240 and check_t_left(DOWN)==0 then -- -50 and data.points["7"].y>data.points["1"].y+50 then
                if left_control[DOWN]==false then

                    left_control[DOWN]=true
                    t_left.y=t_left.y-1 
                else
                    left_control[DOWN]=false
                end
            end


            if data.points["1"].y<140 and check_t_left(ROTATE)==0 then -- -50 and data.points["7"].y>data.points["1"].y+50 then
                if left_control[ROTATE]==false then

                    left_control[ROTATE]=true
                    t_left.rotate = get_rotate_left()
                end
            else
                left_control[ROTATE]=false
            end

            --Left Wrist
            if data.points["7"].y<data.points["1"].y and data.points["4"].y>data.points["1"].y and check_t_left(LEFT)==0 then -- -50 and data.points["7"].y>data.points["1"].y+50 then
                if left_control[LEFT]==false then

                    left_control[LEFT]=true
                    t_left.x=t_left.x-1 
                end
            else
                left_control[LEFT]=false
            end

            --Right Wrist
            if data.points["4"].y<data.points["1"].y and data.points["7"].y>data.points["1"].y and check_t_left(RIGHT)==0 then -- -50 and data.points["7"].y>data.points["1"].y+50 then
                if left_control[RIGHT]==false then

                    left_control[RIGHT]=true
                    t_left.x=t_left.x+1 
                end
            else
                left_control[RIGHT]=false
            end


            proteo.opencv.flip(left_frame,left_frame,1)
            proteo.graphics.changeImage(left_img,left_frame)

            data={}
            data.type="FRAME"
            data.frame=tmp
            js=json.encode(data)
            
            event.peer:send(js)
        end
    end
end 

function check_t_left(d)

    local d_y=0
    local d_x=0
    local d_r=t_left.rotate


    if d==DOWN then d_y=-1 
    elseif d==LEFT then d_x=-1
    elseif d==RIGHT then d_x=1 
    elseif d==ROTATE then d_r=get_rotate_left() end

    for y=1,4 do
        for x=1,4 do
            if t[t_left.type][d_r][y][x] ~=0 then

                if t_left.y+d_y+y <= 0 then  return 1 end
                if t_left.x+d_x+x <= 0 then  return 1 end
                if t_left.x+d_x+x > 10 then  return 1 end

                if floor_left_state[t_left.y+d_y+y][t_left.x+d_x+x] ~=0 then

                    print("COLLIDE")
                    return 1
                end
            end

        end
    end

    return 0
end

function check_t_right(d)

    local d_y=0
    local d_x=0
    local d_r=t_right.rotate


    if d==DOWN then d_y=-1 
    elseif d==LEFT then d_x=-1
    elseif d==RIGHT then d_x=1 
    elseif d==ROTATE then d_r=get_rotate_right() end

    for y=1,4 do
        for x=1,4 do
            if t[t_right.type][d_r][y][x] ~=0 then

                if t_right.y+d_y+y <= 0 then  return 1 end
                if t_right.x+d_x+x <= 0 then  return 1 end
                if t_right.x+d_x+x > 10 then  return 1 end

                if floor_right_state[t_right.y+d_y+y][t_right.x+d_x+x] ~=0 then
                    return 1
                end
            end

        end
    end

    return 0
end

function get_rotate_left()
    if t_left.type==5 then
        return 1
    elseif t_left.type==3 or t_left.type==4 or t_left.type==7 then
        return 1+(t_left.rotate%4)
    else
        return 1+(t_left.rotate%2)
    end
end

function get_rotate_right()
    if t_right.type==5 then
        return 1
    elseif t_right.type==3 or t_right.type==4 or t_right.type==7 then
        return 1+(t_right.rotate%4)
    else
        return 1+(t_right.rotate%2)
    end
end

function fix_t_left()
    for y=1,4 do
        for x=1,4 do
            if t[t_left.type][t_left.rotate][y][x]~=0 then
                floor_left_state[t_left.y+y][t_left.x+x]=t[t_left.type][t_left.rotate][y][x]
            end
        end
    end

    local y=1
    local deleted_line=0
    while y<#floor_left_state do
    --for y=1,#floor_left_state do
        local line_full=true
        for x=1,10 do       
            if floor_left_state[y][x]==0 then
                line_full=false
            end
        end
        if line_full then 
            deleted_line=deleted_line+1
            remove_line_left(y)

        else y=y+1 end
    end

    if deleted_line==1 then
        left_points_value=left_points_value+5
    else
        left_points_value=left_points_value+7*deleted_line
    end

    proteo.gui.setText(left_points,left_points_value)
end

function fix_t_right()
    for y=1,4 do
        for x=1,4 do
            if t[t_right.type][t_right.rotate][y][x]~=0 then
                floor_right_state[t_right.y+y][t_right.x+x]=t[t_right.type][t_right.rotate][y][x]
            end
        end
    end

    local y=1
    while y<#floor_right_state do
    --for y=1,#floor_left_state do
        local line_full=true
        for x=1,10 do       
            if floor_right_state[y][x]==0 then
                line_full=false
            end
        end
        if line_full then remove_line_right(y) 
        else y=y+1 end
    end
end

function new_t_left()
    t_left.type=math.random(#t)
    t_left.y=15
    t_left.x=5
    t_left.rotate=1
end

function new_t_right()
    t_right.type=math.random(#t)
    t_right.y=15
    t_right.x=5
    t_right.rotate=1
end

function update_t_left()
    for y=1,4 do
        for x=1,4 do
            if t[t_left.type][t_left.rotate][y][x]~=0 then
                proteo.graphics.setColor(floor_left[t_left.y+y][t_left.x+x].col,colors[t[t_left.type][t_left.rotate][y][x]])
                proteo.graphics.setColor(floor_left[t_left.y+y][t_left.x+x].border,"white")
            else
                --proteo.graphics.setColor(floor_left[t_left.y+y][t_left.x+x].col,"clear")
                --proteo.graphics.setColor(floor_left[t_left.y+y][t_left.x+x].border,"clear")
            end
        end
    end
end

function update_t_right()
    for y=1,4 do
        for x=1,4 do
            if t[t_right.type][t_right.rotate][y][x]~=0 then
                proteo.graphics.setColor(floor_right[t_right.y+y][t_right.x+x].col,colors[t[t_right.type][t_right.rotate][y][x] ])
                proteo.graphics.setColor(floor_right[t_right.y+y][t_right.x+x].border,"white")
            else
                --proteo.graphics.setColor(floor_left[t_left.y+y][t_left.x+x].col,"clear")
                --proteo.graphics.setColor(floor_left[t_left.y+y][t_left.x+x].border,"clear")
            end
        end
    end
end

function remove_line_left(remove_y)
    for y=remove_y,#floor_left_state-1 do
        for x=1,10 do
            floor_left_state[y][x]=floor_left_state[y+1][x]
        end
    end
end

function remove_line_right(remove_y)
    for y=remove_y,#floor_right_state-1 do
        for x=1,10 do
            floor_right_state[y][x]=floor_right_state[y+1][x]
        end
    end
end

function update_left_state()
    for y=1,#floor_left_state do
        for x=1,10 do
            if floor_left_state[y][x]~=0 then
                proteo.graphics.setColor(floor_left[y][x].col,colors[floor_left_state[y][x]])
                proteo.graphics.setColor(floor_left[y][x].border,"white")
            else
                proteo.graphics.setColor(floor_left[y][x].col,"clear")
                proteo.graphics.setColor(floor_left[y][x].border,"clear")
            end
        end
    end
end

function update_right_state()
    for y=1,#floor_right_state do
        for x=1,10 do
            if floor_right_state[y][x]~=0 then
                proteo.graphics.setColor(floor_right[y][x].col,colors[floor_right_state[y][x] ])
                proteo.graphics.setColor(floor_right[y][x].border,"white")
            else
                proteo.graphics.setColor(floor_right[y][x].col,"clear")
                proteo.graphics.setColor(floor_right[y][x].border,"clear")
            end
        end
    end
end

function create_left_square(x,y,col)
    if floor_left[y]==nil then floor_left[y]={} end
    if floor_left[y][x]==nil then floor_left[y][x]={} end

    if col~=0 then
        floor_left[y][x].col=proteo.graphics.newRect("square",colors[col],"clear", 30*x+MIN_X+1, MAX_Y-199-y*30,28,28)
        floor_left[y][x].border=proteo.graphics.newRect("square","white","clear",  MIN_X+30*x,   MAX_Y-200-y*30,30,30)
    else
        floor_left[y][x].col=proteo.graphics.newRect("square","clear","clear", 30*x+MIN_X+1, MAX_Y-199-y*30,28,28)
        floor_left[y][x].border=proteo.graphics.newRect("square","clear","clear",  MIN_X+30*x,   MAX_Y-200-y*30,30,30)
    end
    proteo.graphics.setLayer(floor_left[y][x].col,TOP)
end

function create_right_square(x,y,col)
    if floor_right[y]==nil then floor_right[y]={} end
    if floor_right[y][x]==nil then floor_right[y][x]={} end

    if col~=0 then
        floor_right[y][x].col=proteo.graphics.newRect("square",colors[col],"clear", 30*x+MAX_X-350+1, MAX_Y-199-y*30,28,28)
        floor_right[y][x].border=proteo.graphics.newRect("square","white","clear",  MAX_X-350+30*x,   MAX_Y-200-y*30,30,30)
    else
        floor_right[y][x].col=proteo.graphics.newRect("square","clear","clear", 30*x+MAX_X-350+1, MAX_Y-199-y*30,28,28)
        floor_right[y][x].border=proteo.graphics.newRect("square","clear","clear",  MAX_X-350+30*x,   MAX_Y-200-y*30,30,30)
    end
    proteo.graphics.setLayer(floor_right[y][x].col,TOP)
end

--------------------
--+++++UPDATE+++++--
--------------------



function update(dt)
        

end

function keydown(k)
    if k==proteo.system.key.LEFT and check_t_left(LEFT)==0 then t_left.x=t_left.x-1 end
    if k==proteo.system.key.RIGHT and check_t_left(RIGHT)==0 then t_left.x=t_left.x+1 end
    if k==proteo.system.key.DOWN and check_t_left(DOWN)==0 then t_left.y=t_left.y-1 end
    if k==32 and check_t_left(ROTATE)==0 then t_left.rotate = get_rotate_left() end

    if k==106 and check_t_right(LEFT)==0 then t_right.x=t_right.x-1 end
    if k==107 and check_t_right(RIGHT)==0 then t_right.x=t_right.x+1 end
    if k==108 and check_t_right(DOWN)==0 then t_right.y=t_right.y-1 end
    if k==105 and check_t_right(ROTATE)==0 then t_right.rotate = get_rotate_right() end

end

function keyup(k)

end

function release(x,y)

end

-------------------
--+++++LOCAL+++++--
-------------------


-------------------
--+++++++++++++++--
-------------------
