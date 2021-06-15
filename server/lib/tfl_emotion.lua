
local inspect = require "inspect"

--require "tfl_utility"

--========= INIT

local tfl_emotion_model=proteo.tflite.modelFromFile(proteo.system.document().."dl/emotion_model_optimized.tflite")

local tfl_interpreter_options=proteo.tflite.createInterpreterOptions()

local tfl_emotion_interpreter=proteo.tflite.createInterpreter(tfl_emotion_model,tfl_interpreter_options)
proteo.tflite.InterpreterAllocateTensors(tfl_emotion_interpreter)

local tfl_emotion_input_tensor =proteo.tflite.InterpreterGetInputTensor(tfl_emotion_interpreter, 0)
local tfl_emotion_inputTensorSize=proteo.tflite.getTensorSize(tfl_emotion_input_tensor)

local tfl_emotion_scores_tensor =proteo.tflite.InterpreterGetOutputTensor(tfl_emotion_interpreter, 0)
local tfl_emotion_scores_outputTensorSize=proteo.tflite.getTensorSize(tfl_emotion_scores_tensor)

local tfl_frame=proteo.opencv.img()
proteo.opencv.setSize(tfl_frame,tfl_emotion_inputTensorSize[2],tfl_emotion_inputTensorSize[3])

function invoke_face_emotion (img)

	proteo.opencv.resize(img,tfl_frame)
	proteo.opencv.convert(tfl_frame,tfl_frame,proteo.opencv.matType.CV_32F,tfl_emotion_inputTensorSize[4])

	
    proteo.opencv.mul(tfl_frame,1.0/255.0,tfl_frame)
	--proteo.opencv.add(tfl_frame,-0.5,tfl_frame)
    --proteo.opencv.mul(tfl_frame,2.0,tfl_frame)

    proteo.tflite.copyImage(tfl_emotion_input_tensor,tfl_frame)

    proteo.tflite.interpreterInvoke(tfl_emotion_interpreter)

    scores = proteo.tflite.tensorToTable(tfl_emotion_scores_tensor)

	print(inspect(scores))

    return scores
end

