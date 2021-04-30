//===============================================================================
//  GUI
//===============================================================================

Blockly.Blocks['gui_minx'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MIN_X");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_minx'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MIN_X';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_midx'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MID_X");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_midx'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MID_X';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_maxx'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MAX_X");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_maxx'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MAX_X';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_miny'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MIN_Y");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_miny'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MIN_Y';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_midy'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MID_Y");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_midy'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MID_Y';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_maxy'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("MAX_Y");
    this.setOutput(true, "Number");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_maxy'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'MAX_Y';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_font'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("font");
    this.appendValueInput("name")
        .setCheck("String")
        .appendField("name:");
    this.appendValueInput("size")
        .setCheck("Number")
        .appendField("size:");
    this.appendValueInput("color")
        .setCheck(null)
        .appendField("color:");
    this.setInputsInline(true);
    this.setOutput(true, "gui_font");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_font'] = function(block) {
  var value_name = Blockly.Lua.valueToCode(block, 'name', Blockly.Lua.ORDER_ATOMIC);
  var value_size = Blockly.Lua.valueToCode(block, 'size', Blockly.Lua.ORDER_ATOMIC);
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);

  var code =value_name+','+value_size+','+value_color;

  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_position'] = {
  init: function() {
    this.appendValueInput("x")
        .setCheck("Number")
        .appendField("X:");
    this.appendValueInput("y")
        .setCheck("Number")
        .appendField("Y:");
    this.appendValueInput("w")
        .setCheck("Number")
        .appendField("W:");
    this.appendValueInput("h")
        .setCheck("Number")
        .appendField("H:");
    this.setInputsInline(true);
    this.setOutput(true, "gui_position");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_position'] = function(block) {
  var value_x = Blockly.Lua.valueToCode(block, 'x', Blockly.Lua.ORDER_ATOMIC);
  var value_y = Blockly.Lua.valueToCode(block, 'y', Blockly.Lua.ORDER_ATOMIC);
  var value_w = Blockly.Lua.valueToCode(block, 'w', Blockly.Lua.ORDER_ATOMIC);
  var value_h = Blockly.Lua.valueToCode(block, 'h', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = value_x+','+value_y+','+value_w+','+value_h;
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_ATOMIC];
};

Blockly.Blocks['gui_newlabel'] = {
  init: function() {
    this.appendValueInput("id")
        .setCheck("String")
        .appendField("create new label with")
        .appendField("id:");
    this.appendValueInput("text")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("text:");
    this.appendValueInput("font")
        .setCheck("gui_font")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("font:");
    this.appendValueInput("back")
        .setCheck(null) //["colour_rgb", "colour_picker"])
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("back color:");
    this.appendDummyInput()
        .appendField("align")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField(new Blockly.FieldDropdown([["left","LEFT"], ["center","CENTER"], ["right","RIGHT"]]), "align");
    this.appendValueInput("position")
        .setCheck("gui_position")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("position:");
    this.setInputsInline(false);
    this.setOutput(true, "gui_object");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newlabel'] = function(block) {
  var value_id = Blockly.Lua.valueToCode(block, 'id', Blockly.Lua.ORDER_ATOMIC);
  var value_text = Blockly.Lua.valueToCode(block, 'text', Blockly.Lua.ORDER_ATOMIC);
  var value_font = Blockly.Lua.valueToCode(block, 'font', Blockly.Lua.ORDER_ATOMIC);
  var value_back = Blockly.Lua.valueToCode(block, 'back', Blockly.Lua.ORDER_ATOMIC);
  var dropdown_align = block.getFieldValue('align');
  var value_position = Blockly.Lua.valueToCode(block, 'position', Blockly.Lua.ORDER_ATOMIC);
  
  var align="proteo.gui.LabelAlignment.Right";
  if (dropdown_align=="CENTER")
  {
    align="proteo.gui.LabelAlignment.Center";
  }else if(dropdown_align=="LEFT")
  {
    align="proteo.gui.LabelAlignment.Left";
  }

  var code = 'proteo.gui.newLabel('+value_id+','+value_text+','+value_font+','+value_back+','+align+','+value_position+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newsimpletextfield'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new text field with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("text:")
        .appendField(new Blockly.FieldTextInput("text"), "text")
        .appendField("font:")
        .appendField(new Blockly.FieldDropdown([["Colaborate","Colaborate"], ["Helvetica","Helvetica"], ["OpenSans","OpenSans"]]), "font")
        .appendField(new Blockly.FieldNumber(0), "fontsize")
        .appendField(new Blockly.FieldColour("#000000"), "fontcolor")
        .appendField("back color:")
        .appendField(new Blockly.FieldColour("#99ff99"), "back")
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0), "h");
    this.appendValueInput("callback")
        .setCheck("gui_callback")
        .appendField("call:");
    this.setInputsInline(true);
    this.setOutput(true, "gui_object");
    this.setColour(120);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newsimpletextfield'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_text = block.getFieldValue('text');
  var dropdown_font = block.getFieldValue('font');
  var number_fontsize = block.getFieldValue('fontsize');
  var colour_fontcolor = block.getFieldValue('fontcolor');
  var colour_back = block.getFieldValue('back');
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  var value_callback = Blockly.Lua.valueToCode(block, 'callback', Blockly.Lua.ORDER_ATOMIC);

