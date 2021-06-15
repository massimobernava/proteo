
local inspect = require "inspect"

require "tfl_utility"

local function decode_hand_bounds (scores, bbox, score_thresh, input_img_w, input_img_h)

	local region_list={}


	for i=1,#hand_anchors do
		
		region = {}
		anchor = hand_anchors[i]
		score0 = scores[i]
		score = 1.0 / (1.0 + math.exp(-score0))

		if score > score_thresh then

            local numkey = kHandDetectKeyNum
            local bbx_idx = (4 + 2 * numkey) * (i-1)

            -- boundary box 
            local sx = bbox[bbx_idx + 1]
            local sy = bbox[bbx_idx + 2]
            local w  = bbox[bbx_idx + 3]
            local h  = bbox[bbx_idx + 4]

            local cx = sx + anchor.x_center * input_img_w
            local cy = sy + anchor.y_center * input_img_h

            cx = cx / input_img_w
            cy = cy / input_img_h
            w  = w / input_img_w
            h  = h / input_img_h

            topleft = {}
            btmright = {}
            topleft.x  = cx - w * 0.5
            topleft.y  = cy - h * 0.5
            btmright.x = cx + w * 0.5
            btmright.y = cy + h * 0.5

            region.score    = score
            region.topleft  = topleft
            region.btmright = btmright

            -- landmark positions (6 keys)
            local keys={}
            for j=1,kHandDetectKeyNum do

            	local lx = bbox[bbx_idx + 4 + (2 * (j-1)) + 1]
                local ly = bbox[bbx_idx + 4 + (2 * (j-1)) + 2]
                lx = lx + anchor.x_center * input_img_w
                ly = ly + anchor.y_center * input_img_h
                lx = lx / input_img_w
                ly = ly / input_img_h

                keys[j] = {}
            	keys[j].x=lx
            	keys[j].y=ly

            end

            region.keys = keys
            table.insert(region_list,region)
		end
	end

	return region_list
end


function compute_detect_hand_to_roi (region)
	
	local input_img_w = tfl_detect_inputTensorSize[2]
    local input_img_h = tfl_detect_inputTensorSize[3]
    --local x_center = (region.topleft.x + region.btmright.x) * input_img_w / 2
    --local y_center = (region.topleft.y + region.btmright.y) * input_img_h / 2

    local x_center = region.keys[kMiddle].x * input_img_w
    local y_center = region.keys[kMiddle].y * input_img_h


    --local box_size = math.sqrt((x_left - x_right) * (x_left - x_right) +
    --                         (y_left - y_right) * (y_left - y_right)) * 2.5
    --local box_size = math.sqrt((region.topleft.x - region.btmright.x) * (region.topleft.x - region.btmright.x) +
    --                        (region.topleft.y - region.btmright.y) * (region.topleft.y - region.btmright.y))*2.0
    local box_size = math.max(math.abs(region.topleft.x - region.btmright.x) * input_img_w,math.abs(region.topleft.y - region.btmright.y) * input_img_h)

    ---se il volto è troppo lontano questo valore diventa troppo piccolo (ad occhio sotto il 40) 

    -- RectTransformationCalculator::TransformNormalizedRect() 
    local width    = math.max(math.abs(region.topleft.x - region.btmright.x) * input_img_w, math.abs(region.keys[kThumb_MCP].x - region.keys[kPinky].x) * input_img_w) --box_size
    local height   = math.max(math.abs(region.topleft.y - region.btmright.y) * input_img_h, math.abs(region.keys[kWrist].y - region.keys[kMiddle].y) * input_img_h) --box_size
    local rotation = region.rotation
    local shift_x =  0.0
    local shift_y =  0.0
    local roi_cx
    local roi_cy

    if rotation == 0.0 then
    
        roi_cx = x_center + (width  * shift_x)
        roi_cy = y_center + (height * shift_y)
    
    else
    
        local dx = (width  * shift_x) * math.cos(rotation) -
                 (height * shift_y) * math.sin(rotation)
        local dy = (width  * shift_x) * math.sin(rotation) +
                 (height * shift_y) * math.cos(rotation)
        roi_cx = x_center + dx
        roi_cy = y_center + dy
    end

    local scale_x = 2.6 --1.5
    local scale_y = 2.6 --1.5
    local long_side = math.max (width, height)
    local roi_w = long_side * scale_x
    local roi_h = long_side * scale_y

    region.roi_center = {x = roi_cx / input_img_w,  y= roi_cy / input_img_h}
    region.roi_size   = {x= roi_w  / input_img_w,  y= roi_h  / input_img_h}

    -- calculate ROI coordinates
    local dx = roi_w * 0.5
    local dy = roi_h * 0.5
    region.roi_coord = {}
    region.roi_coord[1] = {x= - dx, y= - dy}
    region.roi_coord[2] = {x= dx, y= - dy}
    region.roi_coord[3] = {x= dx, y= dy}
    region.roi_coord[4] = {x= - dx, y= dy}

    for i = 1,4 do
        rot_vec (region.roi_coord[i], rotation)
        region.roi_coord[i].x = region.roi_coord[i].x + roi_cx
        region.roi_coord[i].y = region.roi_coord[i].y + roi_cy

        region.roi_coord[i].x = region.roi_coord[i].x  / input_img_h
        region.roi_coord[i].y = region.roi_coord[i].y / input_img_h
    end
