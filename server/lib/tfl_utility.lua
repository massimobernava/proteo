
function calc_intersection_over_union (region0, region1)

    local sx0 = region0.topleft.x
    local sy0 = region0.topleft.y
    local ex0 = region0.btmright.x
    local ey0 = region0.btmright.y
    local sx1 = region1.topleft.x
    local sy1 = region1.topleft.y
    local ex1 = region1.btmright.x
    local ey1 = region1.btmright.y

    local xmin0 = math.min (sx0, ex0)
    local ymin0 = math.min (sy0, ey0)
    local xmax0 = math.max (sx0, ex0)
    local ymax0 = math.max (sy0, ey0)
    local xmin1 = math.min (sx1, ex1)
    local ymin1 = math.min (sy1, ey1)
    local xmax1 = math.max (sx1, ex1)
    local ymax1 = math.max (sy1, ey1)

    local area0 = (ymax0 - ymin0) * (xmax0 - xmin0)
    local area1 = (ymax1 - ymin1) * (xmax1 - xmin1)

    if area0 <= 0 or area1 <= 0 then
        return 0.0
    end

    local intersect_xmin = math.max (xmin0, xmin1)
    local intersect_ymin = math.max (ymin0, ymin1)
    local intersect_xmax = math.min (xmax0, xmax1)
    local intersect_ymax = math.min (ymax0, ymax1)

    local intersect_area = math.max (intersect_ymax - intersect_ymin, 0.0) *
                         math.max (intersect_xmax - intersect_xmin, 0.0)

    return intersect_area / (area0 + area1 - intersect_area)

end

function non_max_suppression (region_list,iou_thresh)

    local region_nms_list={}

    table.sort(region_list, function(a,b) return a.score>b.score end)

    for i = 1,#region_list do
    
        local region_candidate = region_list[i]
        local ignore_candidate = false
        for j = 1,#region_nms_list do
        
            local region_nms = region_nms_list[j]
            local iou = calc_intersection_over_union (region_candidate, region_nms)
            
            if iou >= iou_thresh then
                ignore_candidate = true
                break
            end
        end

        if not ignore_candidate then
            table.insert(region_nms_list,region_candidate)
        end
    end

    return region_nms_list
end

function normalize_radians (angle)

    return angle - 2 * math.pi * math.floor((angle - (-math.pi)) / (2 * math.pi))

end

function compute_rotation (region,k0,k1)

    local x0 = region.keys[k0].x
    local y0 = region.keys[k0].y
    local x1 = region.keys[k1].x
    local y1 = region.keys[k1].y

    local target_angle = math.pi * 0.5
    local rotation = target_angle - math.atan2(-(y1 - y0), x1 - x0)

    region.rotation = normalize_radians(rotation)

end

function rot_vec (vec, rotation)

    local sx = vec.x
    local sy = vec.y
    vec.x = sx * math.cos(rotation) - sy * math.sin(rotation)
    vec.y = sx * math.sin(rotation) + sy * math.cos(rotation)
end

function CalculateScale(min_scale, max_scale, stride_index, num_strides)

     if num_strides == 1 then
        return (min_scale + max_scale) * 0.5
    else
        return min_scale + (max_scale - min_scale) * 1.0 * stride_index / (num_strides - 1.0)
    end

end

