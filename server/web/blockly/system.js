//===============================================================================
//  SYSTEM
//===============================================================================

Blockly.Blocks['system_createtimer'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create a timer ");
    this.appendValueInput("time")
        .setCheck("Number")
        .appendField("activate every");
    this.appendValueInput("callback")
        .setCheck("timer_callback")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("with callback");
    this.setOutput(true, "timer");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_createtimer'] = function(block) {
  var value_time = Blockly.Lua.valueToCode(block, 'time', Blockly.Lua.ORDER_ATOMIC);
  var value_callback = Blockly.Lua.valueToCode(block, 'callback', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = "proteo.system.createTimer("+value_time+","+value_callback+")";
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['system_callbacktimer'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("timer callback")
        .appendField(new Blockly.FieldVariable("dt"), "time");
    this.appendStatementInput("function")
        .setCheck(null);
    this.setOutput(true, "timer_callback");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_callbacktimer'] = function(block) {
  var variable_time = Blockly.Lua.variableDB_.getName(block.getFieldValue('time'), Blockly.Variables.NAME_TYPE);
  var statements_function = Blockly.Lua.statementToCode(block, 'function');
  // TODO: Assemble Lua into code variable.
  var code = 'function('+variable_time+') \n'+statements_function+' end\n';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['system_clock'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("clock");
    this.setOutput(true, "String");
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_clock'] = function(block) {
  var code = 'proteo.system.clock()';
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['system_starttimer'] = {
  init: function() {
    this.appendDummyInput()
        .appendField(new Blockly.FieldDropdown([["start","start"], ["stop","stop"]]), "mode")
        .appendField("timer")
        .appendField(new Blockly.FieldVariable("timer"), "timer");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_starttimer'] = function(block) {
  var dropdown_mode = block.getFieldValue('mode');
  var variable_timer = Blockly.Lua.variableDB_.getName(block.getFieldValue('timer'), Blockly.Variables.NAME_TYPE);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.system.'+dropdown_mode+'Timer('+variable_timer+')\n';
  return code;
};

Blockly.Blocks['system_login'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("login with:");
    this.appendValueInput("user")
        .setCheck("String")
        .appendField("user");
    this.appendValueInput("password")
        .setCheck("String")
        .appendField("password");
    this.appendValueInput("script")
        .setCheck("String")
        .appendField("script");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(230);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_login'] = function(block) {
  var value_user = Blockly.Lua.valueToCode(block, 'user', Blockly.Lua.ORDER_ATOMIC);
  var value_password = Blockly.Lua.valueToCode(block, 'password', Blockly.Lua.ORDER_ATOMIC);
  var value_script = Blockly.Lua.valueToCode(block, 'script', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.system.login('+value_user+','+value_password+','+value_script+')\n';
  return code;
};

Blockly.Blocks['system_local'] = {
  init: function() {
    this.appendValueInput("to")
        .setCheck(null)
        .appendField("set local")
        .appendField(new Blockly.FieldVariable("item"), "NAME")
        .appendField("to");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_local'] = function(block) {
  var variable_name = Blockly.Lua.variableDB_.getName(block.getFieldValue('NAME'), Blockly.Variables.NAME_TYPE);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'local '+variable_name+'='+value_to+'\n';
  return code;
};

Blockly.Blocks['system_document'] = {
  init: function() {
    this.appendValueInput("path")
        .setCheck("String")
        .appendField("path: document/");
    this.setOutput(true, "String");
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_document'] = function(block) {
  var value_path = Blockly.Lua.valueToCode(block, 'path', Blockly.Lua.ORDER_ATOMIC);

  var code = 'proteo.system.document()..'+value_path;

  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['system_readfile'] = {
  init: function() {
    this.appendValueInput("path")
        .setCheck("String")
        .appendField("read text file");
    this.appendValueInput("callback")
        .setCheck("readfile_callback")
        .appendField("and send to");
    this.setInputsInline(true);
    this.setColour(330);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_readfile'] = function(block) {
  var value_path = Blockly.Lua.valueToCode(block, 'path', Blockly.Lua.ORDER_ATOMIC);
  var value_callback = Blockly.Lua.valueToCode(block, 'callback', Blockly.Lua.ORDER_ATOMIC);

  var code = 'proteo.system.readFile('+value_path+','+value_callback+')\n';

  return code;
};

Blockly.Blocks['system_callbackreadfile'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("read file callback")
        .appendField(new Blockly.FieldVariable("filename"), "filename")
        .appendField(new Blockly.FieldVariable("data"), "data");
    this.appendStatementInput("function")
        .setCheck(null);
    this.setOutput(true, "readfile_callback");
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_callbackreadfile'] = function(block) {
  var variable_filename = Blockly.Lua.variableDB_.getName(block.getFieldValue('filename'), Blockly.Variables.NAME_TYPE);
  var variable_data = Blockly.Lua.variableDB_.getName(block.getFieldValue('data'), Blockly.Variables.NAME_TYPE);
  var statements_function = Blockly.Lua.statementToCode(block, 'function');
  // TODO: Assemble Lua into code variable.
  var code = 'function('+variable_data+','+variable_filename+') \n'+statements_function+' end\n';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['system_username'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("USERNAME");
    this.setOutput(true, "String");
    this.setColour(330);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['system_username'] = function(block) {
  var code = 'USERNAME';
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

