//===============================================================================
//  OPENCV
//===============================================================================

Blockly.Blocks['opencv_img'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create image");
    this.setOutput(true, "opencv_img");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_img'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.img()';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['opencv_videocapture'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck(["Number", "String"])
        .appendField("capture video from:");
    this.setOutput(true, "opencv_videocapture");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_videocapture'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.videocapture('+value_from+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['opencv_setframesize'] = {
  init: function() {
    this.appendValueInput("NAME")
        .setCheck("opencv_videocapture")
        .appendField("set video capture");
    this.appendDummyInput()
        .appendField("with frame size")
        .appendField(new Blockly.FieldNumber(0,0), "w")
        .appendField(new Blockly.FieldNumber(0,0), "h");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_setframesize'] = function(block) {
  var value_name = Blockly.Lua.valueToCode(block, 'NAME', Blockly.Lua.ORDER_ATOMIC);
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.setFrameSize('+value_name+','+number_w+','+number_h+')\n';
  return code;
};

Blockly.Blocks['opencv_setbuffersize'] = {
  init: function() {
    this.appendValueInput("NAME")
        .setCheck("opencv_videocapture")
        .appendField("set video capture");
    this.appendDummyInput()
        .appendField("with buffer size")
        .appendField(new Blockly.FieldNumber(0,0), "b")
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_setbuffersize'] = function(block) {
  var value_name = Blockly.Lua.valueToCode(block, 'NAME', Blockly.Lua.ORDER_ATOMIC);
  var number_b = block.getFieldValue('b');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.setBufferSize('+value_name+','+number_b+')\n';
  return code;
};

Blockly.Blocks['opencv_frame'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_videocapture")
        .appendField("get frame from");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("and put to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_frame'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.frame('+value_from+','+value_to+')\n';
  return code;
};

Blockly.Blocks['opencv_setimg'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("set image");
    this.appendDummyInput()
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0, 0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0, 0), "h")
        .appendField("color:")
        .appendField(new Blockly.FieldColour("#000000"), "color");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_setimg'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  var colour_color = block.getFieldValue('color');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.setImg('+value_from+','+number_w+','+number_h+',"'+colour_color+'")\n';
  return code;
};

/*Blockly.Blocks['opencv_resize'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("resize image");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("and put to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_resize'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.resize('+value_from+','+value_to+')\n';
  return code;
};*/

Blockly.Blocks['opencv_resize'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("resize image");
    this.appendDummyInput()
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0, 0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0, 0), "h");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("and put to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_resize'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  
  var code = 'proteo.opencv.resize('+value_from+','+value_to;

  if (number_w>0 && number_h>0)
  {
    code = code+','+ number_h+','+number_w;
  }

  code = code +')\n';

  return code;
};

Blockly.Blocks['opencv_imencode'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("encode image");
    this.setInputsInline(false);
    this.setOutput(true, "String");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_imencode'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.imencode('+value_from+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['opencv_imdecode'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("String")
        .appendField("decode image");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_imdecode'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.imdecode('+value_from+','+value_to+')\n';
  return code;
};

Blockly.Blocks['opencv_size'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("get size of");
    this.setInputsInline(false);
    this.setOutput(true, "String");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_size'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.size('+value_from+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['opencv_flip'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("flip frame");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("around")
        .appendField(new Blockly.FieldDropdown([["x-axis","0"], ["y-axis","1"], [" both axes","-1"]]), "mode")
        .appendField("and put to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_flip'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var dropdown_mode = block.getFieldValue('mode');
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.flip('+value_from+','+value_to+','+dropdown_mode+')\n';
  return code;
};

Blockly.Blocks['opencv_rectangle'] = {
  init: function() {
    this.appendValueInput("x1")
        .setCheck("Number")
        .appendField("draw rectangle x1:");
    this.appendValueInput("y1")
        .setCheck("Number")
        .appendField("y1:");
    this.appendValueInput("x2")
        .setCheck("Number")
        .appendField("x2:");
    this.appendValueInput("y2")
        .setCheck("Number")
        .appendField("y2:");
    this.appendValueInput("line")
        .setCheck("Number")
        .appendField("line size:");
    this.appendValueInput("color")
        .setCheck(null)
        .appendField("color:");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("to image");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};


Blockly.Lua['opencv_rectangle'] = function(block) {
  var value_x1 = Blockly.Lua.valueToCode(block, 'x1', Blockly.Lua.ORDER_ATOMIC);
  var value_y1 = Blockly.Lua.valueToCode(block, 'y1', Blockly.Lua.ORDER_ATOMIC);
  var value_x2 = Blockly.Lua.valueToCode(block, 'x2', Blockly.Lua.ORDER_ATOMIC);
  var value_y2 = Blockly.Lua.valueToCode(block, 'y2', Blockly.Lua.ORDER_ATOMIC);
  var value_line = Blockly.Lua.valueToCode(block, 'line', Blockly.Lua.ORDER_ATOMIC);
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  var code = 'proteo.opencv.rectangle('+value_to+','+value_x1+','+value_y1+','+value_x2+','+value_y2+','+value_line+','+value_color+')\n';
  return code;
};

Blockly.Blocks['opencv_line'] = {
  init: function() {
    this.appendValueInput("x1")
        .setCheck("Number")
        .appendField("draw line x1:");
    this.appendValueInput("y1")
        .setCheck("Number")
        .appendField("y1:");
    this.appendValueInput("x2")
        .setCheck("Number")
        .appendField("x2:");
    this.appendValueInput("y2")
        .setCheck("Number")
        .appendField("y2:");
    this.appendValueInput("line")
        .setCheck("Number")
        .appendField("size:");
    this.appendValueInput("color")
        .setCheck(null)
        .appendField("color:");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("to image");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_line'] = function(block) {
  var value_x1 = Blockly.Lua.valueToCode(block, 'x1', Blockly.Lua.ORDER_ATOMIC);
  var value_y1 = Blockly.Lua.valueToCode(block, 'y1', Blockly.Lua.ORDER_ATOMIC);
  var value_x2 = Blockly.Lua.valueToCode(block, 'x2', Blockly.Lua.ORDER_ATOMIC);
  var value_y2 = Blockly.Lua.valueToCode(block, 'y2', Blockly.Lua.ORDER_ATOMIC);
  var value_line = Blockly.Lua.valueToCode(block, 'line', Blockly.Lua.ORDER_ATOMIC);
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  var code = 'proteo.opencv.line('+value_to+','+value_x1+','+value_y1+','+value_x2+','+value_y2+','+value_line+','+value_color+')\n';
  
  return code;
};

Blockly.Blocks['opencv_getaffinetransform'] = {
  init: function() {
    this.appendValueInput("x1")
        .setCheck("Number")
        .appendField("get affine transform from x1:");
    this.appendValueInput("y1")
        .setCheck("Number")
        .appendField("y1:");
    this.appendValueInput("x2")
        .setCheck("Number")
        .appendField("x2:");
    this.appendValueInput("y2")
        .setCheck("Number")
        .appendField("y2:");
    this.appendValueInput("x3")
        .setCheck("Number")
        .appendField("x3:");
    this.appendValueInput("y3")
        .setCheck("Number")
        .appendField("y3");
    this.appendValueInput("x4")
        .setCheck("Number")
        .appendField("to x4:");
    this.appendValueInput("y4")
        .setCheck("Number")
        .appendField("y4:");
    this.appendValueInput("x5")
        .setCheck("Number")
        .appendField("x5:");
    this.appendValueInput("y5")
        .setCheck("Number")
        .appendField("y5:");
    this.appendValueInput("x6")
        .setCheck("Number")
        .appendField("x6:");
    this.appendValueInput("y6")
        .setCheck("Number")
        .appendField("y6:");
    this.setInputsInline(true);
    this.setOutput(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_getaffinetransform'] = function(block) {
  var value_x1 = Blockly.Lua.valueToCode(block, 'x1', Blockly.Lua.ORDER_ATOMIC);
  var value_y1 = Blockly.Lua.valueToCode(block, 'y1', Blockly.Lua.ORDER_ATOMIC);
  var value_x2 = Blockly.Lua.valueToCode(block, 'x2', Blockly.Lua.ORDER_ATOMIC);
  var value_y2 = Blockly.Lua.valueToCode(block, 'y2', Blockly.Lua.ORDER_ATOMIC);
  var value_x3 = Blockly.Lua.valueToCode(block, 'x3', Blockly.Lua.ORDER_ATOMIC);
  var value_y3 = Blockly.Lua.valueToCode(block, 'y3', Blockly.Lua.ORDER_ATOMIC);
  var value_x4 = Blockly.Lua.valueToCode(block, 'x4', Blockly.Lua.ORDER_ATOMIC);
  var value_y4 = Blockly.Lua.valueToCode(block, 'y4', Blockly.Lua.ORDER_ATOMIC);
  var value_x5 = Blockly.Lua.valueToCode(block, 'x5', Blockly.Lua.ORDER_ATOMIC);
  var value_y5 = Blockly.Lua.valueToCode(block, 'y5', Blockly.Lua.ORDER_ATOMIC);
  var value_x6 = Blockly.Lua.valueToCode(block, 'x6', Blockly.Lua.ORDER_ATOMIC);
  var value_y6 = Blockly.Lua.valueToCode(block, 'y6', Blockly.Lua.ORDER_ATOMIC);

  var code = 'proteo.opencv.getAffineTransform('+value_x1+','+value_y1+','+value_x2+','+value_y2+','+value_x3+','+value_y3+','+value_x4+','+value_y4+','
  +value_x5+','+value_y5+','+value_x6+','+value_y6+')';

  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['opencv_totable'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("convert matrix to table");
    this.setInputsInline(false);
    this.setOutput(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_totable'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.toTable('+value_from+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['opencv_circle'] = {
  init: function() {
    this.appendValueInput("x")
        .setCheck("Number")
        .appendField("draw circle x:");
    this.appendValueInput("y")
        .setCheck("Number")
        .appendField("y:");
    this.appendValueInput("r")
        .setCheck("Number")
        .appendField("r:");
    this.appendValueInput("color")
        .setCheck(null)
        .appendField("color:");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("to image");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_circle'] = function(block) {
  var value_x = Blockly.Lua.valueToCode(block, 'x', Blockly.Lua.ORDER_ATOMIC);
  var value_y = Blockly.Lua.valueToCode(block, 'y', Blockly.Lua.ORDER_ATOMIC);
  var value_r = Blockly.Lua.valueToCode(block, 'r', Blockly.Lua.ORDER_ATOMIC);
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.circle('+value_to+','+value_x+','+value_y+','+value_r+','+value_color+')\n';
  return code;
};

Blockly.Blocks['opencv_imread'] = {
  init: function() {
    this.appendValueInput("path")
        .setCheck("String")
        .appendField("load image");
    this.setOutput(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_imread'] = function(block) {
  var value_path = Blockly.Lua.valueToCode(block, 'path', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.imread('+value_path+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['opencv_imwrite'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("save image");
    this.appendValueInput("to")
        .setCheck("String")
        .appendField("to file path");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_imwrite'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.opencv.imwrite('+value_to+','+value_from+')\n';
  return code;
};

Blockly.Blocks['opencv_addweighted'] = {
  init: function() {
    this.appendValueInput("src1")
        .setCheck("opencv_img")
        .appendField("mix image");
    this.appendValueInput("alpha")
        .setCheck("Number")
        .appendField("* alpha");
    this.appendValueInput("src2")
        .setCheck("opencv_img")
        .appendField("+ image");
    this.appendValueInput("beta")
        .setCheck("Number")
        .appendField("* beta");
    this.appendValueInput("gamma")
        .setCheck("Number")
        .appendField("+ gamma");
    this.appendValueInput("to")
        .setCheck("opencv_img")
        .appendField("and put to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['opencv_addweighted'] = function(block) {
  var value_src1 = Blockly.Lua.valueToCode(block, 'src1', Blockly.Lua.ORDER_ATOMIC);
  var value_alpha = Blockly.Lua.valueToCode(block, 'alpha', Blockly.Lua.ORDER_ATOMIC);
  var value_src2 = Blockly.Lua.valueToCode(block, 'src2', Blockly.Lua.ORDER_ATOMIC);
  var value_beta = Blockly.Lua.valueToCode(block, 'beta', Blockly.Lua.ORDER_ATOMIC);
  var value_gamma = Blockly.Lua.valueToCode(block, 'gamma', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);

  var code = 'proteo.opencv.addWeighted('+value_src1+','+value_alpha+','+value_src2+','+value_beta+','+value_to+','+value_gamma+')\n';
  return code;
};