end

function pack_detect_hand_result (region_list)

	local detect_result={}

	for i = 1,#region_list do
    
        region = region_list[i]

        compute_rotation (region,kWrist,kMiddle)
        compute_detect_hand_to_roi (region)

        table.insert(detect_result,region)
    end

	return detect_result

end

function create_hand_ssd_anchors(input_w,intput_h)

	local anchor_options = {}
	anchor_options.num_layers = 4
	anchor_options.strides = {8,16,16,16}
	anchor_options.aspect_ratios = {1.0}
	anchor_options.feature_map_height ={}

	anchor_options.min_scale = 0.1484375
    anchor_options.max_scale = 0.75
    anchor_options.input_size_height = 128
    anchor_options.input_size_width  = 128
    anchor_options.anchor_offset_x  = 0.5
    anchor_options.anchor_offset_y  = 0.5

    anchor_options.reduce_boxes_in_lowest_layer = false
    anchor_options.interpolated_scale_aspect_ratio = 1.0
	anchor_options.fixed_anchor_size = true

--[[
# Options to generate anchors for SSD object detection models.
            ssd_anchors_calculator_options = SsdAnchorsCalculatorOptions(input_size_width=256, input_size_height=256, min_scale=0.15625, max_scale=0.75
                    , anchor_offset_x=0.5, anchor_offset_y=0.5, num_layers=4
                    , feature_map_width=[], feature_map_height=[]
                    , strides=[16, 32, 32, 32], aspect_ratios=[1.0]
                    , reduce_boxes_in_lowest_layer=False, interpolated_scale_aspect_ratio=1.0
                    , fixed_anchor_size=True)
                    ]]
	return GenerateAnchors(anchor_options)
end


--========= INIT

kHandDetectKeyNum=7

kWrist=1
kIndex=2
kMiddle=3
kRing=4
kPinky=5
kThumb_CMC=6
kThumb_MCP=7

HAND_LM_NUM=21

local tfl_detect_model=proteo.tflite.modelFromFile(proteo.system.document().."dl/palm_detection.tflite")
local tfl_landmark_model=proteo.tflite.modelFromFile(proteo.system.document().."dl/hand_landmark.tflite")

local tfl_interpreter_options=proteo.tflite.createInterpreterOptions()

local tfl_detect_interpreter=proteo.tflite.createInterpreter(tfl_detect_model,tfl_interpreter_options)
proteo.tflite.InterpreterAllocateTensors(tfl_detect_interpreter)

local tfl_landmark_interpreter=proteo.tflite.createInterpreter(tfl_landmark_model,tfl_interpreter_options)
proteo.tflite.InterpreterAllocateTensors(tfl_landmark_interpreter)

local tfl_detect_input_tensor =proteo.tflite.InterpreterGetInputTensor(tfl_detect_interpreter, 0)
local tfl_detect_inputTensorSize=proteo.tflite.getTensorSize(tfl_detect_input_tensor)

local tfl_landmark_input_tensor =proteo.tflite.InterpreterGetInputTensor(tfl_landmark_interpreter, 0)
local tfl_landmark_inputTensorSize=proteo.tflite.getTensorSize(tfl_landmark_input_tensor)

local tfl_detect_scores_tensor =proteo.tflite.InterpreterGetOutputTensor(tfl_detect_interpreter, 1)
local tfl_detect_scores_outputTensorSize=proteo.tflite.getTensorSize(tfl_detect_scores_tensor)
local tfl_detect_bbox_tensor =proteo.tflite.InterpreterGetOutputTensor(tfl_detect_interpreter, 0)
local tfl_detect_bbox_outputTensorSize=proteo.tflite.getTensorSize(tfl_detect_bbox_tensor)

