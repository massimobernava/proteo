Blockly.Blocks['audio_infodrivers'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("print audio drivers information");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['audio_infodrivers'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.audio.infoDrivers()\n';
  return code;
};

Blockly.Blocks['audio_init'] = {
  init: function() {
    this.appendValueInput("driver")
        .setCheck("String")
        .appendField("init audio driver");
    this.setOutput(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['audio_init'] = function(block) {
  var value_driver = Blockly.Lua.valueToCode(block, 'driver', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.audio.init('+value_driver+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['audio_infodevices'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("print audio devices information");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['audio_infodevices'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.audio.infoDevices()\n';
  return code;
};

Blockly.Blocks['audio_opendevice'] = {
  init: function() {
    this.appendValueInput("device")
        .setCheck("String")
        .appendField("open audio device");
    this.appendDummyInput()
        .appendField("capture")
        .appendField(new Blockly.FieldCheckbox("FALSE"), "capture")
        .appendField("frequency")
        .appendField(new Blockly.FieldNumber(48000, 0), "freq")
        .appendField("format")
        .appendField(new Blockly.FieldDropdown([["32-bit floating point samples","AUDIO_F32SYS"], ["32-bit integer samples","AUDIO_S32SYS"], ["signed 16-bit samples","AUDIO_S16SYS"], ["unsigned 16-bit samples","AUDIO_U16SYS"]]), "format")
        .appendField("channels")
        .appendField(new Blockly.FieldNumber(2, 1, 16), "ch")
        .appendField("samples")
        .appendField(new Blockly.FieldNumber(4096, 0), "samples");
    this.setOutput(true, "audio_device");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['audio_opendevice'] = function(block) {
  var value_device = Blockly.Lua.valueToCode(block, 'device', Blockly.Lua.ORDER_ATOMIC);
  var checkbox_capture = block.getFieldValue('capture') == 'TRUE';
  var number_freq = block.getFieldValue('freq');
  var dropdown_format = block.getFieldValue('format');
  var number_ch = block.getFieldValue('ch');
  var number_samples = block.getFieldValue('samples');
  
  var device="nil";
  if(value_device!="") device=value_device;

  var iscapture="false";
  if(checkbox_capture) iscapture="true";

  var code = 'proteo.audio.openDevice('+device+','+iscapture+','+number_freq+',proteo.audio.Format.'+dropdown_format+','+number_ch+','+number_samples+')';
  
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['audio_pausedevice'] = {
  init: function() {
    this.appendValueInput("device")
        .setCheck("audio_device")
        .appendField(new Blockly.FieldDropdown([["start","false"], ["pause","true"]]), "state")
        .appendField("audio device");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['audio_pausedevice'] = function(block) {
  var dropdown_state = block.getFieldValue('state');
  var value_device = Blockly.Lua.valueToCode(block, 'device', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.audio.pauseDevice('+value_device+','+dropdown_state+')\n';
  return code;
};

Blockly.Blocks['audio_loadwav'] = {
  init: function() {
    this.appendValueInput("file")
        .setCheck("String")
        .appendField("load wav");
    this.setOutput(true, "audio_sound");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['audio_loadwav'] = function(block) {
  var value_file = Blockly.Lua.valueToCode(block, 'file', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.audio.loadWav('+value_file+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['audio_convertaudio'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("convert audio")
        .appendField(new Blockly.FieldVariable("item"), "file")
        .appendField("to frequency")
        .appendField(new Blockly.FieldNumber(48000, 0), "freq")
        .appendField("format")
        .appendField(new Blockly.FieldDropdown([["32-bit floating point samples","AUDIO_F32SYS"], ["32-bit integer samples","AUDIO_S32SYS"], ["signed 16-bit samples","AUDIO_S16SYS"], ["unsigned 16-bit samples","AUDIO_U16SYS"]]), "format")
        .appendField("and channel")
        .appendField(new Blockly.FieldNumber(2, 1, 16), "ch");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['audio_convertaudio'] = function(block) {
  var variable_file = Blockly.Lua.variableDB_.getName(block.getFieldValue('file'), Blockly.Variables.NAME_TYPE);
  var number_freq = block.getFieldValue('freq');
  var dropdown_format = block.getFieldValue('format');
  var number_ch = block.getFieldValue('ch');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.audio.convertAudio('+variable_file+','+number_freq+',proteo.audio.Format.'+dropdown_format+','+number_ch+')\n';
  return code;
};

Blockly.Blocks['audio_playaudio'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("play")
        .appendField(new Blockly.FieldVariable("audio"), "audio")
        .appendField("in")
        .appendField(new Blockly.FieldVariable("device"), "device")
        .appendField("with volume")
        .appendField(new Blockly.FieldNumber(100, 0, 100), "volume");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['audio_playaudio'] = function(block) {
  var variable_audio = Blockly.Lua.variableDB_.getName(block.getFieldValue('audio'), Blockly.Variables.NAME_TYPE);
  var variable_device = Blockly.Lua.variableDB_.getName(block.getFieldValue('device'), Blockly.Variables.NAME_TYPE);
  var number_volume = block.getFieldValue('volume');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.audio.playAudio('+variable_device+','+variable_audio+','+number_volume+')\n';
  return code;
};
