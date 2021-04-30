//===============================================================================
//  GRAPHICS
//===============================================================================
Blockly.Blocks['graphics_newimage'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new image with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("from file:")
        .appendField(new Blockly.FieldTextInput("file"), "file")
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0, 0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0, 0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0, 0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0, 0), "h");
    this.setInputsInline(true);
    this.setOutput(true, "graphics_object");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_newimage'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_file = block.getFieldValue('file');
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.graphics.newImage("'+text_id+'","'+text_file+'",'+number_x+','+number_y+','+number_w+','+number_h+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['graphics_newicon'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new icon with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("name:")
        .appendField(new Blockly.FieldTextInput("name"), "name");
    this.appendValueInput("color")
        .setCheck(null)
        .appendField("color:");
    this.appendDummyInput()
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0, 0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0, 0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0, 0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0, 0), "h");
    this.setInputsInline(true);
    this.setOutput(true, "graphics_object");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_newicon'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_name = block.getFieldValue('name');
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.graphics.newIcon("'+text_id+'","'+text_name+'",'+value_color+','+number_x+','+number_y+','+number_w+','+number_h+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['graphics_changeimage'] = {
  init: function() {
    this.appendValueInput("to")
        .setCheck("graphics_object")
        .appendField("change image");
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("with");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_changeimage'] = function(block) {
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.graphics.changeImage('+value_to+','+value_from+')\n';
  return code;
};

Blockly.Blocks['graphics_setbackground'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("set background")
        .appendField(new Blockly.FieldColour("#999999"), "color");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_setbackground'] = function(block) {
  var colour_color = block.getFieldValue('color');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.graphics.setBackground("'+colour_color+'")\n';
  return code;
};

Blockly.Blocks['graphics_newrect'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new rectangle with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id");
    this.appendValueInput("color")
        .setCheck(null)
        .appendField("color:");
    this.appendValueInput("border")
        .setCheck(null)
        .appendField("border:");
    this.appendValueInput("x")
        .setCheck("Number")
        .appendField("x:");
    this.appendValueInput("y")
        .setCheck("Number")
        .appendField("y:");
    this.appendValueInput("w")
        .setCheck("Number")
        .appendField("w:");
    this.appendValueInput("h")
        .setCheck("Number")
        .appendField("h:");
    this.setInputsInline(true);
    this.setOutput(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_newrect'] = function(block) {
  var text_id = block.getFieldValue('id');
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);
  var value_border = Blockly.Lua.valueToCode(block, 'border', Blockly.Lua.ORDER_ATOMIC);
  var value_x = Blockly.Lua.valueToCode(block, 'x', Blockly.Lua.ORDER_ATOMIC);
  var value_y = Blockly.Lua.valueToCode(block, 'y', Blockly.Lua.ORDER_ATOMIC);
  var value_w = Blockly.Lua.valueToCode(block, 'w', Blockly.Lua.ORDER_ATOMIC);
  var value_h = Blockly.Lua.valueToCode(block, 'h', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.graphics.newRect("'+text_id+'",'+value_color+','+value_border+','+value_x+','+value_y+','+value_w+','+value_h+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['graphics_top'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("TOP");
    this.setOutput(true, "Number");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_top'] = function(block) {

  var code = 'TOP';
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['graphics_bottom'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("BOTTOM");
    this.setOutput(true, "Number");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_bottom'] = function(block) {

  var code = 'BOTTOM';
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['graphics_setlayer'] = {
  init: function() {
    this.appendValueInput("object")
        .setCheck("graphics_object")
        .appendField("set layer of ");
    this.appendValueInput("layer")
        .setCheck("Number")
        .appendField("to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_setlayer'] = function(block) {
  var value_object = Blockly.Lua.valueToCode(block, 'object', Blockly.Lua.ORDER_ATOMIC);
  var value_layer = Blockly.Lua.valueToCode(block, 'layer', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.graphics.setLayer('+value_object+','+value_layer+')\n';
  return code;
};

Blockly.Blocks['graphics_setcolor'] = {
  init: function() {
    this.appendValueInput("object")
        .setCheck("graphics_object")
        .appendField("set color of ");
    this.appendValueInput("color")
        .setCheck(null)
        .appendField("to");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['graphics_setcolor'] = function(block) {
  var value_object = Blockly.Lua.valueToCode(block, 'object', Blockly.Lua.ORDER_ATOMIC);
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);
  var code = 'proteo.graphics.setColor('+value_object+','+value_color+')\n';
  return code;
};

Blockly.Blocks['color_clear'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("transparent");
    this.setOutput(true, null);
    this.setColour(30);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['color_clear'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = '"clear"';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};


