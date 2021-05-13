local tetris_lib=require "tetris_lib"



function init()
  tetris_lib.create_template(function(joint)  if (joint[16].y) < (joint[12].y) then
      tetris_lib.left_move_left()
    end
    if (joint[17].y) < (joint[13].y) then
      tetris_lib.left_move_right()
    end
   end)
 end