var callback='""';
  if (value_callback!="")
    callback=value_callback;
  
   var code = 'proteo.gui.newTextField("'+text_id+'","'+text_text+'","'+dropdown_font+'",'+number_fontsize+',"'+colour_fontcolor+'","'+colour_back+'",'+number_x+','+number_y+','+number_w+','+number_h+','+callback+')';

  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newsimplebutton'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new button with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("text:")
        .appendField(new Blockly.FieldTextInput("text"), "text")
        .appendField("font:")
        .appendField(new Blockly.FieldDropdown([["Colaborate","Colaborate"], ["Helvetica","Helvetica"], ["OpenSans","OpenSans"]]), "font")
        .appendField(new Blockly.FieldNumber(0), "fontsize")
        .appendField(new Blockly.FieldColour("#000000"), "fontcolor")
        .appendField("back color:")
        .appendField(new Blockly.FieldColour("#99ff99"), "back")
        .appendField("border size:")
        .appendField(new Blockly.FieldNumber(0), "border")
        .appendField("color:")
        .appendField(new Blockly.FieldColour("#006600"), "color")
        .appendField(new Blockly.FieldDropdown([["squared","squared"], ["rounded","rounded"]]), "squared")
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0), "h");
    this.appendValueInput("callback")
        .setCheck("gui_callback")
        .appendField("call:");
    this.setInputsInline(true);
    this.setOutput(true, "gui_object");
    this.setColour(120);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newsimplebutton'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_text = block.getFieldValue('text');
  var dropdown_font = block.getFieldValue('font');
  var number_fontsize = block.getFieldValue('fontsize');
  var colour_fontcolor = block.getFieldValue('fontcolor');
  var colour_back = block.getFieldValue('back');
  var number_border = block.getFieldValue('border');
  var colour_color = block.getFieldValue('color');
  var dropdown_squared = block.getFieldValue('squared');
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  var value_callback = Blockly.Lua.valueToCode(block, 'callback', Blockly.Lua.ORDER_ATOMIC);

var rounded=true;
if(dropdown_squared!="rounded") rounded=false;