function GenerateAnchors(options)

    local anchors={}

    layer_id = 0
    while layer_id < #options.strides do

        local anchor_height = {}
        local anchor_width = {}
        local aspect_ratios = {}
        local scales = {}

        local last_same_stride_layer = layer_id

        while last_same_stride_layer < #options.strides and options.strides[last_same_stride_layer+1] == options.strides[layer_id+1] do

            local scale = CalculateScale(options.min_scale, options.max_scale, last_same_stride_layer, #options.strides)
            if last_same_stride_layer == 0 and options.reduce_boxes_in_lowest_layer then
                
                -- For first layer, it can be specified to use predefined anchors.
                table.insert(aspect_ratios,1.0)
                table.insert(aspect_ratios,2.0)
                table.insert(aspect_ratios,0.5)
                table.insert(scales,0.1)
                table.insert(scales,scale)
                table.insert(scales,scale)
            else
                for aspect_ratio_id=1,#options.aspect_ratios do
                    table.insert(aspect_ratios,options.aspect_ratios[aspect_ratio_id])
                    table.insert(scales,scale)
                end
                if options.interpolated_scale_aspect_ratio > 0.0 then
                    local scale_next = 1.0
                    if last_same_stride_layer ~= #options.strides - 1 then
                        scale_next =CalculateScale(options.min_scale, options.max_scale, last_same_stride_layer + 1, #options.strides)
                    end
                    table.insert(scales,math.sqrt(scale * scale_next))
                    table.insert(aspect_ratios,options.interpolated_scale_aspect_ratio)
                end
            end
            last_same_stride_layer=last_same_stride_layer+1
        end

        for i=1,#aspect_ratios do
            local ratio_sqrts = math.sqrt(aspect_ratios[i])
            table.insert(anchor_height,scales[i] / ratio_sqrts)
            table.insert(anchor_width,scales[i] * ratio_sqrts)
        end

        local feature_map_height = 0
        local feature_map_width  = 0
        if #options.feature_map_height > 0 then
            feature_map_height = options.feature_map_height[layer_id+1]
            feature_map_width  = options.feature_map_width [layer_id+1]
        else 
            local stride = options.strides[layer_id+1]
            feature_map_height = math.ceil(1.0 * options.input_size_height / stride)
            feature_map_width  = math.ceil(1.0 * options.input_size_width  / stride)
        end

        for y=0,feature_map_height-1 do
            for x=0,feature_map_width-1 do
                for anchor_id=1,#anchor_height do
                    --TODO: Support specifying anchor_offset_x, anchor_offset_y
                    local x_center = (x + options.anchor_offset_x) * 1.0 / feature_map_width
                    local y_center = (y + options.anchor_offset_y) * 1.0 / feature_map_height

                    local new_anchor = {}
                    new_anchor.x_center = x_center
                    new_anchor.y_center = y_center

                    if options.fixed_anchor_size then
                        new_anchor.w = 1.0
                        new_anchor.h = 1.0
                    else 
                        new_anchor.w = anchor_width [anchor_id]
                        new_anchor.h = anchor_height[anchor_id]
                    end
                    table.insert(anchors,new_anchor)
                end
            end
        end
        layer_id = last_same_stride_layer
    end

    return anchors
end

function show_pose(landmarks,image)
    proteo.opencv.line(image,(landmarks[12].x),(landmarks[12].y),(landmarks[13].x),(landmarks[13].y),16,'#cc33cc')
    proteo.opencv.line(image,(landmarks[12].x),(landmarks[12].y),(landmarks[14].x),(landmarks[14].y),16,'#993399')
    proteo.opencv.line(image,(landmarks[13].x),(landmarks[13].y),(landmarks[15].x),(landmarks[15].y),16,'#993399')
    proteo.opencv.line(image,(landmarks[14].x),(landmarks[14].y),(landmarks[16].x),(landmarks[16].y),16,'#cc33cc')
    proteo.opencv.line(image,(landmarks[15].x),(landmarks[15].y),(landmarks[17].x),(landmarks[17].y),16,'#cc33cc')
    proteo.opencv.line(image,(landmarks[13].x),(landmarks[13].y),(landmarks[25].x),(landmarks[25].y),16,'#cc66cc')
    proteo.opencv.line(image,(landmarks[12].x),(landmarks[12].y),(landmarks[24].x),(landmarks[24].y),16,'#cc66cc')
    proteo.opencv.line(image,(landmarks[25].x),(landmarks[25].y),(landmarks[24].x),(landmarks[24].y),16,'#ff99ff')
end

function show_facemesh(landmarks,image)

end

function show_landmark(bbox,image,landmarks)
        
    local size=proteo.opencv.size(image)
    
    proteo.opencv.rectangle(image,bbox.topleft.x*size[2],bbox.topleft.y*size[1],
                                                                    bbox.btmright.x*size[2],bbox.btmright.y*size[1],
                                                                    3,"#FF0000")

    t_score=string.format("%.2f",bbox.score*100)
    proteo.opencv.putText(image,t_score,bbox.topleft.x*size[2],bbox.topleft.y*size[1]-10,0.5,"#FF0000")

    proteo.opencv.line(image,bbox.roi_coord[1].x*size[2],bbox.roi_coord[1].y*size[1],bbox.roi_coord[2].x*size[2],bbox.roi_coord[2].y*size[1],1,"#FFFF00")
    proteo.opencv.line(image,bbox.roi_coord[2].x*size[2],bbox.roi_coord[2].y*size[1],bbox.roi_coord[3].x*size[2],bbox.roi_coord[3].y*size[1],1,"#FFFF00")
    proteo.opencv.line(image,bbox.roi_coord[3].x*size[2],bbox.roi_coord[3].y*size[1],bbox.roi_coord[4].x*size[2],bbox.roi_coord[4].y*size[1],1,"#FFFF00")
    proteo.opencv.line(image,bbox.roi_coord[4].x*size[2],bbox.roi_coord[4].y*size[1],bbox.roi_coord[1].x*size[2],bbox.roi_coord[1].y*size[1],1,"#FFFF00")

    proteo.opencv.rectangle(image,bbox.topleft.x*size[2],bbox.topleft.y*size[1],bbox.btmright.x*size[2],bbox.btmright.y*size[1],1,"#FF0000")
            
    for j=1,#bbox.keys do
        proteo.opencv.circle(image, bbox.keys[j].x*size[2], bbox.keys[j].y*size[1], 2,"#0000FF")
    end

     --LANDMARK
    local mat=proteo.opencv.getAffineTransform(0,0
                                              ,1,0
                                              ,1,1
                                              ,bbox.roi_coord[1].x * size[2], bbox.roi_coord[1].y * size[1]
                                              ,bbox.roi_coord[2].x * size[2], bbox.roi_coord[2].y * size[1]
                                              ,bbox.roi_coord[3].x * size[2], bbox.roi_coord[3].y * size[1]
                                               )
    if landmarks~=nil then
                --proteo.opencv.print(mat)
        mat_table=proteo.opencv.toTable(mat)
              --tprint(mat_table)

        ret={}
        for j=1,#landmarks.joint do
            ret[j]={}
            ret[j].x=landmarks.joint[j].x*mat_table[1][1]+landmarks.joint[j].y*mat_table[1][2]+mat_table[1][3]
            ret[j].y=landmarks.joint[j].x*mat_table[2][1]+landmarks.joint[j].y*mat_table[2][2]+mat_table[2][3]
            ret[j].z=landmarks.joint[j].z

            if j==8 or j==9 or j==250 then
                --proteo.opencv.circle(image, ret[j].x, ret[j].y,8,"#0000FF")
                id=string.format("%d",j)
                proteo.opencv.putText(image,id,ret[j].x, ret[j].y,0.5,"#FF0000")
            --elseif j>=249 and j<=255 then
            --    id=string.format("%d",j)
            --    proteo.opencv.putText(image,id,ret[j].x, ret[j].y,0.5,"#0000FF")
            else
                proteo.opencv.circle(image, ret[j].x, ret[j].y,2,"#00FF00")
            end
        end

        return ret
    end

    return nil
end

function get_landmark(bbox,image,landmarks)
        
    local size=proteo.opencv.size(image)

     --LANDMARK
    local mat=proteo.opencv.getAffineTransform(0,0
                                              ,1,0
                                              ,1,1
                                              ,bbox.roi_coord[1].x * size[2], bbox.roi_coord[1].y * size[1]
                                              ,bbox.roi_coord[2].x * size[2], bbox.roi_coord[2].y * size[1]
                                              ,bbox.roi_coord[3].x * size[2], bbox.roi_coord[3].y * size[1]
                                               )
    if landmarks~=nil then
                --proteo.opencv.print(mat)
        mat_table=proteo.opencv.toTable(mat)
              --tprint(mat_table)

        ret={}
        for j=1,#landmarks.joint do
            ret[j]={}
            ret[j].x=landmarks.joint[j].x*mat_table[1][1]+landmarks.joint[j].y*mat_table[1][2]+mat_table[1][3]
            ret[j].y=landmarks.joint[j].x*mat_table[2][1]+landmarks.joint[j].y*mat_table[2][2]+mat_table[2][3]
            ret[j].z=landmarks.joint[j].z
        end

        return ret
    end

    return nil
end