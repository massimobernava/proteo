Blockly.Blocks['ffmpeg_newencoder'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create encoder")
        .appendField(new Blockly.FieldDropdown([["h264","AV_CODEC_ID_H264"], ["mpeg4","AV_CODEC_ID_MPEG4"], ["h265","AV_CODEC_ID_H265"], ["vp9","AV_CODEC_ID_VP9"]]), "codec")
        .appendField("w")
        .appendField(new Blockly.FieldNumber(0, 0), "w")
        .appendField("h")
        .appendField(new Blockly.FieldNumber(0, 0), "h");
    this.setOutput(true, "ffmpeg_encoder");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['ffmpeg_newencoder'] = function(block) {
  var dropdown_codec = block.getFieldValue('codec');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.ffmpeg.newEncoder(proteo.ffmpeg.CodecID.'+dropdown_codec+','+number_w+','+number_h+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['ffmpeg_newdecoder'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create decoder")
        .appendField(new Blockly.FieldDropdown([["h264","AV_CODEC_ID_H264"], ["mpeg4","AV_CODEC_ID_MPEG4"], ["h265","AV_CODEC_ID_H265"], ["vp9","AV_CODEC_ID_VP9"]]), "codec");
    this.setOutput(true, "ffmpeg_decoder");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['ffmpeg_newdecoder'] = function(block) {
  var dropdown_codec = block.getFieldValue('codec');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.ffmpeg.newDecoder(proteo.ffmpeg.CodecID.'+dropdown_codec+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};


Blockly.Blocks['ffmpeg_newframe'] = {
  init: function() {
    this.appendValueInput("codec")
        .setCheck(["ffmpeg_encoder", "ffmpeg_decoder"])
        .appendField("create frame for codec:");
    this.setOutput(true, "ffmpeg_frame");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['ffmpeg_newframe'] = function(block) {
  var value_codec = Blockly.Lua.valueToCode(block, 'codec', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.ffmpeg.newFrame('+value_codec+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['ffmpeg_newpacket'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create packet");
    this.setOutput(true, "ffmpeg_frame");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['ffmpeg_newpacket'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.ffmpeg.newPacket()';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['ffmpeg_copyimage'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("opencv_img")
        .appendField("copy image ");
    this.appendValueInput("to")
        .setCheck("ffmpeg_frame")
        .appendField("to frame");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['ffmpeg_copyimage'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.ffmpeg.copyImage('+value_to+','+value_from+')\n';
  return code;
};

Blockly.Blocks['ffmpeg_encodeframe'] = {
  init: function() {
    this.appendValueInput("encoder")
        .setCheck("ffmpeg_encoder")
        .appendField("use codec");
    this.appendValueInput("from")
        .setCheck("ffmpeg_frame")
        .appendField("to encode frame");
    this.appendValueInput("to")
        .setCheck("ffmpeg_packet")
        .appendField("and put to packet");
    this.setInputsInline(true);
    this.setOutput(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['ffmpeg_encodeframe'] = function(block) {
  var value_encoder = Blockly.Lua.valueToCode(block, 'encoder', Blockly.Lua.ORDER_ATOMIC);
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.ffmpeg.encodeFrame('+value_encoder+', '+value_from+', '+value_to+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['ffmpeg_encodepacket'] = {
  init: function() {
    this.appendValueInput("packet")
        .setCheck("ffmpeg_packet")
        .appendField("encode packet");
    this.setOutput(true, "String");
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['ffmpeg_encodepacket'] = function(block) {
  var value_packet = Blockly.Lua.valueToCode(block, 'packet', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.ffmpeg.encodePacket('+value_packet+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['ffmpeg_decodepacket'] = {
  init: function() {
    this.appendValueInput("data")
        .setCheck("String")
        .appendField("decode data");
    this.appendValueInput("packet")
        .setCheck("ffmpeg_packet")
        .appendField("to packet");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['ffmpeg_decodepacket'] = function(block) {
  var value_data = Blockly.Lua.valueToCode(block, 'data', Blockly.Lua.ORDER_ATOMIC);
  var value_packet = Blockly.Lua.valueToCode(block, 'packet', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.ffmpeg.decodePacket('+value_packet+','+value_data+')\n';
  return code;
};

Blockly.Blocks['ffmpeg_decodeframe'] = {
  init: function() {
    this.appendValueInput("decoder")
        .setCheck("ffmpeg_decoder")
        .appendField("use codec");
    this.appendValueInput("from")
        .setCheck("ffmpeg_packet")
        .appendField("to decode packet");
    this.appendValueInput("to")
        .setCheck("ffmpeg_frame")
        .appendField("and put to frame ");
    this.setInputsInline(true);
    this.setOutput(true, null);
    this.setColour(65);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['ffmpeg_decodeframe'] = function(block) {
  var value_decoder = Blockly.Lua.valueToCode(block, 'decoder', Blockly.Lua.ORDER_ATOMIC);
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.ffmpeg.decodeFrame('+value_decoder+','+value_from+','+value_to+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['ffmpeg_copyframe'] = {
  init: function() {
    this.appendValueInput("from")
        .setCheck("ffmpeg_frame")
        .appendField("copy frame ");
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


Blockly.Lua['ffmpeg_copyframe'] = function(block) {
  var value_from = Blockly.Lua.valueToCode(block, 'from', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.ffmpeg.copyFrame('+value_to+','+value_from+')\n';
  return code;
};