var callback='""';
  if (value_callback!="")
    callback=value_callback;

   var code = 'proteo.gui.newButton("'+text_id+'","'+text_text+'","'+dropdown_font+'",'+number_fontsize+',"'+colour_fontcolor+'","'+colour_back+'",'+number_border+',"'+colour_color+'",'+rounded+','+number_x+','+number_y+','+number_w+','+number_h+','+callback+')';

  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newbutton'] = {
  init: function() {
    this.appendValueInput("id")
        .setCheck("String")
        .appendField("create new button with")
        .appendField("id:");
    this.appendValueInput("text")
        .setCheck("String")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("text:");
    this.appendValueInput("font")
        .setCheck("gui_font")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("font:");
    this.appendValueInput("back")
        .setCheck(null)
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("back color:");
    this.appendValueInput("border")
        .setCheck("Number")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("border size:");
    this.appendValueInput("color")
        .setCheck(null)
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("border color:");
    this.appendDummyInput()
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField(new Blockly.FieldDropdown([["squared","OPTIONNAME"], ["rounded","OPTIONNAME"]]), "NAME");
    this.appendValueInput("position")
        .setCheck("gui_position")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("position:");
    this.appendValueInput("NAME")
        .setCheck("gui_callback")
        .setAlign(Blockly.ALIGN_RIGHT)
        .appendField("callback:");
    this.setInputsInline(false);
    this.setOutput(true, "gui_object");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newbutton'] = function(block) {
  var value_id = Blockly.Lua.valueToCode(block, 'id', Blockly.Lua.ORDER_ATOMIC);
  var value_text = Blockly.Lua.valueToCode(block, 'text', Blockly.Lua.ORDER_ATOMIC);
  var value_font = Blockly.Lua.valueToCode(block, 'font', Blockly.Lua.ORDER_ATOMIC);
  var value_back = Blockly.Lua.valueToCode(block, 'back', Blockly.Lua.ORDER_ATOMIC);
  var value_border = Blockly.Lua.valueToCode(block, 'border', Blockly.Lua.ORDER_ATOMIC);
  var value_color = Blockly.Lua.valueToCode(block, 'color', Blockly.Lua.ORDER_ATOMIC);
  var dropdown_name = block.getFieldValue('NAME');
  var value_position = Blockly.Lua.valueToCode(block, 'position', Blockly.Lua.ORDER_ATOMIC);
  var value_name = Blockly.Lua.valueToCode(block, 'NAME', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = '...';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_callback'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("gui callback")
        .appendField(new Blockly.FieldVariable("sender"), "sender");
    this.appendStatementInput("function")
        .setCheck(null);
    this.setOutput(true, "gui_callback");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_callback'] = function(block) {
  var variable_sender = Blockly.Lua.variableDB_.getName(block.getFieldValue('sender'), Blockly.Variables.NAME_TYPE);
  var statements_function = Blockly.Lua.statementToCode(block, 'function');
  // TODO: Assemble Lua into code variable.
  var code = 'function('+variable_sender+') \n'+statements_function+' end\n';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newsimplelabel'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new label with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("text:")
        .appendField(new Blockly.FieldTextInput("text"), "text")
        .appendField("font:")
        .appendField(new Blockly.FieldDropdown([["Colaborate","Colaborate"], ["Helvetica","Helvetica"], ["OpenSans","OpenSans"]]), "font")
        .appendField(new Blockly.FieldNumber(0), "fontsize")
        .appendField(new Blockly.FieldColour("#000000"), "fontcolor")
        .appendField("back color:")
        .appendField(new Blockly.FieldColour("#99ff99"), "back")
        .appendField(new Blockly.FieldDropdown([["left","LEFT"], ["center","CENTER"], ["right","RIGHT"]]), "align")
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0), "h");
    this.setInputsInline(true);
    this.setOutput(true, "gui_object");
    this.setColour(120);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newsimplelabel'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_text = block.getFieldValue('text');
  var dropdown_font = block.getFieldValue('font');
  var number_fontsize = block.getFieldValue('fontsize');
  var colour_fontcolor = block.getFieldValue('fontcolor');
  var colour_back = block.getFieldValue('back');
  var dropdown_align = block.getFieldValue('align');
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  
  var align="proteo.gui.LabelAlignment.Right";
  if (dropdown_align=="CENTER")
  {
    align="proteo.gui.LabelAlignment.Center";
  }else if(dropdown_align=="LEFT")
  {
    align="proteo.gui.LabelAlignment.Left";
  }

  var code = 'proteo.gui.newLabel("'+text_id+'","'+text_text+'","'+dropdown_font+'",'+number_fontsize+',"'+colour_fontcolor+'","'+colour_back+'",'+align+','+number_x+','+number_y+','+number_w+','+number_h+')';

  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newsimpledropdown'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new dropbox with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("text:")
        .appendField(new Blockly.FieldTextInput("text"), "text")
        .appendField("font:")
        .appendField(new Blockly.FieldDropdown([["Colaborate","Colaborate"], ["Helvetica","Helvetica"], ["OpenSans","OpenSans"]]), "font")
        .appendField(new Blockly.FieldNumber(0), "fontsize")
        .appendField(new Blockly.FieldColour("#000000"), "fontcolor")
        .appendField("icon:")
        .appendField(new Blockly.FieldDropdown([["option","OPTIONNAME"], ["option","OPTIONNAME"], ["option","OPTIONNAME"]]), "icon")
        .appendField(new Blockly.FieldColour("#000000"), "iconcolor")
        .appendField(new Blockly.FieldDropdown([["stack up","STACKUP"],["normal up","NORMALUP"], ["stack","STACK"], ["normal","NORMAL"]]), "type")
        .appendField("back color:")
        .appendField(new Blockly.FieldColour("#99ff99"), "backcolor")
        .appendField("x:")
        .appendField(new Blockly.FieldNumber(0), "x")
        .appendField("y:")
        .appendField(new Blockly.FieldNumber(0), "y")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0), "h");
    this.appendValueInput("callback")
        .setCheck("gui_callback")
        .appendField("call:");
    this.setInputsInline(true);
    this.setOutput(true, "gui_object");
    this.setColour(120);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newsimpledropdown'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_text = block.getFieldValue('text');
  var dropdown_font = block.getFieldValue('font');
  var number_fontsize = block.getFieldValue('fontsize');
  var colour_fontcolor = block.getFieldValue('fontcolor');
  var dropdown_icon = block.getFieldValue('icon');
  var colour_iconcolor = block.getFieldValue('iconcolor');
  var dropdown_type = block.getFieldValue('type');
  var colour_backcolor = block.getFieldValue('backcolor');
  var number_x = block.getFieldValue('x');
  var number_y = block.getFieldValue('y');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  var value_callback = Blockly.Lua.valueToCode(block, 'callback', Blockly.Lua.ORDER_ATOMIC);
  
  var type="proteo.gui.DropDownType.Normal";
  if (dropdown_type=="STACK")
  {
    type="proteo.gui.DropDownType.Stack";
  }
  else if(dropdown_type=="NORMALUP")
  {
    type="proteo.gui.DropDownType.NormalUp";
  }
  else if(dropdown_type=="STACKUP")
  {
    type="proteo.gui.DropDownType.StackUp";
  }

  var callback='""';
  if (value_callback!="")
    callback=value_callback;

  var code = 'proteo.gui.newDropDown("'+text_id+'","'+text_text+'","'+dropdown_font+'",'+number_fontsize+',"'+colour_fontcolor+'","'+dropdown_icon+'","'+colour_iconcolor+'",'+type+',"'+colour_backcolor+'",'+number_x+','+number_y+','+number_w+','+number_h+','+callback+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_newdropdownitem'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("create new dropdown item with id:")
        .appendField(new Blockly.FieldTextInput("id"), "id")
        .appendField("text:")
        .appendField(new Blockly.FieldTextInput("text"), "text")
        .appendField("font:")
        .appendField(new Blockly.FieldDropdown([["Colaborate","Colaborate"], ["Helvetica","Helvetica"], ["OpenSans","OpenSans"]]), "font")
        .appendField(new Blockly.FieldNumber(0), "fontsize")
        .appendField(new Blockly.FieldColour("#ff0000"), "fontcolor")
        .appendField("back color:")
        .appendField(new Blockly.FieldColour("#ff0000"), "backcolor")
        .appendField("icon:")
        .appendField(new Blockly.FieldDropdown([["option","OPTIONNAME"], ["option","OPTIONNAME"], ["option","OPTIONNAME"]]), "icon")
        .appendField(new Blockly.FieldColour("#ff0000"), "iconcolor")
        .appendField("w:")
        .appendField(new Blockly.FieldNumber(0), "w")
        .appendField("h:")
        .appendField(new Blockly.FieldNumber(0), "h");
    this.setOutput(true, "gui_object");
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_newdropdownitem'] = function(block) {
  var text_id = block.getFieldValue('id');
  var text_text = block.getFieldValue('text');
  var dropdown_font = block.getFieldValue('font');
  var number_fontsize = block.getFieldValue('fontsize');
  var colour_fontcolor = block.getFieldValue('fontcolor');
  var colour_backcolor = block.getFieldValue('backcolor');
  var dropdown_icon = block.getFieldValue('icon');
  var colour_iconcolor = block.getFieldValue('iconcolor');
  var number_w = block.getFieldValue('w');
  var number_h = block.getFieldValue('h');
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.gui.newDropDownItem("'+text_id+'","'+text_text+'","'+dropdown_font+'",'+number_fontsize+',"'+colour_fontcolor+'","'+colour_backcolor+'","'+dropdown_icon+'","'+colour_iconcolor+'",'+number_w+','+number_h+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Blocks['gui_additem'] = {
  init: function() {
    this.appendValueInput("add")
        .setCheck("gui_object")
        .appendField("add:");
    this.appendValueInput("to")
        .setCheck("gui_object")
        .appendField("to:");
    this.setInputsInline(true);
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(160);
 this.setTooltip("");
 this.setHelpUrl("");
  }
};

Blockly.Lua['gui_additem'] = function(block) {
  var value_add = Blockly.Lua.valueToCode(block, 'add', Blockly.Lua.ORDER_ATOMIC);
  var value_to = Blockly.Lua.valueToCode(block, 'to', Blockly.Lua.ORDER_ATOMIC);
  // TODO: Assemble Lua into code variable.
  var code = 'proteo.gui.addItem('+value_to+','+value_add+')\n';
  return code;
};