local tfl_landmark_score_tensor =proteo.tflite.InterpreterGetOutputTensor(tfl_landmark_interpreter, 0)
local tfl_landmark_score_outputTensorSize=proteo.tflite.getTensorSize(tfl_landmark_score_tensor)

local tfl_frame=proteo.opencv.img()
proteo.opencv.setSize(tfl_frame,tfl_detect_inputTensorSize[2],tfl_detect_inputTensorSize[3])

hand_anchors=create_hand_ssd_anchors(tfl_detect_inputTensorSize[3],tfl_detect_inputTensorSize[2])

--ocv_detect_model=proteo.opencv.readnet("pose/pose_detection_model_float32.pb","")
--proteo.opencv.infoNet(ocv_detect_model)

function invoke_hand_detect (img)

	proteo.opencv.resize(img,tfl_frame)
	proteo.opencv.convert(tfl_frame,tfl_frame,proteo.opencv.matType.CV_32F,tfl_detect_inputTensorSize[4])

	
    proteo.opencv.mul(tfl_frame,1.0/255.0,tfl_frame)
	proteo.opencv.add(tfl_frame,-0.5,tfl_frame)
    proteo.opencv.mul(tfl_frame,2.0,tfl_frame)

    proteo.tflite.copyImage(tfl_detect_input_tensor,tfl_frame)
    
    --[[proteo.tflite.copyTensor(tfl_frame,tfl_detect_input_tensor)
    proteo.opencv.mul(tfl_frame,0.5,tfl_frame)
    proteo.opencv.add(tfl_frame,0.5,tfl_frame)
    proteo.opencv.mul(tfl_frame,255.0,tfl_frame)
    proteo.opencv.imwrite(proteo.system.document().."prova.jpg",tfl_frame)]]

    proteo.tflite.interpreterInvoke(tfl_detect_interpreter)

    scores = proteo.tflite.tensorToTable(tfl_detect_scores_tensor)
    bbox = proteo.tflite.tensorToTable(tfl_detect_bbox_tensor)
    score_thresh = 0.5

    region_list = decode_hand_bounds (scores, bbox, score_thresh, tfl_detect_inputTensorSize[3], tfl_detect_inputTensorSize[2])

    iou_thresh = 0.3

    region_nms_list = non_max_suppression (region_list, iou_thresh)

	detect_result=pack_detect_hand_result (region_nms_list)

	--print(inspect(detect_result))

    return detect_result
end

function invoke_hand_landmark(img)
	landmark_result={}

	proteo.tflite.copyImage(tfl_landmark_input_tensor,img)
    proteo.tflite.interpreterInvoke(tfl_landmark_interpreter)

    landmarks = proteo.tflite.tensorToTable(tfl_landmark_score_tensor)

    landmark_result.score = 0 --TODO
    landmark_result.joint={}
    for i = 1,HAND_LM_NUM do
    	landmark_result.joint[i]={}
        landmark_result.joint[i].x = landmarks[3 * (i-1) + 1] / tfl_landmark_inputTensorSize[3]
        landmark_result.joint[i].y = landmarks[3 * (i-1) + 2] / tfl_landmark_inputTensorSize[2]
        landmark_result.joint[i].z = landmarks[3 * (i-1) + 3]
    end


	return landmark_result
end

function generate_hand_landmark_input_image (image, pose)
	
	local size=proteo.opencv.size(image)
	local H=size[1]
    local W=size[2]

	local mat=proteo.opencv.getAffineTransform(pose.roi_coord[1].x * W, pose.roi_coord[1].y * H
											  ,pose.roi_coord[2].x * W, pose.roi_coord[2].y * H
											  ,pose.roi_coord[3].x * W, pose.roi_coord[3].y * H
											  ,0,0
											  ,tfl_landmark_inputTensorSize[3],0
											  ,tfl_landmark_inputTensorSize[3],tfl_landmark_inputTensorSize[2] )
	img_affine=proteo.opencv.img()
	proteo.opencv.setImg(img_affine,tfl_landmark_inputTensorSize[3],tfl_landmark_inputTensorSize[2],"#000000")
	proteo.opencv.warpAffine(image,img_affine,mat)

    --proteo.opencv.imwrite(proteo.system.document().."prova.jpg",img_affine)

    proteo.opencv.convert(img_affine,img_affine,proteo.opencv.matType.CV_32F,tfl_landmark_inputTensorSize[4])
    proteo.opencv.mul(img_affine,1.0/255.0,img_affine)

    return img_affine
end
